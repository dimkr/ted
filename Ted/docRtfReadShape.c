/************************************************************************/
/*									*/
/*  Read RTF shapes.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Property control words inside a shape.				*/
/*									*/
/************************************************************************/

int docRtfShpProperty(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeProperties *	sp;

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrcCurrentLine,ds); return 0;	}

    sp= &(ds->dsShapeProperties);

    if  ( rcw->rcwID != SHPpropLID	&&
	  ds->dsIsChildShape		)
	{ SLDEB(rcw->rcwWord,ds->dsIsChildShape);	}

    switch( rcw->rcwID )
	{
	case SHPpropLEFT:
	    sp->spRect.drX0= arg;
	    break;
	case SHPpropRIGHT:
	    sp->spRect.drX1= arg;
	    break;
	case SHPpropTOP:
	    sp->spRect.drY0= arg;
	    break;
	case SHPpropBOTTOM:
	    sp->spRect.drY1= arg;
	    break;

	case SHPpropLID:
	    sp->spID= arg;
	    break;
	case SHPpropZ:
	    sp->spID= arg;
	    break;

	case SHPpropFHDR:
	    sp->spInHeader= arg != 0;
	    break;

	case SHPpropBX:
	    sp->spHorizontalAttachment= rcw->rcwEnumValue;
	    break;

	case SHPpropBY:
	    sp->spVerticalAttachment= rcw->rcwEnumValue;
	    break;

	case SHPpropWR:
	    sp->spWrapStyle= arg;
	    break;
	case SHPpropWRK:
	    sp->spWrapSide= arg;
	    break;

	case SHPpropFBLWTXT:
	    sp->spShapeBelowText= ( arg != 0 );
	    break;
	case SHPpropLOCKANCHOR:
	    sp->spLockAnchor= ( arg != 0 );
	    break;

	default:
	    SLDEB(rcw->rcwWord,arg);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Text inside a shape.						*/
/*									*/
/************************************************************************/

static int docRtfShpText(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;

    BufferItem *	sectBi= rrc->rrcBi;
    SelectionScope	ss;
    int			externalItemKind= DOCinSHPTXT;

    docInitSelectionScope( &ss );

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    ss.ssInExternalItem= DOCinSHPTXT;
    ss.ssSectNrExternalTo= sectBi->biNumberInParent;

    /* No! to deal with faulty documents.
    rrc->rrcDrawingShape= (DrawingShape *)0;
    */

    if  ( docRtfReadExternalItem( &(ds->dsExternalItem.eiItem),
					&externalItemKind, sis, rrc, &ss ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    docSetExternalItemKind( ds->dsExternalItem.eiItem, externalItemKind );

    /*
    rrc->rrcDrawingShape= ds;
    */
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an {\sn .... } {\sv .... } pair.				*/
/*									*/
/*  NOTE that the concept of an 'RtfControlWord' is abused for		*/
/*	 the shape property.						*/
/*									*/
/************************************************************************/

static int docRtfShpCollectNumbers(	SimpleInputStream *	sis,
					int *			numbers,
					int			count,
					RtfReadingContext *	rrc )
    {
    int		i;
    int		c= sioInGetCharacter( sis );

    for ( i= 0; i < count; i++ )
	{
	int		sign;
	int		val;

	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetCharacter( sis );
	    }

	if  ( c != ';' )
	    { CDEB(c); return -1;	}
	c= sioInGetCharacter( sis );

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetCharacter( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }

	numbers[i]= sign* val;

	if  ( c == '}' )
	    { break;	}
	}

    if  ( c != '}' )
	{ CDEB(c); return -1;	}
    sioInUngetLastRead( sis );

    if  ( count > 0 )
	{ numbers[count]= numbers[0]; }

    return 0;
    }

static int docRtfShpCollectVertices(	SimpleInputStream *	sis,
					ShapeVertex *		vertices,
					int			count,
					RtfReadingContext *	rrc )
    {
    int		i;
    int		c= sioInGetCharacter( sis );

    for ( i= 0; i < count; i++ )
	{
	int		sign;
	int		val;

	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetCharacter( sis );
	    }

	if  ( c != ';' )
	    { CDEB(c); return -1;	}
	c= sioInGetCharacter( sis );
	if  ( c != '(' )
	    { CDEB(c); return -1;	}
	c= sioInGetCharacter( sis );

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetCharacter( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }
	if  ( c != ',' )
	    { CDEB(c); return -1;	}
	c= sioInGetCharacter( sis );

	vertices[i].svX= sign* val;

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetCharacter( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }

	vertices[i].svY= sign* val;

	if  ( c != ')' )
	    { CDEB(c); return -1;	}
	c= sioInGetCharacter( sis );
	}

    if  ( c != '}' )
	{ CDEB(c); return -1;	}
    sioInUngetLastRead( sis );

    if  ( count > 0 )
	{ vertices[count]= vertices[0]; }

    return 0;
    }

int docRtfShpArray(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    int			rval= 0;

    int			c;
    int			sign;

    int			size;
    int			count;

    ShapeVertex *	vertices= (ShapeVertex *)0;
    int *		numbers= (int *)0;

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    sign= 1; size= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}
    while( isdigit( c ) )
	{ size= 10* size+ c- '0'; c= sioInGetCharacter( sis ); }
    size *= sign;

    if  ( c != ';' )
	{ CDEB(c); rval= -1; goto ready;	}
    c= sioInGetCharacter( sis );

    sign= 1; count= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}
    while( isdigit( c ) )
	{ count= 10* count+ c- '0'; c= sioInGetCharacter( sis ); }
    count *= sign;

    sioInUngetLastRead( sis );

    switch( size )
	{
	case 8:
	    vertices= malloc( (count+ 1)* sizeof( ShapeVertex ) );
	    if  ( ! vertices )
		{ LXDEB(count,vertices); rval= -1; goto ready; }
	    if  ( docRtfShpCollectVertices( sis, vertices, count, rrc ) )
		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	case 2:
	case 4:
	    numbers= malloc( (count+ 1)* sizeof( int ) );
	    if  ( ! numbers )
		{ LXDEB(count,numbers); rval= -1; goto ready; }
	    if  ( docRtfShpCollectNumbers( sis, numbers, count, rrc ) )

		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	default:
	    LLDEB(count,size); rval= -1; goto ready;
	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); goto ready;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_pVerticies:
	    ds->dsVertices= vertices;
	    vertices= (ShapeVertex *)0;	/* steal */
	    ds->dsVertexCount= count;
	    break;

	case DSHPprop_pWrapPolygonVertices:
	    ds->dsWrapPolygonVertices= vertices;
	    vertices= (ShapeVertex *)0;	/* steal */
	    ds->dsWrapPolygonVertexCount= count;
	    break;

	case DSHPprop_pSegmentInfo:
	    ds->dsSegmentInfos= numbers;
	    numbers= (int *)0;	/* steal */
	    ds->dsSegmentInfoCount= count;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

  ready:
    if  ( vertices )
	{ free( vertices );	}
    if  ( numbers )
	{ free( numbers );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Consume a picture value attached to a shape.			*/
/*									*/
/************************************************************************/

static int docRtfSaveShapeData(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( utilSetMemoryBuffer( &(ds->dsPictureData), text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }


static int docRtfReadShapePicture(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RtfReadingState		internRrs;

    int				rval;

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			(const RtfControlWord *)0, (const RtfControlWord *)0,
			docRtfSaveShapeData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    docRtfPopReadingState( rrc );

    return rval;
    }

int docRtfShpPicture(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    int			control;
    char		controlWord[TEDszRTFCONTROL+1];
    int			gotArg;
    int			locArg= -1;
    int			c;
    DrawingShape *	ds= rrc->rrcDrawingShape;

    control= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    /*
    LSDEB(rrc->rrcCurrentLine,controlWord);
    */

    if  ( strcmp( controlWord, "pict" ) )
	{ SDEB(controlWord); return -1;	}

#   if 1

    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    if  ( docRtfReadShapePicture( sis, rcw, arg, rrc ) )
	{ SSDEB(rcw->rcwWord,controlWord); return -1;	}

    if  ( ! ds )
	{ XDEB(ds);						}
    else{ ds->dsPictureProperties= rrc->rrcPictureProperties;	}

#   else

    if  ( docRtfReadPict( sis, rcw, arg, rrc ) )
	{ SSDEB(rcw->rcwWord,controlWord); return -1;	}

#   endif

    return 0;
    }

int docRtfShpString(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    int		c;

    c= sioInGetCharacter( sis );
    while( c != '}' )
	{
	if  ( c == EOF )
	    { LDEB(c); return -1;	}

	c= sioInGetCharacter( sis );
	}

    sioInUngetLastRead( sis );

    return 0;
    }

static int docRtfShpGetNumber(	long *			pVal,
				SimpleInputStream *	sis,
				RtfReadingContext *	rrc )
    {
    int			c;
    long		val;
    int			sign= 1;

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    val= 0;

    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}

    while( isdigit( c ) )
	{ val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }

    val *= sign;

    sioInUngetLastRead( sis );

    *pVal= val;
    return 0;
    }

int docRtfShpPositionNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_posh:
	    ds->dsHorizontalAlignment= val;
	    break;
	case DSHPprop_posrelh:
	    ds->dsHorizontalRelativeTo= val;
	    break;
	case DSHPprop_posv:
	    ds->dsVerticalAlignment= val;
	    break;
	case DSHPprop_posrelv:
	    ds->dsVerticalRelativeTo= val;
	    break;

	case DSHPprop_fUseShapeAnchor:
	    ds->ds_fUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fLayoutInCell:
	    ds->ds_fLayoutInCell= val != 0;
	    break;
	case DSHPprop_fAllowOverlap:
	    ds->ds_fAllowOverlap= val != 0;
	    break;
	case DSHPprop_fChangePage:
	    ds->ds_fChangePage= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpTypeNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_shapeType:
	    if  ( rcw->rcwID != DSHPprop_shapeType )
		{ SDEB(docShapeTypeString(val));	}

	    ds->dsShapeType= val;
	    break;

	case DSHPprop_rotation:
	    ds->dsRotation= val;
	    break;

	case DSHPprop_dxWrapDistLeft:
	    ds->dsWrapDistLeftEmu= val;
	    break;
	case DSHPprop_dyWrapDistTop:
	    ds->dsWrapDistTopEmu= val;
	    break;
	case DSHPprop_dxWrapDistRight:
	    ds->dsWrapDistRightEmu= val;
	    break;
	case DSHPprop_dyWrapDistBottom:
	    ds->dsWrapDistBottomEmu= val;
	    break;

	case DSHPprop_hspMaster:
	    ds->dsMasterShape= val;
	    break;
	case DSHPprop_hspNext:
	    ds->dsNextShape= val;
	    break;

	case DSHPprop_xLimo:
	    ds->dsXLimo= val;
	    break;
	case DSHPprop_yLimo:
	    ds->dsXLimo= val;
	    break;

	case DSHPprop_fIsBullet:
	    ds->ds_fIsBullet= val != 0;
	    break;
	case DSHPprop_fFlipV:
	    ds->ds_fFlipV= val != 0;
	    break;
	case DSHPprop_fFlipH:
	    ds->ds_fFlipH= val != 0;
	    break;
	case DSHPprop_fBehindDocument:
	    ds->ds_fBehindDocument= val != 0;
	    break;
	case DSHPprop_fIsButton:
	    ds->ds_fIsButton= val != 0;
	    break;
	case DSHPprop_fHidden:
	    ds->ds_fHidden= val != 0;
	    break;
	case DSHPprop_fReallyHidden:
	    ds->ds_fReallyHidden= val != 0;
	    break;
	case DSHPprop_fArrowheadsOK:
	    ds->ds_fArrowheadsOK= val != 0;
	    break;
	case DSHPprop_fBackground:
	    ds->ds_fBackground= val != 0;
	    break;
	case DSHPprop_fDeleteAttachedObject:
	    ds->ds_fDeleteAttachedObject= val != 0;
	    break;
	case DSHPprop_fEditedWrap:
	    ds->ds_fEditedWrap= val != 0;
	    break;
	case DSHPprop_fHitTestFill:
	    ds->ds_fHitTestFill= val != 0;
	    break;
	case DSHPprop_fHitTestLine:
	    ds->ds_fHitTestLine= val != 0;
	    break;
	case DSHPprop_fInitiator:
	    ds->ds_fInitiator= val != 0;
	    break;
	case DSHPprop_fNoFillHitTest:
	    ds->ds_fNoFillHitTest= val != 0;
	    break;
	case DSHPprop_fNoHitTestPicture:
	    ds->ds_fNoHitTestPicture= val != 0;
	    break;
	case DSHPprop_fNoLineDrawDash:
	    ds->ds_fNoLineDrawDash= val != 0;
	    break;
	case DSHPprop_fOleIcon:
	    ds->ds_fOleIcon= val != 0;
	    break;
	case DSHPprop_fOnDblClickNotify:
	    ds->ds_fOnDblClickNotify= val != 0;
	    break;
	case DSHPprop_fOneD:
	    ds->ds_fOneD= val != 0;
	    break;
	case DSHPprop_fPreferRelativeSize:
	    ds->ds_fPreferRelativeSize= val != 0;
	    break;
	case DSHPprop_fPrint:
	    ds->ds_fPrint= val != 0;
	    break;

	case DSHPprop_fPseudoInline:
	    ds->ds_fPseudoInline= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpLockNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_fLockRotation:
	    ds->ds_fLockRotation= val != 0;
	    break;
	case DSHPprop_fLockAspectRatio:
	    ds->ds_fLockAspectRatio= val != 0;
	    break;
	case DSHPprop_fLockAgainstSelect:
	    ds->ds_fLockAgainstSelect= val != 0;
	    break;
	case DSHPprop_fLockCropping:
	    ds->ds_fLockCropping= val != 0;
	    break;
	case DSHPprop_fLockVerticies:
	    ds->ds_fLockVerticies= val != 0;
	    break;
	case DSHPprop_fLockText:
	    ds->ds_fLockText= val != 0;
	    break;
	case DSHPprop_fLockAdjustHandles:
	    ds->ds_fLockAdjustHandles= val != 0;
	    break;
	case DSHPprop_fLockAgainstGrouping:
	    ds->ds_fLockAgainstGrouping= val != 0;
	    break;
	case DSHPprop_fLockShapeType:
	    ds->ds_fLockShapeType= val != 0;
	    break;

	case DSHPprop_fLockPosition:
	    ds->ds_fLockPosition= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpFillNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_fillType:
	    ds->dsFillType= val;
	    break;
	case DSHPprop_fillOpacity:
	    ds->dsFillOpacity= val;
	    break;
	case DSHPprop_fillBackOpacity:
	    ds->dsFillBackOpacity= val;
	    break;

	case DSHPprop_fillblipflags:
	    ds->dsFillblipflags= val;
	    break;

	case DSHPprop_fillWidth:
	    ds->dsFillWidthEmu= val;
	    break;
	case DSHPprop_fillHeight:
	    ds->dsFillHeightEmu= val;
	    break;
	case DSHPprop_fillAngle:
	    ds->dsFillAngle= val;
	    break;
	case DSHPprop_fillFocus:
	    ds->dsFillFocus= val;
	    break;

	case DSHPprop_fillToLeft:
	    ds->dsFillToLeft= val;
	    break;
	case DSHPprop_fillToTop:
	    ds->dsFillToTop= val;
	    break;
	case DSHPprop_fillToRight:
	    ds->dsFillToRight= val;
	    break;
	case DSHPprop_fillToBottom:
	    ds->dsFillToBottom= val;
	    break;

	case DSHPprop_fillOriginX:
	    ds->dsFillOriginX= val;
	    break;
	case DSHPprop_fillOriginY:
	    ds->dsFillOriginY= val;
	    break;
	case DSHPprop_fillShapeOriginX:
	    ds->dsFillShapeOriginX= val;
	    break;
	case DSHPprop_fillShapeOriginY:
	    ds->dsFillShapeOriginY= val;
	    break;

	case DSHPprop_fillDztype:
	    ds->dsFillDztype= val;
	    break;

	case DSHPprop_fillRectLeft:
	    ds->dsFillRectLeftEmu= val;
	    break;
	case DSHPprop_fillRectTop:
	    ds->dsFillRectTopEmu= val;
	    break;
	case DSHPprop_fillRectRight:
	    ds->dsFillRectRightEmu= val;
	    break;
	case DSHPprop_fillRectBottom:
	    ds->dsFillRectBottomEmu= val;
	    break;

	case DSHPprop_fillShadePreset:
	    ds->dsFillShadePreset= val;
	    break;
	case DSHPprop_fillShadeType:
	    ds->dsFillShadeType= val;
	    break;

	case DSHPprop_fFilled:
	    ds->ds_fFilled= val != 0;
	    break;
	case DSHPprop_fillShape:
	    ds->ds_fillShape= val != 0;
	    break;
	case DSHPprop_fillUseRect:
	    ds->ds_fillUseRect= val != 0;
	    break;
	case DSHPprop_fFillOK:
	    ds->ds_fFillOK= val != 0;
	    break;
	case DSHPprop_fFillShadeShapeOK:
	    ds->ds_fFillShadeShapeOK= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpLineNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_lineType:
	    ds->dsLineType= val;
	    break;

	case DSHPprop_lineFillBlipFlags:
	    ds->dsLineFillBlipFlags= val;
	    break;

	case DSHPprop_lineFillWidth:
	    ds->dsLineFillWidthEmu= val;
	    break;
	case DSHPprop_lineFillHeight:
	    ds->dsLineFillHeightEmu= val;
	    break;
	case DSHPprop_lineWidth:
	    ds->dsLineWidthEmu= val;
	    break;

	case DSHPprop_lineStyle:
	    ds->dsLineStyle= val;
	    break;
	case DSHPprop_lineDashing:
	    ds->dsLineDashing= val;
	    break;
	case DSHPprop_lineStartArrowhead:
	    ds->dsLineStartArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineEndArrowhead:
	    ds->dsLineEndArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineStartArrowWidth:
	    ds->dsLineStartArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineStartArrowLength:
	    ds->dsLineStartArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndArrowWidth:
	    ds->dsLineEndArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineEndArrowLength:
	    ds->dsLineEndArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndCapStyle:
	    ds->dsLineEndCapStyle= val;
	    break;
	case DSHPprop_lineFillDztype:
	    ds->dsLineFillDztype= val;
	    break;
	case DSHPprop_lineJoinStyle:
	    ds->dsLineJoinStyle= val;
	    break;

	case DSHPprop_lineMiterLimit:
	    ds->dsLineMiterLimit= val;
	    break;

	case DSHPprop_fLine:
	    ds->ds_fLine= val != 0;
	    break;
	case DSHPprop_fLineOK:
	    ds->ds_fLineOK= val != 0;
	    break;

	case DSHPprop_fLineUseShapeAnchor:
	    ds->ds_fLineUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fColumnLineOK:
	    ds->ds_fColumnLineOK= val != 0;
	    break;
	case DSHPprop_fColumnLine:
	    ds->ds_fColumnLine= val != 0;
	    break;
	case DSHPprop_fLeftLine:
	    ds->ds_fLeftLine= val != 0;
	    break;
	case DSHPprop_fTopLine:
	    ds->ds_fTopLine= val != 0;
	    break;
	case DSHPprop_fRightLine:
	    ds->ds_fRightLine= val != 0;
	    break;
	case DSHPprop_fBottomLine:
	    ds->ds_fBottomLine= val != 0;
	    break;

	case DSHPprop_fColumnHitTestLine:
	    ds->ds_fColumnHitTestLine= val != 0;
	    break;
	case DSHPprop_fLeftHitTestLine:
	    ds->ds_fLeftHitTestLine= val != 0;
	    break;
	case DSHPprop_fTopHitTestLine:
	    ds->ds_fTopHitTestLine= val != 0;
	    break;
	case DSHPprop_fRightHitTestLine:
	    ds->ds_fRightHitTestLine= val != 0;
	    break;
	case DSHPprop_fBottomHitTestLine:
	    ds->ds_fBottomHitTestLine= val != 0;
	    break;

	case DSHPprop_lineFillShape:
	    ds->ds_lineFillShape= val != 0;
	    break;
	case DSHPprop_lineColumnFillShape:
	    ds->ds_lineColumnFillShape= val != 0;
	    break;
	case DSHPprop_lineLeftFillShape:
	    ds->ds_lineLeftFillShape= val != 0;
	    break;
	case DSHPprop_lineTopFillShape:
	    ds->ds_lineTopFillShape= val != 0;
	    break;
	case DSHPprop_lineRightFillShape:
	    ds->ds_lineRightFillShape= val != 0;
	    break;
	case DSHPprop_lineBottomFillShape:
	    ds->ds_lineBottomFillShape= val != 0;
	    break;

	case DSHPprop_fInsetPen:
	    ds->ds_fInsetPen= val != 0;
	    break;
	case DSHPprop_fLeftInsetPen:
	    ds->ds_fLeftInsetPen= val != 0;
	    break;
	case DSHPprop_fTopInsetPen:
	    ds->ds_fTopInsetPen= val != 0;
	    break;
	case DSHPprop_fRightInsetPen:
	    ds->ds_fRightInsetPen= val != 0;
	    break;
	case DSHPprop_fBottomInsetPen:
	    ds->ds_fBottomInsetPen= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpGroupedNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_relLeft:
	    ds->dsRelRect.drX0= val;
	    break;
	case DSHPprop_relTop:
	    ds->dsRelRect.drY0= val;
	    break;
	case DSHPprop_relRight:
	    ds->dsRelRect.drX1= val;
	    break;
	case DSHPprop_relBottom:
	    ds->dsRelRect.drY1= val;
	    break;

	case DSHPprop_relRotation:
	    ds->dsRelRotation= val;
	    break;
	case DSHPprop_lidRegroup:
	    ds->dsRegroupID= val;
	    break;

	case DSHPprop_groupLeft:
	    ds->dsGroupRect.drX0= val;
	    break;
	case DSHPprop_groupTop:
	    ds->dsGroupRect.drY0= val;
	    break;
	case DSHPprop_groupRight:
	    ds->dsGroupRect.drX1= val;
	    break;
	case DSHPprop_groupBottom:
	    ds->dsGroupRect.drY1= val;
	    break;

	case DSHPprop_fRelChangePage:
	    ds->ds_fRelChangePage= val != 0;
	    break;
	case DSHPprop_fRelFlipH:
	    ds->ds_fRelFlipH= val != 0;
	    break;
	case DSHPprop_fRelFlipV:
	    ds->ds_fRelFlipV= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpShadowNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_shadowType:
	    ds->dsShadowType= val;
	    break;
	case DSHPprop_shadowOpacity:
	    ds->dsShadowOpacity= val;
	    break;

	case DSHPprop_shadowOffsetX:
	    ds->dsShadowOffsetXEmu= val;
	case DSHPprop_shadowOffsetY:
	    ds->dsShadowOffsetYEmu= val;
	    break;

	case DSHPprop_shadowSecondOffsetX:
	    ds->dsShadowOffset2XEmu= val;
	case DSHPprop_shadowSecondOffsetY:
	    ds->dsShadowOffset2YEmu= val;
	    break;

	case DSHPprop_shadowScaleXToX:
	    ds->dsShadowScaleXToX= val;
	case DSHPprop_shadowScaleYToX:
	    ds->dsShadowScaleYToX= val;
	case DSHPprop_shadowScaleXToY:
	    ds->dsShadowScaleXToY= val;
	case DSHPprop_shadowScaleYToY:
	    ds->dsShadowScaleYToY= val;
	    break;

	case DSHPprop_shadowPerspectiveX:
	    ds->dsShadowPerspectiveX= val;
	case DSHPprop_shadowPerspectiveY:
	    ds->dsShadowPerspectiveY= val;
	    break;

	case DSHPprop_shadowWeight:
	    ds->dsShadowWeight= val;
	    break;

	case DSHPprop_shadowOriginX:
	    ds->dsShadowOriginX= val;
	case DSHPprop_shadowOriginY:
	    ds->dsShadowOriginY= val;
	    break;

	case DSHPprop_fShadow:
	    ds->ds_fShadow= val != 0;
	    break;
	case DSHPprop_fshadowObscured:
	    ds->ds_fshadowObscured= val != 0;
	    break;
	case DSHPprop_fShadowOK:
	    ds->ds_fshadowOK= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpConnectNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_cxk:
	    ds->dsConnectionSite= val;
	    break;
	case DSHPprop_cxstyle:
	    ds->dsConnectionStyle= val;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpGeometryNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_geoLeft:
	    ds->dsGeoRect.drX0= val;
	    break;
	case DSHPprop_geoTop:
	    ds->dsGeoRect.drY0= val;
	    break;
	case DSHPprop_geoRight:
	    ds->dsGeoRect.drX1= val;
	    break;
	case DSHPprop_geoBottom:
	    ds->dsGeoRect.drY1= val;
	    break;

	case DSHPprop_adjustValue:
	    ds->dsAdjustValue= val;
	    break;
	case DSHPprop_adjust2Value:
	    ds->dsAdjust2Value= val;
	    break;
	case DSHPprop_adjust3Value:
	    ds->dsAdjust3Value= val;
	    break;
	case DSHPprop_adjust4Value:
	    ds->dsAdjust4Value= val;
	    break;
	case DSHPprop_adjust5Value:
	    ds->dsAdjust5Value= val;
	    break;
	case DSHPprop_adjust6Value:
	    ds->dsAdjust6Value= val;
	    break;
	case DSHPprop_adjust7Value:
	    ds->dsAdjust7Value= val;
	    break;
	case DSHPprop_adjust8Value:
	    ds->dsAdjust8Value= val;
	    break;
	case DSHPprop_adjust9Value:
	    ds->dsAdjust9Value= val;
	    break;
	case DSHPprop_adjust10Value:
	    ds->dsAdjust10Value= val;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpTxboxNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_dxTextLeft:
	    ds->ds_dxTextLeft= val;
	    break;
	case DSHPprop_dyTextTop:
	    ds->ds_dyTextTop= val;
	    break;
	case DSHPprop_dxTextRight:
	    ds->ds_dxTextRight= val;
	    break;
	case DSHPprop_dyTextBottom:
	    ds->ds_dyTextBottom= val;
	    break;

	case DSHPprop_scaleText:
	    ds->ds_scaleText= val;
	    break;
	case DSHPprop_lTxid:
	    ds->ds_lTxid= val;
	    break;

	case DSHPprop_WrapText:
	    ds->ds_WrapText= val;
	    break;
	case DSHPprop_anchorText:
	    ds->ds_anchorText= val;
	    break;
	case DSHPprop_txflTextFlow:
	    ds->ds_txflTextFlow= val;
	    break;
	case DSHPprop_cdirFont:
	    ds->ds_cdirFont= val;
	    break;

	case DSHPprop_fAutoTextMargin:
	    ds->ds_fAutoTextMargin= val != 0;
	    break;
	case DSHPprop_fRotateText:
	    ds->ds_fRotateText= val != 0;
	    break;
	case DSHPprop_fSelectText:
	    ds->ds_fSelectText= val != 0;
	    break;
	case DSHPprop_fFitShapeToText:
	    ds->ds_fFitShapeToText= val != 0;
	    break;
	case DSHPprop_fFitTextToShape:
	    ds->ds_fFitTextToShape= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpNumber(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    switch( rcw->rcwID )
	{
	case DSHPprop_shapePath:
	    ds->ds_shapePath= val;
	    break;

	case DSHPprop_pictureActive:
	case DSHPprop_pictureId:
	    break;

	default:
	    /*SDEB(rcw->rcwWord);*/
	    break;
	}

    return 0;
    }

int docRtfShpColor(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc	)
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    long		val;
    RGB8Color		rgb8;

    if  ( docRtfShpGetNumber( &val, sis, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    bmInitRGB8Color( &rgb8 );
    rgb8.rgb8Red= val & 255; val /= 256;
    rgb8.rgb8Green= val & 255; val /= 256;
    rgb8.rgb8Blue= val & 255; val /= 256;

    switch( rcw->rcwID )
	{
	case DSHPprop_fillColor:
	    ds->dsFillColor= rgb8;
	    break;
	case DSHPprop_fillBackColor:
	    ds->dsFillBackColor= rgb8;
	    break;
	case DSHPprop_fillCrMod:
	    ds->dsFillCrModColor= rgb8;
	    break;

	case DSHPprop_lineColor:
	    ds->dsLineColor= rgb8;
	    break;
	case DSHPprop_lineBackColor:
	    ds->dsLineBackColor= rgb8;
	    break;
	case DSHPprop_lineCrMod:
	    ds->dsLineCrModColor= rgb8;
	    break;

	case DSHPprop_shadowColor:
	    ds->dsShadowColor= rgb8;
	    break;
	case DSHPprop_shadowHighlight:
	    ds->dsShadowHighlightColor= rgb8;
	    break;
	case DSHPprop_shadowCrMod:
	    ds->dsShadowCrModColor= rgb8;
	    break;
	case DSHPprop_c3DExtrusionColor:
	    ds->dsShadowc3DExtrusionColor= rgb8;
	    break;
	case DSHPprop_c3DCrMod:
	    ds->dsShadowc3DCrModColor= rgb8;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShapeProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				control;
    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				locArg= -1;
    int				c;

    int				i;

    const RtfControlWord *	rcwSn;

    control= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sn" ) )
	{ SDEB(controlWord); return -1;	}

    i= 0;
    for (;;)
	{
	c= sioInGetCharacter( sis );
	if  ( c == EOF )
	    { LDEB(c); return -1;	}
	if  ( i >= TEDszRTFCONTROL )
	    {
	    LLDEB(i,TEDszRTFCONTROL);
	    controlWord[i]= '\0';
	    SDEB(controlWord); return -1;
	    }
	if  ( c == '}' )
	    { controlWord[i]= '\0'; break;	}

	controlWord[i++]= c;
	}

    rcwSn= docRtfFindShapePropertyWord( controlWord );
    if  ( ! rcwSn )
	{ SXDEB(controlWord,rcwSn);	}

    control= docRtfFindControl( sis, rrc, &c,
					    controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sv" ) )
	{ SDEB(controlWord); return -1;	}

    if  ( rcwSn )
	{
	if  ( (*rcwSn->rcwApply)( sis, rcwSn, 0, rrc ) )
	    {  SDEB(controlWord); return -1;	}

	c= sioInGetCharacter( sis );
	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetCharacter( sis );
	    }
	
	if  ( c != '}' )
	    { CDEB(c); return -1;	}
	}
    else{
	if  ( docRtfSkipGroup( sis, rcw, DOClevTEXT, rrc ) )
	    { LDEB(1); return -1;	}
	}

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}
    
    if  ( c != '}' )
	{ CDEB(c); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read shape instructions.						*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShpinstGroups[]=
    {
	{ "shptxt",	RTFidSHPTXT,	DOClevPARA, docRtfShpText, },
	{ "sp",		RTFidSP,	DOClevPARA, docRtfShapeProperty, },
    };

static int docRtfShpinst(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, docRtfShpinstGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfShprslt(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    rrc->rrcInIgnoredGroup++;

    res= docRtfReadGroup( sis, DOClevANY,
				(RtfControlWord *)0, 0, 0, rrc,
				(const RtfControlWord *)0,
				(const RtfControlWord *)0,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcInIgnoredGroup--;

    return res;
    }

/************************************************************************/
/*									*/
/*  Read a shape or a drawing object that is translayed to a shape.	*/
/*									*/
/*  1)  Line from drawing objects: Adapt to it.				*/
/*									*/
/************************************************************************/

static int docRtfReadShapeIntern(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc,
					const RtfControlWord *	shapeGroups )
    {
    int			res;
    DrawingShape *	parent= rrc->rrcDrawingShape;
    DrawingShape *	ds;

    ds= malloc( sizeof(DrawingShape) );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docInitDrawingShape( ds );
    ds->dsShapeType= rcw->rcwID;

    rrc->rrcDrawingShape= ds;

    res= docRtfReadGroup( sis, DOClevTEXT,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, shapeGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{
	SLDEB(rcw->rcwWord,res);
	docFreeDrawingShape( rrc->rrcBd, ds );
	}
    else{
	BufferItem *		bi= rrc->rrcBi;
	int			off= bi->biParaStrlen;
	InsertedObject *	io;
	TextParticule *		tp;

	if  ( docInflateTextString( bi, 1 ) )
	    { LLDEB(bi->biParaStrlen,1); return -1; }

	bi->biParaString[off]= ' ';
	bi->biParaStrlen++;
	bi->biParaString[bi->biParaStrlen]= '\0';

	tp= docInsertObject( rrc->rrcBd, bi, &io,
				-1, off, &(rrc->rrcState->rrsTextAttribute) );
	if  ( ! tp )
	    {
	    LDEB(bi->biParaParticuleCount);
	    docFreeDrawingShape( rrc->rrcBd, ds );
	    res= -1;
	    }
	else{
	    io->ioKind= DOCokDRAWING_SHAPE;
	    io->ioDrawingShape= ds;
	    io->ioInline= 0;

	    /*  1  */
	    if  ( ds->dsShapeType == SHPtyLINE		&&
		  ! ds->dsIsChildShape			&&
		  ds->dsVertexCount == 2		)
		{
		ShapeProperties *	sp= &(ds->dsShapeProperties);

		ShapeArrow		sa;

		/*why?*/
		sa= ds->dsLineStartArrow;
		ds->dsLineStartArrow= ds->dsLineEndArrow;
		ds->dsLineEndArrow= sa;

		if  ( ds->dsVertices[0].svX <= ds->dsVertices[1].svX )
		    {
		    sp->spRect.drX1= sp->spRect.drX0+
				ds->dsVertices[1].svX- ds->dsVertices[0].svX;
		    ds->ds_fFlipH= 0;
		    }
		else{
		    sp->spRect.drX1= sp->spRect.drX0+
				ds->dsVertices[0].svX- ds->dsVertices[1].svX;
		    ds->ds_fFlipH= 1;
		    }
		if  ( ds->dsVertices[0].svY <= ds->dsVertices[1].svY )
		    {
		    sp->spRect.drY1= sp->spRect.drY0+
				ds->dsVertices[1].svY- ds->dsVertices[0].svY;
		    ds->ds_fFlipV= 0;
		    }
		else{
		    sp->spRect.drY1= sp->spRect.drY0+
				ds->dsVertices[0].svY- ds->dsVertices[1].svY;
		    ds->ds_fFlipV= 1;
		    }
		}
	    }
	}

    rrc->rrcDrawingShape= parent;
    return res;
    }

/************************************************************************/
/*									*/
/*  The actual shape as opposed to the result in simpler RTF.		*/
/*									*/
/************************************************************************/

static int docRtfReadChildShape(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

static RtfControlWord	docRtfShapeGroups[]=
    {
	{ "shptxt",	RTFidSHPTXT,	DOClevPARA, docRtfShpText, },
	{ "shprslt",	RTFidSHPRSLT,	DOClevPARA, docRtfShprslt, },
	{ "shpgrp",	SHPtyGROUP,	DOClevPARA, docRtfReadChildShape, },
	{ "shp",	SHPtyUNKNOWN,	DOClevPARA, docRtfReadChildShape, },
	{ "shpinst",	RTFidSHPINST,	DOClevPARA, docRtfShpinst, },

	{ 0, 0, 0, }
    };

static int docRtfReadChildShape(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    int			res;
    DrawingShape *	parent= rrc->rrcDrawingShape;
    DrawingShape *	ds;

    ds= docInsertShapeInParent( parent, -1, rcw->rcwID );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    ds->dsShapeType= rcw->rcwID;
    rrc->rrcDrawingShape= ds;

    res= docRtfReadGroup( sis, DOClevTEXT,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, docRtfShapeGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcDrawingShape= parent;
    return res;
    }

int docRtfReadShape(			SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    return docRtfReadShapeIntern( sis, rcw, arg, rrc, docRtfShapeGroups );
    }


static RtfControlWord	docRtfDoGroups[]=
    {
	{ "dptxbxtext",	RTFidSHPTXT,	DOClevPARA, docRtfShpText, },
    };

int docRtfReadDrawingObject(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    return docRtfReadShapeIntern( sis, rcw, arg, rrc, docRtfDoGroups );
    }

