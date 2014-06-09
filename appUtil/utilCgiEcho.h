#   include	<sioGeneral.h>
#   include	<appTagval.h>
#   include	<appCgiIn.h>

/************************************************************************/
/*									*/
/*  Echo some information on a cgi request.				*/
/*									*/
/************************************************************************/

extern void utilCgiEchoTaggedValueList(	TaggedValueList *	tvl,
					const char *		label,
					SimpleOutputStream *	sos );

extern void utilCgiEchoRequest(		const CGIRequest *	cgir,
					SimpleOutputStream *	sos );

