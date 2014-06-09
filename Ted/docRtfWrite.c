/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	<charnames.h>
#   include	<psFont.h>
#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Forward declaration..						*/
/*									*/
/************************************************************************/

static int docRtfSaveItem(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				const DocumentSelection *	ds,
				RtfWritingContext *		rwc );

/************************************************************************/
/*									*/
/*  Save a paragraph in RTF format.					*/
/*									*/
/*  Column counting is approximate as it is only for cosmetic reasons.	*/
/*									*/
/************************************************************************/

static void docRtfParaSaveProperties( SimpleOutputStream *	sos,
				int				fromDefault,
				int				saveIntbl,
				int *				pPropertyChange,
				int *				pTabsSaved,
				int *				pCol,
				const ParagraphProperties *	newPP,
				const ParagraphProperties *	prevPP )
    {
    ParagraphProperties		ppp;
    PropertyMask		updMask;

    PROPmaskCLEAR( &updMask );

    if  ( ! fromDefault )
	{
	PROPmaskCLEAR( &updMask );
	utilPropMaskFill( &updMask, PPprop_COUNT );
	PROPmaskUNSET( &updMask, PPpropIN_TABLE );

	docParaPropertyDifference( &updMask, newPP, prevPP, &updMask );

	if  ( utilPropMaskIsEmpty( &updMask ) )
	    { return;	}
	}

    docInitParagraphProperties( &ppp );

    /*  1  */
    docRtfWriteTag( "\\pard", pCol, sos );
    *pPropertyChange= 1;

    if  ( saveIntbl && newPP->ppInTable )
	{ docRtfWriteTag( "\\intbl", pCol, sos );	}

    PROPmaskCLEAR( &updMask );
    utilPropMaskFill( &updMask, PPprop_COUNT );
    PROPmaskUNSET( &updMask, PPpropIN_TABLE );

    docParaPropertyDifference( &updMask, newPP, &ppp, &updMask );

    docCleanParagraphProperties( &ppp );

    docRtfSaveParagraphProperties( sos, pCol, &updMask, newPP );

    if  ( PROPmaskISSET( &updMask, PPpropTAB_STOPS )	&&
	  newPP->ppTabStopList.tslTabStopCount > 0	)
	{ *pTabsSaved= 1;	}

    return;
    }

static int docRtfSaveObject(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferItem *		bi,
				const TextParticule *		tp )
    {
    InsertedObject *	io= bi->biParaObjects+ tp->tpObjectNumber;
    PictureProperties *	pip= &(io->ioPictureProperties);

    docRtfWriteNextLine( pCol, sos );

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    docRtfWriteDestinationBegin( "\\pict", pCol, sos );
	    docRtfWriteArgTag( "\\wmetafile", pCol, pip->pipMapMode, sos );

	    if  ( pip->pipMetafileIsBitmap )
		{
		docRtfWriteTag( "\\picbmp", pCol, sos );

		if  ( pip->pipMetafileBitmapBpp > 0 )
		    {
		    docRtfWriteArgTag( "\\picbpp",
					pCol, pip->pipMetafileBitmapBpp, sos );
		    }
		}

	    docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

	    docRtfWriteMemoryBuffer( &(io->ioObjectData), pCol, sos );
	    return 0;

	case DOCokMACPICT:
	    docRtfWriteDestinationBegin( "\\pict", pCol, sos );
	    docRtfWriteTag( "\\macpict", pCol, sos );

	    docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

	    docRtfWriteMemoryBuffer( &(io->ioObjectData), pCol, sos );

	    return 0;

	case DOCokPICTPNGBLIP:
	    docRtfWriteDestinationBegin( "\\pict", pCol, sos );
	    docRtfWriteTag( "\\pngblip", pCol, sos );

	    docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

	    docRtfWriteMemoryBuffer( &(io->ioObjectData), pCol, sos );
	    return 0;

	case DOCokPICTJPEGBLIP:
	    docRtfWriteDestinationBegin( "\\pict", pCol, sos );
	    docRtfWriteTag( "\\jpegblip", pCol, sos );

	    docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

	    docRtfWriteMemoryBuffer( &(io->ioObjectData), pCol, sos );
	    return 0;

	case DOCokPICTEMFBLIP:
	    docRtfWriteDestinationBegin( "\\pict", pCol, sos );
	    docRtfWriteTag( "\\emfblip", pCol, sos );

	    docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

	    docRtfWriteMemoryBuffer( &(io->ioObjectData), pCol, sos );
	    return 0;

	case DOCokOLEOBJECT:
	    docRtfWriteDestinationBegin( "\\object", pCol, sos );


	    switch( io->ioRtfEmbedKind )
		{
		case EMBEDkindOBJEMB:
		    docRtfWriteTag( "\\objemb", pCol, sos );
		    break;
		case EMBEDkindOBJLINK:
		    docRtfWriteTag( "\\objlink", pCol, sos );
		    break;
		case EMBEDkindOBJAUTLINK:
		    docRtfWriteTag( "\\objautlink", pCol, sos );
		    break;
		case EMBEDkindOBJSUB:
		    docRtfWriteTag( "\\objsub", pCol, sos );
		    break;
		case EMBEDkindOBJPUB:
		    docRtfWriteTag( "\\objpub", pCol, sos );
		    break;
		case EMBEDkindOBJICEMB:
		    docRtfWriteTag( "\\objicemb", pCol, sos );
		    break;
		case EMBEDkindOBJHTML:
		    docRtfWriteTag( "\\objhtml", pCol, sos );
		    break;
		case EMBEDkindOBJOCX:
		    docRtfWriteTag( "\\objocx", pCol, sos );
		    break;
		default:
		    LDEB(io->ioRtfEmbedKind);
		    break;
		}

	    switch( io->ioRtfResultKind )
		{
		case RESULTkindUNKNOWN:
		    break;
		case RESULTkindRTF:
		    docRtfWriteTag( "\\rsltrtf", pCol, sos );
		    break;
		case RESULTkindTXT:
		    docRtfWriteTag( "\\rslttxt", pCol, sos );
		    break;
		case RESULTkindPICT:
		    docRtfWriteTag( "\\rsltpict", pCol, sos );
		    break;
		case RESULTkindBMP:
		    docRtfWriteTag( "\\rsltbmp", pCol, sos );
		    break;
		case RESULTkindHTML:
		    docRtfWriteTag( "\\rslthtml", pCol, sos );
		    break;
		default:
		    LDEB(io->ioRtfResultKind);
		    break;
		}

	    if  ( io->ioObjectClass )
		{
		docRtfWriteDestinationBegin( "\\*\\objclass ", pCol, sos );
		sioOutPutString( (char *)io->ioObjectClass, sos );
		docRtfWriteDestinationEnd( pCol, sos );
		}

	    if  ( io->ioObjectName )
		{
		docRtfWriteDestinationBegin( "\\*\\objname ", pCol, sos );
		sioOutPutString( (char *)io->ioObjectName, sos );
		docRtfWriteDestinationEnd( pCol, sos );
		}

	    docRtfWriteArgTag( "\\objw", pCol, io->ioTwipsWide, sos );
	    docRtfWriteArgTag( "\\objh", pCol, io->ioTwipsHigh, sos );
	    if  ( io->ioScaleXSet != 100 )
		{
		docRtfWriteArgTag( "\\objscalex", pCol,
						    io->ioScaleXSet, sos );
		}
	    if  ( io->ioScaleYSet != 100 )
		{
		docRtfWriteArgTag( "\\objscaley", pCol,
						    io->ioScaleYSet, sos );
		}

	    docRtfWriteDestinationBegin( "\\*\\objdata ", pCol, sos );
	    docRtfWriteMemoryBuffer( &io->ioObjectData, pCol, sos );

	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		docRtfWriteDestinationBegin(
			    "\\result {\\pict", pCol, sos );

		docRtfWriteArgTag( "\\wmetafile",
					    pCol, pip->pipMapMode, sos );

		docRtfSavePictureTags( &(io->ioPictureProperties), pCol, sos );

		docRtfWriteMemoryBuffer( &io->ioResultData, pCol, sos );

		docRtfWriteDestinationEnd( pCol, sos );
		}

	    docRtfWriteDestinationEnd( pCol, sos );
	    return 0;

	case DOCokEPS_FILE:
	    return 0;

	case DOCokDRAWING_SHAPE:
	    LSDEB(io->ioKind,docObjectKindStr(io->ioKind));
	    return 0;

	default:
	    LSDEB(io->ioKind,docObjectKindStr(io->ioKind));
	    return -1;
	}

    /*  Not reached ..
    return 0;
    */
    }

/************************************************************************/
/*									*/
/*  Reserve a number of columns in the output file.			*/
/*									*/
/************************************************************************/

static void docRtfReserveColumns(	SimpleOutputStream *	sos,
					int			cols,
					int *			pCol,
					RtfWritingContext *	rwc )
    {
    int				col= *pCol;

    if  ( *pCol > 0 && *pCol+ cols > 72 )
	{
	docRtfWriteNextLine( pCol, sos );
	rwc->rwcHasPrecedingTags= 0;
	}

    *pCol= col; return;
    }

/************************************************************************/
/*									*/
/*  Finish/Begin writing a hyperlink.					*/
/*									*/
/************************************************************************/

static int docRtfPopAttribute(		RtfWritingContext *	rwc )
    {
    PushedAttribute *	pa;

    pa= rwc->rwcOutsideFldrsltAttribute;
    if  ( ! pa )
	{ XDEB(pa); return -1;	}

    rwc->rwcTextAttribute= pa->paTextAttribute;
    rwc->rwcOutsideFldrsltAttribute= pa->paNext;

    free( pa );

    return 0;
    }

static void docRtfFinishFldrslt(	SimpleOutputStream *	sos,
					int *			pCol,
					RtfWritingContext *	rwc )
    {
    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteDestinationEnd( pCol, sos );

    rwc->rwcHasPrecedingTags= 0;
    rwc->rwcInFldrslt--;

    if  ( docRtfPopAttribute( rwc ) )
	{ LDEB(1);	}

    return;
    }

static int docRtfPushAttribute(		RtfWritingContext *	rwc )
    {
    PushedAttribute *		pa;

    pa= malloc( sizeof( PushedAttribute ) );
    if  ( ! pa )
	{ XDEB(pa); return- 1;	}
    pa->paNext= rwc->rwcOutsideFldrsltAttribute;
    rwc->rwcOutsideFldrsltAttribute= pa;
    pa->paTextAttribute= rwc->rwcTextAttribute;

    return 0;
    }

static void docRtfWriteStartField(	SimpleOutputStream *	sos,
					const DocumentField *	df,
					int *			pCol,
					RtfWritingContext *	rwc )
    {
    const unsigned char *	bytes= df->dfInstructions.mbBytes;
    int				byteCount= df->dfInstructions.mbSize;

    docRtfWriteDestinationBegin( "\\field", pCol, sos );
    docRtfWriteDestinationBegin( "\\*\\fldinst", pCol, sos );
    docRtfWriteDestinationBegin( "", pCol, sos );

    while( byteCount > 1 && bytes[byteCount- 1] == ' ' )
	{ byteCount--;	}

    docRtfReserveColumns( sos, byteCount, pCol, rwc );

    docRtfEscapeString( bytes, (const unsigned char *)0, pCol, byteCount, sos );

    sioOutPutCharacter( ' ', sos ); *pCol += 1;
    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteDestinationEnd( pCol, sos );

    docRtfWriteDestinationBegin( "\\fldrslt", pCol, sos );
    docRtfWriteDestinationBegin( "", pCol, sos );

    rwc->rwcHasPrecedingTags= 0;
    rwc->rwcInFldrslt++;

    if  ( docRtfPushAttribute( rwc ) )
	{ LDEB(1);	}

    return;
    }

static void docRtfStartBookmark(	SimpleOutputStream *	sos,
					const DocumentField *	df,
					int *			pCol,
					RtfWritingContext *	rwc )
    {
    const char *		markName= (char *)0;
    int				markSize;

    if  ( ! docFieldGetBookmark( df, &markName, &markSize ) )
	{
	docRtfWriteDestinationBegin( "\\*\\bkmkstart ", pCol, sos );

	docRtfEscapeString( (const unsigned char *)markName, 
						    (const unsigned char *)0,
						    pCol, markSize, sos );

	docRtfWriteDestinationEnd( pCol, sos );
	rwc->rwcHasPrecedingTags= 0;
	}

    return;
    }

static void docRtfFinishBookmark(	SimpleOutputStream *	sos,
					const DocumentField *	df,
					int *			pCol,
					RtfWritingContext *	rwc )
    {
    const char *		markName= (char *)0;
    int				markSize;

    if  ( ! docFieldGetBookmark( df, &markName, &markSize ) )
	{
	docRtfWriteDestinationBegin( "\\*\\bkmkend ", pCol, sos );

	docRtfEscapeString( (const unsigned char *)markName, 
						    (const unsigned char *)0,
						    pCol, markSize, sos );

	docRtfWriteDestinationEnd( pCol, sos );
	rwc->rwcHasPrecedingTags= 0;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Enter/Leave a table.						*/
/*									*/
/************************************************************************/

static int docRtfPushTable(	RtfWritingContext *		rwc,
				int *				pCol,
				SimpleOutputStream *		sos,
				const BufferItem *		rowBi,
				const DocumentSelection *	ds )
    {
    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    int			col0= -1;
    int			col1= -1;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    docRtfWriteNextLine( pCol, sos );
    docRtfWriteDestinationBegin( "", pCol, sos );

    rwc->rwcInTable= 1;

    rwc->rwcOutsideTableTextAttribute= rwc->rwcTextAttribute;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask,
			    &(rwc->rwcOutsideTableParagraphProperties),
			    &ppUpdMask, &(rwc->rwcParagraphProperties),
			    colorMap, listStyleMap ) )
	{ LDEB(1); return -1;	}

    if  ( ds )
	{ col0= ds->dsCol0; col1= ds->dsCol1; 	}

    docRtfSaveRowProperties( &(rowBi->biRowProperties), pCol, col0, col1, sos );

    if  ( docCopyRowProperties( &(rwc->rwcRowProperties),
				&(rowBi->biRowProperties), (const int *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRtfPopTable(	RtfWritingContext *	rwc,
				int *			pCol,
				SimpleOutputStream *	sos )
    {
    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    rwc->rwcInTable= 0;

    docCleanRowProperties( &(rwc->rwcRowProperties) );
    docInitRowProperties( &(rwc->rwcRowProperties) );

    rwc->rwcTextAttribute= rwc->rwcOutsideTableTextAttribute;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask,
			    &(rwc->rwcParagraphProperties), &ppUpdMask,
			    &(rwc->rwcOutsideTableParagraphProperties),
			    colorMap, listStyleMap ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save an inidividual header, footer, or footnote.			*/
/*									*/
/************************************************************************/

int docRtfSaveExternalItem(		SimpleOutputStream *	sos,
					int *			pCol,
					const BufferItem *	bi,
					const BufferDocument *	bd,
					const char *		tag,
					RtfWritingContext *	rwc )
    {
    int			i;

    int			savedInTable= rwc->rwcInTable;
    TextAttribute	taOutside;

    if  ( ! bi )
	{ return 0;	}

    if  ( bi->biLevel != DOClevSECT )
	{ SDEB(docLevelStr(bi->biLevel)); return -1;	}

    taOutside= rwc->rwcTextAttribute;

    docRtfWriteDestinationBegin( tag, pCol, sos );

    docCleanParagraphProperties( &(rwc->rwcParagraphProperties) );
    docInitParagraphProperties( &(rwc->rwcParagraphProperties) );
    utilInitTextAttribute( &(rwc->rwcTextAttribute) );

    rwc->rwcInTable= 0;

    docRtfWriteTag( "\\pard", pCol, sos );
    docRtfWriteTag( "\\plain", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	if  ( docRtfSaveItem( sos, pCol, bi->biChildren[i], bd,
						(DocumentSelection *)0, rwc ) )
	    { LDEB(i); return -1;	}
	}

    docCleanParagraphProperties( &(rwc->rwcParagraphProperties) );
    docInitParagraphProperties( &(rwc->rwcParagraphProperties) );
    utilInitTextAttribute( &(rwc->rwcTextAttribute) );

    while( rwc->rwcInFldrslt > 0 )
	{ docRtfFinishFldrslt( sos, pCol, rwc );	}

    if  ( rwc->rwcInTable && docRtfPopTable( rwc, pCol, sos ) )
	{ LDEB(1);	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    rwc->rwcTextAttribute= taOutside;
    rwc->rwcInTable= savedInTable;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a paragraph.							*/
/*									*/
/*  1)  When we are saving a selection, and the selection is inside a	*/
/*	table cell, do not set the \intbl flag.				*/
/*  2)  To make WP 8 happy, always save 'intbl' for the first paragraph	*/
/*	in a table row.							*/
/*									*/
/************************************************************************/

static void docRtfSwitchTextAttributes(	SimpleOutputStream *	sos,
					int *			pCol,
					const BufferDocument *	bd,
					const TextParticule *	tp,
					RtfWritingContext *	rwc )
    {
    TextAttribute		ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    if  ( ta.taFontNumber >= 0 )
	{
	PropertyMask	chgMask;
	PropertyMask	updMask;

	PROPmaskCLEAR( &chgMask );
	utilPropMaskFill( &chgMask, TAprop_COUNT );

	PROPmaskCLEAR( &updMask );

	utilUpdateTextAttribute( &updMask, &(rwc->rwcTextAttribute),
							    &ta, &chgMask );
	if  ( ! utilPropMaskIsEmpty( &updMask ) )
	    {
	    const DocumentProperties *		dp= &(bd->bdProperties);
	    const DocumentFontList *		dfl= &(dp->dpFontList);
	    const DocumentFont *		df;
	    const OfficeCharsetMapping *	ocm;

	    docRtfSaveTextAttribute( sos, pCol, &updMask, &ta );
	    rwc->rwcHasPrecedingTags= 1;

	    rwc->rwcTextOutputMapping= rwc->rwcDefaultOutputMapping;

	    df= dfl->dflFonts+ ta.taFontNumber;
	    ocm= df->dfOfficeCharsetMapping;

	    if  ( ! ocm )
		{
		/* SXDEB(df->dfName,ocm); */
		rwc->rwcTextOutputMapping= (const unsigned char *)0;
		}
	    else{
		if  ( ocm->ocmFromX11ToOffice )
		    { rwc->rwcTextOutputMapping= ocm->ocmFromX11ToOffice; }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save a paragraph to rtf.						*/
/*									*/
/*  8)  MS-Word has a tendency to only pick up certain paragraph	*/
/*	properties, such as the tabs, at the paragraph mark. On the	*/
/*	other do superfluous paragraph marks result in empty paragraphs.*/
/*	Try to find a compromise: The \cell or \row count as a paragraph*/
/*	mark, so no need to save a paragraph mark in a table. This	*/
/*	construct was necessary to activate tabs in headers and footers.*/
/*									*/
/************************************************************************/

static int docRtfSaveParaItem(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferDocument *		bd,
				const BufferItem *		bi,
				const DocumentSelection *	ds,
				RtfWritingContext *		rwc )
    {
    TextParticule *			tp;
    unsigned char *			s;

    int					stroffUpto;
    int					part= 0;
    int					stroff= 0;

    int					saveIntbl;
    int					restartFromDefault= 0;
    int					propChange= 0;
    int					tabsSaved= 0;

    const DocumentField *		df;
    const FieldKindInformation *	fki;

    PropertyMask			ppChgMask;
    PropertyMask			ppUpdMask;

    const int * const			colorMap= (const int *)0;
    const int * const			listStyleMap= (const int *)0;

    saveIntbl= ! ds || ! docSelectionInsideCell( ds );

    if  ( saveIntbl )
	{
	/*  2  */
	if  ( bi->biParaInTable				&&
	      bi->biNumberInParent == 0			&&
	      bi->biParent				&&
	      bi->biParent->biNumberInParent == 0	)
	    { restartFromDefault= 1; }
	}

    docRtfParaSaveProperties( sos, restartFromDefault, saveIntbl,
					&propChange, &tabsSaved, pCol,
					&(bi->biParaProperties),
					&(rwc->rwcParagraphProperties) );

    if  ( propChange )
	{ rwc->rwcHasPrecedingTags= 1;	}

    if  ( restartFromDefault || propChange )
	{
	/*  1  */
	utilInitTextAttribute( &(rwc->rwcTextAttribute) );
	rwc->rwcTextAttribute.taFontSizeHalfPoints= 24;

	/*  2  */
	docRtfWriteTag( "\\plain", pCol, sos );
	rwc->rwcHasPrecedingTags= 1;
	}

    if  ( bi->biParaParticuleCount == 0 )
	{ LDEB(bi->biParaParticuleCount);	}

    if  ( ds && ds->dsBegin.dpBi == bi )
	{
	const int	lastOne= 1;

	stroff= ds->dsBegin.dpStroff;

	if  ( docFindParticuleOfPosition( &part, &(ds->dsBegin), lastOne ) )
	    { LDEB(stroff); return -1;	}
	}

    tp= bi->biParaParticules+ part;
    s= bi->biParaString+ stroff;

    if  ( ds && ds->dsEnd.dpBi == bi )
	{ stroffUpto= ds->dsEnd.dpStroff;	}
    else{ stroffUpto= -1;			}

    for ( ; part < bi->biParaParticuleCount; part++, tp++ )
	{
	if  ( tp->tpKind == DOCkindTEXT		||
	      tp->tpStrlen > 0			)
	    { docRtfSwitchTextAttributes( sos, pCol, bd, tp, rwc ); }

	if  ( rwc->rwcSaveAsLink )
	    {
	    docRtfWriteStartField( sos, &(rwc->rwcSaveAsHyperlinkField),
								pCol, rwc );

	    rwc->rwcSaveAsLink= 0;

	    if  ( rwc->rwcSaveAsLinkAsRef )
		{
		docRtfWriteStartField( sos, &(rwc->rwcSaveAsRefField),
								pCol, rwc );
		}
	    else{
		if  ( rwc->rwcSaveAsLinkAsPageref )
		    {
		    docRtfWriteStartField( sos,
				    &(rwc->rwcSaveAsPagerefField), pCol, rwc );
		    }
		}
	    }

	switch( tp->tpKind )
	    {
	    int		n;

	    case DOCkindCHFTNSEP:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\chftnsep", pCol, sos ); s++; stroff++;
		rwc->rwcHasPrecedingTags= 1;
		continue;

	    case DOCkindCHFTNSEPC:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\chftnsepc", pCol, sos ); s++; stroff++;
		rwc->rwcHasPrecedingTags= 1;
		continue;

	    case DOCkindTAB:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\tab", pCol, sos ); s++; stroff++;
		rwc->rwcHasPrecedingTags= 1;
		continue;

	    case DOCkindLINEBREAK:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\line", pCol, sos );
		docRtfWriteNextLine( pCol, sos );
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		rwc->rwcHasPrecedingTags= 0;
		continue;

	    case DOCkindPAGEBREAK:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\page", pCol, sos );
		docRtfWriteNextLine( pCol, sos );
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		rwc->rwcHasPrecedingTags= 0;
		continue;

	    case DOCkindCOLUMNBREAK:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		docRtfWriteTag( "\\column", pCol, sos );
		docRtfWriteNextLine( pCol, sos );
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		rwc->rwcHasPrecedingTags= 0;
		continue;

	    case DOCkindTEXT:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		if  ( stroffUpto >= 0				&&
		      tp->tpStroff+ tp->tpStrlen > stroffUpto	)
		    { n= stroffUpto- stroff;			}
		else{ n= tp->tpStroff+ tp->tpStrlen- stroff;	}

		if  ( *pCol > 0					&&
		      *pCol+ n > 72				&&
		      n < 72					)
		    { docRtfWriteNextLine( pCol, sos ); }

		if  ( rwc->rwcHasPrecedingTags )
		    {
		    if  ( *pCol > 0 )
			{ sioOutPutString( " ", sos ); *pCol += 1;	}
		    rwc->rwcHasPrecedingTags= 0;
		    }

		docRtfEscapeString( s, rwc->rwcTextOutputMapping, pCol, n, sos );
		stroff += n; s += n;

		continue;
	    case DOCkindOBJECT:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		if  ( docRtfSaveObject( sos, pCol, bi, tp ) )
		    { LDEB(tp->tpKind); }
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		rwc->rwcHasPrecedingTags= 0;
		continue;

	    case DOCkindNOTE:
		{
		DocumentNote *	dn;

		if  ( docGetNote( &dn, bd, bi, tp->tpStroff ) >= 0	&&
		      dn->dnExternalItem.eiItem				)
		    {
		    switch( dn->dnExternalItemKind )
			{
			case DOCinFOOTNOTE:
			    if  ( docRtfSaveExternalItem( sos, pCol,
					dn->dnExternalItem.eiItem,
					bd, "\\footnote", rwc )	)
				{ LDEB(tp->tpKind); return -1;	}
			    break;

			case DOCinENDNOTE:
			    if  ( docRtfSaveExternalItem( sos, pCol,
					dn->dnExternalItem.eiItem,
					bd, "\\footnote\\ftnalt", rwc )	)
				{ LDEB(tp->tpKind); return -1;	}
			    break;

			default:
			    LDEB(dn->dnExternalItemKind);
			    break;
			}
		    }
		else{ LDEB(tp->tpObjectNumber);	}

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		}
		continue;

	    case DOCkindFIELDSTART:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    {
		    docRtfWriteStartField( sos, df, pCol, rwc );
		    rwc->rwcHasPrecedingTags= 0;
		    }

		if  ( df->dfKind == DOCfkBOOKMARK	&&
		      rwc->rwcSaveBookmarks		)
		    {
		    docRtfStartBookmark( sos, df, pCol, rwc );
		    rwc->rwcHasPrecedingTags= 0;
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;

		    count= docCountParticulesInField( bi, part,
						bi->biParaParticuleCount );

		    if  ( count > 0 && tp[1].tpKind == DOCkindTEXT )
			{
			docRtfSwitchTextAttributes( sos, pCol, bd, tp+ 1, rwc );
			}
		    else{
			docRtfSwitchTextAttributes( sos, pCol, bd, tp, rwc );
			}

		    docRtfWriteTag( "\\chftn", pCol, sos );
		    rwc->rwcHasPrecedingTags= 1;

		    part += count+ 1;
		    tp += count+ 1;
		    stroff= tp->tpStroff;
		    s= bi->biParaString+ stroff;

		    continue;
		    }

		if  ( df->dfKind == DOCfkLISTTEXT	&&
		      bi->biParaListOverride > 0	)
		    {
		    if  ( docRtfPushAttribute( rwc ) )
			{ LDEB(1);	}

		    docRtfWriteDestinationBegin( "\\listtext", pCol, sos );
		    rwc->rwcHasPrecedingTags= 1;
		    }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		continue;

	    case DOCkindFIELDEND:
		if  ( tp->tpObjectNumber < 0 )
		    { LDEB(tp->tpObjectNumber); }
		else{
		    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		    fki= DOC_FieldKinds+ df->dfKind;

		    if  ( df->dfKind == DOCfkBOOKMARK	&&
			  rwc->rwcSaveBookmarks		)
			{
			docRtfFinishBookmark( sos, df, pCol, rwc );
			rwc->rwcHasPrecedingTags= 0;
			}

		    if  ( fki->fkiIsFieldInRtf		&&
			  fki->fkiLevel == DOClevTEXT	)
			{
			if  ( rwc->rwcInFldrslt )
			    { docRtfFinishFldrslt( sos, pCol, rwc );	}
			}

		    if  ( df->dfKind == DOCfkCHFTN )
			{ /* nothing */	}

		    if  ( df->dfKind == DOCfkLISTTEXT	&&
			  bi->biParaListOverride > 0	)
			{
			docRtfWriteDestinationEnd( pCol, sos );
			rwc->rwcHasPrecedingTags= 0;

			if  ( docRtfPopAttribute( rwc ) )
			    { LDEB(1);	}
			}
		    }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		rwc->rwcHasPrecedingTags= 0;
		continue;

	    case DOCkindXE:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		if  (  rwc->rwcSaveBookmarks )
		    {
		    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

		    sioOutPutString( "{\\xe {", sos ); *pCol += 5;
		    docRtfEscapeString( df->dfInstructions.mbBytes,
				    (const unsigned char *)0,
				    pCol, df->dfInstructions.mbSize, sos );
		    sioOutPutString( "}}", sos ); *pCol += 2;

		    rwc->rwcHasPrecedingTags= 0;
		    }
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		continue;

	    case DOCkindTC:
		if  ( stroffUpto >= 0 && stroff >= stroffUpto )
		    { break;	}

		if  (  rwc->rwcSaveBookmarks )
		    {
		    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

		    sioOutPutString( "{\\tc {", sos ); *pCol += 5;
		    docRtfEscapeString( df->dfInstructions.mbBytes,
				    (const unsigned char *)0,
				    pCol, df->dfInstructions.mbSize, sos );
		    sioOutPutString( "}}", sos ); *pCol += 2;

		    rwc->rwcHasPrecedingTags= 0;
		    }
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		continue;

	    default:
		LDEB(tp->tpKind);
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		continue;
	    }

	break;
	}

    if  ( stroff == bi->biParaStrlen )
	{
	if  ( rwc->rwcSaveAsLinkAsRef		&&
	      rwc->rwcSaveAsLinkAsPageref	)
	    {
	    if  ( rwc->rwcInFldrslt > 0 )
		{ docRtfFinishFldrslt( sos, pCol, rwc );	}

	    docRtfWriteTag( "\\tab", pCol, sos );

	    docRtfWriteStartField( sos, &(rwc->rwcSaveAsPagerefField),
								pCol, rwc );

	    sioOutPutString( "?", sos ); *pCol += 1;

	    rwc->rwcSaveAsLinkAsRef= 0;
	    rwc->rwcSaveAsLinkAsPageref= 0;
	    }

	while( rwc->rwcInFldrslt > 0 )
	    { docRtfFinishFldrslt( sos, pCol, rwc ); }

	/*  8  */
	if  ( ( ! bi->biParaInTable				&&
	        bi->biInExternalItem == DOCinBODY		)	||
	      ( tabsSaved && ! bi->biParaInTable )			||
	      bi->biNumberInParent < bi->biParent->biChildCount- 1	)
	    {
	    docRtfWriteTag( "\\par", pCol, sos );
	    docRtfWriteNextLine( pCol, sos );
	    rwc->rwcHasPrecedingTags= 0;
	    }
	}

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask, &(rwc->rwcParagraphProperties),
			&ppUpdMask, &(bi->biParaProperties),
			colorMap, listStyleMap ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a Row level item.						*/
/*									*/
/************************************************************************/

static int docRtfSaveRowItem(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferDocument *		bd,
				const BufferItem *		rowBi,
				const DocumentSelection *	ds,
				RtfWritingContext *		rwc )
    {
    int				col;

    if  ( ! ds								||
	  ds->dsBegin.dpBi->biParent != ds->dsEnd.dpBi->biParent	)
	{
	if  ( ! docEqualRows( &(rowBi->biRowProperties),
						&(rwc->rwcRowProperties) ) )
	    {
	    if  ( rwc->rwcInTable && docRtfPopTable( rwc, pCol, sos ) )
		{ LDEB(1);	}
	    }

	if  ( ! rwc->rwcInTable					&&
	      docRtfPushTable( rwc, pCol, sos, rowBi, ds )	)
	    { LDEB(1);	}
	}

    for ( col= 0; col < rowBi->biChildCount; col++ )
	{
	int		par;
	BufferItem *	cellBi= rowBi->biChildren[col];

	if  ( ds && docCompareItemPositions( cellBi, ds->dsBegin.dpBi ) < 0 )
	    { continue;	}

	if  ( ds && docCompareItemPositions( cellBi, ds->dsEnd.dpBi ) > 0 )
	    { continue;	}

	if  ( ! ds						||
	      ( ( ds->dsCol0 < 0 || col >= ds->dsCol0 )	&&
		( ds->dsCol1 < 0 || col <= ds->dsCol1 )	)	)
	    {
	    for ( par= 0; par < cellBi->biChildCount; par++ )
		{
		BufferItem *	paraBi= cellBi->biChildren[par];

		if  ( ds && docCompareItemPositions( paraBi,
						    ds->dsBegin.dpBi ) < 0 )
		    { continue;	}

		if  ( ds && docCompareItemPositions( paraBi,
						    ds->dsEnd.dpBi ) > 0 )
		    { continue;	}

		if  ( docRtfSaveParaItem( sos, pCol, bd, paraBi, ds, rwc ) )
		    { LLDEB(col,par); return -1;	}
		}

	    docRtfWriteTag( "\\cell", pCol, sos );
	    if  ( col != rowBi->biChildCount- 1 )
		{ docRtfWriteNextLine( pCol, sos );	}
	    }
	}

    docRtfWriteTag( "\\row", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return 0;
    }

static int docSaveSectionProperties(	SimpleOutputStream *		sos,
					int *				pCol,
					const DocumentProperties *	dp,
					const DocumentSelection *	ds,
					const SectionProperties *	sp,
					RtfWritingContext *		rwc )
    {
    SectionProperties		spDef;
    PropertyMask		updMask;

    docInitSectionProperties( &spDef );

    PROPmaskCLEAR( &updMask );
    utilPropMaskFill( &updMask, DGprop_COUNT );
    PROPmaskUNSET( &updMask, DGpropHEADER_POSITION );
    PROPmaskUNSET( &updMask, DGpropFOOTER_POSITION );

    utilUpdDocumentGeometry( &(spDef.spDocumentGeometry),
				    &(dp->dpGeometry), &updMask, &updMask );

    PROPmaskCLEAR( &updMask );
    utilPropMaskFill( &updMask, SPprop_COUNT );

    docSectPropertyDifference( &updMask, &spDef, sp, &updMask );

    docCleanSectionProperties( &spDef );
    if  ( ds )
	{ PROPmaskUNSET( &updMask, SPpropTITLEPG );	}

    docRtfWriteNextLine( pCol, sos );
    docRtfWriteTag( "\\sectd", pCol, sos );

    docRtfSaveSectionProperties( sos,
			rwc->rwcDefaultOutputMapping, pCol, &updMask, sp );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a buffer item, hierarchically descending to the leaves of the	*/
/*  document tree.							*/
/*									*/
/************************************************************************/

static int docRtfSaveGroupItem(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				const DocumentSelection *	ds,
				RtfWritingContext *		rwc )
    {
    int				i;

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	if  ( docRtfSaveItem( sos, pCol, bi->biChildren[i], bd, ds, rwc ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

static int docRtfSaveItem(	SimpleOutputStream *		sos,
				int *				pCol,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				const DocumentSelection *	ds,
				RtfWritingContext *		rwc )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    if  ( ds )
	{
	if  ( docCompareItemPositions( bi, ds->dsBegin.dpBi ) < 0	)
	    { return 0;	}

	if  ( docCompareItemPositions( bi, ds->dsEnd.dpBi ) > 0		)
	    { return 0;	}
	}

    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevCELL:
	    if  ( docRtfSaveGroupItem( sos, pCol, bi, bd, ds, rwc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:

	    if  ( docSaveSectionProperties( sos, pCol, dp, ds,
					    &(bi->biSectProperties), rwc ) )
		{ LDEB(1); return -1;	}

	    if  ( ! ds							&&
		  docRtfSaveSectHeadersFooters( sos, pCol, bi, bd, rwc ) )
		{ LDEB(1); return -1;	}

	    if  ( docRtfSaveGroupItem( sos, pCol, bi, bd, ds, rwc ) )
		{ LDEB(1); return -1;	}

	    if  ( rwc->rwcInTable && docRtfPopTable( rwc, pCol, sos ) )
		{ LDEB(rwc->rwcInTable);	}

	    while( rwc->rwcInFldrslt > 0 )
		{ docRtfFinishFldrslt( sos, pCol, rwc );	}

	    if  ( bi->biParent						&&
		  bi->biNumberInParent < bi->biParent->biChildCount- 1	)
		{ docRtfWriteTag( "\\sect", pCol, sos );	}

	    docRtfWriteNextLine( pCol, sos );
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{
		if  ( rwc->rwcInTable && docRtfPopTable( rwc, pCol, sos ) )
		    { LDEB(1);	}

		if  ( docRtfSaveGroupItem( sos, pCol, bi, bd, ds, rwc ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( docRtfSaveRowItem( sos, pCol, bd, bi, ds, rwc ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( docRtfSaveParaItem( sos, pCol, bd, bi, ds, rwc ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }

static void docRtfInitWritingContext(	RtfWritingContext *	rwc )
    {
    int		i;

    utilInitTextAttribute( &(rwc->rwcTextAttribute) );
    docInitParagraphProperties( &(rwc->rwcParagraphProperties) );
    docInitRowProperties( &(rwc->rwcRowProperties) );

    utilInitTextAttribute( &(rwc->rwcOutsideTableTextAttribute) );
    docInitParagraphProperties( &(rwc->rwcOutsideTableParagraphProperties) );

    rwc->rwcOutsideFldrsltAttribute= (PushedAttribute *)0;

    rwc->rwcInFldrslt= 0;
    rwc->rwcInTable= 0;

    rwc->rwcSaveBookmarks= 1;

    rwc->rwcSaveAsLink= 0;
    rwc->rwcSaveAsLinkAsRef= 0;
    rwc->rwcSaveAsLinkAsPageref= 0;
    docInitField( &(rwc->rwcSaveAsHyperlinkField) );
    docInitField( &(rwc->rwcSaveAsRefField) );
    docInitField( &(rwc->rwcSaveAsPagerefField) );

    for ( i= 0; i < 256; i++ )
	{ rwc->rwcDefaultOutputMapping[i]= i;	}

    rwc->rwcTextOutputMapping= rwc->rwcDefaultOutputMapping;
    }

static void docRtfCleanWritingContext(	RtfWritingContext *	rwc )
    {
    docCleanParagraphProperties( &rwc->rwcParagraphProperties );
    docCleanRowProperties( &rwc->rwcRowProperties );

    docCleanParagraphProperties( &rwc->rwcOutsideTableParagraphProperties );

    docCleanField( &(rwc->rwcSaveAsHyperlinkField) );
    docCleanField( &(rwc->rwcSaveAsRefField) );
    docCleanField( &(rwc->rwcSaveAsPagerefField) );
    }

int docRtfSaveDocument(	SimpleOutputStream *		sos,
			BufferDocument *		bd,
			const DocumentSelection *	ds,
			int				saveBookmarks )
    {
    const BufferItem *		bi= &(bd->bdItem);
    int				col= 0;

    RtfWritingContext		rwc;

    if  ( ds )
	{
	ExternalItem *	ei;
	BufferItem *	bodySectBi;

	bi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! bi )
	    { XDEB(bi); return -1;	}
	}

    docRtfInitWritingContext( &rwc );
    rwc.rwcSaveBookmarks= saveBookmarks;

    docRtfWriteDestinationBegin( "\\rtf1\\ansi", &col, sos );
    docRtfWriteNextLine( &col, sos );

    if  ( docRtfSaveDocumentProperties( sos, &col, &rwc, bd ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocNotesSeparators( sos, &col, bd, &rwc ) )
	{ LDEB(1); docRtfCleanWritingContext( &rwc ); return -1;	}

    if  ( docRtfSaveItem( sos, &col, bi, bd, ds, &rwc ) )
	{ LDEB(bi->biLevel); return -1; }

    while( rwc.rwcInFldrslt > 0 )
	{ docRtfFinishFldrslt( sos, &col, &rwc ); }

    if  ( rwc.rwcInTable && docRtfPopTable( &rwc, &col, sos ) )
	{ LDEB(rwc.rwcInTable);	}

    docRtfWriteDestinationEnd( &col, sos );
    docRtfWriteNextLine( &col, sos );

    docRtfCleanWritingContext( &rwc );

    return 0;
    }

int docRtfSaveSelectionAsLink(	SimpleOutputStream *		sos,
				const BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				asRef,
				int				asPageref,
				const unsigned char *		fileName,
				int				fileSize,
				const unsigned char *		markName,
				int				markSize )
    {
    const BufferItem *		bi= &(bd->bdItem);
    int				col= 0;

    RtfWritingContext		rwc;

    docRtfInitWritingContext( &rwc );

    rwc.rwcSaveBookmarks= 0;
    rwc.rwcSaveAsLink= 1;
    rwc.rwcSaveAsLinkAsRef= asRef;
    rwc.rwcSaveAsLinkAsPageref= asPageref;

    if  ( docFieldSetHyperlink( &(rwc.rwcSaveAsHyperlinkField),
						    fileName, fileSize,
						    markName, markSize ) )
	{ LDEB(1); docRtfCleanWritingContext( &rwc ); return -1;	}

    if  ( asRef && docFieldSetRef( &(rwc.rwcSaveAsRefField),
						    markName, markSize ) )
	{ LDEB(asRef); docRtfCleanWritingContext( &rwc ); return -1;	}

    if  ( asPageref && docFieldSetPageref( &(rwc.rwcSaveAsPagerefField),
						    markName, markSize ) )
	{ LDEB(asPageref); docRtfCleanWritingContext( &rwc ); return -1; }

    docRtfWriteDestinationBegin( "\\rtf0\\ansi", &col, sos );
    docRtfWriteNextLine( &col, sos );

    if  ( docRtfSaveDocumentProperties( sos, &col, &rwc, bd ) )
	{ LDEB(1); docRtfCleanWritingContext( &rwc ); return -1;	}

    docRtfWriteNextLine( &col, sos );

    if  ( docRtfSaveItem( sos, &col, bi, bd, ds, &rwc ) )
	{ LDEB(bi->biLevel); docRtfCleanWritingContext( &rwc ); return -1; }

    while( rwc.rwcInFldrslt > 0 )
	{ docRtfFinishFldrslt( sos, &col, &rwc ); }

    if  ( rwc.rwcInTable && docRtfPopTable( &rwc, &col, sos ) )
	{ LDEB(rwc.rwcInTable);	}

    docRtfWriteDestinationEnd( &col, sos );
    docRtfWriteNextLine( &col, sos );

    docRtfCleanWritingContext( &rwc );

    return 0;
    }
