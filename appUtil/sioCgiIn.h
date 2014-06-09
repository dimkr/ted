/************************************************************************/
/*									*/
/*  Simple io stream derived from the POST or PUT input of a CGI	*/
/*  request. (It consumes the input to the PUT or the POST.		*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>
#   include	<appCgiIn.h>

extern SimpleInputStream * sioInCgiOpen(	CGIRequest *	cgir );

