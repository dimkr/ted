/************************************************************************/
/*									*/
/*  Manage a menu to contol the font encoding in a font selection tool.	*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<psFont.h>
#   include	<utilFontEncoding.h>

#   include	<appGuiBase.h>
#   include	<appFrame.h>

typedef struct AppEncodingMenu
    {
    AppOptionmenu	aemEncodingOptionmenu;
    APP_WIDGET		aemFontEncodingOptions[CHARSETidxCOUNT];

    int			aemCharsetIndex;
    } AppEncodingMenu;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void appEncodingMenuUnsetEncoding(
				AppEncodingMenu *		aem );

extern void appEncodingMenuSetEncoding(
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff,
				int				enc );

extern void appEncodingMenuSetDocEncoding(
				AppEncodingMenu *	aem,
				const int *		fontForEncoding,
				int			enc );

extern void appEncodingMenuFillOptionmenu(
				char * const *			opts,
				APP_OITEM_CALLBACK_T		callBack,
				void *				through,
				AppEncodingMenu *		aem );

extern void appEncodingMenuGetOptionTexts(
				char **				opts,
				EditApplication *		ea );

extern void appEncodingMenuAdaptToFamilyEncodings(
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff );

extern int appEncodingMenuAdaptToFamily(
				int *				pFontNumber,
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff );

extern int appEncodingMenuAdaptToCharsetIndex(
				int *				pFontNumber,
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff,
				int				charsetIdx );

