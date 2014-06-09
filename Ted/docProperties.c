/************************************************************************/
/*									*/
/*  Document Properties administration routines.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appUnit.h>
#   include	<utilMatchFont.h>

#   include	"docBuf.h"
#   include	"docFind.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

void docInitDocumentStatistics(		DocumentStatistics *	ds )
    {
    ds->dsPageCount= 0;
    ds->dsParagraphCount= 0;
    ds->dsLineCount= 0;
    ds->dsWordCount= 0;
    ds->dsCharacterCount= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Paragraph numbering.						*/
/*									*/
/************************************************************************/

void docInitParagraphNumber(	ParagraphNumber*	pn )
    {
    pn->pnLevel= -1;
    pn->pnNumberStyle= DOCpnDEC;
    pn->pnJustification= 0; /* left */
    pn->pnUsePrevText= 0;

    pn->pnUseHangingIndent= 0;

    pn->pnStartAt= 0;
    pn->pnIndent= 0;
    pn->pnSpace= 0;

    utilInitTextAttribute( &(pn->pnTextAttribute) );

    pn->pnTextBefore[0]= '\0';
    pn->pnTextAfter[0]= '\0';

    return;
    }

/************************************************************************/
/*									*/
/*  Merge the font list of one document into that of another one.	*/
/*  Update the references accordingly.					*/
/*									*/
/*  a)  Scan all paragraphs to remember text attributes. This is done	*/
/*	with a 'Find' routine thet finds nothing.			*/
/*									*/
/*  1)  NOTE that the default color 0 is not really a color.		*/
/*  2)  As we do not want to look for colors everywhere, we also map	*/
/*	unused colors. This differs from the approach to fonts.		*/
/*									*/
/************************************************************************/

typedef struct ResourceUsed
    {
    int *	ruFontsUsed;
    int *	ruListStyleUsed;
    } ResourceUsed;

/*  a  */
static int docMergeScanParaAttributes(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    ResourceUsed *		ru= (ResourceUsed *)through;

    const TextParticule *	tp;
    int				part;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; part++, tp++ )
	{
	TextAttribute	ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	ru->ruFontsUsed[ta.taFontNumber]= 1;
	}

    if  ( bi->biParaListOverride > 0 )
	{ ru->ruListStyleUsed[bi->biParaListOverride]= 1;	}

    return 1;
    }

static int docMergeListTables(	BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const int *			lsUsed,
				int *				lsMap,
				const int *			listUsed,
				const int *			fontMap,
				const int *			colorMap )
    {
    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);

    const DocumentListTable *	dltFrom= &(dpFrom->dpListTable);
    DocumentListTable *		dltTo= &(dpTo->dpListTable);
    const ListOverrideTable *	lotFrom= &(dpFrom->dpListOverrideTable);
    ListOverrideTable *		lotTo= &(dpTo->dpListOverrideTable);
    const DocumentList *	dl;
    const ListOverride *	lo;

    int				from;
    int				listStylesAdded= 0;

    lo= lotFrom->lotOverrides;
    for ( from= 0; from < lotFrom->lotOverrideCount; lo++, from++ )
	{
	int	oldCount= lotTo->lotOverrideCount;

	int	listStyle;

	if  ( ! lsUsed[from] )
	    { continue;	}

	listStyle= docMergeListOverrideIntoTable( lotTo, lo,
							fontMap, colorMap );
	if  ( listStyle < 0 )
	    { LDEB(listStyle); return -1;	}
	lsMap[from]= listStyle;

	if  ( listStyle >= oldCount )
	    { listStylesAdded++;	}
	}

    dl= dltFrom->dltLists;
    for ( from= 0; from < dltFrom->dltListCount; dl++, from++ )
	{
	int	listIndex;

	if  ( ! listUsed[from] )
	    { continue;	}

	listIndex= docMergeListIntoTable( dltTo, dltFrom->dltLists+ from,
							fontMap, colorMap );
	if  ( listIndex < 0 )
	    { LLDEB(from,listIndex); return -1;	}
	}

    if  ( listStylesAdded > 0						&&
	  docClaimListNumberTreeNodes( &(bdTo->bdListNumberTrees),
				       &(bdTo->bdListNumberTreeCount),
				       lotTo->lotOverrideCount )	)
	{ LDEB(listStylesAdded); return -1;	}

    return 0;
    }

static void docCountListFontsUsed(	BufferDocument *	bdFrom,
					const int *		lsUsed,
					int *			listUsed,
					int *			fontUsed )
    {
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);

    const DocumentListTable *	dltFrom= &(dpFrom->dpListTable);
    const ListOverrideTable *	lotFrom= &(dpFrom->dpListOverrideTable);

    const DocumentList *	dl;
    ListOverride *		lo;

    int				from;

    lo= lotFrom->lotOverrides;
    for ( from= 0; from < lotFrom->lotOverrideCount; lo++, from++ )
	{
	int				listIndex;

	int				level;
	const ListOverrideLevel *	lol;

	if  ( ! lsUsed[from] )
	    { continue;	}

	listIndex= docGetListOfOverride( lo, dltFrom );
	if  ( listIndex < 0 )
	    { LDEB(listIndex);		}
	else{ listUsed[listIndex]= 1;	}

	lol= lo->loLevels;
	for ( level= 0; level < lo->loLevelCount; lol++, level++ )
	    {
	    const DocumentListLevel *	dll= &(lol->lolListLevel);

	    if  ( ! lol->lolOverrideFormat )
		{ continue;	}
	    if  ( ! PROPmaskISSET( &(dll->dllTextAttributeMask),
						    TApropDOC_FONT_NUMBER ) )
		{ continue;	}

	    fontUsed[dll->dllTextAttribute.taFontNumber]= 1;
	    }
	}

    dl= dltFrom->dltLists;
    for ( from= 0; from < dltFrom->dltListCount; dl++, from++ )
	{
	int				level;
	const DocumentListLevel *	dll= dl->dlLevels;

	if  ( ! listUsed[from] )
	    { continue;	}

	for ( level= 0; level < dl->dlLevelCount; dll++, level++ )
	    {
	    if  ( ! PROPmaskISSET( &(dll->dllTextAttributeMask),
						    TApropDOC_FONT_NUMBER ) )
		{ continue;	}

	    fontUsed[dll->dllTextAttribute.taFontNumber]= 1;
	    }
	}

    return;
    }

int docMergeDocumentLists(	int **				pFontMap,
				int **				pColorMap,
				int **				pListStyleMap,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const PostScriptFontList *	psfl )
    {
    int				rval= 0;

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    DocumentFontList *		dflTo= &(dpTo->dpFontList);

    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    const DocumentFontList *	dflFrom= &(dpFrom->dpFontList);
    const DocumentFont *	dfFrom;
    const RGB8Color *		rgb8From;

    const DocumentListTable *	dltFrom= &(dpFrom->dpListTable);
    const ListOverrideTable *	lotFrom= &(dpFrom->dpListOverrideTable);

    int *			fontMap= (int *)0;
    int *			fontUsed= (int *)0;

    int *			colorMap= (int *)0;

    int *			lsUsed= (int *)0;
    int *			lsMap= (int *)0;
    int *			listUsed= (int *)0;

    int				from;
    int				to;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    ResourceUsed		ru;

    /*****/

    if  ( dflFrom->dflFontCount > 0 )
	{
	fontMap= (int *)malloc( dflFrom->dflFontCount* sizeof( int ) );
	if  ( ! fontMap )
	    { LXDEB(dflFrom->dflFontCount,fontMap); rval= -1; goto ready; }
	fontUsed= (int *)malloc( dflFrom->dflFontCount* sizeof( int ) );
	if  ( ! fontUsed )
	    { LXDEB(dflFrom->dflFontCount,fontUsed); rval= -1; goto ready; }

	for ( from= 0; from < dflFrom->dflFontCount; from++ )
	    { fontMap[from]= -1; fontUsed[from]= 0; }
	}

    if  ( lotFrom->lotOverrideCount > 0 )
	{
	lsUsed= malloc( lotFrom->lotOverrideCount* sizeof(int) );
	if  ( ! lsUsed )
	    { LXDEB(lotFrom->lotOverrideCount,lsUsed); rval= -1; goto ready; }
	lsMap= malloc( lotFrom->lotOverrideCount* sizeof(int) );
	if  ( ! lsMap )
	    { LXDEB(lotFrom->lotOverrideCount,lsMap); rval= -1; goto ready; }

	for ( from= 0; from < lotFrom->lotOverrideCount; from++ )
	    { lsMap[from]= -1; lsUsed[from]= 0; }

	lsMap[0]= 0;
	}

    if  ( dltFrom->dltListCount > 0 )
	{
	listUsed= malloc( dltFrom->dltListCount* sizeof(int) );
	if  ( ! listUsed )
	    { LXDEB(dltFrom->dltListCount,listUsed); rval= -1; goto ready; }

	for ( from= 0; from < dltFrom->dltListCount; from++ )
	    { listUsed[from]= 0; }
	}

    /*****/

    /*  1  */
    if  ( dpFrom->dpColorCount > 1 )
	{
	colorMap= (int *)malloc( dpFrom->dpColorCount* sizeof( int ) );
	if  ( ! colorMap )
	    { LXDEB(dpFrom->dpColorCount,colorMap); rval= -1; goto ready; }

	colorMap[0]= 0;
	}

    /*  a  */
    ru.ruFontsUsed= fontUsed;
    ru.ruListStyleUsed= lsUsed;
    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bdFrom,
				    docMergeScanParaAttributes, (void *)&ru );
    if  ( res != 1 )
	{ LDEB(res); rval= -1; goto ready;	}

    docCountListFontsUsed( bdFrom, lsUsed, listUsed, fontUsed );

    /*****/

    dfFrom= dflFrom->dflFonts;
    for ( from= 0; from < dflFrom->dflFontCount; dfFrom++, from++ )
	{
	DocumentFont *	dfTo;

	if  ( ! fontUsed[from] )
	    { continue;	}

	if  ( dfFrom->dfDocFontNumber < 0 )
	    { LDEB(dfFrom->dfDocFontNumber); continue;	} 
	if  ( ! dfFrom->dfFamilyStyle )
	    { XDEB(dfFrom->dfFamilyStyle); continue;	}
	if  ( ! dfFrom->dfName )
	    { XDEB(dfFrom->dfName); continue;	}

	to= docMergeFontIntoFontlist( dflTo, dfFrom );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	fontMap[from]= to;

	dfTo= dflTo->dflFonts+ to;
	dfTo->dfUsed= 1;
	if  ( utilFindPsFontForDocFont( dfTo, dflTo, psfl ) )
	    { LSDEB(from,dfTo->dfName); rval= -1; goto ready;	}
	}

    /*****/

    /*  1  */
    rgb8From= dpFrom->dpColors+ 1;
    for ( from= 1; from < dpFrom->dpColorCount; rgb8From++, from++ )
	{
	/*  2  */

	to= docAllocateDocumentColor( dpTo, rgb8From );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	colorMap[from]= to;
	}

    /*****/

    if  ( docMergeListTables( bdTo, bdFrom, lsUsed, lsMap, listUsed,
							fontMap, colorMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal */
    *pFontMap= fontMap; fontMap= (int *)0;
    *pColorMap= colorMap; colorMap= (int *)0;
    *pListStyleMap= lsMap; lsMap= (int *)0;

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( fontUsed )
	{ free( fontUsed );	}

    if  ( colorMap )
	{ free( colorMap );	}

    if  ( lsMap )
	{ free( lsMap );	}
    if  ( lsUsed )
	{ free( lsUsed );	}

    if  ( listUsed )
	{ free( listUsed );	}

    return rval;
    }

