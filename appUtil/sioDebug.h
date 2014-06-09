/************************************************************************/
/*									*/
/*  Simple io streams, Fork input or output.. Usually for debugging	*/
/*  purposes.								*/
/*									*/
/*  Additionally, a pure debugging output stream is provided.		*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInDebugOpen(
					SimpleInputStream *	sisIn );

extern SimpleOutputStream * sioOutDebugOpen(
					SimpleOutputStream *	sosOut );


extern SimpleOutputStream * sioOutAppDebugOpen( void );

