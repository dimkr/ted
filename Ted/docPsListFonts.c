/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*  Manage resources for layout.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<utilMatchFont.h>

#   include	<appImage.h>
#   include	"docLayout.h"
#   include	"docFind.h"
#   include	<sioHex.h>
#   include	<sioMemory.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>

#   include	<appDebugon.h>

int docPsClaimParticuleData(	const BufferItem *	bi,
				ParticuleData **	pParticuleData )
    {
    static ParticuleData *	PSPrintGeometry;
    static int			PSPrintGeometryCount;

    if  ( bi->biParaParticuleCount > PSPrintGeometryCount )
	{
	ParticuleData *	fresh;
	unsigned int	size;

	size= ( bi->biParaParticuleCount+ 1 )* sizeof( ParticuleData );

	fresh= (ParticuleData *)realloc( PSPrintGeometry,
			bi->biParaParticuleCount* sizeof( ParticuleData ) );
	if  ( ! fresh )
	    { LXDEB(bi->biParaParticuleCount,fresh); return -1;	}

	PSPrintGeometry= fresh;
	}

    *pParticuleData= PSPrintGeometry; return 0;
    }

/************************************************************************/
/*									*/
/*  Find the fonts of an image.						*/
/*									*/
/************************************************************************/

typedef int (*LIST_FONTS_PS)(	PostScriptTypeList *	pstl,
				SimpleInputStream *	sis,
				const PostScriptFontList * psfl,
				const char *		prefix,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			twipsWide,
				int			twipsHigh );

int docPsListImageFonts(	PostScriptTypeList *		pstl,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const PostScriptFontList *	psfl,
				const char *			prefix )
    {
    LIST_FONTS_PS		listFontsPs;

    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisMeta;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	    listFontsPs= appMetaListFontsPs;
	    break;

	case DOCokMACPICT:
	    listFontsPs= appMacPictListFontsPs;
	    break;

	default:
	    LDEB(pip->pipType); return 0;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); return -1;	}

    if  ( (*listFontsPs)( pstl, sisMeta, psfl, prefix, pip->pipMapMode,
				    pip->pip_xWinExt, pip->pip_yWinExt,
				    pip->pipTwipsWide, pip->pipTwipsHigh ) )
	{ LDEB(1); return -1;	}

    sioInClose( sisMeta );
    sioInClose( sisMem );

    return 0;
    }

static int docPsListObjectFonts( PostScriptTypeList *		pstl,
				const InsertedObject *		io,
				const PostScriptFontList *	psfl,
				const char *			prefix )
    {
    switch( io->ioKind )
	{
	case DOCokMACPICT:
	case DOCokPICTWMETAFILE:
	    return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioObjectData), psfl, prefix );
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioResultData), psfl, prefix );
		}
	    else{ LDEB(io->ioResultKind); return 0;	}

	case DOCokEPS_FILE:
	    LDEB(io->ioKind);
	    return 0;

	case DOCokDRAWING_SHAPE:
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of PostScriptFonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetDocumentFonts
    {
    const DocumentFontList *		gdfDocumentFontList;
    const PostScriptFontList *		gdfPostScriptFontList;
    PostScriptTypeList *		gdfPostScriptTypeList;
    } GetDocumentFonts;

static int docPsPrintGetParaFonts(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

static int docPsPrintGetShapeFonts(
				const DrawingShape *		ds,
				const BufferDocument *		bd,
				void *				through )
    {
    int		rval= 0;
    int		i;

    if  ( ds->dsExternalItem.eiItem )
	{
	DocumentPosition	dp;

	if  ( ! docFirstPosition( &dp, ds->dsExternalItem.eiItem ) )
	    {
	    DocumentSelection	dsIgnored;

	    docInitDocumentSelection( &dsIgnored );

	    if  ( docFindFindNextInTree( &dsIgnored, bd, &dp,
				    docPsPrintGetParaFonts, through ) != 1 )
		{ LDEB(1); rval= -1;	}
	    }
	}

    for ( i= 0; i < ds->dsChildCount; i++ )
	{
	if  ( docPsPrintGetShapeFonts( ds->dsChildren[i], bd, through ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

/*  a  */
static int docPsPrintGetParaFonts(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const DocumentFontList *	dfl= gdf->gdfDocumentFontList;
    const PostScriptFontList *	psfl= gdf->gdfPostScriptFontList;
    PostScriptTypeList *	pstl= gdf->gdfPostScriptTypeList;

    int				attributeNumber= -1;

    AfmFontInfo *		afi;
    int				encoding;

    InsertedObject *		io;

    const TextParticule *	tp;
    int				part;

    tp= bi->biParaParticules; part= 0;

    while( part < bi->biParaParticuleCount )
	{
	const int	appearsInText= 1;

	switch( tp->tpKind )
	    {
	    case DOCkindTEXT:
	    case DOCkindTAB:
		if  ( tp->tpTextAttributeNumber >= 0			&&
		      tp->tpTextAttributeNumber != attributeNumber	)
		    {
		    TextAttribute	ta;

		    utilGetTextAttributeByNumber( &ta,
					    &(bd->bdTextAttributeList),
					    tp->tpTextAttributeNumber );

		    afi= utilPsGetAfi( &encoding, dfl, psfl, &ta );
		    if  ( ! afi )
			{ XDEB(afi); return -1;	}

		    if  ( utilRememberPostsciptFace( pstl,
				    encoding, afi, &ta, "f", appearsInText ) )
			{ LDEB(1); return -1;	}

		    attributeNumber= tp->tpTextAttributeNumber;
		    }

		while( part+ 1 < bi->biParaParticuleCount )
		    {
		    if  ( tp[1].tpKind != DOCkindTEXT	&&
			  tp[1].tpKind != DOCkindTAB	)
			{ break;	}
		    if  ( tp[1].tpTextAttributeNumber != attributeNumber )
			{ break;	}

		    part++; tp++;
		    }
		part++; tp++; break;

	    case DOCkindOBJECT:
		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  ( docPsListObjectFonts( pstl, io, psfl, "f" ) )
		    { LDEB(tp->tpKind); return -1;	}

		if  ( io->ioKind == DOCokDRAWING_SHAPE			&&
		      io->ioDrawingShape				&&
		      docPsPrintGetShapeFonts( io->ioDrawingShape,
							bd, through )	)
		    { LDEB(1);	}

		part++; tp++; break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
		part++; tp++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }
	}

    return 1;
    }


int docPsPrintGetDocumentFonts(	BufferDocument *		bd,
				PostScriptTypeList *		pstl,
				const PostScriptFontList *	psfl )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);

    GetDocumentFonts		gdf;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    gdf.gdfDocumentFontList= dfl;
    gdf.gdfPostScriptFontList= psfl;
    gdf.gdfPostScriptTypeList= pstl;

    /*  a  */
    if  ( docFirstPosition( &dpBeginFrom, &(bd->bdItem) ) )
	{ LDEB(1); return -1;;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bd,
				    docPsPrintGetParaFonts, (void *)&gdf );
    if  ( res != 1 )
	{ LDEB(res); return -1;	}

    return 0;
    }

