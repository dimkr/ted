/************************************************************************/
/*									*/
/*  Date manipulation.							*/
/*									*/
/*  These routines are totally naive with respect to the difference	*/
/*  between Julian and Gregorian dates. Everything is Gregorian.	*/
/*									*/
/*  Dates are relative to Jan 1, 1970. Values very far in the past are	*/
/*  idiotic.								*/
/*									*/
/************************************************************************/

extern long utilDateGregorianDayNumber(	int	year,
					int	month,
					int	day );

extern void utilDateSplitGregorianDate(	long	value,
					int *	pYear,
					int *	pMonth,
					int *	pDay );
