#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a buffer in planar format to chunky format.			*/
/*									*/
/*  No attempt was made to write optimal code.				*/
/*  Only certain 'bits per sample' are supported.			*/
/*									*/
/************************************************************************/

int bmPlanarToChunky(	unsigned char *			to,
			const unsigned char *		from,
			const BitmapDescription *	bd )
    {
    const unsigned char *	fr[6];
    unsigned int		sh[6];
    int				spl;
    int				pixelCount;
    int				pos;
    int				by;

    int				samplesPerByte;
    int				bytesPerComponent;
    unsigned char		mask;
    unsigned int		v;
    int				bits;

    if  ( bd->bdSamplesPerPixel == 1 )
	{
	LDEB(bd->bdSamplesPerPixel);
	memcpy( to, from, bd->bdBufferLength );
	return 0;
	}

    if  ( bd->bdSamplesPerPixel > sizeof(fr)/sizeof(const unsigned char *) )
	{ LDEB(bd->bdSamplesPerPixel); return -1;	}

    bytesPerComponent= bd->bdBufferLength/ bd->bdSamplesPerPixel;
    pixelCount= bd->bdPixelsWide* bd->bdPixelsHigh;

    for ( spl= 0; spl < bd->bdSamplesPerPixel; spl++ )
	{ fr[spl]= from+ spl* bytesPerComponent;	}

    if  ( bd->bdBitsPerSample >= 8 )
	{
	int	bytesPerSample= bd->bdBitsPerSample/ 8;

	if  ( bd->bdBitsPerSample % 8 != 0 )
	    { LDEB(bd->bdBitsPerSample); return -1;	}

	for ( pos= 0; pos < bytesPerComponent; pos++ )
	    {
	    for ( spl= 0; spl < bd->bdSamplesPerPixel; spl++ )
		{
		for ( by= 0; by < bytesPerSample; by++ )
		    { *(to++)= *(fr[spl]++);	}
		}
	    }

	return 0;
	}

    if  ( 8 % bd->bdBitsPerSample != 0 )
	{ LDEB(bd->bdBitsPerSample); return -1; }

/*
memcpy( to, from, bd->bdBufferLength );
return 0;
*/

    samplesPerByte= 8/ bd->bdBitsPerSample;
    mask= 1;
    for ( spl= 0; spl < bd->bdBitsPerSample; spl++ )
	{ mask= 2* mask;	}
    mask--;

    for ( spl= 0; spl < bd->bdSamplesPerPixel; spl++ )
	{
	fr[spl]= from+ spl* bytesPerComponent;
	sh[spl]= 8- bd->bdBitsPerSample;
	}

    v= 0; bits= 0;
    for ( pos= 0; pos < pixelCount; pos++ )
	{
	for ( spl= 0; spl < bd->bdSamplesPerPixel; spl++ )
	    {
	    v |= ( *(fr[spl]) >> sh[spl] ) & mask;

	    bits += bd->bdBitsPerSample;
	    if  ( bits >= 8 )
		{ bits -= 8; *(to++)= v; v= 0;	}
	    else{ v <<= bd->bdBitsPerSample;	}

	    if  ( sh[spl] < bd->bdBitsPerSample )
		{ fr[spl]++; sh[spl]= 8;	}
	    sh[spl] -= bd->bdBitsPerSample;
	    }
	}

    if  ( bits > 0 )
	{ *(to++)= v;	}

    return 0;
    }
