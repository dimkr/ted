/************************************************************************/
/*									*/
/*  MD5 hash/message digest.						*/
/*									*/
/*  Borrowed/adapted from public code by RSA Data Security.		*/
/*  Refer to the copyright notice below from the original version.	*/
/*									*/
/************************************************************************/

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#    include "utilMD5.h"
#    include "utilBase64.h"
#    include "appDebugon.h"

/* Constants for MD5Transform routine.
 */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void utilMD5Transform(	UtilUint32		state[4],
				const unsigned char	block[64] );

static void utilMD5Encode(	unsigned char *		output,
				UtilUint32 *		input,
				unsigned int		len );

static void utilMD5Decode(	UtilUint32 *		output,
				const unsigned char *	input,
				unsigned int		len );

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UtilUint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UtilUint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UtilUint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UtilUint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/************************************************************************/
/*									*/
/*  MD5 initialization. Begins an MD5 operation, writing a new context.	*/
/*									*/
/*  1)  Count= 0.							*/
/*  2)  Load magic initialization constants.				*/
/*									*/
/************************************************************************/

void utilMD5Init(	MD5Context *	md5c )
    {
    /*  1  */
    md5c->md5cCount[0]= md5c->md5cCount[1]= 0;

    /*  2  */
    md5c->md5cState[0]= 0x67452301;
    md5c->md5cState[1]= 0xefcdab89;
    md5c->md5cState[2]= 0x98badcfe;
    md5c->md5cState[3]= 0x10325476;
    }

/************************************************************************/
/*									*/
/*  MD5 block update operation. Continues an MD5 message-digest		*/
/*  operation, processing another message block, and updating the	*/
/*  context.								*/
/*									*/
/*  1)  Compute number of bytes mod 64.					*/
/*  2)  Update number of bits.						*/
/*  3)  Transform as many times as possible.				*/
/*  4)  Buffer remaining input.						*/
/*									*/
/************************************************************************/

void utilMD5Update(	MD5Context *		md5c,
			const unsigned char *	bytes,
			unsigned int		count )
    {
    unsigned int	i;
    unsigned int	idx;
    unsigned int	partLen;

    /*  1  */
    idx= (unsigned int)((md5c->md5cCount[0] >> 3) & 0x3F);

    /*  2  */
    if  (  ( md5c->md5cCount[0] += ((UtilUint32)count << 3))
					    < ((UtilUint32)count << 3))
	{
	md5c->md5cCount[1]++;
	}
    md5c->md5cCount[1] += ((UtilUint32)count >> 29);

    partLen= 64 - idx;

    /*  3  */
    if  ( count >= partLen )
	{
	memcpy( &(md5c->md5cBuffer[idx]), bytes, partLen );
	utilMD5Transform( md5c->md5cState, md5c->md5cBuffer );

	for ( i = partLen; i + 63 < count; i += 64 )
	    { utilMD5Transform( md5c->md5cState, &(bytes[i]) );	}
	
	idx= 0;
	}
    else{ i= 0;	}

    /*  4  */
    memcpy( &(md5c->md5cBuffer[idx]), &(bytes[i]), count- i );

    return;
    }

/************************************************************************/
/*									*/
/*  MD5 finalization. Ends an MD5 message-digest operation, writing the	*/
/*  the message digest and zeroizing the context.			*/
/*									*/
/*  1)  Save number of bits.						*/
/*  2)  Pad out to 56 mod 64.						*/
/*  3)  Append length (before padding)					*/
/*  4)  Store state in digest						*/
/*  5)  Zeroize sensitive information.					*/
/*									*/
/************************************************************************/

void utilMD5Final(	unsigned char *	digest,
			MD5Context *	md5c )
    {
    unsigned char		bits[8];
    unsigned int		idx;
    unsigned int		padLen;

    /*  1  */
    utilMD5Encode( bits, md5c->md5cCount, 8 );

    /*  2  */
    idx= (unsigned int)((md5c->md5cCount[0] >> 3) & 0x3f);
    padLen= (idx < 56) ? (56 - idx) : (120 - idx);
    utilMD5Update( md5c, PADDING, padLen );

    /*  3  */
    utilMD5Update( md5c, bits, 8 );

    /*  4  */
    utilMD5Encode( digest, md5c->md5cState, 16 );

    /*  5  */
    memset( (void *)md5c, 0, sizeof(MD5Context) );

    return;
    }

/************************************************************************/
/*									*/
/*  MD5 basic transformation. Transforms state based on block.		*/
/*									*/
/************************************************************************/

static void utilMD5Transform(	UtilUint32		state[4],
				const unsigned char	block[64] )
    {
    UtilUint32		a= state[0];
    UtilUint32		b= state[1];
    UtilUint32		c= state[2];
    UtilUint32		d= state[3];
    UtilUint32		x[16];

    utilMD5Decode( x, block, 64 );

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zeroize sensitive information. */
    memset( (void *)x, 0, sizeof(x) );

    return;
    }

/************************************************************************/
/*									*/
/*  Encodes input (UtilUint32) into output (unsigned char). Assumes len	*/
/*  is a multiple of 4.							*/
/*									*/
/************************************************************************/

static void utilMD5Encode(	unsigned char *	output,
				UtilUint32 *	input,
				unsigned int	len )
    {
    unsigned int i, j;

    for ( i = 0, j = 0; j < len; i++, j += 4 )
	{
	output[j  ]= (unsigned char) (input[i]        & 0xff);
	output[j+1]= (unsigned char)((input[i] >>  8) & 0xff);
	output[j+2]= (unsigned char)((input[i] >> 16) & 0xff);
	output[j+3]= (unsigned char)((input[i] >> 24) & 0xff);
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Decodes input (unsigned char) into output (UtilUint32). Assumes len	*/
/*  is a multiple of 4.							*/
/*									*/
/************************************************************************/

static void utilMD5Decode(	UtilUint32 *		output,
				const unsigned char *	input,
				unsigned int		len )
    {
    unsigned int i, j;

    for ( i = 0, j = 0; j < len; i++, j += 4 )
	{
	output[i]=	 ((UtilUint32)input[j  ])		|
			(((UtilUint32)input[j+1]) <<  8)	|
			(((UtilUint32)input[j+2]) << 16)	|
			(((UtilUint32)input[j+3]) << 24)	;
	}
    
    return;
    }

/************************************************************************/
/*									*/
/*  Convert a binary MD5 to base 64 notation.				*/
/*									*/
/************************************************************************/

void utilMD5Base64(		char *			digestBase64,
				const unsigned char *	digest )
    {
    unsigned char		digest18[18];

    const unsigned char *	from;
    char *			to;
    int				i;

    memset( digest18, 0, 18 );
    memcpy( digest18, digest, 16 );

    from= digest18; to= digestBase64;
    for ( i= 0; i < 16; from += 3, to += 4, i += 3 )
	{
	int			d[4];

	util_Base64Make( d, from );

	to[0]= UTIL_Base64Digits[d[0]];
	to[1]= UTIL_Base64Digits[d[1]];
	to[2]= UTIL_Base64Digits[d[2]];
	to[3]= UTIL_Base64Digits[d[3]];
	}

    to[-2]= '=';
    to[-1]= '=';
    to[ 0]= '\0';

    return;
    }

int utilMD5FromBase64(		unsigned char *		x_digest,
				const char *		digestBase64 )
    {
    int			done= 0;
    const int *		ida= utilBase64GetIndexArray();

    unsigned char	target[18];
    unsigned char *	to= target;

    memset( to, 0, 18 );

    while( done < 24 )
	{
	int		n= 0;
	int		d[4];

	for ( n= 0; n < 4; n++ )
	    { d[n]= 0; }

	for ( n= 0; n < 4; n++ )
	    {
	    if  ( ! digestBase64[n] || digestBase64[n] == '=' )
		{ break;	}

	    d[n]= ida[digestBase64[n]&0xff];

	    if  ( d[n] < 0 || d[n] >= 64 )
		{
		SLDEB(digestBase64,d[n]);
		return -1;
		}
	    }

	digestBase64 += n; done += n;

	if  ( n == 0 )
	    { break; }
	if  ( n == 1 )
	    { LDEB(n); break; }

	*(to++)= utilBase64Byte0( d );

	if  ( n == 2 )
	    { break; }

	*(to++)= utilBase64Byte1( d );

	if  ( n == 3 )
	    { break; }

	*(to++)= utilBase64Byte2( d );
	}

    if  ( digestBase64[0] && digestBase64[0] != '=' )
	{ SDEB(digestBase64); return -1;	}

    memcpy( x_digest, target, 16 );

    return 0;
    }
