#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*  Translation back and forth between color encoding and string.	*/
/************************************************************************/

const char *	bmcoStrings[]=
    {
    "black on white",
    "white on black",
    "RGB",
    "RGB palette",
    0
    };

const char *	bmcoIntToString( int colorEncodingInt )
    {
    if  ( colorEncodingInt >= 0						&&
	  colorEncodingInt < sizeof( bmcoStrings )/sizeof(char *) -1	)
	{ return bmcoStrings[colorEncodingInt]; }

    LDEB(colorEncodingInt); return (char *)0;
    }

int		bmcoStringToInt( const char * colorEncodingString )
    {
    int	i;

    for ( i= 0; i < sizeof( bmcoStrings )/sizeof(char *) -1; i++ )
	{
	if  ( ! strcmp( colorEncodingString, bmcoStrings[i] ) )
	    { return i;	}
	}

    SDEB(colorEncodingString); return -1;
    }

/************************************************************************/
/*  Translation back and forth between resolution unit and string.	*/
/************************************************************************/

const char *	bmunStrings[]=
    {
    "m",
    "inch",
    "point",
    "pixel",
    0
    };

const char * bmunIntToString( int unitInt )
    {
    if  ( unitInt >= 0						&&
	  unitInt < sizeof( bmcoStrings )/sizeof(char *) -1	)
	{ return bmunStrings[unitInt]; }

    LDEB(unitInt); return (char *)0;
    }

int bmunStringToInt( const char * unitString )
    {
    int	i;

    for ( i= 0; i < sizeof( bmunStrings )/sizeof(char *) -1; i++ )
	{
	if  ( ! strcmp( unitString, bmunStrings[i] ) )
	    { return i;	}
	}

    SDEB(unitString); return -1;
    }

/************************************************************************/
/*									*/
/*  Initialise a BitmapDescription.					*/
/*									*/
/************************************************************************/

void bmInitDescription	( BitmapDescription *	bd )
    {
    bd->bdBufferLength= 0;
    bd->bdBytesPerRow= 0;
    bd->bdPixelsWide= 0;
    bd->bdPixelsHigh= 0;
    bd->bdBitsPerSample= 0;
    bd->bdSamplesPerPixel= 0;
    bd->bdBitsPerPixel= 0;
    bd->bdXResolution= 0;
    bd->bdYResolution= 0;
    bd->bdUnit= BMunILLEGALVALUE;
    bd->bdColorEncoding= BMcoILLEGALVALUE;

    bd->bdHasAlpha= 0;

    /********************************************/
    /*  Or any other member of the union.	*/
    /********************************************/
    bd->bdColorCount= 0;
    bd->bdRGB8Palette= (RGB8Color *)0;
    }

/************************************************************************/
/*									*/
/*  Clean a BitmapDescription.						*/
/*									*/
/************************************************************************/

void bmCleanDescription	( BitmapDescription *	bd )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bd->bdRGB8Palette )
		{ free( bd->bdRGB8Palette );	}
	    break;
	default:
	    break;
	}
    }


/************************************************************************/
/*									*/
/*  Copy a BitmapDescription.						*/
/*									*/
/************************************************************************/

int bmCopyDescription	(	BitmapDescription *		to,
				const BitmapDescription *	from )
    {
    switch( from->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    *to= *from;
	    to->bdRGB8Palette= malloc( to->bdColorCount* sizeof(RGB8Color) );
	    if  ( ! to->bdRGB8Palette )
		{ XDEB(to->bdRGB8Palette); return -1;	}

	    memcpy( to->bdRGB8Palette, from->bdRGB8Palette, 
				    from->bdColorCount* sizeof( RGB8Color ) );
	    break;
	default:
	    *to= *from;
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Byte masks								*/
/*									*/
/************************************************************************/

unsigned char	Bmc1Masks[8]=
    { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

unsigned char	Bmc7Masks[8]=
    { 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe };

/************************************************************************/
/*									*/
/*  Geometry.								*/
/*									*/
/************************************************************************/

void bmImageSizeTwips(	int *				pImageWideTwips,
			int *				pImageHighTwips,
			const BitmapDescription *	bd )
    {
    int		imageWideTwips;
    int		imageHighTwips;

    switch( bd->bdUnit )
	{
	case BMunM:
	    imageWideTwips= (int) ( ( TWIPS_PER_M* bd->bdPixelsWide )/
							bd->bdXResolution );
	    imageHighTwips= (int) ( ( TWIPS_PER_M* bd->bdPixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunINCH:
	    imageWideTwips= (int)( ( 72* 20* bd->bdPixelsWide )/
							bd->bdXResolution );
	    imageHighTwips= (int)( ( 72* 20* bd->bdPixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunPOINT:
	    imageWideTwips= (int)( ( 20* bd->bdPixelsWide )/
							bd->bdXResolution );
	    imageHighTwips= (int)( ( 20* bd->bdPixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunPIXEL:
	    imageWideTwips= 20* bd->bdPixelsWide;
	    imageHighTwips= 20* bd->bdPixelsHigh;
	    break;

	default:
	    LDEB(bd->bdUnit);
	    imageWideTwips= 20* bd->bdPixelsWide;
	    imageHighTwips= 20* bd->bdPixelsHigh;
	    break;
	}

    *pImageWideTwips= imageWideTwips;
    *pImageHighTwips= imageHighTwips;

    return;
    }

/************************************************************************/
/*									*/
/*  Derive the sizes of a description from the other fields.		*/
/*									*/
/************************************************************************/

int bmCalculateSizes(	BitmapDescription *	bd )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 2;	}
	    else{ bd->bdSamplesPerPixel= 1;	}

	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case BMcoRGB:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 4;	}
	    else{ bd->bdSamplesPerPixel= 3;	}

	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 4;	}
	    else{ bd->bdSamplesPerPixel= 3;	}

	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    bd->bdBytesPerRow= ( bd->bdBitsPerPixel* bd->bdPixelsWide+ 7 )/ 8;
    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

    return 0;
    }

