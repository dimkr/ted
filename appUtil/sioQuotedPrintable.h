/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the MIME (RFC 2045)		*/
/*  'Quoted Printable' content transfer encoding.			*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInQuotedPrintableOpen(
					    SimpleInputStream * sisQuoted );

extern SimpleOutputStream * sioOutQuotedPrintableOpen(
					    SimpleOutputStream * sosQuoted );
