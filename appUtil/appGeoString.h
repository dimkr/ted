/************************************************************************/
/*									*/
/*  Geometry Specification strings.					*/
/*									*/
/************************************************************************/

extern void appGeoLengthToString(	char *		target,
					int		twips,
					int		unitInt );

extern void appGeoRectangleToString(	char *		target,
					int		widthTwips,
					int		heightTwips,
					int		unitInt );

extern int appGeoRectangleFromString(	const char *	s,
					int		defaultUnitInt,
					int *		pWidth,
					int *		pHeight );

extern int appGeoLengthFromString(	const char *	s,
					int		defaultUnitInt,
					int *		pValue );
