/************************************************************************/
/*									*/
/*  Ted: Link page on format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedApp.h"
#   include	"tedLinkTool.h"
#   include	"appInspector.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedLinkAdaptToText(	LinkTool *		lt,
				const char *		fileName,
				const char *		markName )
    {
    int				gotSomething;
    int				isLocal;
    int				changed;

    const char *		fileSaved= lt->ltFileSet;
    const char *		markSaved= lt->ltMarkSet;

    if  ( ! fileSaved )
	{ fileSaved= "";	}
    if  ( ! markSaved )
	{ markSaved= "";	}

    if  ( ! fileName )
	{ fileName= "";	}
    if  ( ! markName )
	{ markName= "";	}

    isLocal= fileName[0] == '\0';

    gotSomething= markName[0] != '\0' || fileName[0] != '\0';
    changed= strcmp( fileName, fileSaved ) || strcmp( markName, markSaved );

    appGuiEnableWidget( lt->ltSetLinkButton,
			! lt->ltDocumentReadonly && changed && gotSomething );
    appGuiEnableWidget( lt->ltRemoveLinkButton, gotSomething && ! changed );

    appGuiEnableWidget( lt->ltFollowLinkButton, gotSomething );

    appGuiEnableWidget( lt->ltMarkListWidget, isLocal );

    if  ( lt->ltLinkAsItems[LINKkindHYPERLINK] )
	{
	appGuiEnableWidget( lt->ltLinkAsItems[LINKkindPAGE_NUMBER], isLocal );
	appGuiEnableWidget( lt->ltLinkAsItems[LINKkindBOOKMARK_TEXT], isLocal );
	appGuiEnableWidget( lt->ltLinkAsItems[LINKkindTEXT_PLUS_PAGE], isLocal);

	if  ( lt->ltLinkKind != LINKkindHYPERLINK && ! isLocal )
	    {
	    lt->ltLinkKind= LINKkindHYPERLINK;
	    appSetOptionmenu( &(lt->ltLinkAsOptionmenu), LINKkindHYPERLINK );
	    }
	}

    return;
    }

/*  A  */
static APP_LIST_CALLBACK_H( tedLinkBookmarkChosen, w, voidlt, voidlcs )
    {
    LinkTool *		lt= (LinkTool *)voidlt;

    char *			fileName;
    char *			markName;

    markName= appGuiGetStringFromListCallback( w, voidlcs );
    if  ( ! markName )
	{ XDEB(markName); return;	}
    fileName= appGetStringFromTextWidget( lt->ltFileTextWidget );
    if  ( ! fileName )
	{ XDEB(fileName); return;	}

    appStringToTextWidget( lt->ltMarkTextWidget, markName );

    tedLinkAdaptToText( lt, fileName, markName );

    appFreeStringFromListCallback( markName );
    appFreeStringFromTextWidget( fileName );

    return;
    }

/*  B  */
static APP_TXTYPING_CALLBACK_H( tedLinkDestinationChanged, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    char *			fileName;
    char *			markName;

    fileName= appGetStringFromTextWidget( lt->ltFileTextWidget );
    if  ( ! fileName )
	{ XDEB(fileName); return;	}
    markName= appGetStringFromTextWidget( lt->ltMarkTextWidget );
    if  ( ! markName )
	{ XDEB(markName); return;	}

    tedLinkAdaptToText( lt, fileName, markName );

    appFreeStringFromTextWidget( fileName );
    appFreeStringFromTextWidget( markName );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of link has been chosen.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedLinkKindChosen, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    int			linkKind;

    linkKind= appGuiGetOptionmenuItemIndex( &(lt->ltLinkAsOptionmenu), w );
    if  ( linkKind < 0 || linkKind >= LINKkind_COUNT )
	{ LLDEB(linkKind,LINKkind_COUNT); return;	}

    lt->ltLinkKind= linkKind;

    return;
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRevertPushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;

    if  ( lt->ltFileChosen )
	{
	free( lt->ltFileChosen );
	lt->ltFileChosen= (char *)0;
	}

    if  ( lt->ltMarkChosen )
	{
	free( lt->ltMarkChosen );
	lt->ltMarkChosen= (char *)0;
	}

    if  ( lt->ltFileSet )
	{
	lt->ltFileChosen= strdup( lt->ltFileSet );
	if  ( ! lt->ltFileChosen )
	    { XDEB(lt->ltFileChosen);	}
	}

    if  ( lt->ltMarkSet )
	{
	lt->ltMarkChosen= strdup( lt->ltMarkSet );
	if  ( ! lt->ltMarkChosen )
	    { XDEB(lt->ltMarkChosen);	}
	}

    if  ( lt->ltFileChosen )
	{ appStringToTextWidget( lt->ltFileTextWidget, lt->ltFileChosen ); }
    else{ appStringToTextWidget( lt->ltFileTextWidget, "" );		   }

    if  ( lt->ltMarkChosen )
	{ appStringToTextWidget( lt->ltMarkTextWidget, lt->ltMarkChosen ); }
    else{ appStringToTextWidget( lt->ltMarkTextWidget, "" );		   }

    tedLinkAdaptToText( lt, lt->ltFileChosen, lt->ltMarkChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkChangePushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    EditApplication *	ea= lt->ltApplication;

    char *		file= (char *)0;
    char *		mark= (char *)0;

    int			asRef;
    int			asPageref;

    file= appGetStringFromTextWidget( lt->ltFileTextWidget );
    if  ( ! file )
	{ XDEB(file); goto ready;	}
    mark= appGetStringFromTextWidget( lt->ltMarkTextWidget );
    if  ( ! mark )
	{ XDEB(mark); goto ready;	}

    switch( lt->ltLinkKind )
	{
	case LINKkindHYPERLINK:
	    asRef= asPageref= 0;
	    break;

	case LINKkindPAGE_NUMBER:
	    asRef= 0; asPageref= 1;
	    break;

	case LINKkindBOOKMARK_TEXT:
	    asRef= 1; asPageref= 0;
	    break;

	case LINKkindTEXT_PLUS_PAGE:
	    asRef= asPageref= 1;
	    break;

	default:
	    LDEB(lt->ltLinkKind);
	    asRef= asPageref= 0;
	    break;
	}

    if  ( tedAppSetHyperlink( ea, file, mark, asRef, asPageref ) )
	{ SSDEB(file,mark);	}

  ready:
    if  ( file )
	{ appFreeStringFromTextWidget( file );	}
    if  ( mark )
	{ appFreeStringFromTextWidget( mark );	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Follow Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkFollowPushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    EditApplication *	ea= lt->ltApplication;

    char *		file= (char *)0;
    char *		mark= (char *)0;

    int			fileSize;
    int			markSize;

    file= appGetStringFromTextWidget( lt->ltFileTextWidget );
    if  ( ! file )
	{ XDEB(file); goto ready;	}
    mark= appGetStringFromTextWidget( lt->ltMarkTextWidget );
    if  ( ! mark )
	{ XDEB(mark); goto ready;	}

    fileSize= strlen( file );
    markSize= strlen( mark );

    if  ( tedAppFollowLink( (APP_WIDGET)w, ea, file, fileSize, mark, markSize ) )
	{ SSDEB(file,mark);	}

  ready:
    if  ( file )
	{ appFreeStringFromTextWidget( file );	}
    if  ( mark )
	{ appFreeStringFromTextWidget( mark );	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Remove Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRemovePushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    EditApplication *	ea= lt->ltApplication;

    if  ( tedAppRemoveHyperlink( ea ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a link tool. I.E. the 'Link' page on the format tool to manage	*/
/*  Hyperlinks.								*/
/*									*/
/************************************************************************/

void tedFormatFillLinkPage(	LinkTool *			lt,
				const LinkToolResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET		label;
    APP_WIDGET		row;

    const int		listVisibleItems= 9;

    lt->ltPageResources= lpr;

    /* FULL WIDTH */
    appMakeLabelInColumn( &label, pageWidget, lpr->lprFileText );
    appMakeTextInColumn( &(lt->ltFileTextWidget), pageWidget, 0, 1 );
    appMakeLabelInColumn( &label, pageWidget, lpr->lprMarkText );
    appMakeTextInColumn( &(lt->ltMarkTextWidget), pageWidget, 0, 1 );

    appGuiSetTypingCallbackForText( lt->ltFileTextWidget,
				tedLinkDestinationChanged, (void *)lt );
    appGuiSetTypingCallbackForText( lt->ltMarkTextWidget,
				tedLinkDestinationChanged, (void *)lt );

    appGuiMakeListInColumn( &(lt->ltMarkListWidget), pageWidget,
			listVisibleItems, tedLinkBookmarkChosen, (void *)lt );

    appMakeColumnFrameInColumn( &(lt->ltLinkAsFrame), &(lt->ltLinkAsPaned),
						    pageWidget,
						    lpr->lprLinkAsText );

    appMakeOptionmenuInColumn( &(lt->ltLinkAsOptionmenu),
						    lt->ltLinkAsPaned );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(lt->ltFollowLinkButton), &(lt->ltRemoveLinkButton),
	    lpr->lprFollowLinkText, lpr->lprRemoveLinkText,
	    tedLinkFollowPushed, tedLinkRemovePushed, (void *)lt );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedLinkRevertPushed, tedLinkChangePushed, (void *)lt );

    lt->ltSetLinkButton= is->isApplyButton;

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a link tool					*/
/*									*/
/************************************************************************/

void tedFormatInitLinkTool(		LinkTool *		lt )
    {
    int		i;

    lt->ltCurrentDocumentId= 0;

    for ( i= 0; i < LINKkind_COUNT; i++ )
	{ lt->ltLinkAsItems[i]= (APP_WIDGET)0; }

    lt->ltFileTextWidget= (APP_WIDGET)0;
    lt->ltMarkTextWidget= (APP_WIDGET)0;

    lt->ltSetLinkButton= (APP_WIDGET)0;
    lt->ltFollowLinkButton= (APP_WIDGET)0;
    lt->ltRemoveLinkButton= (APP_WIDGET)0;
    lt->ltCancelButton= (APP_WIDGET)0;

    lt->ltFileSet= (char *)0;
    lt->ltMarkSet= (char *)0;
    lt->ltFileChosen= (char *)0;
    lt->ltMarkChosen= (char *)0;

    appInitOptionmenu( &(lt->ltLinkAsOptionmenu) );

    return;
    }

void tedFormatCleanLinkTool(		LinkTool *		lt )
    {
    if  ( lt->ltFileSet )
	{ free( lt->ltFileSet );	}
    if  ( lt->ltMarkSet )
	{ free( lt->ltMarkSet );	}

    if  ( lt->ltFileChosen )
	{ free( lt->ltFileChosen );	}
    if  ( lt->ltMarkChosen )
	{ free( lt->ltMarkChosen );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the tabs page.						*/
/*									*/
/************************************************************************/

void tedLinkToolFillChoosers(	LinkTool *			lt,
				const LinkToolResources *	ltr )
    {
    const LinkToolResources *	lpr= lt->ltPageResources;

    appFillInspectorMenu( LINKkind_COUNT, LINKkindHYPERLINK,
			lt->ltLinkAsItems, lpr->lprLinkAsItemTexts,
			&(lt->ltLinkAsOptionmenu),
			tedLinkKindChosen, (void *)lt );

    return;
    }

void tedFinishLinkTool(		LinkTool *			lt,
				const LinkToolResources *	ltr )
    {
    appOptionmenuRefreshWidth( &(lt->ltLinkAsOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the link tool.						*/
/*									*/
/************************************************************************/

void tedRefreshLinkTool(	LinkTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    DocumentFieldList *		dfl= &(bd->bdFieldList);

    DocumentSelection		dsHyperlink;
    int				startPart;
    int				endPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    const char *		markName= (const char *)0;
    int				markSize= 0;

    *pEnabled= ( sd->sdIsSingleParagraph	&&
	    ( sd->sdBeginInHyperlink	||
	      ! sd->sdIsIBarSelection	)	);

    if  ( ! *pEnabled )
	{ return;	}

    if  ( lt->ltCurrentDocumentId != sd->sdDocumentId )
	{
	tedFillBookmarkList( lt->ltMarkListWidget, dfl );

	lt->ltCurrentDocumentId= sd->sdDocumentId;
	}

    if  ( ! docGetHyperlinkForPosition( bd, &(ds->dsBegin),
				&dsHyperlink, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{ *pPref= 8;	}

    if  ( lt->ltFileSet )
	{
	free( lt->ltFileSet );
	lt->ltFileSet= (char *)0;
	}
    if  ( lt->ltMarkSet )
	{
	free( lt->ltMarkSet );
	lt->ltMarkSet= (char *)0;
	}

    if  ( lt->ltFileSet )
	{
	free( lt->ltFileSet );
	lt->ltFileSet= (char *)0;
	}
    if  ( lt->ltMarkSet )
	{
	free( lt->ltMarkSet );
	lt->ltMarkChosen= (char *)0;
	}

    lt->ltDocumentReadonly= sd->sdDocumentReadonly;

    if  ( fileSize+ markSize > 0 )
	{
	char *		saved;

	if  ( fileSize > 0 )
	    {
	    saved= (char *)malloc( fileSize+ 1 );

	    if  ( ! saved )
		{ XDEB(saved);	}
	    else{
		strncpy( saved, fileName, fileSize )[fileSize]= '\0';
		appStringToTextWidget( lt->ltFileTextWidget, saved );
		lt->ltFileSet= saved;
		fileName= lt->ltFileSet;

		lt->ltFileChosen= strdup( lt->ltFileSet );
		if  ( ! lt->ltFileChosen )
		    { XDEB(lt->ltFileChosen);	}
		}
	    }
	else{ appStringToTextWidget( lt->ltFileTextWidget, "" ); }

	if  ( markSize > 0 )
	    {
	    saved= (char *)malloc( markSize+ 1 );

	    if  ( ! saved )
		{ XDEB(saved);	}
	    else{
		strncpy( saved, markName, markSize )[markSize]= '\0';
		appStringToTextWidget( lt->ltMarkTextWidget, saved );
		lt->ltMarkSet= saved;
		markName= lt->ltMarkSet;

		lt->ltMarkChosen= strdup( lt->ltMarkSet );
		if  ( ! lt->ltMarkChosen )
		    { XDEB(lt->ltMarkChosen);	}
		}
	    }
	else{ appStringToTextWidget( lt->ltMarkTextWidget, "" ); }

	appGuiEnableWidget( lt->ltRemoveLinkButton, ! lt->ltDocumentReadonly );
	appGuiEnableWidget( lt->ltFollowLinkButton, 1 );
	}
    else{
	appGuiEnableWidget( lt->ltRemoveLinkButton, 0 );
	appGuiEnableWidget( lt->ltFollowLinkButton, 0 );

	appStringToTextWidget( lt->ltFileTextWidget, "" );
	appStringToTextWidget( lt->ltMarkTextWidget, "" );
	}

    appGuiEnableWidget( lt->ltMarkListWidget, fileSize > 0 );

    appGuiEnableWidget( lt->ltSetLinkButton, 0 );

    lt->ltLinkKind= LINKkindHYPERLINK;
    appSetOptionmenu( &(lt->ltLinkAsOptionmenu), LINKkindHYPERLINK );

    appGuiEnableOptionmenu( &(lt->ltLinkAsOptionmenu),
					    fileSize+ markSize == 0 );

    tedLinkAdaptToText( lt, fileName, markName );

    *pEnabled= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Get link tool resources.						*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedLinkSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolLink",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Hyperlink" ),
    APP_RESOURCE( "hyperlinkSetLink",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set Link" ),

    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "hyperlinkRemoveLink",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"hyperlinkRemoveLink Link" ),
    };

static AppConfigurableResource TED_TedLinkToolResourceTable[]=
{
    APP_RESOURCE( "hyperlinkDocument",
		offsetof(LinkToolResources,lprFileText),
		"Document" ),
    APP_RESOURCE( "hyperlinkBookmark",
		offsetof(LinkToolResources,lprMarkText),
		"Bookmark" ),

    APP_RESOURCE( "hyperlinkLinkAs",
		offsetof(LinkToolResources,lprLinkAsText),
		"Link as" ),

    APP_RESOURCE( "hyperlinkLinkAsHyperlink",
		offsetof(LinkToolResources,
		lprLinkAsItemTexts[LINKkindHYPERLINK]),
		"Hyperlink" ),
    APP_RESOURCE( "hyperlinkLinkAsPageNumber",
		offsetof(LinkToolResources,
		lprLinkAsItemTexts[LINKkindPAGE_NUMBER]),
		"Page Number" ),
    APP_RESOURCE( "hyperlinkLinkAsBookmarkText",
		offsetof(LinkToolResources,
		lprLinkAsItemTexts[LINKkindBOOKMARK_TEXT]),
		"Bookmark Text" ),
    APP_RESOURCE( "hyperlinkLinkAsTextPlusPage",
		offsetof(LinkToolResources,
		lprLinkAsItemTexts[LINKkindTEXT_PLUS_PAGE]),
		"Text and Page Number" ),

    APP_RESOURCE( "hyperlinkRemoveLink",
		offsetof(LinkToolResources,lprRemoveLinkText),
		"Remove Link" ),
    APP_RESOURCE( "hyperlinkFollowLink",
		offsetof(LinkToolResources,lprFollowLinkText),
		"Follow Link" ),
    APP_RESOURCE( "hyperlinkCancel",
		offsetof(LinkToolResources,lprCancelText),
		"Cancel" ),
};

void tedLinkToolGetResourceTable(	EditApplication *		ea,
					LinkToolResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)lpr,
				TED_TedLinkToolResourceTable,
				sizeof(TED_TedLinkToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedLinkSubjectResourceTable,
				sizeof(TED_TedLinkSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
