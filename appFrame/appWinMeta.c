#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appWinMeta.h>
#   include	<utilPs.h>
#   include	<utilMatchFont.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

static void appWinMetaSetTransform(	DeviceContext *		dc,
					int			mapMode,
					int			xWinOrg,
					int			yWinOrg,
					int			xWinExt,
					int			yWinExt,
					int			xViewOrg,
					int			yViewOrg,
					int			xViewExt,
					int			yViewExt,
					int			twipsWide,
					int			twipsHigh )
    {
    int		xwe;
    int		ywe;
    int		xve;
    int		yve;

    switch( mapMode )
	{
	case MM_TEXT:
	case MM_LOMETRIC:
	case MM_HIMETRIC:
	case MM_LOENGLISH:
	case MM_HIENGLISH:
	    LDEB(mapMode);
	    break;

	case MM_TWIPS:
	    dc->dcTransform.dct_xWinOrg= 0;
	    dc->dcTransform.dct_yWinOrg= 0;
	    dc->dcTransform.dct_xWinExt=  twipsWide;
	    dc->dcTransform.dct_yWinExt= -twipsHigh;

	    dc->dcTransform.dct_xViewOrg= 0;
	    dc->dcTransform.dct_yViewOrg= 0;
	    dc->dcTransform.dct_xViewExt= twipsWide;
	    dc->dcTransform.dct_yViewExt= twipsHigh;

	    break;

	case MM_ISOTROPIC:
	    xwe= xWinExt;
	    ywe= yWinExt;
	    xve= xViewExt;
	    yve= yViewExt;

	    if  ( xwe < 0 )	{ xwe= -xwe;	}
	    if  ( ywe < 0 )	{ ywe= -ywe;	}
	    if  ( xve < 0 )	{ xve= -xve;	}
	    if  ( yve < 0 )	{ yve= -yve;	}

	    if  ( xwe < ( ywe* xve )/ yve )
		{ xwe=  ( ywe* xve )/ yve;	}
	    else{
		if  ( ywe < ( xwe* yve )/ xve )
		    { ywe=  ( xwe* yve )/ xve;	}
		}

	    if  ( xWinExt < 0 )
		{ xWinExt= -xwe;	}
	    else{ xWinExt=  xwe;	}

	    if  ( yWinExt < 0 )
		{ yWinExt= -ywe;	}
	    else{ yWinExt=  ywe;	}

	    dc->dcTransform.dct_xWinOrg= xWinOrg;
	    dc->dcTransform.dct_yWinOrg= yWinOrg;
	    dc->dcTransform.dct_xWinExt= xWinExt;
	    dc->dcTransform.dct_yWinExt= yWinExt;

	    dc->dcTransform.dct_xViewOrg= xViewOrg;
	    dc->dcTransform.dct_yViewOrg= yViewOrg;
	    dc->dcTransform.dct_xViewExt= xViewExt;
	    dc->dcTransform.dct_yViewExt= yViewExt;

	    break;

	case MM_ANISOTROPIC:
	    dc->dcTransform.dct_xWinOrg= xWinOrg;
	    dc->dcTransform.dct_yWinOrg= yWinOrg;
	    dc->dcTransform.dct_xWinExt= xWinExt;
	    dc->dcTransform.dct_yWinExt= yWinExt;

	    dc->dcTransform.dct_xViewOrg= xViewOrg;
	    dc->dcTransform.dct_yViewOrg= yViewOrg;
	    dc->dcTransform.dct_xViewExt= xViewExt;
	    dc->dcTransform.dct_yViewExt= yViewExt;

	    break;

	default:
	    LDEB(mapMode);
	    break;
	}

    dc->dcMapMode= mapMode;

    return;
    }

int appMetaInitDeviceContext(	DeviceContext *			dc,
				const PostScriptFontList *	psfl,
				int				objectCount,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				xViewExt,
				int				yViewExt,
				int				twipsWide,
				int				twipsHigh )
    {
    dc->dcDrawBorders= 1;
    dc->dcFillInsides= 1;
    dc->dcString= (unsigned char *)0;

    dc->dcFillHatched= 0;
    dc->dcFillPattern= 0;

    docInitFontList( &(dc->x_dcFontList) );
    dc->dcPostScriptFontList= psfl;
    appInitDrawingData( &(dc->dcDrawingData) );

    dc->dcPen.lpStyle= PS_SOLID;
    dc->dcPen.lpWidth= 1;
    dc->dcPen.lpColor.rgb8Red= 0;
    dc->dcPen.lpColor.rgb8Green= 0;
    dc->dcPen.lpColor.rgb8Blue= 0;

    dc->dcBrush.lbStyle= BS_SOLID;
    dc->dcBrush.lbColor.rgb8Red= 255;
    dc->dcBrush.lbColor.rgb8Green= 255;
    dc->dcBrush.lbColor.rgb8Blue= 255;

    dc->dcBkColor.rgb8Red= 255;
    dc->dcBkColor.rgb8Green= 255;
    dc->dcBkColor.rgb8Blue= 255;

    dc->dcTextColor.rgb8Red= 0;
    dc->dcTextColor.rgb8Green= 0;
    dc->dcTextColor.rgb8Blue= 0;

    dc->dcBkMode= BKMODE_OPAQUE;

    dc->dcExtraTextSpacing= 0;
    dc->dcJustificationAmount= 0;
    dc->dcJustificationSpaces= 0;
    dc->dcTextAlignment= TA_LEFT|TA_TOP;

    appWinMetaSetTransform( dc, mapMode,
				    0, 0, xWinExt, yWinExt,
				    0, 0, xViewExt, yViewExt,
				    twipsWide, twipsHigh );
    dc->dcTransform.dctPushed= (DeviceCoordinateTransform *)0;

    dc->dcTwipsWide= twipsWide;
    dc->dcTwipsHigh= twipsHigh;

    dc->dcClipX0= 0;
    dc->dcClipY0= 0;
    dc->dcClipX1= 0;
    dc->dcClipY1= 0;

    dc->dcObjects= (MetaFileObject *)0;
    dc->dcObjectCount= 0;

    if  ( objectCount > 0 )
	{
	int		ob;

	dc->dcObjects= (MetaFileObject *)
			    malloc( objectCount* sizeof(MetaFileObject) );
	if  ( ! dc->dcObjects )
	    { XDEB(dc->dcObjects); return -1;	}

	for ( ob= 0; ob < objectCount; ob++ )
	    { dc->dcObjects[ob].mfoType= MFtypeFREE;	}

	dc->dcObjectCount= objectCount;
	}

    dc->dcPoints= (APP_POINT *)0;
    dc->dcAfi= (AfmFontInfo *)0;
    dc->dcFontEncoding= -1;

    dc->dcRedSet= -1;
    dc->dcGreenSet= -1;
    dc->dcBlueSet= -1;
    dc->dcPenSet.lpStyle= -1;

    utilInitTextAttribute( &(dc->dcTextAttributeSet) );

    return 0;
    }

void appMetaCleanObject( MetaFileObject *	mfo )
    {
    AppBitmapImage *	abi;

    switch( mfo->mfoType )
	{
	case MFtypeFREE:
	    return;
	case MFtypeBRUSH:
	    return;
	case MFtypePEN:
	    return;
	case MFtypeFONT:
	    return;
	case MFtypePALETTE:
	    return;
	case MFtypePATTERNBRUSH:
	    abi= &(mfo->mfoPatternBrush.pbAbi);
	    appCleanBitmapImage( abi );
	    appInitBitmapImage( abi );
	    return;

	default:
	    LDEB(mfo->mfoType);
	    return;
	}
    }

void appMetaCleanDeviceContext(	DeviceContext *		dc )
    {
    int		ob;

    if  ( dc->dcString )
	{ free( dc->dcString );	}

    docCleanFontList( &(dc->x_dcFontList) );
    appCleanDrawingData( &(dc->dcDrawingData) );

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType != MFtypeFREE )
	    { appMetaCleanObject( dc->dcObjects+ ob );	}
	}

    if  ( dc->dcObjects )
	{ free( dc->dcObjects );	}

    if  ( dc->dcPoints )
	{ free( dc->dcPoints );	}

    while( dc->dcTransform.dctPushed )
	{
	DeviceCoordinateTransform *	toFree= dc->dcTransform.dctPushed;

	dc->dcTransform= *dc->dcTransform.dctPushed;

	free( toFree );
	}

    return;
    }

int appMetaGetColor(		SimpleInputStream *	sis,
				RGB8Color *		rgb8 )
    {
    rgb8->rgb8Red= sioInGetCharacter( sis );
    rgb8->rgb8Green= sioInGetCharacter( sis );
    rgb8->rgb8Blue= sioInGetCharacter( sis );

    (void) sioInGetCharacter( sis );

    return 0;
    }

int appMetaCreateBrushIndirect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			ob;
    LogicalBrush *	lb;
    char *		hatch="";
    char *		style="";

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    dc->dcObjects[ob].mfoType= MFtypeBRUSH;

    lb= &(dc->dcObjects[ob].mfoLogicalBrush);
    lb->lbTilePixmap= (APP_BITMAP_IMAGE)0;
    lb->lbStyle= sioEndianGetLeInt16( sis );
    
    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    style= "SOLID";
	    appMetaGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_HOLLOW:
	    style= "HOLLOW";
	    appMetaGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_HATCHED:
	    style= "HATCHED";
	    appMetaGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_PATTERN:
	    style= "PATTERN";
	    appMetaGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	default:
	    LDEB(dc->dcObjects[ob].mfoLogicalBrush.lbStyle);
	}

    switch( lb->lbHatch )
	{
	case HS_HORIZONTAL:	hatch= "HORIZONTAL";	break;
	case HS_VERTICAL:	hatch= "VERTICAL";	break;
	case HS_FDIAGONAL:	hatch= "FDIAGONAL";	break;
	case HS_BDIAGONAL:	hatch= "BDIAGONAL";	break;
	case HS_CROSS:		hatch= "CROSS";		break;
	case HS_DIAGCROSS:	hatch= "DIAGCROSS";	break;
	default:
	    break;
	}

    WMFDEB(appDebug("CreateBrushIndirect(%s,%s) ob=%d\n",
						    style, hatch, ob ));

    style= style; hatch= hatch; /* use them */

    return 0;
    }

int appMetaCreatePalette(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			ob;
    int			done;
    int			count;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    dc->dcObjects[ob].mfoType= MFtypePALETTE;

    (void) sioEndianGetLeInt16( sis );
    count= sioEndianGetLeInt16( sis );
    /*
    if  ( 2* count != recordSize- 5 )
	{ LLDEB(count,recordSize);	}
    */
    for ( done= 5; done < recordSize; done++ )
	{ (void) sioEndianGetLeInt16( sis ); }

    WMFDEB(appDebug("CreatePalette(...) ob=%d\n", ob ));

    return 0;
    }

int appMetaCreatePatternBrush(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			ob;
    int			got;
    int			done= 3;
    AppBitmapImage *	abi;

    PatternBrush *	pb;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}

    dc->dcObjects[ob].mfoType= MFtypePATTERNBRUSH;

    pb= &(dc->dcObjects[ob].mfoPatternBrush);

    pb->pbType= 0;
    pb->pbUsage= 0;
    abi= &(pb->pbAbi);
    appInitBitmapImage( abi );
    pb->pbTilePixmap= (APP_BITMAP_IMAGE)0;

    pb->pbType= sioEndianGetLeInt16( sis ); done++;
    pb->pbUsage= sioEndianGetLeInt16( sis ); done++;

    if  ( pb->pbUsage != 0 )
	{
	LLDEB(pb->pbType,pb->pbUsage);
	for ( done= done; done < recordSize; done++ )
	    { (void) sioEndianGetLeInt16( sis ); }
	WMFDEB(appDebug("CreatePatternBrush(...) ob=%d???? recordSize=%d\n",
							    ob, recordSize ));
	return 0;
	}

    switch( pb->pbType )
	{
	case 3:
	case 5:
	    got= bmBmpReadDib( &(abi->abiBitmap), &(abi->abiBuffer), sis );
	    if  ( got < 0 || got > 2* ( recordSize- done ) )
		{
		LLDEB(got,recordSize-done );

		appCleanBitmapImage( abi );
		appInitBitmapImage( abi );
		return -1;
		}
	    if  ( got % 2 )
		{ sioInGetCharacter( sis ); got++;	}
	    done += got/ 2;
	    break;
	default:
	    LLDEB(pb->pbType,pb->pbUsage);
	    for ( done= done; done < recordSize; done++ )
		{ (void) sioEndianGetLeInt16( sis ); }
	    WMFDEB(appDebug("CreatePatternBrush(...) ob=%d???? recordSize=%d\n",
							    ob, recordSize ));
	    return 0;
	}

    while( done < recordSize )
	{ (void) sioEndianGetLeInt16( sis ); done++;	}

    WMFDEB(appDebug("CreatePatternBrush(...) ob=%d recordSize=%d\n",
							    ob, recordSize ));

    return 0;
    }

int appMetaCreatePenIndirect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			ob;
    int			ignore;
    int			done;

    int			x;
    int			y;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    dc->dcObjects[ob].mfoType= MFtypePEN;

    dc->dcObjects[ob].mfoLogicalPen.lpStyle= sioEndianGetLeInt16( sis );
    x= sioEndianGetLeInt16( sis );
    y= sioEndianGetLeInt16( sis );
    dc->dcObjects[ob].mfoLogicalPen.lpWidth= x;
    appMetaGetColor( sis, &dc->dcObjects[ob].mfoLogicalPen.lpColor );

    WMFDEB(appDebug("CreatePenIndirect( style=%d,x,y=%d,%d) ob=%d\n",
				dc->dcObjects[ob].mfoLogicalPen.lpStyle,
				x,y, ob ));

    for ( done= 8; done < recordSize; done++ )
	{ ignore= sioEndianGetLeInt16( sis ); }

    return 0;
    }

int appMetaCreateFontIndirect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int				rval= 0;

    LogicalFont *		lf;

    int				familyStyle= DFstyleFNIL;

    int				count;
    int				done;
    int				ob;

    int				fontNum;
    DocumentFont		dfNew;
    DocumentFont *		df;
    DocumentFontList *		dfl= &(dc->x_dcFontList);
    const PostScriptFontList *	psfl= dc->dcPostScriptFontList;

    docInitDocumentFont( &dfNew );

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    dc->dcObjects[ob].mfoType= MFtypeFONT;

    lf= &(dc->dcObjects[ob].mfoLogicalFont);

    lf->lfHeight= sioEndianGetLeInt16( sis );
    lf->lfWidth= sioEndianGetLeInt16( sis );
    lf->lfEscapement= sioEndianGetLeInt16( sis );
    lf->lfOrientation= sioEndianGetLeInt16( sis );
    lf->lfWeight= sioEndianGetLeInt16( sis );

    lf->lfItalic= sioInGetCharacter( sis );
    lf->lfUnderline= sioInGetCharacter( sis );
    lf->lfStrikeOut= sioInGetCharacter( sis );
    lf->lfCharSet= sioInGetCharacter( sis );
    lf->lfOutPrecision= sioInGetCharacter( sis );
    lf->lfClipPrecision= sioInGetCharacter( sis );
    lf->lfQuality= sioInGetCharacter( sis );
    lf->lfPitchAndFamily= sioInGetCharacter( sis );

    count= 0;
    for ( done= 12; done < recordSize; done++ )
	{
	lf->lfFaceName[count++]= sioInGetCharacter( sis );
	lf->lfFaceName[count++]= sioInGetCharacter( sis );
	}

    lf->lfFaceName[count]= '\0';

    {
    DeviceCoordinateTransform *	dct= &(dc->dcTransform);

    if  ( lf->lfHeight < 0 )
	{ done= ( dc->dcTwipsHigh* -lf->lfHeight )/ dct->dct_yWinExt; }
    else{ done= ( dc->dcTwipsHigh*  lf->lfHeight )/ dct->dct_yWinExt; }

    if  ( done < 0 )
	{ done= -done;	}
    }

    utilInitTextAttribute( &(lf->lfTextAttribute) );

    lf->lfTextAttribute.taFontNumber= 0;
    lf->lfTextAttribute.taFontSizeHalfPoints= ( done+ 5 )/ 10;
    lf->lfTextAttribute.taFontIsBold= lf->lfWeight > 500;
    lf->lfTextAttribute.taFontIsSlanted= lf->lfItalic != 0;
    lf->lfTextAttribute.taTextIsUnderlined= lf->lfUnderline != 0;
    lf->lfTextAttribute.taHasStrikethrough= lf->lfStrikeOut != 0;

    switch( lf->lfPitchAndFamily & 0xf0 )
	{
	case FF_ROMAN:		familyStyle= DFstyleFROMAN;	break;
	case FF_SWISS:		familyStyle= DFstyleFSWISS;	break;
	case FF_MODERN:		familyStyle= DFstyleFMODERN;	break;
	case FF_SCRIPT:		familyStyle= DFstyleFSCRIPT;	break;
	case FF_DECORATIVE:	familyStyle= DFstyleFDECOR;	break;
	case 0x60:		familyStyle= DFstyleFTECH;	break;

	case FF_DONTCARE:
	default:
	    break;
	}

    dfNew.dfCharset= lf->lfCharSet;
    if  ( docFontSetFamilyName( &dfNew, lf->lfFaceName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docFontSetFamilyStyle( &dfNew, familyStyle ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fontNum= docMergeFontIntoFontlist( dfl, &dfNew );
    if  ( fontNum < 0 )
	{ SLDEB(lf->lfFaceName,fontNum); goto ready;	}

    df= dfl->dflFonts+ fontNum;
    if  ( utilFindPsFontForDocFont( df, dfl, psfl ) )
	{ SLDEB(df->dfName,fontNum);	}

    lf->lfTextAttribute.taFontNumber= fontNum;

    lf->lfPrivateFont= -1;

    WMFDEB(appDebug("CreateFontIndirect(h=%d,w=%d,..\"%s\") ob=%d\n",
			    lf->lfHeight,
			    lf->lfWidth,
			    lf->lfFaceName, ob ));

  ready:

    docCleanDocumentFont( &dfNew );

    return rval;
    }

int appMeta_ExtTextOut(		SimpleInputStream *	sis,
				int			recordSize,
				int *			pX0,
				int *			pY0,
				int *			pCount,
				int *			pStyle,
				int *			pX1,
				int *			pY1,
				int *			pH1,
				int *			pW1,
				DeviceContext *		dc )
    {
    int			x0;
    int			y0;
    int			count;
    int			style;

    int			h1;
    int			w1;
    int			y1;
    int			x1;

    int			done;

    int			ignore;
    int			i;

    unsigned char *	str;

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );
    count= sioEndianGetLeInt16( sis );
    style= sioEndianGetLeInt16( sis );

    done= 7;

    if  ( style )
	{
	h1= sioEndianGetLeInt16( sis );
	w1= sioEndianGetLeInt16( sis );
	y1= sioEndianGetLeInt16( sis );
	x1= sioEndianGetLeInt16( sis );

	done += 4;
	}
    else{ h1= w1= y1= x1= 0;	}

    str= (unsigned char *)realloc( dc->dcString, count+ 2 );
    if  ( ! str )
	{ LXDEB(count,str); return -1;	}
    dc->dcString= str;

    for ( i= 0; i < count; i += 2 )
	{
	str[i+0]= sioInGetCharacter( sis );
	str[i+1]= sioInGetCharacter( sis );
	done++;
	}
    str[count]= '\0';

    for ( ; done < recordSize; done++ )
	{ ignore= sioEndianGetLeInt16( sis ); }

    WMFDEB(appDebug( "ExtTextOut( x=%d, y= %d, opts= %d, rect=[%d+%d,%d+%d], ",
					    x0, y0, style, x1, w1, y1, h1 ));
    WMFDEB(appDebug( "\"%s\", count= %d, .... )\n", str, count ));

    *pX0= x0;
    *pY0= y0;
    *pCount= count;
    *pStyle= style;
    *pX1= x1;
    *pY1= y1;
    *pH1= h1;
    *pW1= w1;

    return 0;
    }

int appMeta_TextOut(	SimpleInputStream *	sis,
			int			recordSize,
			int *			pX0,
			int *			pY0,
			int *			pCount,
			DeviceContext *		dc )
    {
    int			x0;
    int			y0;
    int			count;

    int			i;

    unsigned char *	str;

    count= sioEndianGetLeInt16( sis );

    str= (unsigned char *)realloc( dc->dcString, count+ 2 );
    if  ( ! str )
	{ LXDEB(count,str); return -1;	}
    dc->dcString= str;

    for ( i= 0; i < count; i += 2 )
	{
	str[i+0]= sioInGetCharacter( sis );
	str[i+1]= sioInGetCharacter( sis );
	}
    str[count]= '\0';

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("TextOut( \"%s\", x=%d, y= %d )\n", str, x0, y0 ));

    *pX0= x0;
    *pY0= y0;
    *pCount= count;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Manage the current coordinate transform.				*/
/*									*/
/************************************************************************/

int appMetaSetMapMode(		DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		mapMode;

    mapMode= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetMapMode(mapMode=%d)\n", mapMode ));

    appWinMetaSetTransform( dc, mapMode,
				    DC_xWinOrg(dc), DC_yWinOrg(dc),
				    DC_xWinExt(dc), DC_yWinExt(dc),
				    DC_xViewOrg(dc), DC_yViewOrg(dc),
				    DC_xViewExt(dc), DC_yViewExt(dc),
				    dc->dcTwipsWide, dc->dcTwipsHigh );

    return 0;
    }

int appMetaOffsetWindowOrg(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		x0;
    int		y0;

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("OffsetWindowOrg(yWinOrg+=%d,xWinOrg+=%d)\n", y0,x0 ));

    dc->dcTransform.dct_yWinOrg += y0;
    dc->dcTransform.dct_xWinOrg += x0;

    return 0;
    }

int appMetaSetWindowOrg(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		x0;
    int		y0;

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetWindowOrg(yWinOrg=%d,xWinOrg=%d)\n", y0,x0 ));

    dc->dcTransform.dct_yWinOrg= y0;
    dc->dcTransform.dct_xWinOrg= x0;

    return 0;
    }

int appMetaSetWindowExt(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		w;
    int		h;

    h= sioEndianGetLeInt16( sis );
    w= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetWindowExt(yWinExt=%d,xWinExt=%d)\n", h, w ));

    dc->dcTransform.dct_yWinExt= h;
    dc->dcTransform.dct_xWinExt= w;

    /*
    Necessary?
    appWinMetaSetTransform( dc, dc->dcMapMode,
				    DC_xWinOrg(dc), DC_yWinOrg(dc),
				    DC_xWinExt(dc), DC_yWinExt(dc),
				    DC_xViewOrg(dc), DC_yViewOrg(dc),
				    DC_xViewExt(dc), DC_yViewExt(dc),
				    dc->dcTwipsWide, dc->dcTwipsHigh );
    */

    return 0;
    }

int appMetaSetViewportOrg(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		x0;
    int		y0;

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetViewportOrg(yViewOrg=%d,xViewOrg=%d)\n", y0,x0 ));

    dc->dcTransform.dct_yViewOrg= y0;
    dc->dcTransform.dct_xViewOrg= x0;

    return 0;
    }

int appMetaSetViewportExt(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		w;
    int		h;

    h= sioEndianGetLeInt16( sis );
    w= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetViewportExt(yViewExt=%d,xViewExt=%d)\n", h, w ));

    dc->dcTransform.dct_yViewExt= h;
    dc->dcTransform.dct_xViewExt= w;

    return 0;
    }

int appMetaScaleViewportExt(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		yd;
    int		yn;
    int		xd;
    int		xn;

    yd= sioEndianGetLeInt16( sis );
    yn= sioEndianGetLeInt16( sis );
    xd= sioEndianGetLeInt16( sis );
    xn= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetScaleViewportExt(xe*=%d/%d,ye*=%d/%d)\n",
							xn, xd, yn, yd ));

    dc->dcTransform.dct_yViewExt= ( xn* dc->dcTransform.dct_yViewExt )/ yd;
    dc->dcTransform.dct_yViewExt= ( yn* dc->dcTransform.dct_yViewExt )/ yd;

    return 0;
    }

int appMetaScaleWindowExt(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		yd;
    int		yn;
    int		xd;
    int		xn;

    yd= sioEndianGetLeInt16( sis );
    yn= sioEndianGetLeInt16( sis );
    xd= sioEndianGetLeInt16( sis );
    xn= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetScaleWindowExt(xe*=%d/%d,ye*=%d/%d)\n",
							xn, xd, yn, yd ));

    dc->dcTransform.dct_yWinExt= ( xn* dc->dcTransform.dct_yWinExt )/ yd;
    dc->dcTransform.dct_yWinExt= ( yn* dc->dcTransform.dct_yWinExt )/ yd;

    return 0;
    }

int appMetaSaveDC(		DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    DeviceCoordinateTransform *	dct;

    WMFDEB(appDebug("SaveDC()\n" ));

    dct= malloc( sizeof(DeviceCoordinateTransform) );
    if  ( ! dct )
	{ XDEB(dct); return -1;	}

    *dct= dc->dcTransform;
    dc->dcTransform.dctPushed= dct;

    return 0;
    }

int appMetaRestoreDC(		DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		ignore= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("RestoreDC(%d)\n", ignore ));

    if  ( ignore >= 0 )
	{ LDEB(ignore);	}

    if  ( dc->dcTransform.dctPushed )
	{
	DeviceCoordinateTransform *	toFree= dc->dcTransform.dctPushed;

	dc->dcTransform= *dc->dcTransform.dctPushed;

	free( toFree );
	}
    else{ XDEB(dc->dcTransform.dctPushed);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect a series of points.						*/
/*									*/
/************************************************************************/

int appWinMetaGetPoints(	DeviceContext *		dc,
				int			count,
				SimpleInputStream *	sis )
    {
    APP_POINT *		xp;

    int			done;

    xp= (APP_POINT *)realloc( dc->dcPoints, (count+ 1)* sizeof(APP_POINT) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}
    dc->dcPoints= xp;

    for ( done= 0; done < count; xp++, done++ )
	{
	int	x= sioEndianGetLeInt16( sis );
	int	y= sioEndianGetLeInt16( sis );

	xp->x= DC_xViewport( x, dc );
	xp->y= DC_yViewport( y, dc );
	}

    *xp= dc->dcPoints[0];

    return 0;
    }


/************************************************************************/
/*									*/
/*  Handle Clipping by ignoring it.					*/
/*									*/
/************************************************************************/

int appMetaOffsetClipRgn(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int		x0;
    int		y0;

    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("OffsetClipRgn(yClpOrg+=%d,xClpOrg+=%d)\n", y0,x0 ));

    dc->dcClipY0 += y0;
    dc->dcClipX0 += x0;
    dc->dcClipY1 += y0;
    dc->dcClipX1 += x0;

    return 0;
    }

int appMetaExcludeClipRect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			y0;
    int			x0;
    int			y1;
    int			x1;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("ExcludeClipRect(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    return 0;
    }

int appMetaSelectClipRgn(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    long		region;

    switch( recordSize )
	{
	case 4:
	    region= sioEndianGetLeInt16( sis );
	    break;
	case 5:
	    region= sioEndianGetLeInt32( sis );
	    break;
	default:
	    LDEB(recordSize); return -1;
	}

    WMFDEB(appDebug("SelectClipRgn(%ld)\n", region));

    return 0;
    }

int appMetaIntersectClipRect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    int			y0;
    int			x0;
    int			y1;
    int			x1;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("IntersectClipRect(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember text alighnment.						*/
/*									*/
/************************************************************************/

int appMetaSetTextAlign(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    switch( recordSize )
	{
	case 4:
	    dc->dcTextAlignment= sioEndianGetLeInt16( sis );
	    break;
	case 5:
	    dc->dcTextAlignment= sioEndianGetLeInt32( sis );
	    break;
	default:
	    LDEB(recordSize); return -1;
	}

    WMFDEB(appDebug("SetTextAlign( 0x%x: ", dc->dcTextAlignment ));

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:	WMFDEB(appDebug(" NOUPDATECP" )); break;
	case TA_UPDATECP:	WMFDEB(appDebug(" UPDATECP" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x01 ));
	    break;
	}

    switch( dc->dcTextAlignment & 0x07 )
	{
	case TA_LEFT:	WMFDEB(appDebug(" LEFT" )); break;
	case TA_RIGHT:	WMFDEB(appDebug(" RIGHT" )); break;
	case TA_CENTER:	WMFDEB(appDebug(" CENTER" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x07 ));
	    break;
	}

    switch( dc->dcTextAlignment & 0x18 )
	{
	case TA_TOP:		WMFDEB(appDebug(" TOP" )); break;
	case TA_BOTTOM:		WMFDEB(appDebug(" BOTTOM" )); break;
	case TA_BASELINE:	WMFDEB(appDebug(" BASELINE" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x18 ));
	    break;
	}

    WMFDEB(appDebug(" )\n" ));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Color Setting.							*/
/*									*/
/************************************************************************/

int appMetaSetBkColor(		DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    appMetaGetColor( sis, &(dc->dcBkColor) );

    WMFDEB(appDebug("SetBkColor( r=%d, g=%d, b= %d )\n",
					dc->dcBkColor.rgb8Red,
					dc->dcBkColor.rgb8Green,
					dc->dcBkColor.rgb8Blue ));

    return 0;
    }

int appMetaSetTextColor(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    appMetaGetColor( sis, &(dc->dcTextColor) );

    WMFDEB(appDebug("SetTextColor( r=%d, g=%d, b= %d )\n",
					dc->dcTextColor.rgb8Red,
					dc->dcTextColor.rgb8Green,
					dc->dcTextColor.rgb8Blue ));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract a bitmap from the metafile stream.				*/
/*									*/
/************************************************************************/

int appMetaBitmapImage(		AppBitmapImage **	pAbi,
				SimpleInputStream *	sis,
				int			expectBytes )
    {
    AppBitmapImage *	abi;
    int			done;

    abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    done= bmBmpReadDib( &(abi->abiBitmap), &(abi->abiBuffer), sis );
    if  ( done < 0 || done > expectBytes )
	{
	LLDEB(done,expectBytes);
	appCleanBitmapImage( abi ); free( abi );
	return -1;
	}

    while( done < expectBytes )
	{ sioInGetCharacter( sis ); done++;	}

    *pAbi= abi; return 0;
    }

int appMeta_GetCounts(	SimpleInputStream *	sis,
			int			recordSize,
			int *			pCount,
			int **			pCounts,
			DeviceContext *		dc )
    {
    int			count;
    int			i;

    static int *	counts;

    count= sioEndianGetLeInt16( sis );

    counts= (int *)realloc( counts, count* sizeof(int) );
    if  ( ! counts )
	{ LXDEB(count,counts); return -1;	}

    for ( i= 0; i < count; i++ )
	{ counts[i]= sioEndianGetLeInt16( sis ); }

    *pCount= count; *pCounts= counts; return 0;
    }

