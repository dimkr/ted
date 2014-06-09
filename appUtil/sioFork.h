/************************************************************************/
/*									*/
/*  Simple io streams, Fork input or output.. Usually for debugging	*/
/*  purposes.								*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInForkOpen(
					SimpleInputStream *	sisIn,
					SimpleOutputStream *	sosFork );

extern SimpleOutputStream * sioOutForkOpen(
					SimpleOutputStream *	sosOut,
					SimpleOutputStream *	sosFork );

