#   ifndef	APP_UNIT_H
#   define	APP_UNIT_H

#   include	<time.h>

/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   define	UNITtyCM	1
#   define	UNITtyINCH	2
#   define	UNITtyPOINTS	3
#   define	UNITtyPICAS	4
#   define	UNITtyMM	5

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int appUnitTypeInt(	const char *	unitTypeString );

extern const char * appUnitTypeString(	int	unitTypeInt );

extern double appUnitFromTwips(	int	twips,
				int	unitTypeInt );

extern double appUnitToTwips(	double	units,
				int	unitTypeInt );

extern int appWordFormatDate(	char *			target,
				int			maxlen,
				const struct tm *	tm,
				const char *		wordPicture );

extern void appInvalidateTime(	struct tm *		tm );

#   endif	/*  APP_UNIT_H  */
