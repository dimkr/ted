/************************************************************************/
/*									*/
/*  The 'Bullets and Numbering' page of the format tool.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>

#   include	"docEvalField.h"
#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	"appGuiKeys.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF
#	include <Xm/Text.h>
#	include <Xm/Form.h>
#	include <Xm/Label.h>
#	include <Xm/PushB.h>
#	include <Xm/PushBG.h>
#	include <Xm/RowColumn.h>
#   endif

/************************************************************************/
/*									*/
/*  Supported styles:							*/
/*									*/
/************************************************************************/

static const int TED_ListToolStyles[LISTTOOLcountSTYLES]=
    {
    0,
    1,
    2,
    3,
    4,
    22,
    23,
    255,
    };

static void tedListLevelRefreshFormatMenu(	ListTool *	lt );
static void tedFormatToolRefreshListPage(	ListTool *	lt );

/************************************************************************/
/*									*/
/*  Format the level text for a certain list level.			*/
/*									*/
/************************************************************************/

static void tedListToolSetCurrentLevelPath(
				    int *			numberPath,
				    int				level,
				    ListTool *			lt )
    {
    int			l;

    if  ( lt->ltPrevLevel >= 0 )
	{
	for ( l= 0; l <= level; l++ )
	    { numberPath[l]= lt->ltPrevPath[l];	}

	if  ( level > lt->ltPrevLevel )
	    {
	    for ( l= lt->ltPrevLevel+ 1; l <= level; l++ )
		{ numberPath[l]= 0;	}
	    }
	else{
	    numberPath[level]= lt->ltPrevPath[level]+ 1;
	    }
	}
    else{
	for ( l= 0; l <= level; l++ )
	    { numberPath[l]= lt->ltHerePath[l];	}
	}

    return;
    }

static int tedListToolFormatLevelText(	
				    unsigned char *		listtext,
				    int				maxlen,
				    int *			offsets,
				    int				maxoffsets,
				    ListTool *			lt,
				    const DocumentListLevel *	dll,
				    int				level )
    {
    int		numberPath[DLmaxLEVELS];
    int		size;

    tedListToolSetCurrentLevelPath( numberPath, level, lt );

    size= docListLevelFormatParagraphNumber( listtext, maxlen,
						offsets, maxoffsets,
						level, numberPath,
						lt->ltStartPath,
						lt->ltFormatPath, dll );
    if  ( size < 0 )
	{ listtext[0]= '\0'; return -1;	}

    return size;
    }

/************************************************************************/
/*									*/
/*  Switch between 'edit' and string format for the level text.		*/
/*									*/
/************************************************************************/

static void tedListToolSetFormatEditable(
				ListTool *			lt,
				int				editable )
    {
    if  ( editable )
	{
	appGuiSetWidgetVisible( lt->ltNumberFormatCurrentLabel, 0 );
	appGuiSetWidgetVisible( lt->ltNumberFormatLabelTail, 0 );

	appGuiSetWidgetVisible( lt->ltNumberFormatCurrentText, 1 );
	appGuiSetWidgetVisible( lt->ltNumberFormatTextTail, 1 );

#	ifdef USE_MOTIF
	XmProcessTraversal( lt->ltNumberFormatCurrentText, XmTRAVERSE_CURRENT );
#	endif
	}
    else{
	appGuiSetWidgetVisible( lt->ltNumberFormatCurrentText, 0 );
	appGuiSetWidgetVisible( lt->ltNumberFormatTextTail, 0 );

	appGuiSetWidgetVisible( lt->ltNumberFormatCurrentLabel, 1 );
	appGuiSetWidgetVisible( lt->ltNumberFormatLabelTail, 1 );

#	ifdef USE_MOTIF
	XmProcessTraversal( lt->ltNumberFormatCurrentLabel, XmTRAVERSE_CURRENT );
#	endif
	}

    lt->ltFormatEditable= editable != 0;
    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the controls with the current list level.			*/
/*									*/
/************************************************************************/

static void tedListToolRefreshLevelFormat(
					ListTool *			lt,
					const DocumentListLevel *	dll,
					int				level )
    {
    unsigned char		listtext[50+1];
    char			scratch[50+1];
    int				offsets[2* DLmaxLEVELS+ 2];
    int				from;
    int				upto;
    int				size;

    const int			editable= 0;

    tedListToolSetFormatEditable( lt, editable );

    size= tedListToolFormatLevelText( listtext, sizeof(listtext)- 1,
						offsets, 2* DLmaxLEVELS+ 2,
						lt, dll, level );

    from= 0; upto= offsets[lt->ltFormatIndex];
    memcpy( scratch, listtext+ from, upto- from );
    scratch[upto- from]= '\0';
    appGuiChangeLabelText( lt->ltNumberFormatHead, scratch );

    from= offsets[lt->ltFormatIndex]; upto= offsets[lt->ltFormatIndex+1];
    memcpy( scratch, listtext+ from, upto- from );
    scratch[upto- from]= '\0';
    appStringToTextWidget( lt->ltNumberFormatCurrentText, (char *)scratch );
#   ifdef USE_MOTIF
    XmTextSetSelection( lt->ltNumberFormatCurrentText,
						0, upto- from, CurrentTime );
#   endif
#   ifdef USE_GTK
    gtk_entry_select_region( GTK_ENTRY( lt->ltNumberFormatCurrentText ),
						0, upto- from );
#   endif

    if  ( upto == from )
	{ appGuiChangeLabelText( lt->ltNumberFormatCurrentLabel, " " );	}
    else{
	appGuiChangeLabelText( lt->ltNumberFormatCurrentLabel,
							(char *)scratch );
	}

    from= offsets[lt->ltFormatIndex+ 1]; upto= size;
    memcpy( scratch, listtext+ from, upto- from );
    scratch[upto- from]= '\0';
    appGuiChangeLabelText( lt->ltNumberFormatLabelTail, scratch );
    appGuiChangeLabelText( lt->ltNumberFormatTextTail, scratch );

    return;
    }

/************************************************************************/
/*									*/
/*  A level to be used in the format string has been chosen.		*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedListFormatLevelChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    short			number= -1;

    int				changed= 0;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

#   ifdef USE_MOTIF
    XtVaGetValues( w,	XmNpositionIndex,	&number,
			NULL );
#   endif

    if  ( number < 0 || number > level )
	{ LLDEB(number,level); return;	}

    docListLevelSetNumber( &changed, dll, number, lt->ltFormatIndex/ 2 );

    if  ( changed )
	{ tedFormatToolRefreshListPage( lt );	}

    return;
    }

static void tedListDeleteFormatLevel(	ListTool *	lt )
    {
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    docListLevelDeleteNumber( dll, lt->ltFormatIndex/ 2 );

    if  ( lt->ltFormatIndex > 0 )
	{ lt->ltFormatIndex--;	}

    if  ( lt->ltFormatIndex > 2* dll->dllNumberSize )
	{ lt->ltFormatIndex=  2* dll->dllNumberSize; }

    tedFormatToolRefreshListPage( lt );

    return;
    }

static APP_OITEM_CALLBACK_H( tedListDeleteFormatLevelH, w, voidlt )
    { tedListDeleteFormatLevel( (ListTool *)voidlt );	}

static APP_OITEM_CALLBACK_H( tedListEditLevelText, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    tedListToolSetFormatEditable( lt, 1 );

    return;
    }


static void tedListInsertFormatLevel(	ListTool *	lt )
    {
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    docListLevelInsertNumber( dll, level, lt->ltFormatIndex/ 2 );

    if  ( lt->ltFormatIndex % 2 == 0 )
	{ lt->ltFormatIndex++;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

static APP_OITEM_CALLBACK_H( tedListInsertFormatLevelH, w, voidlt )
    {
    tedListInsertFormatLevel( (ListTool *)voidlt );
    }

/************************************************************************/
/*									*/
/*  Display the popup that belongs to this drawn popdown.		*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( tedListToolNumberPopup, w, voidlt, mouseEvent )
    {
    ListTool *		lt= (ListTool *)voidlt;

#   ifdef USE_MOTIF
    XmMenuPosition( lt->ltNumberFormatMenu,
					(XButtonPressedEvent *)mouseEvent );
    XtManageChild( lt->ltNumberFormatMenu );
#   endif

    return;
    }

static void tedListLevelRefreshFormatMenu(	ListTool *	lt )
    {
#   ifdef USE_MOTIF
    const ListsPageResources *	lpr= lt->ltPageResources;
    int				level= lt->ltCurrentLevel;

    Arg				al[20];
    int				ac= 0;
    int				acCommon;
    Widget			option;

    char			scratch[50];
    char			acci[30+1];

    int				i;

    ac= 0;
    XtSetArg( al[ac], XmNmarginWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNborderWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNhighlightThickness,	1 ); ac++;
    XtSetArg( al[ac], XmNshadowThickness,	1 ); ac++;
    XtSetArg( al[ac], XmNfillOnArm,		True ); ac++;

    acCommon= ac;

    /**/
    appEmptyParentWidget( lt->ltNumberFormatMenu );

    if  ( lt->ltFormatIndex % 2 != 0 )
	{
	for ( i= 0; i <= level; i++ )
	    {
	    unsigned char		texti[30+1];

	    ac= acCommon;

	    if  ( docListLevelFormatLevelNumber( texti, 30,
					lt->ltCurrPath[i]+ lt->ltStartPath[i],
					lt->ltFormatPath[i] ) < 0 )
		{
		LLDEB(i,lt->ltCurrPath[i]);
		strcpy( texti, "?" );
		}

	    sprintf( scratch, "%d: %s", i+ 1, texti );
	    sprintf( acci, "<Key>%d", i+ 1 );

	    XtSetArg( al[ac], XmNaccelerator, acci ); ac++;

	    option= XmCreatePushButton( lt->ltNumberFormatMenu, 
							scratch, al, ac );

	    XtAddCallback( option,
		    XmNactivateCallback, tedListFormatLevelChosen, (void *)lt );

	    XtManageChild( option );
	    }

	ac= acCommon;

	option= XmCreatePushButton( lt->ltNumberFormatMenu, 
					lpr->lprDeleteLevelNumber, al, ac );

	XtAddCallback( option,
		XmNactivateCallback, tedListDeleteFormatLevelH, (void *)lt );

	XtManageChild( option );
	}
    else{
	ac= acCommon;

	option= XmCreatePushButton( lt->ltNumberFormatMenu, 
					lpr->lprEditLevelText, al, ac );

	XtAddCallback( option,
		XmNactivateCallback, tedListEditLevelText, (void *)lt );

	XtManageChild( option );
	}

    ac= acCommon;

    option= XmCreatePushButton( lt->ltNumberFormatMenu, 
				    lpr->lprInsertLevelNumber, al, ac );

    XtAddCallback( option,
	    XmNactivateCallback, tedListInsertFormatLevelH, (void *)lt );

    XtManageChild( option );

#   endif
    return;
    }

static void tedListToolSetLevelHeader(		ListTool *	lt,
						const char *	level )
    {
    const ListsPageResources *	lpr= lt->ltPageResources;
    int				levlen= strlen( level );
    int				size;
    const char *		from;
    unsigned char *		scratch;
    unsigned char *		to;

    size= 0;

    from= lpr->lprListLevel;
    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'd' )
	    { from++; size += levlen;	}
	else{ size++;			}

	from++;
	}

    scratch= to= malloc( size+ 1 );
    if  ( ! scratch )
	{ LXDEB(size,scratch); return; }

    from= lpr->lprListLevel;
    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'd' )
	    {
	    from++; strcpy( to, level ); to += levlen;	}
	else{ *(to++)= *from;				}

	from++;
	}

    *(to)= '\0';

    appGuiSetFrameTitle( lt->ltListLevelFrame, scratch );

    free( scratch );

    return;
    }


static void tedListToolRefreshIndents(	ListTool *			lt,
					const DocumentListLevel *	dll )
    {
    char			scratch[50];

    appGeoLengthToString( scratch,
	    dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( lt->ltFirstIndentText, scratch );

    appGeoLengthToString( scratch, dll->dllLeftIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( lt->ltLeftIndentText, scratch );
    }

static void tedListToolRefreshCurrentLevel(	ListTool *	lt )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const DocumentListLevel *	dll;
    int				level= lt->ltCurrentLevel;

    int				i;
    char			scratch[50];

    if  ( level < 0 || level >= dl->dlLevelCount )
	{
	tedListToolSetLevelHeader( lt, "" );
	appStringToTextWidget( lt->ltStartAtText, "" );
	appStringToTextWidget( lt->ltNumberFormatHead, "" );
	appStringToTextWidget( lt->ltNumberFormatCurrentLabel, "" );
	appStringToTextWidget( lt->ltNumberFormatCurrentText, "" );
	appStringToTextWidget( lt->ltNumberFormatLabelTail, "" );
	appStringToTextWidget( lt->ltNumberFormatTextTail, "" );

	appSetOptionmenu( &(lt->ltNumberStyleOptionmenu), -1 );
	appSetOptionmenu( &(lt->ltJustifyOptionmenu), -1 );
	appSetOptionmenu( &(lt->ltFollowOptionmenu), -1 );

	appGuiSetWidgetVisible( lt->ltNumberFormatCurrentLabel, 0 );

	return;
	}

    appGuiSetWidgetVisible( lt->ltNumberFormatCurrentLabel, 1 );

    sprintf( scratch, "%d", level+ 1 );
    tedListToolSetLevelHeader( lt, scratch );

    tedListToolSetCurrentLevelPath( lt->ltCurrPath, lt->ltCurrentLevel, lt );

    dll= &(dl->dlLevels[level]);

    tedListToolRefreshIndents( lt, dll );

    appIntegerToTextWidget( lt->ltStartAtText, dll->dllStartAt );

    for ( i= 0; i < LISTTOOLcountSTYLES; i++ )
	{
	if  ( dll->dllNumberStyle == TED_ListToolStyles[i] )
	    { break;	}
	}
    if  ( i >= LISTTOOLcountSTYLES )
	{ i= -1;	}

    appSetOptionmenu( &(lt->ltNumberStyleOptionmenu), i );
    appSetOptionmenu( &(lt->ltJustifyOptionmenu), dll->dllJustification );
    appSetOptionmenu( &(lt->ltFollowOptionmenu), dll->dllFollow );

    /**/
    tedListToolRefreshLevelFormat( lt, dll, level );
    tedListLevelRefreshFormatMenu( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a certain tab in the list.					*/
/*									*/
/************************************************************************/

static void tedListToolSelectLevel(	ListTool *	lt,
					int		level )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);

    if  ( level >= 0 && level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); level= -1;	}

    if  ( level >= 0 )
	{
	const DocumentListLevel *	dll= &(dl->dlLevels[level]);

	lt->ltCurrentLevel= level;

	appGuiSelectPositionInListWidget( lt->ltListLevelList, level );

	if  ( lt->ltFormatIndex > 2* dll->dllNumberSize )
	    { lt->ltFormatIndex=  2* dll->dllNumberSize; }
	}
    else{
	lt->ltCurrentLevel= -1;

	appGuiRemoveSelectionFromListWidget( lt->ltListLevelList );
	lt->ltFormatIndex= 0;
	}

    tedListToolRefreshCurrentLevel( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the list of tabs and the currently selected tab.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshListPage(	ListTool *	lt )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const ListOverride *	lo= &(lt->ltListOverrideChosen);

    appGuiEmptyListWidget( lt->ltListLevelList );

    if  ( dl->dlLevelCount > 0 )
	{
	char				scratch[100];
	unsigned char			listtext[50];

	const DocumentListLevel *	dll;
	int				level;

	docListGetFormatPath( lt->ltStartPath, lt->ltFormatPath,
					&dll, dl->dlLevelCount- 1, dl, lo );

	dll= dl->dlLevels;
	for ( level= 0; level < dl->dlLevelCount; dll++, level++ )
	    {
	    tedListToolFormatLevelText( listtext, sizeof(listtext)- 1,
							    (int *)0, 0,
							    lt, dll, level );

	    sprintf( scratch, "%d: %s", level+ 1, (char *)listtext );

	    appGuiAddValueToListWidget( lt->ltListLevelList, scratch );
	    }

	tedListToolSelectLevel( lt, lt->ltCurrentLevel );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the tabs tool to the current selection in the document.	*/
/*									*/
/************************************************************************/

static void tedFormatToolNoList(	ListTool *		lt,
					InspectorSubject *	is )
    {
    docCleanDocumentList( &(lt->ltListPropertiesSet) );
    docCleanDocumentList( &(lt->ltListPropertiesChosen) );

    docInitDocumentList( &(lt->ltListPropertiesSet) );
    docInitDocumentList( &(lt->ltListPropertiesChosen) );

    docCleanListOverride( &(lt->ltListOverrideSet) );
    docCleanListOverride( &(lt->ltListOverrideChosen) );

    docInitListOverride( &(lt->ltListOverrideSet) );
    docInitListOverride( &(lt->ltListOverrideChosen) );

    lt->ltCurrentLevel= -1;
    lt->ltHereLevel= -1;
    lt->ltPrevLevel= -1;
    lt->ltFormatIndex= 0;
    lt->ltCurrentParagraphNumber= -1;

    appGuiEnableWidget( lt->ltRemoveFromListButton, 0 );
    appGuiEnableWidget( is->isApplyButton, 0 );
    appGuiEnableWidget( is->isRevertButton, 0 );

    appGuiEnableWidget( lt->ltListLevelFrame, 0 );
    appGuiEnableWidget( lt->ltSetLevelButton, 0 );

    return;
    }



static int tedFormatToolSetList(	ListTool *			lt,
					InspectorSubject *		is,
					int				paraNr,
					int				ls,
					int				level,
					ListOverride *			lo,
					DocumentList *			dl,
					ListNumberTreeNode *		root )
    {
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;

    if  ( docCopyDocumentList( &(lt->ltListPropertiesSet), dl,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyDocumentList( &(lt->ltListPropertiesChosen), dl,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}

    if  ( docCopyListOverride( &(lt->ltListOverrideSet), lo,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyListOverride( &(lt->ltListOverrideChosen), lo,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}

    lt->ltHereLevel= level;
    lt->ltFormatIndex= 0;

    if  ( paraNr != lt->ltCurrentParagraphNumber			||
	  lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >=
			    lt->ltListPropertiesChosen.dlLevelCount	)
	{ lt->ltCurrentLevel= level;	}

    lt->ltCurrentParagraphNumber= paraNr;

    docListNumberTreeGetNumberPath( lt->ltHerePath, root, level, paraNr );

    if  ( docListNumberTreeGetPrevPath( lt->ltPrevPath, &(lt->ltPrevLevel),
							root, paraNr ) )
	{
	int i;

	for ( i= 0; i <= level; i++ )
	    { lt->ltPrevPath[i]= lt->ltHerePath[i];	}

	lt->ltPrevLevel= -1;
	}

    appGuiEnableWidget( lt->ltRemoveFromListButton, 1 );
    appGuiEnableWidget( is->isApplyButton, 1 );
    appGuiEnableWidget( is->isRevertButton, 1 );

    appGuiEnableWidget( lt->ltListLevelFrame, 1 );
    appGuiEnableWidget( lt->ltSetLevelButton, 1 );

    return 0;
    }

void tedFormatToolRefreshListTool(
				ListTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    ListOverride *			lo;
    DocumentList *			dl;
    ListNumberTreeNode *		root;
    const DocumentProperties *		dp= &(bd->bdProperties);

    int					i;

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{
	lt->ltPrevPath[i]= 0;
	lt->ltHerePath[i]= 0;
	lt->ltCurrPath[i]= 0;
	}

    lt->ltPrevLevel= -1;
    lt->ltHereLevel= -1;
    lt->ltFormatIndex= 0;

    if  ( ! sd->sdHasLists					||
	  sd->sdListOverride < 1				||
	  docGetListOfParagraph( &lo, &root, &dl,
				    sd->sdListOverride, bd )	)
	{ tedFormatToolNoList( lt, is );	}
    else{
	if  ( tedFormatToolSetList( lt, is, sd->sdFirstListParaNr,
			sd->sdListOverride, sd->sdListLevel, lo, dl, root ) )
	    { LDEB(sd->sdFirstListParaNr); return;	}

	lt->ltTabIntervalTwips= dp->dpTabIntervalTwips;
	}

    if  ( sd->sdListOverride > 0 && sd->sdMultiList )
	{ appGuiEnableWidget( lt->ltSetListButton, 1 );	}
    else{ appGuiEnableWidget( lt->ltSetListButton, 0 );	}

    /**/

    if  ( lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >= lt->ltListPropertiesChosen.dlLevelCount	)
	{
	if  ( lt->ltListPropertiesChosen.dlLevelCount > 0 )
	    { lt->ltCurrentLevel= 0;	}
	else{ lt->ltCurrentLevel= -1;	}
	}

    tedFormatToolRefreshListPage( lt );

    *pEnabled= ds->dsSelectionScope.ssInExternalItem == DOCinBODY;

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve format string from text box.				*/
/*									*/
/************************************************************************/

static void tedListToolSyncNumberFormat(int *			pChanged,
					ListTool *		lt,
					DocumentListLevel *	dll )
    {
    char *	val;

    val= appGetStringFromTextWidget( lt->ltNumberFormatCurrentText );

    docListLevelSetText( pChanged, dll,
		(unsigned char *)val, lt->ltFormatIndex/ 2 );

    appFreeStringFromTextWidget( val );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Set List' button on the list tool has been pushed.		*/
/*  The 'Set Level' button on the list tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListSetListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;

    const ListOverride *	lo= &(lt->ltListOverrideChosen);

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );

    docInitParagraphProperties( &pp );

    pp.ppListOverride= lo->loIndex;

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListSetLevelPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    if  ( lt->ltCurrentLevel < 0 || lt->ltCurrentLevel >= dl->dlLevelCount )
	{ LLDEB(lt->ltCurrentLevel,dl->dlLevelCount); return;	}

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    pp.ppListLevel= lt->ltCurrentLevel;

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Remove from List' button on the list tool has been pushed.	*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListRemoveFromListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListNewListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;

    if  ( tedAppSetNewList( ea ) )
	{ LDEB(1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the list tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListRevertPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;

    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;

    if  ( docCopyDocumentList( &(lt->ltListPropertiesChosen),
			    &(lt->ltListPropertiesSet), fontMap, colorMap ) )
	{ LDEB(1); return;	}

    if  ( docCopyListOverride( &(lt->ltListOverrideChosen),
			    &(lt->ltListOverrideSet), fontMap, colorMap ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  A level was selected in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedListToolLevelChosen, w, voidlt, voidlcs )
    {
    ListTool *			lt= (ListTool *)voidlt;
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const DocumentListLevel *	dll;

    int				level;

    level= appGuiGetPositionFromListCallback( w, voidlcs );

    /*  No: takes the role of 'Revert'
    if  ( level == lt->ltCurrentLevel )
	{ return;	}
    */

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    lt->ltCurrentLevel= level;
    dll= &(dl->dlLevels[level]);

    if  ( lt->ltFormatIndex > 2* dll->dllNumberSize )
	{ lt->ltFormatIndex=  2* dll->dllNumberSize; }

    tedListToolRefreshCurrentLevel( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the current list in the document.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeList, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);

    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				li;
    int				lipfi;
    int				changed;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    if  ( lt->ltFormatIndex % 2 == 0 && lt->ltFormatEditable )
	{ tedListToolSyncNumberFormat( &changed, lt, dll );	}

    /**/
    lipfi= dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips;
    if  ( appGetLengthFromTextWidget( lt->ltFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    li= dll->dllLeftIndentTwips;
    if  ( appGetLengthFromTextWidget( lt->ltLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					0, 0, maxValue, adaptToMax ) )
	{ return;	}

    if  ( dll->dllLeftIndentTwips != li )
	{
	dll->dllLeftIndentTwips= li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( dll->dllFirstIndentTwips != lipfi- li )
	{
	dll->dllFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	}


    /**/
    if  ( appGetIntegerFromTextWidget( lt->ltStartAtText,
				    &(dll->dllStartAt), 1, 0, INT_MAX, 0 ) )
	{ return;	}

    if  ( tedAppChangeCurrentList( ea, dl ) )
	{ LDEB(1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  A number style has been chosen in the menu in the list level	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedListNumberStyleChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				style;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    style= appGuiGetOptionmenuItemIndex( &(lt->ltNumberStyleOptionmenu), w );
    if  ( style < 0 || style >= LISTTOOLcountSTYLES )
	{ LLDEB(style,LISTTOOLcountSTYLES); return;	}

    dll->dllNumberStyle= TED_ListToolStyles[style];

    tedFormatToolRefreshListPage( lt );

    return;
    }

static APP_OITEM_CALLBACK_H( tedListJustifyChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				justify;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    justify= appGuiGetOptionmenuItemIndex( &(lt->ltJustifyOptionmenu), w );
    if  ( justify < 0 || justify >= DOClla_COUNT )
	{ LLDEB(justify,DOClla_COUNT); return;	}

    dll->dllJustification= justify;

    if  ( dll->dllJustification == DOCllaCENTER		||
	  dll->dllJustification == DOCllaRIGHT		)
	{
	int		refresh= 0;

	if  ( dll->dllLeftIndentTwips < 20 )
	    {
	    dll->dllLeftIndentTwips= lt->ltTabIntervalTwips;
	    PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	    refresh= 1;
	    }

	if  ( dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips < 20	&&
	      dll->dllLeftIndentTwips- lt->ltTabIntervalTwips/ 2 >= 20	)
	    {
	    dll->dllFirstIndentTwips= -lt->ltTabIntervalTwips/ 2;
	    PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	    refresh= 1;
	    }

	if  ( refresh )
	    { tedListToolRefreshIndents( lt, dll );	}
	}

    return;
    }

static APP_OITEM_CALLBACK_H( tedListFollowChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				follow;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    follow= appGuiGetOptionmenuItemIndex( &(lt->ltFollowOptionmenu), w );
    if  ( follow < 0 || follow >= DOCllf_COUNT )
	{ LLDEB(follow,DOCllf_COUNT); return;	}

    dll->dllFollow= follow;

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedLevelStartAtChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    if  ( appGetIntegerFromTextWidget( lt->ltStartAtText,
				    &(dll->dllStartAt), 1, 0, INT_MAX, 0 ) )
	{ return;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelFirstIndentChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				li;
    int				lipfi;
    int				changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    li= dll->dllLeftIndentTwips;
    lipfi=  dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips;

    if  ( tedFormatValidateDimension( &lipfi, &changed,
					    lt->ltFirstIndentText, lipfi ) )
	{ return;	}

    if  ( dll->dllLeftIndentTwips != li )
	{
	dll->dllLeftIndentTwips= li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( dll->dllFirstIndentTwips != lipfi- li )
	{
	dll->dllFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelLeftIndentChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				li;
    int				lipfi;
    int				changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    li= dll->dllLeftIndentTwips;
    lipfi=  dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips;

    if  ( tedFormatValidateDimension( &li, &changed,
						lt->ltLeftIndentText, li ) )
	{ return;	}

    if  ( dll->dllLeftIndentTwips != li )
	{
	dll->dllLeftIndentTwips= li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( dll->dllFirstIndentTwips != lipfi- li )
	{
	dll->dllFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	}

    return;
    }



/************************************************************************/

static void tedNavigLevelComponent(	ListTool *	lt,
					int		to )
    {
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    if  ( to < 0 || to > 2* dll->dllNumberSize )
	{ return;	}

    if  ( lt->ltFormatIndex % 2 == 0 && lt->ltFormatEditable )
	{
	int		changed= 0;

	tedListToolSyncNumberFormat( &changed, lt, dll );

	if  ( changed )
	    { tedFormatToolRefreshListPage( lt );	}
	}

    lt->ltFormatIndex= to;

    tedListToolSetFormatEditable( lt, 0 );
    tedListToolRefreshLevelFormat( lt, dll, level );
    tedListLevelRefreshFormatMenu( lt );

    return;
    }

# ifdef USE_MOTIF
static void tedPrevLevelComponentH(	APP_WIDGET	x,
					void *		voidlt,
					XEvent *	event,
					Boolean *	pRefused )
    {
    ListTool *			lt= (ListTool *)voidlt;

    tedNavigLevelComponent( lt, lt->ltFormatIndex- 1 );
    }

static void tedNextLevelComponentH(	APP_WIDGET	x,
					void *		voidlt,
					XEvent *	event,
					Boolean *	pRefused )
    {
    ListTool *			lt= (ListTool *)voidlt;

    tedNavigLevelComponent( lt, lt->ltFormatIndex+ 1 );
    }
# endif

static APP_EVENT_HANDLER_H( tedListToolNumberKeyPress, w, voidlt, keyEvent )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    APP_KEY_VALUE		keySym;
    unsigned char		scratch[40];
    int				gotString;
    int				gotKey;
    unsigned int		state;

    int				changed= 0;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    appGuiGetStringFromKeyboardEvent( (APP_INPUT_CONTEXT)0, w, keyEvent,
				&gotString, &gotKey, &state,
				scratch, sizeof(scratch), &keySym );

    switch( keySym )
	{
	case KEY_KP_Left:
	case KEY_Left:
	    tedNavigLevelComponent( lt, lt->ltFormatIndex- 1 );
	    break;

	case KEY_KP_Right:
	case KEY_Right:
	    tedNavigLevelComponent( lt, lt->ltFormatIndex+ 1 );
	    break;

	case KEY_KP_Home:
	case KEY_Home:
	    tedNavigLevelComponent( lt, 0 );
	    break;

	case KEY_KP_End:
	case KEY_End:
	    tedNavigLevelComponent( lt, 2* dll->dllNumberSize );
	    break;

	case KEY_KP_Enter:
	case KEY_Return:
	    if  ( lt->ltFormatIndex % 2 == 0 )
		{ tedListToolSetFormatEditable( lt, 1 );	}
	    else{
#		ifdef USE_MOTIF
		Position	screenX;
		Position	screenY;

		XtTranslateCoords( lt->ltNumberFormatCurrentLabel, 0, 0,
							&screenX, &screenY );

		XtVaSetValues( lt->ltNumberFormatMenu,
					XmNx,	screenX,
					XmNy,	screenY,
					NULL );

		XtManageChild( lt->ltNumberFormatMenu );
#		endif
		}
	    break;

	case KEY_KP_Insert:
	case KEY_Insert:
	    tedListInsertFormatLevel( lt );
	    break;

	case KEY_KP_Delete:
	case KEY_Delete:
	case KEY_BackSpace:
	    if  ( lt->ltFormatIndex % 2 == 0 )
		{
		docListLevelSetText( &changed, dll,
			    (unsigned char *)"", lt->ltFormatIndex/ 2 );

		if  ( changed )
		    { tedFormatToolRefreshListPage( lt );	}
		}
	    else{ tedListDeleteFormatLevel( lt );	}
	    break;

	default:
	    return;
	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedListToolGetNumberFormat, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    if  ( lt->ltFormatIndex % 2 == 0 && lt->ltFormatEditable )
	{
	int		changed= 0;

	tedListToolSyncNumberFormat( &changed, lt, dll );

	if  ( changed )
	    { tedFormatToolRefreshListPage( lt );	}

	tedListToolSetFormatEditable( lt, 0 );
	}

    return;
    }

static void appMakeHBoxInRow(		APP_WIDGET *	pHBox,
					APP_WIDGET	row,
					int		position,
					int		colspan )
    {
#   ifdef USE_MOTIF
    Arg			al[20];
    int			ac= 0;

    Widget		hbox;

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ colspan ); ac++;

    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;

    hbox= XmCreateForm( row, WIDGET_NAME, al, ac );

    XtManageChild( hbox );

    *pHBox= hbox; return;
#   endif
    }

static void appMakeLabelAndRowRow(	APP_WIDGET *		pRow,
					APP_WIDGET *		pLabel,
					APP_WIDGET *		pHBox,
					APP_WIDGET		column,
					const char *		labelText )
    {
    APP_WIDGET		label;
    APP_WIDGET		hbox;
    APP_WIDGET		row;

    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		hboxColumn= 1;
    const int		hboxColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeHBoxInRow( &hbox, row, hboxColumn, hboxColspan );

    *pRow= row; *pLabel= label; *pHBox= hbox; return;
    }

/************************************************************************/
/*									*/
/*  Make the tabs page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillListsPage(	ListTool *			lt,
				const ListsPageResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		startLabel;
    APP_WIDGET		styleLabel;
    APP_WIDGET		justifyLabel;
    APP_WIDGET		followLabel;
    APP_WIDGET		firstLabel;
    APP_WIDGET		leftLabel;

    APP_WIDGET		row;

    const int		visibleItems= 9;

    const int		textColumns= 10;

    /**/
    lt->ltPageResources= lpr;

    /**/
    docInitDocumentList( &(lt->ltListPropertiesSet) );
    docInitDocumentList( &(lt->ltListPropertiesChosen) );

    docInitListOverride( &(lt->ltListOverrideSet) );
    docInitListOverride( &(lt->ltListOverrideChosen) );

    lt->ltCurrentLevel= -1;
    lt->ltCurrentParagraphNumber= -1;


    /**/
    appGuiMakeListInColumn( &(lt->ltListLevelList), pageWidget,
			    visibleItems, tedListToolLevelChosen, (void *)lt );

    /**/
    appMakeColumnFrameInColumn( &(lt->ltSelectionFrame),
				    &(lt->ltSelectionPaned),
				    pageWidget, lpr->lprSelection );

    appInspectorMakeButtonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetListButton), &(lt->ltRemoveFromListButton),
		    lpr->lprSetList, lpr->lprRemoveFromList,
		    tedFormatListSetListPushed,
		    tedFormatListRemoveFromListPushed, lt );

    appInspectorMakeButtonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetLevelButton), &(lt->ltNewListButton),
		    lpr->lprSetLevel, lpr->lprNewList,
		    tedFormatListSetLevelPushed,
		    tedFormatListNewListPushed, lt );

    /**/
    appMakeColumnFrameInColumn( &(lt->ltListLevelFrame),
				    &(lt->ltListLevelPaned),
				    pageWidget, lpr->lprListLevel );

    /**/
    appInspectorMakeMenuRow( &row, &(lt->ltNumberStyleOptionmenu),
				    &styleLabel, lt->ltListLevelPaned,
				    lpr->lprNumberStyle );

    /**/
    appMakeLabelAndRowRow( &row, &(lt->ltFormatLabel),
				    &(lt->ltNumberFormatHBox),
				    lt->ltListLevelPaned,
				    lpr->lprNumberFormat );
# ifdef USE_MOTIF
{
    Display *		display= XtDisplay( row );
    int			screen= DefaultScreen( display );
    Pixel		whitePixel= WhitePixel( display, screen );
    Pixel		blackPixel= BlackPixel( display, screen );

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    lt->ltNumberFormatHead= XmCreateLabel(
				lt->ltNumberFormatHBox, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftWidget,		lt->ltNumberFormatHead ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    XtSetArg( al[ac], XmNtraversalOn,		True ); ac++;
    XtSetArg( al[ac], XmNmappedWhenManaged,	False ); ac++;

    lt->ltNumberFormatCurrentLabel= XmCreateLabel(
				lt->ltNumberFormatHBox, WIDGET_NAME, al, ac );

    {
	Pixel	backPixel;
	Pixel	forePixel;

	XtVaGetValues(	lt->ltNumberFormatCurrentLabel,
				    XmNbackground,	&forePixel,
				    XmNforeground,	&backPixel,
				    NULL );

	XtVaSetValues(	lt->ltNumberFormatCurrentLabel,
				    XmNbackground,	backPixel,
				    XmNforeground,	forePixel,
				    NULL );
    }

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftWidget,		lt->ltNumberFormatHead ); ac++;

    XtSetArg( al[ac], XmNcolumns,		2 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		TXmargH ); ac++;
    XtSetArg( al[ac], XmNborderWidth,		0 ); ac++;

    XtSetArg( al[ac], XmNbackground,		whitePixel ); ac++;
    XtSetArg( al[ac], XmNforeground,		blackPixel ); ac++;

    XtSetArg( al[ac], XmNmappedWhenManaged,	False ); ac++;

    lt->ltNumberFormatCurrentText= XmCreateText(
				lt->ltNumberFormatHBox, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftWidget,		lt->ltNumberFormatCurrentLabel ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    lt->ltNumberFormatLabelTail= XmCreateLabel(
				lt->ltNumberFormatHBox, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftWidget,		lt->ltNumberFormatCurrentText ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    XtSetArg( al[ac], XmNmappedWhenManaged,	False ); ac++;

    lt->ltNumberFormatTextTail= XmCreateLabel(
				lt->ltNumberFormatHBox, WIDGET_NAME, al, ac );

    XtManageChild( lt->ltNumberFormatHead );
    XtManageChild( lt->ltNumberFormatCurrentLabel );
    XtManageChild( lt->ltNumberFormatCurrentText );
    XtManageChild( lt->ltNumberFormatLabelTail );
    XtManageChild( lt->ltNumberFormatTextTail );

    XtAddEventHandler( lt->ltFormatLabel, ButtonReleaseMask, False,
					tedPrevLevelComponentH, (void *)lt );
    XtAddEventHandler( lt->ltNumberFormatHead, ButtonReleaseMask, False,
					tedPrevLevelComponentH, (void *)lt );
    XtAddEventHandler( lt->ltNumberFormatLabelTail, ButtonReleaseMask, False,
					tedNextLevelComponentH, (void *)lt );
    XtAddEventHandler( lt->ltNumberFormatTextTail, ButtonReleaseMask, False,
					tedNextLevelComponentH, (void *)lt );

    XtAddEventHandler( lt->ltNumberFormatCurrentLabel, ButtonPressMask, False,
					tedListToolNumberPopup, (void *)lt );
    XtAddEventHandler( lt->ltNumberFormatCurrentLabel, KeyPressMask, False,
					tedListToolNumberKeyPress, (void *)lt );

					/********************************/
					/*  Attach to label, managing	*/
					/*  unmanaging the text		*/
					/*  confuses Motif.		*/
					/********************************/
    ac= 0;
    lt->ltNumberFormatMenu= XmCreatePopupMenu(
					    lt->ltFormatLabel,
					    WIDGET_NAME,
					    al, ac );

    lt->ltFormatEditable= 0;
}
# endif


    /**/
    appGuiSetGotValueCallbackForText( lt->ltNumberFormatCurrentText,
				    tedListToolGetNumberFormat, (void *)lt );

    /**/
    appMakeLabelAndTextRow( &row, &firstLabel, &(lt->ltFirstIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelFirstIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltFirstIndentText,
				    tedLevelFirstIndentChanged, (void *)lt );
    /**/
    appMakeLabelAndTextRow( &row, &leftLabel, &(lt->ltLeftIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltLeftIndentText,
				    tedLevelLeftIndentChanged, (void *)lt );

    /**/
    appMakeLabelAndTextRow( &row, &startLabel, &(lt->ltStartAtText),
				    lt->ltListLevelPaned,
				    lpr->lprStartAt, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltStartAtText,
					tedLevelStartAtChanged, (void *)lt );
    /**/
    appInspectorMakeMenuRow( &row, &(lt->ltJustifyOptionmenu),
				    &justifyLabel, lt->ltListLevelPaned,
				    lpr->lprJustify );

    appInspectorMakeMenuRow( &row, &(lt->ltFollowOptionmenu),
				    &followLabel, lt->ltListLevelPaned,
				    lpr->lprFollowedBy );

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatListRevertPushed, tedFormatChangeList, lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedFormatFillListChoosers(		ListTool *			lt )
    {
    const ListsPageResources *	lpr= lt->ltPageResources;

    appFillInspectorMenu( LISTTOOLcountSTYLES, 0,
			lt->ltNumberStyleItems, lpr->lprStyleOptionTexts,
			&(lt->ltNumberStyleOptionmenu),
			tedListNumberStyleChosen, (void *)lt );

    appFillInspectorMenu( DOClla_COUNT, DOCllaLEFT,
			lt->ltJustifyItems, lpr->lprJustifyOptionTexts,
			&(lt->ltJustifyOptionmenu),
			tedListJustifyChosen, (void *)lt );

    appFillInspectorMenu( DOCllf_COUNT, DOCllfTAB,
			lt->ltFollowItems, lpr->lprFollowOptionTexts,
			&(lt->ltFollowOptionmenu),
			tedListFollowChosen, (void *)lt );

    return;
    }

void tedFormatFinishListPage(		ListTool *			lt )
    {
    appOptionmenuRefreshWidth( &(lt->ltNumberStyleOptionmenu) );
    appOptionmenuRefreshWidth( &(lt->ltJustifyOptionmenu) );
    appOptionmenuRefreshWidth( &(lt->ltFollowOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a tabs tool.				*/
/*									*/
/************************************************************************/

void tedFormatCleanListsTool(	ListTool *			lt )
    {
    docCleanDocumentList( &(lt->ltListPropertiesSet) );
    docCleanDocumentList( &(lt->ltListPropertiesChosen) );
    
    docCleanListOverride( &(lt->ltListOverrideSet) );
    docCleanListOverride( &(lt->ltListOverrideChosen) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get list tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedListSubjectResourceTable[]=
    {
    APP_RESOURCE( "listToolParagraphNumber",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Bullets and Numbering" ),
    APP_RESOURCE( "listToolChangeList",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Change List" ),
    APP_RESOURCE( "listToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    };

static AppConfigurableResource TED_TedListToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "listToolSelection",
	offsetof(ListsPageResources,lprSelection),
	"Selection" ),
    APP_RESOURCE( "listToolSetList",
	offsetof(ListsPageResources,lprSetList),
	"Set List" ),
    APP_RESOURCE( "listToolRemoveFromList",
	offsetof(ListsPageResources,lprRemoveFromList),
	"Remove from List" ),
    APP_RESOURCE( "listToolSetLevel",
	offsetof(ListsPageResources,lprSetLevel),
	"Set Level" ),
    APP_RESOURCE( "listToolNewList",
	offsetof(ListsPageResources,lprNewList),
	"New List" ),

    /**/
    APP_RESOURCE( "listToolListLevel",
	offsetof(ListsPageResources,lprListLevel),
	"Level %d" ),

    APP_RESOURCE( "listToolLevelFirstIndent",
	offsetof(ListsPageResources,lprLevelFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "listToolLevelLeftIndent",
	offsetof(ListsPageResources,lprLevelLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "listToolListLevelStartAt",
	offsetof(ListsPageResources,lprStartAt),
	"First Number" ),
    APP_RESOURCE( "listToolListLevelNoRestart",
	offsetof(ListsPageResources,lprNoRestart),
	"Continuous" ),

    /**/
    APP_RESOURCE( "listToolLevelNumberStyle",
	offsetof(ListsPageResources,lprNumberStyle),
	"Style" ),
    APP_RESOURCE( "listToolLevelNumberFormat",
	offsetof(ListsPageResources,lprNumberFormat),
	"Format" ),

    /**/
    APP_RESOURCE( "listToolListLevelStyleNar",
	    offsetof(ListsPageResources,lprStyleOptionTexts[0]),
	    "1, 2, 3" ),
    APP_RESOURCE( "listToolListLevelStyleNruc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[1]),
	    "I, II, III" ),
    APP_RESOURCE( "listToolListLevelStyleNrlc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[2]),
	    "i, ii, iii" ),
    APP_RESOURCE( "listToolListLevelStyleNauc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[3]),
	    "A, B, C" ),
    APP_RESOURCE( "listToolListLevelStyleNalc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[4]),
	    "a, b, c" ),
    APP_RESOURCE( "listToolListLevelStyleNar0",
	    offsetof(ListsPageResources,lprStyleOptionTexts[5]),
	    "01, 02, 10, 11" ),
    APP_RESOURCE( "listToolListLevelStyleBullet",
	    offsetof(ListsPageResources,lprStyleOptionTexts[6]),
	    "Bullet" ),
    APP_RESOURCE( "listToolListLevelStyleNone",
	    offsetof(ListsPageResources,lprStyleOptionTexts[7]),
	    "None" ),

    /**/
    APP_RESOURCE( "listToolLevelJustify",
	offsetof(ListsPageResources,lprJustify),
	"Justify" ),
    /**/
    APP_RESOURCE( "listToolListLevelJustifyLeft",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaLEFT]),
	    "Left" ),
    APP_RESOURCE( "listToolListLevelJustifyCentered",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaCENTER]),
	    "Centered" ),
    APP_RESOURCE( "listToolListLevelJustifyRight",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaRIGHT]),
	    "Right" ),

    /**/
    APP_RESOURCE( "listToolLevelFollowedBy",
	offsetof(ListsPageResources,lprFollowedBy),
	"Separator" ),
    /**/
    APP_RESOURCE( "listToolListLevelFollowTab",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfTAB]),
	    "Tab" ),
    APP_RESOURCE( "listToolListLevelFollowSpace",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfSPACE]),
	    "Space" ),
    APP_RESOURCE( "listToolListLevelFollowNone",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfNONE]),
	    "None" ),

    /**/
    APP_RESOURCE( "listToolDeleteLevelNumber",
	    offsetof(ListsPageResources,lprDeleteLevelNumber),
	    "Delete" ),
    APP_RESOURCE( "listToolInsertLevelNumber",
	    offsetof(ListsPageResources,lprInsertLevelNumber),
	    "Insert" ),
    APP_RESOURCE( "listToolEditLevelText",
	    offsetof(ListsPageResources,lprEditLevelText),
	    "Edit" ),
    };

void tedFormatToolGetListsResourceTable(EditApplication *		ea,
					ListsPageResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)lpr,
				TED_TedListToolResourceTable,
				sizeof(TED_TedListToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedListSubjectResourceTable,
				sizeof(TED_TedListSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
