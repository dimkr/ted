#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/List.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

void appGuiMakeListInColumn(	Widget *		pList,
				Widget			column,
				int			visibleItems,
				XtCallbackProc		callback,
				void *			through )
    {
    Widget		list;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		False ); ac++;
    XtSetArg( al[ac], XmNallowResize,		False ); ac++;
    XtSetArg( al[ac], XmNvisibleItemCount,	visibleItems ); ac++;
    /* NO!
    */
    XtSetArg( al[ac], XmNlistSizePolicy,	XmCONSTANT ); ac++;
    XtSetArg( al[ac], XmNwidth,			25 ); ac++;

    list= XmCreateScrolledList( column, WIDGET_NAME, al, ac );

    XtManageChild( list );

    if  ( callback )
	{
	XtAddCallback( list, XmNbrowseSelectionCallback, callback, through );
	}

    appMotifTurnOfSashTraversal( column );

    *pList= list; return;
    }

void appGuiEmptyListWidget(	Widget			list )
    {
    XmListDeselectAllItems( list );
    XmListDeleteAllItems( list );

    return;
    }

void appGuiAddValueToListWidget(	Widget		list,
					const char *	value )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)value );

    XmListAddItemUnselected( list, labelString, 0 );

    XmStringFree( labelString );

    return;
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	list,
						int		position )
    {
    XmListSelectPos( list, position+ 1, False );

    return;
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list )
    {
    XmListDeselectAllItems( list );

    return;
    }

int appGuiGetPositionFromListCallback(		APP_WIDGET	list,
						void *		voidlcs )
    {
    XmListCallbackStruct *	lcs= (XmListCallbackStruct *)voidlcs;

    return lcs->item_position- 1;
    }

char * appGuiGetStringFromListCallback(		APP_WIDGET	list,
						void *		voidlcs )
    {
    XmListCallbackStruct *	lcs= (XmListCallbackStruct *)voidlcs;
    char *			text= (char *)0;

    XmStringGetLtoR( lcs->item, XmFONTLIST_DEFAULT_TAG, &text );

    return text;
    }

void appFreeStringFromListCallback(	char *		s )
    { XtFree( s );	}

#   endif
