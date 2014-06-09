/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Manage inserted objects.						*/
/*									*/
/************************************************************************/

void docCleanObject(	BufferDocument *	bd,
			InsertedObject *	io )
    {
    utilCleanMemoryBuffer( &io->ioObjectData );
    utilCleanMemoryBuffer( &io->ioResultData );

    if  ( io->ioObjectName )
	{ free( io->ioObjectName );	}
    if  ( io->ioObjectClass )
	{ free( io->ioObjectClass );	}
    
    if  ( io->ioDrawingShape )
	{ docFreeDrawingShape( bd, io->ioDrawingShape );	}

    return;
    }

void docInitObject(	InsertedObject *	io )
    {
    io->ioKind= DOCokUNKNOWN;
    io->ioResultKind= DOCokUNKNOWN;
    io->ioRtfResultKind= RESULTkindUNKNOWN;
    io->ioRtfEmbedKind= EMBEDkindOBJEMB;

    docInitPictureProperties( &(io->ioPictureProperties) );

    io->ioInline= 1;

    io->ioTwipsWide= 0;
    io->ioTwipsHigh= 0;

    io->ioPixelsWide= 0;
    io->ioPixelsHigh= 0;
    io->ioScaleXSet= 100;
    io->ioScaleYSet= 100;
    io->ioScaleXUsed= 100;
    io->ioScaleYUsed= 100;

    io->ioX0Twips= 0;
    docInitLayoutPosition( &(io->ioY0Position) );

    io->ioTopCropTwips= 0;
    io->ioBottomCropTwips= 0;
    io->ioLeftCropTwips= 0;
    io->ioRightCropTwips= 0;

    utilInitMemoryBuffer( &io->ioObjectData );
    utilInitMemoryBuffer( &io->ioResultData );
    
    io->ioObjectName= (unsigned char *)0;
    io->ioObjectClass= (unsigned char *)0;

    io->ioDrawingShape= (DrawingShape *)0;

    io->ioPixmap= (APP_BITMAP_IMAGE)0;
    io->ioPrivate= (void *)0;

    return;
    }

void docObjectAdaptToPictureGeometry(	InsertedObject *		io,
					const PictureProperties *	pip )
    {
    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    io->ioScaleXSet= pip->pipScaleXSet;
    io->ioScaleYSet= pip->pipScaleYSet;
    io->ioScaleXUsed= pip->pipScaleXUsed;
    io->ioScaleYUsed= pip->pipScaleYUsed;

    io->ioTopCropTwips= pip->pipTopCropTwips;
    io->ioBottomCropTwips= pip->pipBottomCropTwips;
    io->ioLeftCropTwips= pip->pipLeftCropTwips;
    io->ioRightCropTwips= pip->pipRightCropTwips;

    return;
    }

int docObjectSetData(	InsertedObject *	io,
			const unsigned char *	bytes,
			int			size )
    { return utilSetMemoryBuffer( &(io->ioObjectData), bytes, size ); }

int docSetResultData(	InsertedObject *	io,
			const unsigned char *	bytes,
			int			size )
    { return utilSetMemoryBuffer( &(io->ioResultData), bytes, size );	}

int docSetObjectName(	InsertedObject *	io,
			const unsigned char *	name,
			int			len )
    {
    unsigned char *	fresh= (unsigned char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectName= fresh;
    memcpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

int docSetObjectClass(	InsertedObject *	io,
			const unsigned char *	name,
			int			len )
    {
    unsigned char *	fresh= (unsigned char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectClass= fresh;
    memcpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

InsertedObject * docClaimObjectCopy(	BufferDocument *	bd,
					BufferItem *		bi,
					int *			pNr,
					const InsertedObject *	ioFrom )
    {
    InsertedObject *	ioTo;
    int			objectNumber;

    ioTo= docClaimObject( &objectNumber, bi );
    if  ( ! ioTo )
	{ XDEB(ioTo); return (InsertedObject *)0;	}

    if  ( ioFrom->ioObjectName						&&
	  docSetObjectName( ioTo, ioFrom->ioObjectName,
			    strlen( (char *)ioFrom->ioObjectName ) )	)
	{ LDEB(1); docCleanObject( bd, ioTo ); return (InsertedObject *)0; }

    if  ( ioFrom->ioObjectClass						&&
	  docSetObjectClass( ioTo, ioFrom->ioObjectClass,
			    strlen( (char *)ioFrom->ioObjectClass ) )	)
	{ LDEB(1); docCleanObject( bd, ioTo ); return (InsertedObject *)0; }

    if  ( utilCopyMemoryBuffer( &ioTo->ioObjectData, &ioFrom->ioObjectData ) ||
	  utilCopyMemoryBuffer( &ioTo->ioResultData, &ioFrom->ioResultData ) )
	{
	LDEB(1);
	docCleanObject( bd, ioTo ); docInitObject( ioTo );
	return (InsertedObject *)0;
	}

    ioTo->ioKind= ioFrom->ioKind;
    ioTo->ioResultKind= ioFrom->ioResultKind;
    ioTo->ioRtfResultKind= ioFrom->ioRtfResultKind;
    ioTo->ioRtfEmbedKind= ioFrom->ioRtfEmbedKind;
    ioTo->ioTwipsWide= ioFrom->ioTwipsWide;
    ioTo->ioTwipsHigh= ioFrom->ioTwipsHigh;
    ioTo->ioScaleXSet= ioFrom->ioScaleXSet;
    ioTo->ioScaleYSet= ioFrom->ioScaleYSet;
    ioTo->ioScaleXUsed= ioFrom->ioScaleXUsed;
    ioTo->ioScaleYUsed= ioFrom->ioScaleYUsed;
    ioTo->ioPixelsWide= ioFrom->ioPixelsWide;
    ioTo->ioPixelsHigh= ioFrom->ioPixelsHigh;

    ioTo->ioPictureProperties= ioFrom->ioPictureProperties;

    ioTo->ioTopCropTwips= ioFrom->ioTopCropTwips;
    ioTo->ioBottomCropTwips= ioFrom->ioBottomCropTwips;
    ioTo->ioLeftCropTwips= ioFrom->ioLeftCropTwips;
    ioTo->ioRightCropTwips= ioFrom->ioRightCropTwips;

    return ioTo;
    }

/************************************************************************/
/*									*/
/*  Do cleanup provoked by the deletion of a particule.			*/
/*									*/
/************************************************************************/

void docCleanParticuleObject(	BufferDocument *	bd,
				BufferItem *		bi,
				TextParticule *		tp )
    {
    InsertedObject *	io;

    if  ( tp->tpKind != DOCkindOBJECT )
	{ return;	}

    io= bi->biParaObjects+ tp->tpObjectNumber;

    docCleanObject( bd, io );
    docInitObject( io );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an object.							*/
/*									*/
/*  NOTE that objects have text attributes. This is for the following	*/
/*  reasons:								*/
/*  *)  The calculation of the descender height below the object.	*/
/*  *)  This makes editing around the object and saving it MUCH easier.	*/
/*									*/
/************************************************************************/

TextParticule *	docInsertObject(	BufferDocument *	bd,
					BufferItem *		paraBi,
					InsertedObject **	pIo,
					int			n,
					int			off,
					const TextAttribute *	ta )
    {
    TextParticule *	tp;
    InsertedObject *	io;

    int			objectNumber;
    int			textAttributeNumber;

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return (TextParticule *)0;	}

    io= docClaimObject( &objectNumber, paraBi );
    if  ( ! io )
	{ LXDEB(paraBi->biParaObjectCount+1,io); return (TextParticule *)0; }

    tp= docInsertTextParticule( paraBi, n, off, 1,
					DOCkindOBJECT, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return tp;	}

    tp->tpObjectNumber= paraBi->biParaObjectCount++;

    *pIo= io;

    return tp;
    }

InsertedObject *	docClaimObject(	int *			pNr,
					BufferItem *		bi )
    {
    InsertedObject *	io;

    io= (InsertedObject *)realloc( bi->biParaObjects,
		    ( bi->biParaObjectCount+ 1 )* sizeof( InsertedObject ) );
    if  ( ! io )
	{ LXDEB(bi->biParaObjectCount+1,io); return io; }

    bi->biParaObjects= io;
    io += bi->biParaObjectCount;

    docInitObject( io );

    *pNr= bi->biParaObjectCount; return io;
    }

/************************************************************************/
/*									*/
/*  Copy particule data from one paragraph to another.			*/
/*									*/
/*  9)  Not needed, the document is marked as changed, so this can wait	*/
/*	until it is saved.						*/
/*									*/
/************************************************************************/

int docCopyParticuleData(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				TextParticule *		tpTo,
				const TextParticule *	tpFrom )
    {
    int				nr;
    const DocumentFieldList *	dflFrom= &(dcj->dcjBdFrom->bdFieldList);

    BufferDocument *		bdTo= dcj->dcjBdTo;

    switch( tpFrom->tpKind )
	{
	case DOCkindTEXT:
	case DOCkindTAB:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    break;

	case DOCkindOBJECT:
	    {
	    const InsertedObject *	ioFrom;
	    InsertedObject *		ioTo;

	    ioTo= docClaimObjectCopy( bdTo, biTo, &nr,
			biFrom->biParaObjects+ tpFrom->tpObjectNumber );
	    if  ( ! ioTo )
		{ XDEB(ioTo); return -1;	}

	    tpTo->tpObjectNumber= biTo->biParaObjectCount++;

	    ioFrom= biFrom->biParaObjects+ tpFrom->tpObjectNumber;

	    if  ( ioFrom->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( ! ioFrom->ioDrawingShape )
		    { XDEB(ioFrom->ioDrawingShape);	}
		else{
		    ioTo->ioDrawingShape= docCopyDrawingShape( dcj, eo,
						    ioFrom->ioDrawingShape );
		    if  ( ! ioTo->ioDrawingShape )
			{ XDEB(ioTo->ioDrawingShape);	}
		    }
		}

	    /*  9
	    if  ( ioTo->ioBliptag == 0 )
		{ ioTo->ioBliptag= appGetTimestamp();	}
	    */
	    }
	    break;

	case DOCkindFIELDEND:
	    if  ( tpFrom->tpObjectNumber < 0				||
		  tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	||
		  dcj->dcjFieldMap[tpFrom->tpObjectNumber] < 0		)
		{
		LDEB(tpFrom->tpObjectNumber);
		tpTo->tpObjectNumber= -1;
		}
	    else{
		tpTo->tpObjectNumber=
				dcj->dcjFieldMap[tpFrom->tpObjectNumber];
		}
	    break;

	case DOCkindFIELDSTART:
	case DOCkindXE:
	case DOCkindTC:
	    {
	    const DocumentField *		dfFrom;
	    DocumentField *			dfTo;
	    const FieldKindInformation *	fki;

	    dfFrom= dflFrom->dflFields+ tpFrom->tpObjectNumber;
	    fki= DOC_FieldKinds+ dfFrom->dfKind;

	    dfTo= docClaimFieldCopy( &(bdTo->bdFieldList), &nr, dfFrom );
	    if  ( ! dfTo )
		{ XDEB(dfTo); return -1;	}

	    if  ( dfTo->dfKind < 0 || dfTo->dfKind >= DOC_FieldKindCount )
		{ LLDEB(dfFrom->dfKind,dfTo->dfKind); return -1; }

	    tpTo->tpObjectNumber= nr;

	    if  ( tpFrom->tpObjectNumber < 0				||
		  tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	)
		{ LDEB(tpFrom->tpObjectNumber); }
	    else{ dcj->dcjFieldMap[tpFrom->tpObjectNumber]= nr;		}

	    eo->eoFieldUpdate |= fki->fkiCalculateWhen;
	    }

	    break;

	case DOCkindNOTE:
	    if  ( docCopyNote( dcj, eo, biTo, tpTo, biFrom, tpFrom ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(tpFrom->tpKind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a hyperlink to another paragraph.				*/
/*									*/
/*  If the hyperlink is relative to the name of the target document, it */
/*  it is converted to a relative link (the document name is removed)	*/
/*									*/
/************************************************************************/

int docCopyFieldRelative(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				TextParticule *		tpTo,
				const TextParticule *	tpFrom )
    {
    DocumentField *		dfFrom;

    const char *		fileName;
    int				fileSize;
    const char *		markName;
    int				markSize;

    const BufferDocument *	bdFrom= dcj->dcjBdFrom;
    BufferDocument *		bdTo= dcj->dcjBdTo;

    dfFrom= bdFrom->bdFieldList.dflFields+ tpFrom->tpObjectNumber;

    if  ( ! docFieldGetHyperlink( dfFrom, &fileName, &fileSize,
						&markName, &markSize )	&&
	  fileSize == dcj->dcjRefFileSize 				&&
	  markSize > 0							&&
	  ! strncmp( fileName, dcj->dcjRefFileName, fileSize )		)
	{
	const DocumentFieldList *	dflFrom= &(bdFrom->bdFieldList);
	DocumentField *			dfTo;
	char *				copy= (char *)malloc( markSize+ 1 );
	int				fieldNumber;

	if  ( ! copy )
	    { XDEB(copy); return -1;	}

	strncpy( copy, markName, markSize )[markSize]= '\0';

	dfTo= docClaimField( &fieldNumber, &(bdTo->bdFieldList) );
	if  ( ! dfTo )
	    { XDEB(dfTo); return -1;	}

	if  ( docFieldSetHyperlink( dfTo, (unsigned char *)0, 0,
					(unsigned char *)copy, markSize ) )
	    { SDEB(copy); return -1;	}

	dfTo->dfKind= dfFrom->dfKind;
	tpTo->tpObjectNumber= fieldNumber;

	if  ( tpFrom->tpObjectNumber < 0				||
	      tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	)
	    { LDEB(tpFrom->tpObjectNumber); }
	else{ dcj->dcjFieldMap[tpFrom->tpObjectNumber]= fieldNumber;	}

	free( copy );

	return 0;
	}

    if  ( docCopyParticuleData( dcj, eo, biTo, biFrom, tpTo, tpFrom ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a mapping for field numbers.				*/
/*									*/
/************************************************************************/

int *	docAllocateFieldMap(	const BufferDocument *	bdFrom )
    {
    int *	fieldMap;
    int		i;

    fieldMap= malloc( bdFrom->bdFieldList.dflFieldCount* sizeof(int)+ 1 );
    if  ( ! fieldMap )
	{ LXDEB(bdFrom->bdFieldList.dflFieldCount,fieldMap); return (int *)0; }
    for ( i= 0; i < bdFrom->bdFieldList.dflFieldCount; i++ )
	{ fieldMap[i]= -1;	}

    return fieldMap;
    }

/************************************************************************/
/*									*/
/*  Close the objects in a buffer item.					*/
/*									*/
/************************************************************************/

static void docCleanParaObjects(	int *			pNoteCount,
					int *			pBulletsDeleted,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					void *			voidadd,
					DOC_CLOSE_OBJECT	closeObject )
    {
    int			part;
    TextParticule *	tp;

    int			noteCount= 0;

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind == DOCkindNOTE )
	    { noteCount++;	}

	if  ( tp->tpKind == DOCkindOBJECT )
	    { (*closeObject)( bd, paraBi, tp, voidadd );	}
	}

    if  ( paraBi->biParaListOverride > 0 )
	{
	if  ( docRemoveParagraphFromList( paraBi, bd ) )
	    { LDEB(1);	}

	(*pBulletsDeleted)++;
	}

    *pNoteCount += noteCount;
    return;
    }

void docCleanItemObjects(	int *			pNoteCount,
				int *			pBulletsDeleted,
				int *			pParagraphCount,
				BufferDocument *	bd,
				BufferItem *		bi,
				void *			voidadd,
				DOC_CLOSE_OBJECT	closeObject )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevROW:
	case DOClevCELL:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		docCleanItemObjects( pNoteCount, pBulletsDeleted,
						    pParagraphCount,
						    bd, bi->biChildren[i],
						    voidadd, closeObject );
		}
	    break;

	case DOClevPARA:
	    docCleanParaObjects( pNoteCount, pBulletsDeleted,
						bd, bi, voidadd, closeObject );
	    (*pParagraphCount)++;
	    break;

	default:
	    LDEB(bi->biLevel); return;
	}

    return;
    }

