#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<Xm/Label.h>

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
    Widget		label;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)labelText );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    label= XmCreateLabel( row, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    XtManageChild( label );

    *pLabel= label;
    }

void appMakeLabelInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				const char *		labelText )
    {
    Widget		label;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)labelText );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    label= XmCreateLabel( column, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    XtManageChild( label );

    appMotifTurnOfSashTraversal( column );

    *pLabel= label; return;
    }

void appGuiChangeLabelText(	APP_WIDGET	labelWidget,
				const char *	label )
    {
    XmString			labelString;

    labelString= XmStringCreateLocalized( (char *)label );

    XtVaSetValues( labelWidget,
			XmNlabelString,		labelString,
			NULL );

    XmStringFree( labelString );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get the font of a label widget.					*/
/*									*/
/************************************************************************/

APP_FONT * appGuiGetLabelFont(	APP_WIDGET	w )
    {
    APP_FONT *		rval= (APP_FONT *)0;
    XmFontList		fontList= (XmFontList)0;

#   if 1
    XtVaGetValues( w,	XmNfontList,	&fontList,
			NULL );
#   else
    /*# include <Xm/XmP.h>*/
    fontList= _XmGetDefaultFontList( w, XmLABEL_FONTLIST );
#   endif

    if  ( ! fontList )
	{ XDEB(fontList);	}
    else{
	XmFontContext	context;
	XmFontListEntry	entry;

	XmFontListInitFontContext( &context, fontList );

	entry= XmFontListNextEntry( context );
	if  ( ! entry )
	    { XDEB(entry);	}
	else{
	    XtPointer		fontPtr;
	    XmFontType		fontType;

	    fontPtr= XmFontListEntryGetFont( entry, &fontType );

	    if  ( fontType == XmFONT_IS_FONT )
		{ rval= (XFontStruct *)fontPtr;	}
	    else{
		int		fontCount;
		char **		fontNameList;
		XFontStruct **	fontList;

		fontCount = XFontsOfFontSet( (XFontSet)fontPtr,
						   &fontList, &fontNameList );
		if  ( fontCount == 0 )
		    { LDEB(fontCount);			}
		else{ rval= fontList[0];	}
		}

	    /* NO! Pointer into the belly of the font list.
	    XmFontListEntryFree( &entry );
	    */
	    }

	XmFontListFreeFontContext( context );

	/* NO! Pointer into the belly of the label widget.
	XmFontListFree( fontList );
	*/
	}

    if  ( ! rval )
	{ XDEB(rval);	}

    return rval;
    }

#   endif
