#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_GTK

void appMakeTextInRow(		APP_WIDGET *		pText,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		text;

    text= gtk_entry_new();

    if  ( textColumns > 0 )
	{
	GtkStyle *	gs= gtk_widget_get_style( text );
	GdkFont *	gf;

#	if GTK_MAJOR_VERSION < 2
	gf= gs->font;
#	else
	gf= gtk_style_get_font( gs );
#	endif

	gtk_widget_set_usize( text, ( 55* textColumns*
				( gf->ascent+ gf->descent ) )/ 100, -1 );
	}

    gtk_table_attach( GTK_TABLE( row ),
			text,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( text );

    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );

    *pText= text;
    }

/************************************************************************/
/*									*/
/*  Make a text widget that is contained in a column of widgets		*/
/*									*/
/************************************************************************/

void appMakeTextInColumn(	APP_WIDGET *	pText,
				APP_WIDGET	column,
				int		textColumns,
				int		textEnabled )
    {
    APP_WIDGET		text;

    if  ( textColumns > 0 )
	{ text= gtk_entry_new_with_max_length( textColumns );	}
    else{ text= gtk_entry_new();				}

    gtk_box_pack_start( GTK_BOX( column ), text, FALSE, TRUE, 0 );

    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );

    gtk_widget_show( text );

    *pText= text;
    }

void appRefuseTextValue(	APP_WIDGET		text )
    {
    gtk_entry_select_region( GTK_ENTRY( text ), 0, 
			strlen( gtk_entry_get_text( GTK_ENTRY( text ) ) ) );
    return;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void appStringToTextWidget(		APP_WIDGET	text,
					const char *	s )
    {
    gtk_entry_set_text( GTK_ENTRY( text ), s );

    return;
    }

/************************************************************************/
/*									*/
/*  Turn a text widget on or off.					*/
/*									*/
/************************************************************************/

void appEnableText(		APP_WIDGET	text,
				int		enabled )
    {
    gtk_entry_set_editable( GTK_ENTRY( text ), enabled != 0 );
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve strings from text widgets.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	appGetStringFromTextWidget(	APP_WIDGET	text )
    {
    return strdup( gtk_entry_get_text( GTK_ENTRY( text ) ) );
    }

/*  2  */
void appFreeStringFromTextWidget(	char *		s )
    {
    free( s );
    }

void appGuiSetTypingCallbackForText(	APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through )
    {
    gtk_signal_connect( GTK_OBJECT( text ), "changed",
					(GtkSignalFunc)callBack, through );

    return;
    }

void appGuiSetGotValueCallbackForText(
				APP_WIDGET			text,
				APP_TXACTIVATE_CALLBACK_T	callBack,
				void *				through )
    {
    gtk_signal_connect( GTK_OBJECT( text ), "activate",
					(GtkSignalFunc)callBack, through );

    return;
    }

#   endif
