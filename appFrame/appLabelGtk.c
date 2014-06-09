#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_GTK

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a row with a label and a text widget.				*/
/*									*/
/************************************************************************/

void appMakeLabelInRow(		APP_WIDGET *		pLabel,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				const char *		labelText )
    {
    GtkWidget *		label= gtk_label_new( labelText );

    gtk_label_set_line_wrap( GTK_LABEL( label ), FALSE );
    gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );

    gtk_table_attach( GTK_TABLE( row ),
			label,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			/*
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );
			*/
			ROW_XPADDING_GTK, ROW_YPADDING_GTK+ 2 );

    gtk_widget_show( label );

    *pLabel= label;

    return;
    }

void appMakeLabelInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				const char *		labelText )
    {
    GtkWidget *		label= gtk_label_new( labelText );

    gtk_box_pack_start( GTK_BOX( column ), label, FALSE, TRUE, 0 );

    gtk_label_set_line_wrap( GTK_LABEL( label ), FALSE );
    gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );

    gtk_widget_show( label );

    *pLabel= label;
    }

void appGuiChangeLabelText(	APP_WIDGET	labelWidget,
				const char *	label )
    {
    gtk_label_set_text( GTK_LABEL( labelWidget ), label );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get the font of a label widget.					*/
/*									*/
/************************************************************************/

APP_FONT * appGuiGetLabelFont(	APP_WIDGET	w )
    {
    GtkStyle *	gs= gtk_widget_get_style( w );
    GdkFont *	gf;

#   if GTK_MAJOR_VERSION < 2
    gf= gs->font;
#   else
    gf= gtk_style_get_font( gs );
#   endif

    return gf;
    }

#   endif
