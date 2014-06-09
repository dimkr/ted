#   ifndef	UTIL_BASE26_H
#   define	UTIL_BASE26_H

/************************************************************************/
/*									*/
/*  Base 26 numbers like in Excel column headers.			*/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int utilBase26String(	char *		to,
				int		maxlen,
				int		n,
				int		upper );

extern int utilBase26Int(	const char *	from );

#   endif	/*  UTIL_BASE26_H  */
