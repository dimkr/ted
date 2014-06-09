#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<Xm/Frame.h>
#   include	<Xm/Label.h>
#   include	<Xm/Form.h>
#   include	<Xm/PanedW.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a title widget.						*/
/*									*/
/************************************************************************/

static void appMakeFrameTitleWidget(	Widget		frame,
					const char *	title )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	titleString;
    Widget	titleWidget;

    titleString= XmStringCreateLocalized( (char *)title );

    ac= 0;
    XtSetArg( al[ac], XmNchildType,	XmFRAME_TITLE_CHILD ); ac++;
    XtSetArg( al[ac], XmNlabelString,	titleString ); ac++;
    XtSetArg( al[ac], XmNchildHorizontalAlignment,
					XmALIGNMENT_CENTER ); ac++;

    titleWidget= XmCreateLabel( frame, WIDGET_NAME, al, ac );

    XmStringFree( titleString );

    XtManageChild( titleWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a frame, possibly with a title.				*/
/*									*/
/************************************************************************/

static void appMakeFrame(	Widget *	pFrame,
				Widget		parent,
				const char *	title )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		frame;

    ac= 0;
    XtSetArg( al[ac],	XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac],	XmNallowResize,		True ); ac++;
    frame= XmCreateFrame( parent, WIDGET_NAME, al, ac );

    if  ( title )
	{ appMakeFrameTitleWidget( frame, title );	}

    XtManageChild( frame );

    *pFrame= frame; return;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a column itself.	*/
/*									*/
/************************************************************************/

void appMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title )
    {
    Widget		frame;
    Widget		bboard;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    appMakeFrame( &frame, parent, title );

    ac= 0;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,	0 ); ac++;
    bboard= XmCreateForm( frame, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( bboard, WIDGET_NAME, al, ac );

    XtManageChild( paned );
    XtManageChild( bboard );
    XtManageChild( frame );

    *pFrame= frame, *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a row itself.	*/
/*									*/
/************************************************************************/

void appMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
				APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				int			columnCount,
				const char *		title )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		frame;
    Widget		row;

    appMakeFrame( &frame, parent, title );

    ac= 0;
    XtSetArg( al[ac],	XmNfractionBase,	columnCount ); ac++;
    XtSetArg( al[ac],	XmNallowResize,		True ); ac++;
    XtSetArg( al[ac],	XmNskipAdjust,		True ); ac++;

    row= XmCreateForm( frame, WIDGET_NAME, al, ac );

    XtManageChild( row );
    XtManageChild( frame );

    *pFrame= frame; *pRow= row; return;
    }

/************************************************************************/
/*									*/
/*  Change the title of a frame.					*/
/*									*/
/*  Changing the label text of the label child does not work, so we	*/
/*  destroy it and make a fresh one with the new text.			*/
/*									*/
/************************************************************************/

int appGuiSetFrameTitle(		APP_WIDGET		frame,
					const unsigned char *	title )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    int			i;

    XtVaGetValues( frame,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    for ( i= 0; i < childCount; i++ )
	{
	if  ( XmIsLabel( children[i] ) )
	    {
	    unsigned char	childType= XmFRAME_GENERIC_CHILD;

	    XtVaGetValues( children[i],
				XmNchildType,	&childType,
				NULL );

	    if  ( childType == XmFRAME_TITLE_CHILD )
		{ XtDestroyWidget( children[i] ); break;	}
	    }
	}

    appMakeFrameTitleWidget( frame, (const char *)title );

    return 0;
    }

#   endif	/*  USE_MOTIF  */
