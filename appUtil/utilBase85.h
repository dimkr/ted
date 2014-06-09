/************************************************************************/
/*									*/
/*  Utilities to translate back and forth between binary and base85	*/
/*									*/
/************************************************************************/

#   ifndef		UTIL_BASE85_H
#   define		UTIL_BASE85_H

extern const unsigned char	UTIL_Base85Digits[];

/************************************************************************/
/*									*/
/*  Routine declaratations.						*/
/*									*/
/************************************************************************/

extern const int * utilBase85GetIndexArray( void );

extern int utilBase85Make(	int *			d,
				const unsigned char *	b,
				int			last );

#   endif	/*	UTIL_BASE85_H	*/
