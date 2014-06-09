/************************************************************************/
/*									*/
/*  Height tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_HEIGHT_TOOL_H
#   define	TED_HEIGHT_TOOL_H

/************************************************************************/
/*									*/
/*  A combination of a pulldown menu and a text widget to manage a	*/
/*  height property.							*/
/*									*/
/************************************************************************/

#   define	HC_FREE		0
#   define	HC_EXACTLY	1
#   define	HC_AT_LEAST	2
#   define	HC__COUNT	3

typedef struct HeightChooser
    {
    AppOptionmenu	hcOptionmenu;
    APP_WIDGET		hcText;

    APP_WIDGET		hcMenuItems[HC__COUNT];
    int			hcHow;
    } HeightChooser;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedInitHeightChooser(	HeightChooser *		hc );

extern void tedFormatMakeHeightRow(
				APP_WIDGET *			pRow,
				void *				through,
				APP_WIDGET			parent,
				HeightChooser *			hc,
				APP_TXACTIVATE_CALLBACK_T	callback );

extern void tedFormatRefreshHeightChooser(	HeightChooser *	hc,
						int		height );

extern void tedFormatHeightChosen(	int			how,
					HeightChooser *		hc,
					int			defaultValue );

extern void tedFormatFillHeightChooser(	HeightChooser *		hc,
					APP_OITEM_CALLBACK_T	callback,
					void *			voidtft,
					const char *		freeText,
					const char *		atLeastText,
					const char *		exactlyText );

extern int tedFormatToolGetHeight(	int *		pHeight,
					HeightChooser *	hc );

#   endif	/*  TED_HEIGHT_TOOL_H */
