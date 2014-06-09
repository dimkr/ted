/************************************************************************/
/*									*/
/*  Utilities to translate back and forth between binary and base64	*/
/*									*/
/************************************************************************/

#   ifndef		UTIL_BASE64_H
#   define		UTIL_BASE64_H

extern const unsigned char	UTIL_Base64Digits[];

#   define	util_Base64Make(d,b)				\
			(d)[0]=	( ( (b)[0] & 0xfc ) >> 2 ) ;	\
			(d)[1]=	( ( (b)[0] & 0x03 ) << 4 ) |	\
				( ( (b)[1] & 0xf0 ) >> 4 ) ;	\
			(d)[2]=	( ( (b)[1] & 0x0f ) << 2 ) |	\
				( ( (b)[2] & 0xc0 ) >> 6 ) ;	\
			(d)[3]=	( ( (b)[2] & 0x3f ) >> 0 ) ;

#   define	utilBase64Byte0( d )	( ( ( (d)[0] & 0x3f ) << 2 )	|\
					  ( ( (d)[1] & 0x30 ) >> 4 )	)

#   define	utilBase64Byte1( d )	( ( ( (d)[1] & 0x0f ) << 4 )	|\
					  ( ( (d)[2] & 0x3c ) >> 2 )	)

#   define	utilBase64Byte2( d )	( ( ( (d)[2] & 0x03 ) << 6 )	|\
					  ( ( (d)[3] & 0x3f ) >> 0 )	)


/************************************************************************/
/*									*/
/*  Routine declaratations.						*/
/*									*/
/************************************************************************/

extern const int * utilBase64GetIndexArray( void );

#   endif	/*	UTIL_BASE64_H	*/
