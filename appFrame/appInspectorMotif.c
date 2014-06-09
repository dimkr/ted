#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<Xm/PanedW.h>
#   include	<Xm/Form.h>

#   include	<appDebugon.h>

void appInspectorMakePageParent(	AppInspector *	ai )
    {
    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac],	XmNallowResize,		True ); ac++;
    XtSetArg( al[ac],	XmNskipAdjust,		False ); ac++;
    ai->aiPageParent= XmCreateForm( ai->aiPaned, WIDGET_NAME, al, ac );

    XtManageChild( ai->aiPageParent );

    appMotifTurnOfSashTraversal( ai->aiPaned );

    return;
    }

void appInspectorChoosePage(	AppInspector *		ai,
				int			andMenu,
				int			subject )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    int			i;

    childCount= 0;
    XtVaGetValues( ai->aiPageParent,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    for ( i= 0; i < childCount; i++ )
	{
	XtVaSetValues( children[i],
			XmNmappedWhenManaged, i == subject,
			NULL );
	}

    if  ( andMenu )
	{ appSetOptionmenu( &(ai->aiSubjectOptionmenu), subject );	}

    return;
    }

static void appInspectorPageChosen(	APP_WIDGET	w,
					void *		vai,
					void *		call_data )
    {
    AppInspector *	ai= (AppInspector *)vai;

    short		subject= -1;

    const int		andMenu= False;

    XtVaGetValues( w,
		    XmNpositionIndex,	&subject,
		    NULL );

    if  ( subject < 0 )
	{ LDEB(subject); return;	}

    if  ( ai->aiNotifySubject )
	{ (*ai->aiNotifySubject)( ai, ai->aiCurrentSubject, subject ); }

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;

    return;
    }

void appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
					APP_WIDGET *	pMenuitem,
					AppInspector *	ai,
					const char *	label )
    {
    Widget		item;
    Widget		page;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac],	XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac],	XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac],	XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac],	XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac],	XmNmarginHeight,	PWmargH ); ac++;
    XtSetArg( al[ac],	XmNspacing,		PWspacing ); ac++;

    page= XmCreatePanedWindow( ai->aiPageParent, WIDGET_NAME, al, ac );
    XtManageChild( page );

    item= appAddItemToOptionmenu( &(ai->aiSubjectOptionmenu), label,
				    appInspectorPageChosen, (void *)ai );

    *pPage= page; *pMenuitem= item; return;
    }

void appInspectorEnablePage(	AppInspector *	ai,
				int		subject,
				int		enabled )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    childCount= 0;
    XtVaGetValues( ai->aiSubjectOptionmenu.aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( subject < 0 || subject >= childCount )
	{ LLDEB(subject,childCount);				}
    else{ XtSetSensitive( children[subject], enabled != 0 );	}

    childCount= 0;
    XtVaGetValues( ai->aiPageParent,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( subject < 0 || subject >= childCount )
	{ LLDEB(subject,childCount);				}
    else{ XtSetSensitive( children[subject], enabled != 0 );	}

    return;
    }

#   endif /*  USE_MOTIF	*/
