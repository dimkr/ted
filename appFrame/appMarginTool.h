/************************************************************************/
/*									*/
/*  A group of widgets to manipulate page margins.			*/
/*									*/
/************************************************************************/

#   ifndef APP_MARGIN_TOOL_H
#   define APP_MARGIN_TOOL_H

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appGuiBase.h>

#   include	<utilPropMask.h>

#   include	<appUnit.h>
#   include	<geo2DInteger.h>
#   include	<appGeoString.h>

typedef struct AppMarginTool
    {
    APP_WIDGET			amtLeftColumn;
    APP_WIDGET			amtRightColumn;
    APP_WIDGET			amtInsideWidget;
    APP_WIDGET			amtMarginFrame;

    APP_WIDGET			amtLeftMarginText;
    APP_WIDGET			amtTopMarginText;
    APP_WIDGET			amtRightMarginText;
    APP_WIDGET			amtBottomMarginText;
    } AppMarginTool;

typedef struct MarginToolResources
    {
    char *	mtrTitleText;
    char *	mtrLeftMarginText;
    char *	mtrTopMarginText;
    char *	mtrRightMarginText;
    char *	mtrBottomMarginText;
    } MarginToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMarginToolShowMargins(	AppMarginTool *			amt,
					int				unitInt,
					const DocumentGeometry *	dg );

extern int appMarginToolGetMargins(	PropertyMask *		pUpdMask,
					const PropertyMask *	chgMask,
					int			unitType,
					AppMarginTool *		amt,
					DocumentGeometry *	dg );

extern int appMarginToolCheckMargins(	const DocumentGeometry * dg );

extern void appMakeMarginToolWidgets(
				APP_WIDGET			parent,
				const MarginToolResources *	mtr,
				AppMarginTool *			amt,
				APP_TXACTIVATE_CALLBACK_T	gotValue,
				void *				through );

#   endif /*  APP_MARGIN_TOOL_H  */
