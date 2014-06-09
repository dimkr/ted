#   ifndef	APP_PAPER_CHOOSER_H
#   define	APP_PAPER_CHOOSER_H

#   include	<appFrame.h>
#   include	<utilPropMask.h>

/************************************************************************/
/*									*/
/*  A paper chooser to be embedded in another window.			*/
/*									*/
/************************************************************************/

struct PaperChooser;

typedef void (*PaperChooserCallback)(
				struct PaperChooser *		pc,
				void *				through,
				const DocumentGeometry *	dg );

typedef struct PaperChooser
    {
    APP_WIDGET			pcFrame;
    APP_WIDGET			pcVerticalColumn;

    AppOptionmenu		pcOptionmenu;
    APP_WIDGET			pcSizeText;

    APP_WIDGET			pcOrientationRow;
    APP_WIDGET			pcPortraitRadio;
    APP_WIDGET			pcLandscapeRadio;

    int				pcCustomPaperSize;
    int				pcSizeChosen;
    int				pcLandscapeChosen;
    DocumentGeometry		pcGeometryChosen;

    int				pcSizeOptionCount;
    APP_WIDGET *		pcSizeOptions;

    int				pcUnitType;

    PaperChooserCallback	pcCallback;
    void *			pcCallbackThrough;

    int				pcProgrammatic;
    } PaperChooser;

# define appPaperChooserRetreshMenuWidth( pc ) \
			    appOptionmenuRefreshWidth( &((pc)->pcOptionmenu) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appPaperChooserShowSize(	PaperChooser *		pc,
					int			sizeChosen );

extern int appPaperChooserGetSize(	PropertyMask *		pUpdMask,
					PaperChooser *		pc,
					DocumentGeometry *	dg );

extern void appPaperChooserFillMenu(	PaperChooser *		pc,
					const char *		customLabel );

extern void appPaperChooserAdaptToGeometry(
				PaperChooser *			pc,
				const DocumentGeometry *	dg );

extern void appMakePaperChooserWidgets(	APP_WIDGET		parent,
					const char *		title,
					int			unitType,
					PaperChooser *		pc,
					PaperChooserCallback	callback,
					void *			through );

extern void appPaperChooserAddOrientationToggles( PaperChooser * pc,
					const char *		portrait,
					const char *		landscape );

extern void appInitPaperChooser(	PaperChooser *		pc );
extern void appCleanPaperChooser(	PaperChooser *		pc );

#   endif	/*  APP_PAPER_CHOOSER_H  */
