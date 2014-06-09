#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Map the values of the samples for the individual color components.	*/
/*									*/
/************************************************************************/

static void bmMapLine8(	unsigned char *			b,
			const int *			map,
			int				bypr,
			int				spp )
    {
    int		i;
    int		j;

    j= 0;
    for ( i= 0; i < bypr; i++ )
	{
	*b= map[spp* *b+ ( j % spp )]; j++;

	b++;
	}

    return;
    }

static void bmMapLine4(	unsigned char *			b,
			const int *			map,
			int				bypr,
			int				spp )
    {
    int		i;
    int		j;

    j= 0;
    for ( i= 0; i < bypr; i++ )
	{
	unsigned char	us= 0;

	us |= map[ spp* ( ( *b & 0x0f ) >> 0 ) + ( j % spp ) ] << 0; j++;
	us |= map[ spp* ( ( *b & 0xf0 ) >> 4 ) + ( j % spp ) ] << 4; j++;

	*(b++)= us;
	}

    return;
    }

static void bmMapLine2(	unsigned char *			b,
			const int *			map,
			int				bypr,
			int				spp )
    {
    int		i;
    int		j;

    j= 0;
    for ( i= 0; i < bypr; i++ )
	{
	unsigned char	us= 0;

	us |= map[ spp* ( ( *b & 0x03 ) >> 0 ) + ( j % spp ) ] << 0; j++;
	us |= map[ spp* ( ( *b & 0x0c ) >> 2 ) + ( j % spp ) ] << 2; j++;
	us |= map[ spp* ( ( *b & 0x30 ) >> 4 ) + ( j % spp ) ] << 4; j++;
	us |= map[ spp* ( ( *b & 0xc0 ) >> 6 ) + ( j % spp ) ] << 6; j++;

	*(b++)= us;
	}

    return;
    }

static void bmMapLine1(	unsigned char *			b,
			const int *			map,
			int				bypr,
			int				spp )
    {
    int		i;
    int		j;

    j= 0;
    for ( i= 0; i < bypr; i++ )
	{
	unsigned char	us= 0;

	us |= map[ spp* ( ( *b & 0x01 ) >> 0 ) + ( j % spp ) ] << 0; j++;
	us |= map[ spp* ( ( *b & 0x02 ) >> 1 ) + ( j % spp ) ] << 1; j++;
	us |= map[ spp* ( ( *b & 0x04 ) >> 2 ) + ( j % spp ) ] << 2; j++;
	us |= map[ spp* ( ( *b & 0x08 ) >> 3 ) + ( j % spp ) ] << 3; j++;
	us |= map[ spp* ( ( *b & 0x10 ) >> 4 ) + ( j % spp ) ] << 4; j++;
	us |= map[ spp* ( ( *b & 0x20 ) >> 5 ) + ( j % spp ) ] << 5; j++;
	us |= map[ spp* ( ( *b & 0x40 ) >> 6 ) + ( j % spp ) ] << 6; j++;
	us |= map[ spp* ( ( *b & 0x80 ) >> 7 ) + ( j % spp ) ] << 7; j++;

	*(b++)= us;
	}

    return;
    }

int bmMapImageColors(		const BitmapDescription *	bd,
				const int *			map,
				unsigned char *			buffer )
    {
    int			i;

    int			spp= bd->bdSamplesPerPixel;
    int			bypr= bd->bdBytesPerRow;

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB:
	    switch( bd->bdBitsPerSample )
		{
		case 1:
		    for ( i= 0; i < bd->bdPixelsHigh; i++ )
			{ bmMapLine1( buffer+ i* bypr, map, bypr, spp ); }
		    break;

		case 2:
		    for ( i= 0; i < bd->bdPixelsHigh; i++ )
			{ bmMapLine2( buffer+ i* bypr, map, bypr, spp ); }
		    break;

		case 4:
		    for ( i= 0; i < bd->bdPixelsHigh; i++ )
			{ bmMapLine4( buffer+ i* bypr, map, bypr, spp ); }
		    break;

		case 8:
		    for ( i= 0; i < bd->bdPixelsHigh; i++ )
			{ bmMapLine8( buffer+ i* bypr, map, bypr, spp ); }
		    break;

		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
		    return -1;
		}
	    break;

	case BMcoRGB8PALETTE:
	    switch( spp )
		{
		RGB8Color *	rgb8;

		case 3:
		    rgb8= bd->bdRGB8Palette;
		    for ( i= 0; i < bd->bdColorCount; rgb8++, i++ )
			{
			rgb8->rgb8Red=	 map[spp* rgb8->rgb8Red+ 0];
			rgb8->rgb8Green= map[spp* rgb8->rgb8Green+ 1];
			rgb8->rgb8Blue=	 map[spp* rgb8->rgb8Blue+ 2];
			}
		    break;

		case 4:
		    rgb8= bd->bdRGB8Palette;
		    for ( i= 0; i < bd->bdColorCount; rgb8++, i++ )
			{
			rgb8->rgb8Red=	 map[spp* rgb8->rgb8Red+ 0];
			rgb8->rgb8Green= map[spp* rgb8->rgb8Green+ 1];
			rgb8->rgb8Blue=	 map[spp* rgb8->rgb8Blue+ 2];
			rgb8->rgb8Alpha= map[spp* rgb8->rgb8Red+ 3];
			}
		    break;

		default:
		    LLDEB(bd->bdColorEncoding,spp);
		    return -1;
		}
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    return 0;
    }

