/************************************************************************/
/*									*/
/*  Infrastructure to make a 'Zoom' document menu.			*/
/*									*/
/************************************************************************/

#   define	ZOOM_COUNT	7

extern void appZoomChangeFactor( APP_WIDGET		option,
				APP_WIDGET		menu,
				APP_WIDGET *		options,
				int			optionCount,
				int *			pLogSqrt2Magnification,
				double *		pMagnification,
				DocumentRectangle *	drSelected );

extern void appUpdateZoomMenu(	APP_WIDGET	menu,
				APP_WIDGET *	options,
				int		optionCount,
				int		logs2mag );

extern void appMakeZoomMenu(	EditApplication *	ea,
				EditDocument *		ed,
				APP_WIDGET *		pZoomMenu,
				APP_WIDGET		menubar,
				int			logSqrt2Magnification,
				const char *		zoomText,
				AppMenuItem *		optionItems,
				APP_WIDGET *		options,
				int			optionCount );
