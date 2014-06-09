#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMacPict.h>
#   include	<utilPs.h>
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
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Initialize/Clean a mac pict device.					*/
/*									*/
/*  1)  1=BLACK= (foreground), 0=WHITE= (background)			*/
/*									*/
/************************************************************************/

void appMacPictInitDeviceHeader(	MacpictDevice *		md,
					int			outWide,
					int			outHigh,
					int			outTwipsWide,
					int			outTwipsHigh )
    {
    md->mdVersion= 0;

    md->mdInX0= 0;
    md->mdInY0= 0;
    md->mdInX1= 0;
    md->mdInY1= 0;

    md->mdRectX0= 0;
    md->mdRectY0= 0;
    md->mdRectX1= 0;
    md->mdRectY1= 0;

    md->mdOvalX0= 0;
    md->mdOvalY0= 0;
    md->mdOvalX1= 0;
    md->mdOvalY1= 0;

    md->mdPolyX0= 0;
    md->mdPolyY0= 0;
    md->mdPolyX1= 0;
    md->mdPolyY1= 0;

    md->mdRRectX0= 0;
    md->mdRRectY0= 0;
    md->mdRRectX1= 0;
    md->mdRRectY1= 0;
    md->mdRRectOvalWide= 12;
    md->mdRRectOvalHigh= 12;

    md->mdOutWide= outWide;
    md->mdOutHigh= outHigh;
    md->mdOutTwipsWide= outTwipsWide;
    md->mdOutTwipsHigh= outTwipsHigh;

    md->mdPenMode= PENMODE_patOr;
    md->mdPenX= 0;
    md->mdPenY= 0;

    /*  1  */
    memset( md->mdPenPattern, 1, 8 );
    md->mdPenIsSolid= 1;

    /*  1  */
    memset( md->mdFillPattern, 0, 8 );
    md->mdFillIsSolid= 1;

    /*  1  */
    memset( md->mdBackPattern, 0, 8 );
    md->mdBackIsSolid= 1;

    md->mdRedMax= 65536;
    md->mdGreenMax= 65536;
    md->mdBlueMax= 65536;

    bmInitRGB8Color( &(md->mdForeColor) );
    bmInitRGB8Color( &(md->mdBackColor) );
    bmInitRGB8Color( &(md->mdColorSet) );

    md->mdBackColor.rgb8Red= 255;
    md->mdBackColor.rgb8Green= 255;
    md->mdBackColor.rgb8Blue= 255;

    md->mdForeColor.rgb8Red= 0;
    md->mdForeColor.rgb8Green= 0;
    md->mdForeColor.rgb8Blue= 0;

    md->mdFontName= (char *)0;
    utilInitTextAttribute( &(md->mdTextAttribute) );
    md->mdPrivateFont= -1;
    md->mdAfi= (AfmFontInfo *)0;
    md->mdFontEncoding= -1;

    md->mdStippleSet= STIPPLE_UNDEF;
    md->mdFillStippled= 0;
    md->mdTilePixmap= (APP_BITMAP_IMAGE)0;

    md->mdTextString= (unsigned char *)0;
    md->mdTextStringLength= 0;
    md->mdPolyPoints= (APP_POINT *)0;
    md->mdPolyPointCount= 0;

    docInitFontList( &(md->mdFontList) );
    appInitDrawingData( &(md->mdDrawingData) );

    return;
    }

void appMacPictCleanDeviceHeader(	MacpictDevice *		md )
    {
    if  ( md->mdFontName )
	{ free( md->mdFontName ); }
    if  ( md->mdTextString )
	{ free( md->mdTextString ); }
    if  ( md->mdPolyPoints )
	{ free( md->mdPolyPoints ); }

    if  ( md->mdTilePixmap )
	{ appDrawFreePixmap( &(md->mdDrawingData), md->mdTilePixmap );	}

    docCleanFontList( &(md->mdFontList) );
    appCleanDrawingData( &(md->mdDrawingData) );

    return;
    }

/************************************************************************/
/*									*/
/*  Read the header of a macpict metafile.				*/
/*									*/
/*  2)  Read picture size.						*/
/*  3)  Read picture frame.						*/
/*  4)  Read version.							*/
/*									*/
/************************************************************************/

int appMacPictGetDeviceHeader(	MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			pictureSize;
    int			version;

    int			x0;
    int			y0;
    int			x1;
    int			y1;

    int			c;
    int			i;
    long		l;

    /*  2  */
    pictureSize= sioEndianGetBeInt16( sis );

    /*  3  */
    y0= sioEndianGetBeInt16( sis );
    x0= sioEndianGetBeInt16( sis );
    y1= sioEndianGetBeInt16( sis );
    x1= sioEndianGetBeInt16( sis );

    /*  4  */
    c= sioInGetCharacter( sis );
    switch( c )
	{
	case 0:
	    c= sioInGetCharacter( sis );
	    if  ( c != 0x11 )
		{ XDEB(c); return -1;	}

	    version= sioInGetCharacter( sis );
	    if  ( version != 2 )
		{ LDEB(version); return -1;	}

	    c= sioInGetCharacter( sis );
	    if  ( c != 0xff )
		{ XDEB(c); return -1;	}

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0xc00 )
		{ XDEB(c); return -1;	}

	    l= sioEndianGetBeInt32( sis );
	    /*
	    if  ( l != -1 )
		{ LDEB(l); return -1;	}
	    */

	    for ( i= 0; i < 8; i++ )
		{ c= sioEndianGetBeInt16( sis ); }

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); /*return -1;*/	}
	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); /*return -1;*/	}
	    break;

	case 0x11:
	    version= sioInGetCharacter( sis );
	    if  ( version != 1 )
		{ LDEB(version); return -1;	}
	    break;

	default:
	    XDEB(c); return -1;
	}


    md->mdVersion= version;

    md->mdInX0= x0;
    md->mdInY0= y0;
    md->mdInX1= x1;
    md->mdInY1= y1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the next opcode from a macpict metafile.			*/
/*									*/
/************************************************************************/

int appMacPictGetOpcode(	unsigned int *		pOpcode,
				int *			pBytes,
				const MacpictDevice *	md,
				SimpleInputStream *	sis )
    {
    int			bytes= -1;
    unsigned int	opcode;

    switch( md->mdVersion )
	{
	case 1:
	    opcode= sioInGetCharacter( sis );
	    break;
	case 2:
	    opcode= sioEndianGetBeUint16( sis );
	    break;
	default:
	    LDEB(md->mdVersion); return -1;
	}

    if  ( opcode == EOF )
	{ LDEB(opcode); return -1;	}

    bytes= -1;
    if  ( opcode >= 0x100 && opcode <= 0x7fff )
	{
	bytes= 2* ( ( opcode >> 8 ) & 0xff );
	opcode= opcode & 0xff;
	}


    *pBytes= bytes;
    *pOpcode= opcode;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a color.							*/
/*									*/
/************************************************************************/

int appMacPictGetColor(	RGB8Color *		rgb8,
			const MacpictDevice *	md,
			SimpleInputStream *	sis )
    {
    unsigned int	r= sioEndianGetBeUint16( sis );
    unsigned int	g= sioEndianGetBeUint16( sis );
    unsigned int	b= sioEndianGetBeUint16( sis );

    rgb8->rgb8Red= r/ 256;
    rgb8->rgb8Green= g/ 256;
    rgb8->rgb8Blue= b/ 256;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a text string.						*/
/*									*/
/************************************************************************/

int appMacPictGetCountAndString(	MacpictDevice *		md,
					int *			pCount,
					SimpleInputStream *	sis )
    {
    int			count= sioInGetCharacter( sis );

    int			i;
    unsigned char *	fresh;

    fresh= realloc( md->mdTextString, count+ 1 );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}
    md->mdTextString= (unsigned char *)fresh;
    md->mdTextStringLength= count;

    for ( i= 0; i < count; fresh++, i++ )
	{
	*fresh= sioInGetCharacter( sis );

	if  ( *fresh == '\r' )
	    { *fresh= '\n';	}
	else{ *fresh= docMAC_ROMAN_to_ISO1[*fresh];	}
	}
    *fresh= '\0';

    *pCount= count;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the points of a polyline/polygon.				*/
/*									*/
/************************************************************************/

int appMacPictGetPoly(		MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			bytes;
    int			count;

    APP_POINT *		xp;
    int			done;

    bytes= sioEndianGetBeInt16( sis );
    bytes -= 2;
    bytes -= 8;

    if  ( bytes % 4 || bytes <= 4 )
	{ LDEB(bytes); return -1;	}

    count= bytes/ 4;

    xp= (APP_POINT *)realloc( md->mdPolyPoints, (count+ 1)* sizeof(APP_POINT) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}

    md->mdPolyPoints= xp;
    md->mdPolyPointCount= count;

    md->mdPolyY0= sioEndianGetBeInt16( sis );
    md->mdPolyX0= sioEndianGetBeInt16( sis );
    md->mdPolyY1= sioEndianGetBeInt16( sis );
    md->mdPolyX1= sioEndianGetBeInt16( sis );

    for ( done= 0; done < count; xp++, done++ )
	{
	int	y= sioEndianGetBeInt16( sis );
	int	x= sioEndianGetBeInt16( sis );

	xp->x= MD_X( x, md );
	xp->y= MD_Y( y, md );
	}

    *xp= md->mdPolyPoints[0];

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read the colors in a mac pict palette.				*/
/*									*/
/************************************************************************/

int appMacPictReadPaletteColors(	BitmapDescription *	bd,
					int *			pBytesRead,
					const MacpictDevice *	md,
					int			colorCount,
					int			flags,
					SimpleInputStream *	sis )
    {
    int		col;

    RGB8Color *	rgb8;

    int		bytesRead= 0;

    rgb8= realloc( bd->bdRGB8Palette, colorCount* sizeof( RGB8Color ) );
    if  ( ! rgb8 )
	{ LXDEB(colorCount,bd->bdRGB8Palette); return -1;	}
    bd->bdRGB8Palette= rgb8;

    for ( col= 0; col < colorCount; rgb8++, col++ )
	{
	int	idx;

	idx= sioEndianGetBeInt16( sis ); bytesRead += 2;
	if  ( idx < 0 )
	    { LDEB(idx); idx= 0;			}
	if  ( idx >= colorCount )
	    { LLDEB(idx,colorCount); idx= colorCount- 1; }

	if  ( flags & 0x8000 )
	    { idx= col;	}

	rgb8->rgb8Red= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Green= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Blue= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	}

    *pBytesRead= bytesRead;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read one row of PackBits data.					*/
/*									*/
/************************************************************************/

int appMacPictReadPacBitsRow(	unsigned char *			to,
				int *				pBytesRead,
				int				bytesPerRow,
				const MacpictDevice *		md,
				const BitmapDescription *	bd,
				SimpleInputStream *		sis )
    {
    int			byteCount;
    int			done= 0;
    int			bytesRead= 0;

    if  ( bytesPerRow > 250 )
	{ byteCount= sioEndianGetBeInt16( sis ); bytesRead += 2;	}
    else{ byteCount= sioInGetCharacter( sis ); bytesRead++;		}

    /*
    if  ( byteCount > bd->bdBytesPerRow )
	{ LLDEB(byteCount,bd->bdBytesPerRow); return -1;	}
    */

    bytesRead += byteCount;

    while( byteCount > 0 )
	{
	int	c0= sioInGetCharacter( sis );
	int	len;
	int	use;

	if  ( c0 == EOF )
	    { LDEB(c0); return -1;	}

	if  ( c0 == 0x80 )
	    { XDEB(c0); byteCount--; continue;	}

	if  ( c0 & 0x80 )
	    {
	    int		c1= sioInGetCharacter( sis );

	    len= 257- c0; use= len;

	    if  ( done+ len > bd->bdBytesPerRow )
		{
		if  ( ! ( bd->bdBytesPerRow % 2 )	||
		      done+ len > bd->bdBytesPerRow+ 1	)
		    { LLLDEB(done,len,bd->bdBytesPerRow);	}

		use= bd->bdBytesPerRow- done;
		}

	    byteCount -= 2; done += use;

	    while( use > 0 )
		{ *(to++)= c1; use--; }
	    }
	else{
	    len= c0+ 1; use= len;

	    if  ( done+ len > bd->bdBytesPerRow )
		{
		if  ( ! ( bd->bdBytesPerRow % 2 )	||
		      done+ len > bd->bdBytesPerRow+ 1	)
		    { LLLDEB(done,len,bd->bdBytesPerRow);	}

		use= bd->bdBytesPerRow- done;
		}

	    byteCount -= 1+ len; done += use;

	    while( use > 0 )
		{ *(to++)= sioInGetCharacter( sis ); use--; len--; }
	    while( len > 0 )
		{ (void) sioInGetCharacter( sis ); len--; }
	    }
	}

    *pBytesRead= bytesRead;
    return 0;
    }

static int appMacPictAddFont(		MacpictDevice *		md,
					const char *		name )
    {
    int			rval= 0;

    int			fontNum;
    int			fontStyle;

    DocumentFont	dfNew;

    docInitDocumentFont( &dfNew );

    if  ( docFontSetFamilyName( &dfNew, name ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fontStyle= utilFontFamilyStyle( name );
    if  ( docFontSetFamilyStyle( &dfNew, fontStyle ) )
	{ SLDEB(name,fontStyle); rval= -1; goto ready; }

    fontNum= docMergeFontIntoFontlist( &(md->mdFontList), &dfNew );
    if  ( fontNum < 0 )
	{ SLDEB(name,fontNum); rval= -1; goto ready; }

    if  ( md->mdTextAttribute.taFontNumber != fontNum )
	{
	md->mdTextAttribute.taFontNumber= fontNum;
	md->mdPrivateFont= -1;
	md->mdAfi= (AfmFontInfo *)0;
	md->mdFontEncoding= -1;
	}

  ready:
    docCleanDocumentFont( &dfNew );

    return rval;
    }

int appMacPictGetFontName(		MacpictDevice *		md,
					SimpleInputStream *	sis )
    {
    int			rval= 0;

    int			dataLength= sioEndianGetBeInt16( sis );
    int			oldFontId= sioEndianGetBeInt16( sis );
    int			nameLength= sioInGetCharacter( sis );

    int			i;
    char *		fresh;

    oldFontId= oldFontId;

    fresh= realloc( md->mdFontName, nameLength+ 1 );
    if  ( ! fresh )
	{ LXDEB(nameLength,fresh); rval= -1; goto ready;	}
    md->mdFontName= fresh;

    for ( i= 0; i < nameLength; fresh++, i++ )
	{ *fresh= sioInGetCharacter( sis ); }
    *fresh= '\0';

    PICTDEB(appDebug( "FontName( %d, %d, %d, \"%s\" )\n",
	    dataLength, oldFontId, nameLength, md->mdFontName ));

    if  ( dataLength > 3+ nameLength )
	{ LLDEB(dataLength,3+nameLength); }

    for ( i= 3+ nameLength; i < dataLength; i++ )
	{ (void) sioInGetCharacter( sis );	}

    if  ( appMacPictAddFont( md, md->mdFontName ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

int appMacPictGetTxFont(		MacpictDevice *		md,
					SimpleInputStream *	sis )
    {
    int			rval= 0;

    int			val= sioEndianGetBeInt16( sis );
    char *		name= "?";

    PICTDEB(appDebug( "TxFont( %d )\n", val ));

    switch( val )
	{
	case 0:		name= "Chicago";	break;
	case 1:		name= "Helvetica";	break;
	case 2:		name= "Times";		break;
	case 3:		name= "Helvetica";	break;
	case 4:		name= "Courier";	break;
	case 20:	name= "Times";		break;
	case 21:	name= "Helvetica";	break;
	case 22:	name= "Courier";	break;
	case 23:	name= "Symbol";		break;
	case 15011:	name= "Avantgarde";	break;

	default:	goto ready;
	}

    if  ( appMacPictAddFont( md, name ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

int appMacPictGetOvSize(		MacpictDevice *		md,
					SimpleInputStream *	sis )
    {
    md->mdRRectOvalHigh= sioEndianGetBeInt16( sis );
    md->mdRRectOvalWide= sioEndianGetBeInt16( sis );

    PICTDEB(appDebug( "OvSize( %d, %d )\n",
			    md->mdRRectOvalWide, md->mdRRectOvalHigh ));

    return 0;
    }

int appMacPictGetTxFace(		MacpictDevice *		md,
					SimpleInputStream *	sis )
    {
    unsigned int	val= sioInGetCharacter( sis );

    int			isBold= 0;
    int			isSlanted= 0;

    PICTDEB(appDebug( "TxFace( 0x%02x )\n", val ));

    if  ( val & 1 )
	{ isBold= 1; val &= ~1;	}
    if  ( val & 2 )
	{ isSlanted= 1; val &= ~2;	}

    if(val)LDEB(val);

    if  ( md->mdVersion > 1 )
	{ (void)sioInGetCharacter( sis );	}

    if  ( md->mdTextAttribute.taFontIsBold != isBold )
	{
	md->mdTextAttribute.taFontIsBold= isBold;
	md->mdPrivateFont= -1;
	md->mdAfi= (AfmFontInfo *)0;
	md->mdFontEncoding= -1;
	}

    if  ( md->mdTextAttribute.taFontIsSlanted != isSlanted )
	{
	md->mdTextAttribute.taFontIsSlanted= isSlanted;
	md->mdPrivateFont= -1;
	md->mdAfi= (AfmFontInfo *)0;
	md->mdFontEncoding= -1;
	}

    return 0;
    }

int appMacPictGetTxSize(		MacpictDevice *		md,
					SimpleInputStream *	sis )
    {
    int		size= sioEndianGetBeInt16( sis );
    double	scale;

    int		inWide;
    int		inHigh;

    PICTDEB(appDebug( "TxSize( %d )\n", size ));

    inWide= md->mdInX1- md->mdInX0;
    inHigh= md->mdInY1- md->mdInY0;

    scale= sqrt( ( (double)md->mdOutTwipsWide* md->mdOutTwipsHigh )/
						( (double)inWide* inHigh ) );

    size= ( scale* size/ 10 );

    if  ( md->mdTextAttribute.taFontSizeHalfPoints != size )
	{
	md->mdTextAttribute.taFontSizeHalfPoints= size;
	md->mdPrivateFont= -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Arcs:								*/
/*									*/
/*  The origin of arcs is at the top of the circle. The orientation is	*/
/*  clockwise.								*/
/*									*/
/************************************************************************/

int appMacPictGetArc(		MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    md->mdArcY0= sioEndianGetBeInt16( sis );
    md->mdArcX0= sioEndianGetBeInt16( sis );
    md->mdArcY1= sioEndianGetBeInt16( sis );
    md->mdArcX1= sioEndianGetBeInt16( sis );

    md->mdArcA0= sioEndianGetBeInt16( sis );
    md->mdArcA1= sioEndianGetBeInt16( sis );

    return 0;
    }

int appMacPictGetOval(		MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    md->mdOvalY0= sioEndianGetBeInt16( sis );
    md->mdOvalX0= sioEndianGetBeInt16( sis );
    md->mdOvalY1= sioEndianGetBeInt16( sis );
    md->mdOvalX1= sioEndianGetBeInt16( sis );

    return 0;
    }

int appMacPictGetShortLine(	int *			pX0,
				int *			pY0,
				int *			pX1,
				int *			pY1,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			y0= sioEndianGetBeInt16( sis );
    int			x0= sioEndianGetBeInt16( sis );
    int			y1;
    int			x1;
    int			c;

    c= sioInGetCharacter( sis );
    if  ( c & 0x80 )
	{ x1= x0+ c- 256;	}
    else{ x1= x0+ c;	}

    c= sioInGetCharacter( sis );
    if  ( c & 0x80 )
	{ y1= y0+ c- 256;	}
    else{ y1= y0+ c;	}


    PICTDEB(appDebug( "ShortLine( %d+ %d, %d+ %d )\n",
				    x0, x1- x0, y0, y1- y0 ));

    md->mdPenX= x1;
    md->mdPenY= y1;

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

int appMacPictGetShortLineFrom(	int *			pX0,
				int *			pY0,
				int *			pX1,
				int *			pY1,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			y0= md->mdPenY;
    int			x0= md->mdPenX;
    int			y1;
    int			x1;
    int			c;

    c= sioInGetCharacter( sis );
    if  ( c & 0x80 )
	{ x1= x0+ c- 256;	}
    else{ x1= x0+ c;	}

    c= sioInGetCharacter( sis );
    if  ( c & 0x80 )
	{ y1= y0+ c- 256;	}
    else{ y1= y0+ c;	}

    PICTDEB(appDebug( "ShortLineFrom( %d+ %d, %d+ %d )\n",
				    x0, x1- x0, y0, y1- y0 ));


    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

int appMacPictGetLineFrom(	int *			pX0,
				int *			pY0,
				int *			pX1,
				int *			pY1,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int		y0= sioEndianGetBeInt16( sis );
    int		x0= sioEndianGetBeInt16( sis );
    int		y1= md->mdPenY;
    int		x1= md->mdPenX;

    PICTDEB(appDebug( "LineFrom( %d, %d )\n", x0, y0 ));

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

int appMacPictGetLine(		int *			pX0,
				int *			pY0,
				int *			pX1,
				int *			pY1,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			y0= sioEndianGetBeInt16( sis );
    int			x0= sioEndianGetBeInt16( sis );
    int			y1= sioEndianGetBeInt16( sis );
    int			x1= sioEndianGetBeInt16( sis );

    PICTDEB(appDebug( "Line( %d, %d, %d, %d )\n", x0, y0, x1, y1 ));

    md->mdPenX= x1;
    md->mdPenY= y1;

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a pattern.							*/
/*									*/
/************************************************************************/

int appMacPictGetPattern(		MacpictDevice *		md,
					SimpleInputStream *	sis,
					int			unsetStipple,
					int *			pIsSolid,
					unsigned char		pattern[8] )
    {
    int		i;
    int		isSolid;

    pattern[0]= sioInGetCharacter( sis );
    isSolid= ( pattern[0] == 0x00 || pattern[0] == 0xff );

    for ( i= 1; i < 8; i++ )
	{
	pattern[i]= sioInGetCharacter( sis );

	if  ( pattern[i] != pattern[0] )
	    { isSolid= 0;	}
	}

    if  ( md->mdStippleSet == unsetStipple )
	{ md->mdStippleSet=   STIPPLE_UNDEF;	}

    *pIsSolid= isSolid;
    return 0;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect						*/
/*									*/
/*  2)  Get bytes per row and palette flag.				*/
/*  3)  Get image frame.						*/
/*  4)  Default is a black and white image.				*/
/*  5)  For color images, get additional information.			*/
/*  6)  Retrieve palette.						*/
/*  7)  Finish bitmap administration and allocate buffer.		*/
/*  8)  Read source and destination rectangle.				*/
/*  9)  Read raster operation. (Assume copy anyway)			*/
/*  10) Read bitmap data.						*/
/*  11) Later versions of mac pict have an even number of bytes per	*/
/*	operation.							*/
/*									*/
/************************************************************************/

int appMacPictGetImage(			AppBitmapImage *	abi,
					int *			pY0Source,
					int *			pX0Source,
					int *			pY1Source,
					int *			pX1Source,
					int *			pY0Dest,
					int *			pX0Dest,
					int *			pY1Dest,
					int *			pX1Dest,
					MacpictDevice *		md,
					SimpleInputStream *	sis,
					int			packed,
					int			direct,
					int			clipRegion )
    {
    unsigned int	bytesPerRow;
    int			hasPalette= 0;

    int			y0Frame;
    int			x0Frame;
    int			y1Frame;
    int			x1Frame;

    int			y0Source;
    int			x0Source;
    int			y1Source;
    int			x1Source;

    int			y0Dest;
    int			x0Dest;
    int			y1Dest;
    int			x1Dest;

    int			rop;

    BitmapDescription *	bd= &(abi->abiBitmap);

    int			bytesRead= 0;
    int			rowsArePlanar= 0;

    /*  2  */
    if  ( direct )
	{
	int n;
	bytesPerRow= 0;

	n= sioEndianGetBeUint16( sis ); bytesRead += 2;
	n= sioEndianGetBeUint16( sis ); bytesRead += 2;
	n= sioEndianGetBeUint16( sis ); bytesRead += 2;
	}
    else{
	bytesPerRow= sioEndianGetBeUint16( sis ); bytesRead += 2;
	if  ( bytesPerRow & 0x8000 )
	    {
	    hasPalette= 1;
	    bytesPerRow &= ~0x8000;
	    }
	}

    /*  3  */
    y0Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;

    bd->bdPixelsWide= x1Frame- x0Frame;
    bd->bdPixelsHigh= y1Frame- y0Frame;

    bd->bdBytesPerRow= bytesPerRow;

    /*  4  */
    bd->bdBitsPerPixel= 1;
    bd->bdSamplesPerPixel= 1;
    bd->bdBitsPerSample= 1;
    bd->bdColorEncoding= BMcoBLACKWHITE;

    /*  5  */
    if  ( direct || hasPalette )
	{
	int		version;
	int		packType;
	long		packSize;
	int		xRes;
	int		yRes;
	int		pixelType;
	int		planeBytes;
	int		table;
	int		reserved;

	version= sioEndianGetBeInt16( sis ); bytesRead += 2;
	packType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	if  ( packType == 4 )
	    { packed= 1;	}
	packSize= sioEndianGetBeInt32( sis ); bytesRead += 4;
	xRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	yRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	pixelType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdBitsPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdSamplesPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdBitsPerSample= sioEndianGetBeInt16( sis ); bytesRead += 2;
	planeBytes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	table= sioEndianGetBeInt32( sis ); bytesRead += 4;
	reserved= sioEndianGetBeInt32( sis ); bytesRead += 4;

	if  ( bd->bdSamplesPerPixel == 4 )
	    { bd->bdHasAlpha= 1;	}

	if  ( direct && bd->bdSamplesPerPixel > 1 )
	    {
bd->bdColorEncoding= BMcoRGB;
LDEB(bd->bdColorEncoding);
	    if  ( bd->bdSamplesPerPixel == 3 && bd->bdBitsPerSample == 8 )
		{ rowsArePlanar= 1;	}
	    }

	if  ( direct )
	    {
	    bytesPerRow=
		( bd->bdBitsPerPixel* bd->bdPixelsWide+ 7 )/ 8;
	    }

	version= version;
	packType= packType;
	packSize= packSize;
	xRes= xRes;
	yRes= yRes;
	pixelType= pixelType;
	planeBytes= planeBytes;
	table= table;
	reserved= reserved;
	}

    /*  6  */
    if  ( hasPalette )
	{
	long		seed;
	int		flags;
	int		colorCount;

	int		done;

	seed= sioEndianGetBeInt32( sis ); bytesRead += 4;
	flags= sioEndianGetBeUint16( sis ); bytesRead += 2;
	colorCount= sioEndianGetBeInt16( sis )+ 1; bytesRead += 2;

	bd->bdColorEncoding= BMcoRGB8PALETTE;

	if  ( appMacPictReadPaletteColors( bd, &done, md,
						    colorCount, flags, sis ) )
	    { LDEB(hasPalette); return -1;	}

	bytesRead += done;
	}

    /*  7  */
    bmCalculateSizes( bd );

    abi->abiBuffer= malloc( bd->bdBufferLength+
					rowsArePlanar* bd->bdBytesPerRow );
    if  ( ! abi->abiBuffer )
	{
	LXDEB(bd->bdBufferLength,abi->abiBuffer);
	return -1;
	}

    /*  8  */
    y0Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Source= sioEndianGetBeInt16( sis ); bytesRead += 2;

    y0Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;

    /*  9  */
    rop= sioEndianGetBeInt16( sis ); bytesRead += 2;
    if  ( rop != 0 )
	{ LDEB(rop);	}

    if  ( clipRegion )
	{
	int	i;
	int	count= sioEndianGetBeInt16( sis ); bytesRead += 2;

	for ( i= 2; i < count; i++ )
	    { (void) sioInGetCharacter( sis ); bytesRead++; }
	}

    /*  10  */
    if  ( ! packed || bytesPerRow < 8 )
	{
	int		row;

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    unsigned char *	to;
	    int			i;

	    to= abi->abiBuffer+ row* bd->bdBytesPerRow;

	    for ( i= 0; i < bytesPerRow; i++ )
		{ *(to++)= sioInGetCharacter( sis ); bytesRead++;	}
	    }
	}
    else{
	int	row;

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    int			done;
	    unsigned char *	to;

	    if  ( rowsArePlanar )
		{ to= abi->abiBuffer+ bd->bdBufferLength;	}
	    else{ to= abi->abiBuffer+ row* bd->bdBytesPerRow;	}

	    if  ( appMacPictReadPacBitsRow(
				    to, &done, bytesPerRow, md, bd, sis ) )
		{ LDEB(row); return -1; }

	    if  ( rowsArePlanar )
		{
		const unsigned char *	r;
		const unsigned char *	g;
		const unsigned char *	b;
		int			i;

		r= abi->abiBuffer+ bd->bdBufferLength;
		g= r+ bd->bdPixelsWide;
		b= g+ bd->bdPixelsWide;

		to= abi->abiBuffer+ row* bd->bdBytesPerRow;

		for ( i= 0; i < bd->bdPixelsWide; i++ )
		    {
		    *(to++)= *(r++);
		    *(to++)= *(g++);
		    *(to++)= *(b++);
		    }
		}

	    bytesRead += done;
	    }
	}

    /*  11  */
    if  ( md->mdVersion > 1 && bytesRead % 2 )
	{ (void)sioInGetCharacter( sis );	}

    *pY0Source= y0Source;
    *pX0Source= x0Source;
    *pY1Source= y1Source;
    *pX1Source= x1Source;
    *pY0Dest= y0Dest;
    *pX0Dest= x0Dest;
    *pY1Dest= y1Dest;
    *pX1Dest= x1Dest;

    return 0;
    }

int appMacPictGetGlyphState(	MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int	dataLength= sioEndianGetBeInt16( sis );
    int	i;
    int	c;

    int	outlinePreferred= 0;
    int	preserveGlyph= 0;
    int	fractionalWidth= 0;
    int	scalingDisabled= 0;

    for ( i= 0; i < dataLength; i++ )
	{
	c= sioInGetCharacter( sis );

	if  ( i == 0 )
	    { outlinePreferred= c;	}
	if  ( i == 1 )
	    { preserveGlyph= c;	}
	if  ( i == 2 )
	    { fractionalWidth= c;	}
	if  ( i == 3 )
	    { scalingDisabled= c;	}
	}

    PICTDEB(appDebug( "GlyphState( %d:%x,%x,%x,%x )\n",
				dataLength,
				outlinePreferred,
				preserveGlyph,
				fractionalWidth,
				scalingDisabled ));

    md->mdPenX= 0;
    md->mdPenY= 0;

    return 0;
    }

int appMacPictGetDHText(	int *			pCount,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int	dh= sioInGetCharacter( sis );
    int	count;

    if  ( appMacPictGetCountAndString( md, &count, sis ) )
	{ LDEB(1); return -1;	}

    PICTDEB(appDebug( "DHText( %d, %d: \"%s\" )\n",
				dh, count, md->mdTextString ));

    md->mdPenX += dh;

    if  ( md->mdVersion > 1 && count % 2 )
	{ (void)sioInGetCharacter( sis );	}

    *pCount= count;
    return 0;
    }

int appMacPictGetDVText(	int *			pCount,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			dv= sioInGetCharacter( sis );
    int			count;

    if  ( appMacPictGetCountAndString( md, &count, sis ) )
	{ LDEB(1); return -1;	}

    PICTDEB(appDebug( "DVText( %d, %d: \"%s\" )\n",
				dv, count, md->mdTextString ));

    md->mdPenY += dv;

    if  ( md->mdVersion > 1 && count % 2 )
	{ (void)sioInGetCharacter( sis );	}

    *pCount= count;
    return 0;
    }

int appMacPictGetDHDVText(	int *			pCount,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int	dh= sioInGetCharacter( sis );
    int	dv= sioInGetCharacter( sis );
    int	count;

    if  ( appMacPictGetCountAndString( md, &count, sis ) )
	{ LDEB(1); return -1;	}

    PICTDEB(appDebug( "DHDVText( %d, %d, %d: \"%s\" )\n",
				dh, dv, count, md->mdTextString ));

    md->mdPenX += dh;
    md->mdPenY += dv;

    if  ( md->mdVersion > 1 && count % 2 == 0 )
	{ (void)sioInGetCharacter( sis );	}

    *pCount= count;
    return 0;
    }

int appMacPictGetLongText(	int *			pCount,
				MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int	y= sioEndianGetBeInt16( sis );
    int	x= sioEndianGetBeInt16( sis );
    int	count;

    if  ( appMacPictGetCountAndString( md, &count, sis ) )
	{ LDEB(1); return -1;	}

    PICTDEB(appDebug( "LongText( %d, %d, %d: \"%s\" )\n",
				x, y, count, md->mdTextString ));

    md->mdPenX= x;
    md->mdPenY= y;

    if  ( md->mdVersion > 1 && count % 2 == 0 )
	{ (void)sioInGetCharacter( sis );	}

    *pCount= count;
    return 0;
    }

const char * appMacPictCommentName( int			n )
    {
    static char	scratch[22];

    switch( n )
	{
	case PICTCMT_Proprietary:	return "Proprietary";
	case PICTCMT_MacDrawBegin:	return "MacDrawBegin";
	case PICTCMT_MacDrawEnd:	return "MacDrawEnd";
	case PICTCMT_GroupedBegin:	return "GroupedBegin";
	case PICTCMT_GroupedEnd:	return "GroupedEnd";
	case PICTCMT_BitmapBegin:	return "BitmapBegin";
	case PICTCMT_BitmapEnd:		return "BitmapEnd";
	case PICTCMT_ArrowAtEnd:	return "ArrowAtEnd";
	case PICTCMT_ArrowAtStart:	return "ArrowAtStart";
	case PICTCMT_ArrowBothEnds:	return "ArrowBothEnds";
	case PICTCMT_ArrowEnd:		return "ArrowEnd";
	case PICTCMT_SetGray:		return "SetGray";
	case PICTCMT_TextBegin:		return "TextBegin";
	case PICTCMT_TextEnd:		return "TextEnd";
	case PICTCMT_StringBegin:	return "StringBegin";
	case PICTCMT_StringEnd:		return "StringEnd";
	case PICTCMT_TextCenter:	return "TextCenter";
	case PICTCMT_LineLayoutOff:	return "LineLayoutOff";
	case PICTCMT_LineLayoutOn:	return "LineLayoutOn";
	case PICTCMT_ClientLineLayout:	return "ClientLineLayout";
	case PICTCMT_PolyBegin:		return "PolyBegin";
	case PICTCMT_PolyEnd:		return "PolyEnd";
	case PICTCMT_PolyIgnore:	return "PolyIgnore";
	case PICTCMT_PolySmooth:	return "PolySmooth";
	case PICTCMT_PolyClose:		return "PolyClose";
	case PICTCMT_RotateBegin:	return "RotateBegin";
	case PICTCMT_RotateEnd:		return "RotateEnd";
	case PICTCMT_RotateCenter:	return "RotateCenter";
	case PICTCMT_DashedLine:	return "DashedLine";
	case PICTCMT_DashedStop:	return "DashedStop";
	case PICTCMT_SetLineWidth:	return "SetLineWidth";
	case PICTCMT_PostScriptBegin:	return "PostScriptBegin";
	case PICTCMT_PostScriptEnd:	return "PostScriptEnd";
	case PICTCMT_PostScriptHandle:	return "PostScriptHandle";
	case PICTCMT_PostScriptFile:	return "PostScriptFile";
	case PICTCMT_TextIsPostScript:	return "TextIsPostScript";
	case PICTCMT_ResourcePS:	return "ResourcePS";
	case PICTCMT_PSBeginNoSave:	return "PSBeginNoSave";
	case PICTCMT_FormsPrinting:	return "FormsPrinting";
	case PICTCMT_EndFormsPrinting:	return "EndFormsPrinting";
	case PICTCMT_CMBeginProfile:	return "CMBeginProfile";
	case PICTCMT_CMEndProfile:	return "CMEndProfile";
	case PICTCMT_CMEnableMatching:	return "CMEnableMatching";
	case PICTCMT_CMDisableMatching:	return "CMDisableMatching";
	}

    sprintf( scratch, "%d", n );
    return scratch;
    }
