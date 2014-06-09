/************************************************************************/
/*									*/
/*  Manipulate lists.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Make a fresh default list level to be inserted in a document.	*/
/*									*/
/************************************************************************/

static int docDefaultListLevel(	DocumentListLevel *		dllTo,
				int				level,
				int				deftab,
				int				listIsHybrid,
				const DocumentListTable *	dlt,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    int			rval= 0;

    unsigned char *	text= (unsigned char *)0;
    unsigned char *	numbers= (unsigned char *)0;

    unsigned char *	t;
    unsigned char *	n;

    int			l;

    docCleanDocumentListLevel( dllTo );
    docInitDocumentListLevel( dllTo );

    /*  1  */
    if  ( listIsHybrid )
	{
	int		found= 1;

	while( found )
	    {
	    int			i;
	    const DocumentList *	dl;
	    int			levelTemplateID= rand();

	    found= 0;

	    dl= dlt->dltLists;
	    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
		{
		int				j;
		const DocumentListLevel *	dll;

		dll= dl->dlLevels;
		for ( j= 0; j < dl->dlLevelCount; dll++, j++ )
		    {
		    if  ( dll->dllTemplateID == levelTemplateID )
			{ found= 1; break;	}
		    }

		if  ( found )
		    { break;	}
		}

	    if  ( ! found )
		{ dllTo->dllTemplateID= levelTemplateID; }
	    }
	}

    text= t= malloc( 1+ 2* level+ 1+ 1 );
    if  ( ! text )
	{ LXDEB(level,text); rval= -1; goto ready;	}

    numbers= n= malloc( level+ 1+ 1 );
    if  ( ! numbers )
	{ LXDEB(level,numbers); rval= -1; goto ready;	}

    *(t++)= 2* level+ 1;

    for ( l= 0; l < level; l++ )
	{
	*(n++)= t- text;
	*(t++)= l; *(t++)= '.';
	}

    *(n++)= t- text; *(n++)= '\0';
    *(t++)= l; *(t++)= '\0'; 

    dllTo->dllTextSize= 1+ text[0];
    dllTo->dllText= text; text= (unsigned char *)0;

    dllTo->dllNumberSize= level+ 1;
    dllTo->dllNumbers= numbers; numbers= (unsigned char *)0;

    dllTo->dllTextAttribute= *taSet;
    dllTo->dllTextAttributeMask= *taSetMask;

    dllTo->dllFirstIndentTwips= -deftab/ 2;
    dllTo->dllLeftIndentTwips= ( level+ 1 )* deftab;
    PROPmaskADD( &(dllTo->dllParaPropertyMask), PPpropFIRST_INDENT );
    PROPmaskADD( &(dllTo->dllParaPropertyMask), PPpropLEFT_INDENT );

  ready:

    if  ( text )
	{ free( text );		}
    if  ( numbers )
	{ free( numbers );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a fresh default list to be inserted in a document.		*/
/*									*/
/*  0)  Restart from scratch.						*/
/*  1)  Make a unique list ID. The procedure is far from optimal, but	*/
/*	the number of lists in the document is small, this does not	*/
/*	really hurt.							*/
/*  2)  Make a unique template ID.					*/
/*  3)  Word does not set list style properties by default. (probably	*/
/*	never)								*/
/*  4)  Word makes anonymous lists.					*/
/*  5)  Word 2000+ makes hybrid lists by default.			*/
/*									*/
/************************************************************************/

static int docDefaultList(	DocumentList *			dlTo,
				const DocumentListTable *	dlt,
				int				deftab,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    int			found;
    int			level;

    /*  0,3,4  */
    docCleanDocumentList( dlTo );
    docInitDocumentList( dlTo );

    /*  1  */
    found= 1;
    while( found )
	{
	int			i;
	const DocumentList *	dl;
	int			listID= rand();

	found= 0;

	dl= dlt->dltLists;
	for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	    {
	    if  ( dl->dlListID == listID )
		{ found= 1; break; }
	    }

	if  ( ! found )
	    { dlTo->dlListID= listID;	}
	}

    /*  2  */
    found= 1;
    while( found )
	{
	int			i;
	const DocumentList *	dl;
	int			templateID= rand();

	found= 0;

	dl= dlt->dltLists;
	for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	    {
	    if  ( dl->dlListTemplateID == templateID )
		{ found= 1; break; }
	    }

	if  ( ! found )
	    { dlTo->dlListTemplateID= templateID;	}
	}

    /*  5  */
    dlTo->dlListHybrid= 1;
    dlTo->dlRestartPerSect= 0;
    dlTo->dlLevelCount= DLmaxLEVELS;

    for ( level= 0; level < DLmaxLEVELS; level++ )
	{
	if  ( docDefaultListLevel( dlTo->dlLevels+ level,
					level, deftab, dlTo->dlListHybrid,
					dlt, taSetMask, taSet ) )
	    { LDEB(level); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a fresh list into the document and a list override that	*/
/*  refers to it.							*/
/*									*/
/************************************************************************/

int docNewList(			int *			pLs,
				BufferDocument *	bd,
				const PropertyMask *	taSetMask,
				const TextAttribute *	taSet )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentListTable *		dlt= &(dp->dpListTable);
    ListOverrideTable *		lot= &(dp->dpListOverrideTable);
    int				ls= lot->lotOverrideCount;

    DocumentList *		dl;
    ListOverride *		lo;

    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;

    if  ( ls == 0 )
	{ ls= 1;	}

    if  ( docClaimListNumberTreeNodes( &(bd->bdListNumberTrees),
				       &(bd->bdListNumberTreeCount),
				       ls+ 1 )	)
	{ LDEB(ls+ 1); return -1;	}

    if  ( docDocumentListTableAddList( dlt,
			    (const DocumentList *)0, fontMap, colorMap ) )
	{ LDEB(dlt->dltListCount); return -1;	}

    dl= dlt->dltLists+ (dlt->dltListCount- 1);

    if  ( docDefaultList( dl, dlt, dp->dpTabIntervalTwips, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    if  ( docListOverrideTableAddOverride( lot,
			    (const ListOverride *)0, ls, fontMap, colorMap ) )
	{ LDEB(lot->lotOverrideCount); return -1;	}

    lo= lot->lotOverrides+ (lot->lotOverrideCount- 1);
    lo->loListID= dl->dlListID;
    lo->loListIndex= dlt->dltListCount- 1;

    *pLs= ls;
    return 0;
    }
