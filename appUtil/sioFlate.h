/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in In/Deflate format	*/
/*  using zlib.								*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInFlateOpen(SimpleInputStream *	sisFlate );

extern SimpleOutputStream * sioOutFlateOpen(
					SimpleOutputStream *	sosFlate,
					int			gzipEmbedded );
