/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the PostsScript ASCII base 85	*/
/*  encoding.								*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInBase85Open(	SimpleInputStream * sisBase85 );

extern SimpleOutputStream * sioOutBase85Open(	SimpleOutputStream * sosBase85 );
