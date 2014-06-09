#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a row consisting of a label and a text widget.			*/
/*									*/
/************************************************************************/

void appMakeLabelAndTextRow(	APP_WIDGET *		pRow,
				APP_WIDGET *		pLabel,
				APP_WIDGET *		pText,
				APP_WIDGET		column,
				const char *		labelText,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		label;
    APP_WIDGET		text;
    APP_WIDGET		row;

    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		textColumn= 1;
    const int		textColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeTextInRow( &text, row, textColumn, textColspan,
						textColumns, textEnabled );

    *pRow= row; *pLabel= label; *pText= text; return;
    }

void appMakeToggleAndTextRow(	APP_WIDGET *		pRow,
				APP_WIDGET *		pToggle,
				APP_WIDGET *		pText,
				APP_WIDGET		column,
				char *			labelText,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		toggle;
    APP_WIDGET		text;
    APP_WIDGET		row;

    const int		textColumn= 1;
    const int		textColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    toggle= appMakeToggleInRow( row, labelText, callback, through, 0 );
    appMakeTextInRow( &text, row, textColumn, textColspan,
						    textColumns, textEnabled );

    *pRow= row; *pToggle= toggle; *pText= text; return;
    }
