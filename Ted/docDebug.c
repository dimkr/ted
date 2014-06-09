#   include	"tedConfig.h"

#   include	"docBuf.h"
#   include	<stdio.h>
#   include	<appDebugon.h>

#   define	LIST_PARA_STRING	0
#   define	LIST_PARTICULES		1
#   define	LIST_FIRST_LINE		0
#   define	LIST_LINES		0
#   define	LIST_LINE_PARTICULES	0
#   define	CHECK_GEOMETRY		0

static int docCheckGroupLeft(	const BufferItem *	bi )
    {
    int		rval= 0;

    int		prev= 0;
    int		here;

    if  ( bi->biParent && bi->biNumberInParent > 0 )
	{
	prev= bi->biParent->biChildren[bi->biNumberInParent-1]->
						biLeftParagraphs;
	}

    here= bi->biLeftParagraphs;

    if  ( here- prev !=
	    bi->biChildren[bi->biChildCount- 1]->biLeftParagraphs )
	{
	SLDEB(docLevelStr(bi->biLevel),here- prev);
	LDEB(bi->biChildren[bi->biChildCount- 1]->biLeftParagraphs);
	rval= -1;
	}

    return rval;
    }

static int docCheckChild(	const BufferItem *	parent,
				const BufferItem *	child,
				int			i,
				const LayoutPosition *	lpTop,
				LayoutPosition *	lpBelow )
    {
    int		rval= 0;

#   if  CHECK_GEOMETRY
    if  ( lpTop )
	{
	if  ( ! DOC_SAME_POSITION( &(child->biTopPosition), lpTop ) )
	    {
	    appDebug( "############## %s %d in %s:\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel) );
	    LLDEB(child->biTopPosition.lpPage,lpTop->lpPage);
	    LLDEB(child->biTopPosition.lpPageYTwips,lpTop->lpPageYTwips);
	    rval= -1;
	    }
	}
#   endif

    if  ( child->biParent != parent )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	XXDEB(child->biParent,parent);
	rval= -1;
	}

    if  ( child->biNumberInParent != i )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biNumberInParent,i);
	rval= -1;
	}

    if  ( child->biLevel != parent->biLevel+ 1 )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biLevel,parent->biLevel);
	rval= -1;
	}

    if  ( child->biInExternalItem != parent->biInExternalItem )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biInExternalItem,parent->biInExternalItem);
	rval= -1;
	}

    if  ( child->biLevel == DOClevPARA )
	{
	const BufferItem *	rowBi= child;

	while( rowBi && rowBi->biLevel != DOClevROW )
	    { rowBi= rowBi->biParent; }

	if  ( ! rowBi )
	    { SXDEB("#######",rowBi); rval= -1; }
	else{
	    if  ( child->biParaInTable != rowBi->biRowHasTableParagraphs )
		{
		SDEB("#######");
		LLDEB(child->biParaInTable,rowBi->biRowHasTableParagraphs);
		rval= -1;
		}
	    }
	}

    if  ( lpBelow )
	{ *lpBelow= child->biBelowPosition;	}

    return rval;
    }

int docCheckItem(	const BufferItem *	bi )
    {
    int			i;
    int			rval= 0;

    if  ( bi->biLevel == DOClevPARA )
	{
	const BufferItem *	rowBi;
	int			part;
	const TextParticule *	tp;
	int			stroff;

	if  ( bi->biLeftParagraphs != bi->biNumberInParent+ 1 )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
	    LLDEB(bi->biLeftParagraphs,bi->biNumberInParent+1);
	    }

	rowBi= bi;
	while( rowBi && rowBi->biLevel != DOClevROW )
	    { rowBi= rowBi->biParent; }

	if  ( ! rowBi )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
	    XDEB(rowBi);
	    }
	else{
	    if  ( rowBi->biRowHasTableParagraphs != bi->biParaInTable )
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLDEB(rowBi->biRowHasTableParagraphs,bi->biParaInTable);
		}
	    }

	stroff= 0;
	tp= bi->biParaParticules;
	for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	    {
	    if  ( tp->tpStroff > bi->biParaStrlen	||
		  tp->tpStroff != stroff		)
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLLLDEB(part,tp->tpStroff,stroff,bi->biParaStrlen);
		rval= -1;
		}

	    stroff= tp->tpStroff+ tp->tpStrlen;
	    if  ( stroff < 0			||
		  tp->tpStrlen < 0		||
		  stroff >  bi->biParaStrlen	)
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLLLDEB(part,stroff,tp->tpStrlen,bi->biParaStrlen);
		rval= -1;
		}
	    }
	if  ( stroff != bi->biParaStrlen )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
	    LLDEB(stroff,bi->biParaStrlen);
	    rval= -1;
	    }
	}

    if  ( bi->biLevel == DOClevROW )
	{
	if  ( bi->biRowHasTableParagraphs )
	    {
	    if  ( bi->biRowCellCount <= 0 )
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLDEB(bi->biRowHasTableParagraphs,bi->biRowCellCount);
		}
	    if  ( bi->biRowCellCount != bi->biChildCount )
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLDEB(bi->biRowCellCount,bi->biChildCount);
		}
	    }
	else{
	    if  ( bi->biRowCellCount != 0 )
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLDEB(bi->biRowHasTableParagraphs,bi->biRowCellCount);
		}
	    }
	}

    if  ( bi->biChildCount > 0 )
	{
	if  ( docCheckGroupLeft( bi ) )
	    { rval= -1;	}

	if  ( ! bi->biParent )
	    {
	    DocumentPosition	dp;
	    docInitDocumentPosition( &dp );

	    if  ( ! docFirstPosition( &dp, (BufferItem *)bi ) )
		{
		const BufferItem *	paraBi= dp.dpBi;
		int			n= 1;

		for (;;)
		    {
		    if  ( docNumberOfParagraph( paraBi ) != n )
			{ LLDEB(docNumberOfParagraph(paraBi),n); rval= -1; }

		    paraBi= docNextParagraph( (BufferItem *)paraBi );
		    if  ( ! paraBi )
			{ break;	}
		    n++;
		    }
		}
	    }
	}

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	if  ( docCheckChild( bi, bi->biChildren[i], i,
				(LayoutPosition *)0, (LayoutPosition *)0 ) )
	    { rval= -1;	}

	if  ( docCheckItem( bi->biChildren[i] ) )
	    { rval= -1;	}
	}

    return rval;
    }

int docCheckRootItem(	const BufferItem *	bi )
    {
    while( bi->biParent )
	{ bi= bi->biParent;	}

    return docCheckItem( bi );
    }

static void docListChildren(	int			indent,
				const BufferItem *	bi )
    {
    int			i;

    LayoutPosition	lp;

    lp= bi->biTopPosition;

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	docCheckChild( bi, bi->biChildren[i], i, &lp, &lp );

	docListItem( indent+ 4, bi->biChildren[i] );
	}

    if  ( bi->biChildCount > 0 )
	{ docCheckGroupLeft( bi );	}

#   if CHECK_GEOMETRY
    if  ( ! DOC_SAME_POSITION( &(bi->biBelowPosition), &lp ) )
	{
	appDebug( "############## %s :\n", docLevelStr(bi->biLevel) );

	LLDEB(bi->biBelowPosition.lpPage,lp.lpPage);
	LLDEB(bi->biBelowPosition.lpPageYTwips,lp.lpPageYTwips);
	}
#   endif

    }

void docListItem(	int			indent,
			const BufferItem *	bi )
    {
#   if LIST_PARTICULES || LIST_LINES
    int			i;
#   endif

#   if LIST_PARTICULES
    TextParticule *	tp;
#   endif

#   if LIST_LINES
    TextLine *		tl;
#   endif

    appDebug( "%*sITEM %4d 0x%08lx: %s LEFT= %d\n",
			    indent, "", bi->biNumberInParent,
			    (unsigned long)bi,
			    docLevelStr( bi->biLevel ),
			    bi->biLeftParagraphs );
    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevCELL:
	    appDebug( "%*s{ %4d children\n", indent+ 4, "",
						bi->biChildCount );

	    docListChildren( indent, bi );

	    break;

	case DOClevROW:
	    appDebug( "%*s{ %s %4d children %d cells\n", indent+ 4, "",
				bi->biRowHasTableParagraphs?"T:":"--",
				bi->biChildCount,
				bi->biRowCellCount );

	    docListChildren( indent, bi );

	    break;

	case DOClevPARA:
	    appDebug( "%*s{ NR= %d %d particules, %d lines\n", indent+ 4, "",
						docNumberOfParagraph( bi ),
						bi->biParaParticuleCount,
						bi->biParaLineCount );

#	    if LIST_PARA_STRING
	    {
	    int		stroff= 0;

	    while( stroff < bi->biParaStrlen )
		{
		int		l= bi->biParaStrlen- stroff;

		if  ( l > 60 )
		    { l= 60; }

		appDebug( "%*s\"%.*s\"\n",
				    indent+ 4, "",
				    (int)l, (char *)bi->biParaString+ stroff );

		stroff += l;
		}
	    }
#	    endif

#	    if LIST_PARTICULES
	    tp= bi->biParaParticules;
	    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
		{ docListParticule( indent+ 4, "PART", i, bi, tp );	}
#	    endif

#	    if LIST_LINES
	    tl= bi->biParaLines;
	    for ( i= 0; i < bi->biParaLineCount; tl++, i++ )
		{ docListTextLine( indent+ 4, "LINE", i, bi, tl );	}
#	    endif

#	    if LIST_FIRST_LINE
	    docListTextLine( indent+ 4, "LINE", 0, bi, bi->biParaLines );
#	    endif

	    break;
	case DOClevOUT:
	default:
	    break;
	}

    appDebug( "%*s}\n", indent+ 4, "" );
    
    return;
    }

void docListRootItem(	int			indent,
			const BufferItem *	bi )
    {
    while( bi->biParent )
	{ bi= bi->biParent;	}

    docListItem( indent, bi );

    return;
    }

const char * docKindStr( int kind )
    {
    static char	scratch[12];

    switch( kind )
	{
	case DOCkindTEXT:		return "txt";
	case DOCkindTAB:		return "TAB";
	case DOCkindOBJECT:		return "OBJ";
	case DOCkindFIELDSTART:		return "FLS";
	case DOCkindFIELDEND:		return "FLE";
	case DOCkindNOTE:		return "NOT";
	case DOCkindXE:			return "XE ";
	case DOCkindTC:			return "TC ";
	case DOCkindLINEBREAK:		return "LIN";
	case DOCkindPAGEBREAK:		return "PAG";
	case DOCkindCOLUMNBREAK:	return "COL";
	case DOCkindCHFTNSEP:		return "SEP";
	case DOCkindCHFTNSEPC:		return "SEC";

	default:
	    sprintf( scratch, "%3d", kind );
	    return scratch;
	}
    }

const char * docLevelStr( int level )
    {
    static char	scratch[12];

    switch( level )
	{
	case DOClevANY:		return "ANY ";
	case DOClevOUT:		return "OUT ";
	case DOClevDOC:		return "DOC ";
	case DOClevSECT:	return "SECT";
	case DOClevROW:		return "ROW ";
	case DOClevCELL:	return "CELL";
	case DOClevPARA:	return "PARA";
	case DOClevTEXT:	return "TEXT";
	default:
	    sprintf( scratch, "%4d", level );
	    return scratch;
	}
    }

const char * docExternalKindStr( int inExternalItem )
    {
    static char	scratch[12];

    switch( inExternalItem )
	{
	case DOCinBODY:			return "BODY";

	case DOCinSECT_HEADER:		return "SECT_HEADER";
	case DOCinFIRST_HEADER:		return "FIRST_HEADER";
	case DOCinLEFT_HEADER:		return "LEFT_HEADER";
	case DOCinRIGHT_HEADER:		return "RIGHT_HEADER";

	case DOCinSECT_FOOTER:		return "SECT_FOOTER";
	case DOCinFIRST_FOOTER:		return "FIRST_FOOTER";
	case DOCinLEFT_FOOTER:		return "LEFT_FOOTER";
	case DOCinRIGHT_FOOTER:		return "RIGHT_FOOTER";

	case DOCinFOOTNOTE:		return "FOOTNOTE";
	case DOCinENDNOTE:		return "ENDNOTE";

	case DOCinFTNSEP:		return "FTNSEP";
	case DOCinFTNSEPC:		return "FTNSEPC";
	case DOCinFTNCN:		return "FTNCN";

	case DOCinAFTNSEP:		return "AFTNSEP";
	case DOCinAFTNSEPC:		return "AFTNSEPC";
	case DOCinAFTNCN:		return "AFTNCN";

	case DOCinSHPTXT:		return "SHPTXT";

	default:
	    sprintf( scratch, "%d", inExternalItem );
	    return scratch;
	}
    }

const char * docObjectKindStr(	int	objectKind )
    {
    static char	scratch[12];

    switch( objectKind )
	{
	case DOCokUNKNOWN:		return "UNKNOWN";
	case DOCokPICTWMETAFILE:	return "PICTWMETAFILE";
	case DOCokPICTPNGBLIP:		return "PICTPNGBLIP";
	case DOCokPICTJPEGBLIP:		return "PICTJPEGBLIP";
	case DOCokPICTEMFBLIP:		return "PICTEMFBLIP";
	case DOCokMACPICT:		return "MACPICT";
	case DOCokPMMETAFILE:		return "PMMETAFILE";
	case DOCokDIBITMAP:		return "DIBITMAP";
	case DOCokWBITMAP:		return "WBITMAP";
	case DOCokOLEOBJECT:		return "OLEOBJECT";
	case DOCokEPS_FILE:		return "EPS_FILE";
	case DOCokDRAWING_SHAPE:	return "DRAWING_SHAPE";

	default:
	    sprintf( scratch, "%d", objectKind );
	    return scratch;
	}
    }

const char * docFieldKindStr( int kind )
    {
    static char	scratch[12];

    if  ( kind >= 0 && kind < DOC_FieldKindCount )
	{ return DOC_FieldKinds[kind].fkiLabel;	}

    sprintf( scratch, "%4d", kind );
    return scratch;
    }

const char * docAttributeStr(	const TextAttribute *		ta )
    {
    static char	scratch[5];
    char *	to= scratch;

    if  ( ta->taFontIsBold )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}
    if  ( ta->taFontIsSlanted )
	{ *(to++)= 'I';	}
    else{ *(to++)= 'i';	}
    if  ( ta->taTextIsUnderlined )
	{ *(to++)= 'U';	}
    else{ *(to++)= 'u';	}

    *(to)= '\0';

    return scratch;
    }

void docListParticule(	int			indent,
			const char *		label,
			int			n,
			const BufferItem *	bi,
			const TextParticule *	tp )
    {
    appDebug( "%*s%s %3d: [S %4d..%4d] %s",
		    indent, "", label, n,
		    tp->tpStroff, tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ) );

    if  ( tp->tpStrlen > 0 )
	{
	appDebug( " \"%.*s\"",
		    (int)tp->tpStrlen,
		    bi->biParaString+ tp->tpStroff );
	}

    if  ( tp->tpKind == DOCkindOBJECT		||
	  tp->tpKind == DOCkindFIELDSTART	||
	  tp->tpKind == DOCkindFIELDEND		||
	  tp->tpObjectNumber >= 0		)
	{ appDebug( " OBNR=%d", tp->tpObjectNumber );	}

    appDebug( "\n" );

    return;
    }

void docListTextLine(	int			indent,
			const char *		label,
			int			n,
			const BufferItem *	bi,
			const TextLine *	tl )
    {
#   if LIST_LINE_PARTICULES
    TextParticule *	tp;
    int			part;
#   endif

    if  ( ! tl )
	{ LDEB(tl); return;	}

    appDebug( "%*s%s %3d: PG= %3d S:[%4d..%4d] P:[%3d..%3d] ",
	    indent, "", label, n,
	    tl->tlTopPosition.lpPage,
	    tl->tlStroff, tl->tlStroff+ tl->tlStrlen,
	    tl->tlFirstParticule, tl->tlFirstParticule+ tl->tlParticuleCount );

    appDebug( "\"%.*s\"\n",
		    (int)tl->tlStrlen,
		    bi->biParaString+ tl->tlStroff );

#   if LIST_LINE_PARTICULES

    tp= bi->biParaParticules+ tl->tlFirstParticule;
    for ( part= tl->tlFirstParticule;
          part < tl->tlFirstParticule+ tl->tlParticuleCount;
	  tp++, part++ )
	{ docListParticule( indent+ 4, "PART", part, bi, tp ); }

#   endif

    return;
    }

void docLogRectangle(	const char *			label,
			const DocumentRectangle *	dr )
    {
    appDebug( "%s: [%4d+%4d]x[%4d+%4d=%4d]\n", label,
		dr->drX0, dr->drX1- dr->drX0+ 1,
		dr->drY0, dr->drY1- dr->drY0+ 1, dr->drY1 );

    return;
    }

void docLogRectangles(	const char *			label1,
			const DocumentRectangle *	dr1,
			const char *			label2,
			const DocumentRectangle *	dr2 )
    {
    appDebug( "%s: [%4d+%4d]x[%4d+%4d] %s [%4d+%4d]x[%4d+%4d]\n",
		label1,
		dr1->drX0, dr1->drX1- dr1->drX0+ 1,
		dr1->drY0, dr1->drY1- dr1->drY0+ 1,
		label2,
		dr2->drX0, dr2->drX1- dr2->drX0+ 1,
		dr2->drY0, dr2->drY1- dr2->drY0+ 1 );

    return;
    }

void docListNotes(	const BufferDocument *	bd )
    {
    int				i;
    const DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    {
	    appDebug( "%4d DELETED\n", i );
	    continue;
	    }

	appDebug( "%4d %-8s %3d: NR=%3d PG=%3d SECT=%2d PARA=%3d STROFF=%3d\n",
	    i, docExternalKindStr( dn->dnExternalItemKind ),
	    i, dn->dnNoteNumber,
	    dn->dnReferringPage,
	    dn->dnSectNr, dn->dnParaNr, dn->dnStroff );
	}

    return;
    }
