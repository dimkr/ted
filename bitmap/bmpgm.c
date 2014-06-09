#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a PBM/PGM/PPM file.						*/
/*									*/
/************************************************************************/

static int bmPbmGetNumber(	int *	pNum,
				FILE *	f )
    {
    int		num= 0;
    int		c= getc( f );

    for (;;)
	{
	if  ( isspace( c ) )
	    { c= getc( f ); continue;	}

	if  ( isdigit( c ) )
	    { break;	}

	if  ( c == '#' )
	    {
	    for (;;)
		{
		c= getc( f );
		if  ( c == EOF )
		    { CDEB(c); return -1;	}
		if  ( c == '\n' )
		    { c= getc( f ); break;	}
		}

	    continue;
	    }

	CDEB(c); return -1;
	}

    while( isdigit( c ) )
	{ num= 10* num+ c- '0'; c= getc( f ); }

    ungetc( c, f ); *pNum= num; return 0;
    }

int bmReadPbmFile(	const char *		filename,
			unsigned char **	pBuf,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    FILE *		f;
    int			privateFormat;
    int			c;
    int			row;
    int			col;

    int			wide;
    int			high;
    int			mval= 1;
    int			shift;

    unsigned char *	buffer;
    unsigned char *	to;

    f= fopen( filename, "rb" );
    if  ( ! f )
	{ SXDEB(filename,f); return -1;	}

    c= getc( f );
    if  ( c != 'P' )
	{ CDEB(c); fclose( f ); return -1;	}

    c= getc( f );
    switch( c )
	{
	case '1': case '2': case '3': case '4': case '5': case '6':
	    privateFormat= c- '0'; break;
	default:
	    CDEB(c); fclose( f ); return -1;
	}

    if  ( bmPbmGetNumber( &wide, f ) )
	{ LDEB(1); fclose( f ); return -1;	}
    if  ( bmPbmGetNumber( &high, f ) )
	{ LDEB(1); fclose( f ); return -1;	}

    bd->bdXResolution= bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    bd->bdHasAlpha= 0;
    bd->bdColorCount= 0;

    bd->bdPixelsWide= wide;
    bd->bdPixelsHigh= high;

    switch( privateFormat )
	{
	case 1: case 4:
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdBitsPerSample= 1;
	    break;

	case 2: case 5:
	    if  ( bmPbmGetNumber( &mval, f ) )
		{ LDEB(1); fclose( f ); return -1;	}

	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdBitsPerSample= 8;
	    break;

	case 3: case 6:
	    if  ( bmPbmGetNumber( &mval, f ) )
		{ LDEB(1); fclose( f ); return -1;	}

	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= 8;
	    break;

	default:
	    CDEB(c); fclose( f ); return -1;
	}

    bmCalculateSizes( bd );

    buffer= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! buffer )
	{ XDEB(buffer); fclose( f ); return -1;	}

    switch( privateFormat )
	{
	case 1:
	    memset( buffer, 0, bd->bdBufferLength );
	    for ( row= 0; row < high; row++ )
		{
		to= buffer+ row* bd->bdBytesPerRow; shift= 7;
		for ( col= 0; col < wide; col++ )
		    {
		    if  ( bmPbmGetNumber( &c, f ) )
			{
			LLDEB(row,col);
			free( buffer ); fclose( f ); return -1;
			}
		    *to |= ( c != 0 ) << shift;
		    if  ( shift == 0 )
			{ shift= 7; to++;	}
		    else{ shift--;		}
		    }
		}
	    break;

	case 4:
	    c= getc( f );
	    for ( row= 0; row < high; row++ )
		{
		to= buffer+ row* bd->bdBytesPerRow;
		if  ( fread( to, 1, bd->bdBytesPerRow, f ) !=
							bd->bdBytesPerRow )
		    {
		    LDEB(bd->bdBufferLength);
		    free( buffer ); fclose( f ); return -1;
		    }
		}
	    break;

	case 5:
	    switch( bd->bdBitsPerPixel )
		{
		case 8:
		    c= getc( f );
		    if  ( fread( buffer, 1, bd->bdBufferLength, f ) !=
							bd->bdBufferLength )
			{
			LDEB(bd->bdBufferLength);
			free( buffer ); fclose( f ); return -1;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerPixel);
		    free( buffer ); fclose( f ); return -1;
		}
	    break;

	case 2:
	    switch( bd->bdBitsPerPixel )
		{
		case 8:
		    to= buffer;
		    for ( col= 0; col < bd->bdBufferLength; col++ )
			{
			if  ( bmPbmGetNumber( &c, f ) )
			    {
			    LDEB(col);
			    free( buffer ); fclose( f );
			    return -1;
			    }
			if  ( c > 255 )
			    { CDEB(c); c= 255;	}
			*(to++)= ( 255* c )/ mval;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerPixel);
		    free( buffer ); fclose( f ); return -1;
		}
	    break;

	case 3:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    to= buffer;
		    for ( col= 0; col < bd->bdBufferLength; col++ )
			{
			if  ( bmPbmGetNumber( &c, f ) )
			    {
			    LDEB(col);
			    free( buffer ); fclose( f );
			    return -1;
			    }
			if  ( c > 255 )
			    { CDEB(c); c= 255;	}
			*(to++)= ( 255* c )/ mval;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerSample);
		    free( buffer ); fclose( f ); return -1;
		}
	    break;

	case 6:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    c= getc( f );
		    if  ( fread( buffer, 1, bd->bdBufferLength, f ) !=
							bd->bdBufferLength )
			{
			LDEB(bd->bdBufferLength);
			free( buffer ); fclose( f ); return -1;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerSample);
		    free( buffer ); fclose( f ); return -1;
		}
	    break;

	default:
	    free( buffer );
	    LDEB(privateFormat); fclose( f ); return -1;
	}

    fclose( f );

    *pBuf= buffer;
    *pPrivateFormat= privateFormat;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a PGM file.							*/
/*									*/
/************************************************************************/

int bmCanWritePbmFile(	const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
#   if 1
    return -1;
#   else
    if  ( bd->bdColorEncoding != BMcoBLACKWHITE		&&
	  bd->bdColorEncoding != BMcoWHITEBLACK		)
	{ return -1;	}

    if  ( bd->bdBitsPerPixel > 8 )
	{ return -1;	}

    return 0;
#   endif
    }

int bmWritePbmFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { LDEB(-1); return -1; }
