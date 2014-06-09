/************************************************************************/
/*									*/
/*  Mail Dialog.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<appFrame.h>
#   include	<sioSmtp.h>

/************************************************************************/
/*									*/
/*  Represents a mail dialog.						*/
/*									*/
/************************************************************************/
#   define	FILEL	400

typedef struct AppMailDialog
    {
    AppDialog			amdDialog;

    AppOptionmenu		amdContentOptionmenu;

    APP_WIDGET			amdButtonRow;
    APP_WIDGET			amdCancelButton;
    APP_WIDGET			amdSendButton;

    char *			amdSubjectText;
    char *			amdToText;
    char *			amdFromText;
    char *			amdCcText;
    char *			amdBccText;

    APP_WIDGET			amdSubjectWidget;
    APP_WIDGET			amdToWidget;
    APP_WIDGET			amdFromWidget;
    APP_WIDGET			amdCcWidget;
    APP_WIDGET			amdBccWidget;

    int				amdContentChosen;

    APP_WIDGET			amdOption;
    EditApplication *		amdApplication;

    char *			amdContentText;
    char *			amdNoneText;
    char *			amdCancelText;
    char *			amdSendText;
    char *			amdMsgOkText;

    char *			amdSubjectMissing;
    char *			amdToMissing;
    char *			amdFromMissing;

    char *			amdDefaultContent;

    char *			amdMailFrom;
    char *			amdMailHost;
    char *			amdMailPort;

    int				amdContentOptionCount;
    APP_WIDGET			amdContentOptions[1]; /* LAST ! */
    } AppMailDialog;

static AppMailDialog *	APP_MailDialog;

static AppConfigurableResource APP_MailDialogresourceTable[]=
    {
	APP_RESOURCE( "mailDialogSubject",
		    offsetof(AppMailDialog,amdSubjectText),
		    "Subject" ),
	APP_RESOURCE( "mailDialogTo",
		    offsetof(AppMailDialog,amdToText),
		    "To" ),
	APP_RESOURCE( "mailDialogFrom",
		    offsetof(AppMailDialog,amdFromText),
		    "From" ),
	APP_RESOURCE( "mailDialogCc",
		    offsetof(AppMailDialog,amdCcText),
		    "Cc" ),
	APP_RESOURCE( "mailDialogBcc",
		    offsetof(AppMailDialog,amdBccText),
		    "Bcc" ),

	APP_RESOURCE( "mailDialogContent",
		    offsetof(AppMailDialog,amdContentText),
		    "Content Type" ),
	APP_RESOURCE( "mailDialogNone",
		    offsetof(AppMailDialog,amdNoneText),
		    "None" ),

	APP_RESOURCE( "mailDialogCancel",
		    offsetof(AppMailDialog,amdCancelText),
		    "Cancel" ),
	APP_RESOURCE( "mailDialogSend",
		    offsetof(AppMailDialog,amdSendText),
		    "Send" ),
	APP_RESOURCE( "mailDialogMsgOk",
		    offsetof(AppMailDialog,amdMsgOkText),
		    "Ok" ),

	APP_RESOURCE( "mailDialogNoSubject",
		    offsetof(AppMailDialog,amdSubjectMissing),
		    "Please enter a Subject" ),
	APP_RESOURCE( "mailDialogNoTo",
		    offsetof(AppMailDialog,amdToMissing),
		    "Please enter a Recipient" ),
	APP_RESOURCE( "mailDialogNoFrom",
		    offsetof(AppMailDialog,amdFromMissing),
		    "Please enter a Sender" ),

	APP_RESOURCE( "mailContent",
		    offsetof(AppMailDialog,amdDefaultContent),
		    "" ),

	APP_RESOURCE( "mailFrom",
		    offsetof(AppMailDialog,amdMailFrom),
		    "" ),
	APP_RESOURCE( "mailHost",
		    offsetof(AppMailDialog,amdMailHost),
		    "localhost" ),
	APP_RESOURCE( "mailPort",
		    offsetof(AppMailDialog,amdMailPort),
		    (char *)0 ),
    };

/************************************************************************/
/*  Error call back from smtp software.					*/
/************************************************************************/

static char * APP_MailErrorMessages[APP_SYSTEMeCOUNT];

static AppConfigurableResource APP_MailErrorMessageresourceTable[]=
    {
	APP_RESOURCE( "mailErrorNoError",
		    APP_SYSTEMeNOERROR* sizeof(char *),
		    "No error" ),
	APP_RESOURCE( "mailErrorNoMemory",
		    APP_SYSTEMeNOMEM* sizeof(char *),
		    "Insufficient memory" ),
	APP_RESOURCE( "mailErrorNoFrom",
		    APP_SYSTEMeFROM* sizeof(char *),
		    "No sender given" ),
	APP_RESOURCE( "mailErrorNoTo",
		    APP_SYSTEMeRCPT* sizeof(char *),
		    "No recipients given" ),
	APP_RESOURCE( "mailErrorSmtp",
		    APP_SYSTEMeSMTP* sizeof(char *),
		    "A mail transfer error was received" ),
	APP_RESOURCE( "mailErrorWrite",
		    APP_SYSTEMeWRITE* sizeof(char *),
		    "Could not write data to mail server" ),
	APP_RESOURCE( "mailErrorRead",
		    APP_SYSTEMeREAD* sizeof(char *),
		    "Could not read data from server" ),
	APP_RESOURCE( "mailErrorUname",
		    APP_SYSTEMeUNAME* sizeof(char *),
		    "Could not determine the name of the local machine" ),
	APP_RESOURCE( "mailErrorHostaddr",
		    APP_SYSTEMeHOST* sizeof(char *),
		    "Could not resolve the address of the destination" ),
	APP_RESOURCE( "mailErrorService",
		    APP_SYSTEMeSERV* sizeof(char *),
		    "Unknown service" ),
	APP_RESOURCE( "mailErrorSocket",
		    APP_SYSTEMeSOCK* sizeof(char *),
		    "Could not create a socket to the server" ),
	APP_RESOURCE( "mailErrorConnect",
		    APP_SYSTEMeCONN* sizeof(char *),
		    "Could not connect to the mail server" ),
    };

static void appMailComplain(	void *		voidamd,
				int		message,
				const char *	subject )
    {
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    if  ( subject && subject[0] )
	{
	appQuestionRunSubjectErrorDialog( amd->amdApplication,
				amd->amdDialog.adTopWidget, amd->amdOption,
				APP_MailErrorMessages[message], subject );
	}
    else{
	appQuestionRunErrorDialog( amd->amdApplication,
				amd->amdDialog.adTopWidget, amd->amdOption,
				APP_MailErrorMessages[message] );
	}

    return;
    }

/************************************************************************/
/*  Show the correct content type in the menu.				*/
/************************************************************************/

static void appMailDialogShowContent(	AppMailDialog *	amd )
    {
    appSetOptionmenu( &(amd->amdContentOptionmenu), amd->amdContentChosen );
    return;
    }

/************************************************************************/
/*									*/
/*  A content type was chosen.						*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appContentChosen, w, voidamd )
    {
    int			i;
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    i= appGuiGetOptionmenuItemIndex(  &(amd->amdContentOptionmenu), w );
    if  ( i < 0 || i >= amd->amdContentOptionCount )
	{ LLDEB(i,amd->amdContentOptionCount); return;	}

    amd->amdContentChosen= i;

    return;
    }

/************************************************************************/
/*									*/
/*  'To' changed.							*/
/*									*/
/************************************************************************/

static void appMailCheckTo(	AppMailDialog *		amd )
    {
    char *	to;
    char *	s;

    s= to= appGetStringFromTextWidget( amd->amdToWidget );

    while( *s == ' ' )
	{ s++;	}

    appGuiEnableWidget( amd->amdSendButton, *s != '\0' );

    if  ( *s != '\0' )
	{
	appGuiSetDefaultButtonForDialog( &(amd->amdDialog),
							amd->amdSendButton );
	}

    appFreeStringFromTextWidget( to );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appMailToChanged, w, voidamd )
    {
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    appMailCheckTo( amd );

    return;
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appMailDialogCancelPushed, w, voidamd )
    {
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    appGuiBreakDialog( &(amd->amdDialog), AQDrespCANCEL );

    return;
    }

static APP_BUTTON_CALLBACK_H( appMailDialogSendPushed, w, voidamd )
    {
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    appGuiBreakDialog( &(amd->amdDialog), AQDrespOK );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the list of content types.					*/
/*									*/
/************************************************************************/

static void appMailDialogFillContentMenu(	AppMailDialog *		amd,
						EditApplication *	ea )
    {
    int				i;
    int				defaultContent= -1;

    MailContent *		mc;
    AppConfigurableResource *	acr;
    int				ignored= 0;

    acr= (AppConfigurableResource *)malloc(
		    ea->eaMailContentCount* sizeof(AppConfigurableResource) );
    if  ( acr )
	{
	AppConfigurableResource *	acri= acr;

	mc= ea->eaMailContents;
	for ( i= 0; i < ea->eaMailContentCount; acri++, mc++, i++ )
	    {
	    APP_SET_RESOURCE( acri, mc->mcResourceName,
				    i* sizeof(MailContent)+
					    offsetof(MailContent,mcNameText),
				    mc->mcNameText );

	    if  ( ! strcmp( mc->mcResourceName, amd->amdDefaultContent ) )
		{ defaultContent= i;	}
	    }

	appGuiGetResourceValues( &ignored,
				    ea, mc, acr, ea->eaMailContentCount );

	free( acr );
	}

    appEmptyOptionmenu( &(amd->amdContentOptionmenu) );

    if  ( defaultContent >= 0 )
	{ ea->eaDefaultMailContent= defaultContent;	}

    mc= ea->eaMailContents;
    for ( i= 0; i < ea->eaMailContentCount; mc++, i++ )
	{
	amd->amdContentOptions[i]=
	    appAddItemToOptionmenu( &(amd->amdContentOptionmenu),
			    mc->mcNameText, appContentChosen, (void *)amd );

	if  ( i == ea->eaDefaultMailContent )
	    { defaultContent= i;	}
	if  ( i == 0 )
	    { defaultContent= i;	}
	}

    amd->amdContentOptionCount= ea->eaMailContentCount;

    if  ( ea->eaMailContentCount == 0 )
	{
	amd->amdContentOptions[i]=
		appAddItemToOptionmenu( &(amd->amdContentOptionmenu),
			amd->amdNoneText, appContentChosen, (void *)amd );

	appGuiEnableOptionmenu( &(amd->amdContentOptionmenu), 0 );
	}

    if  ( ea->eaDefaultMailContent >= 0 )
	{ amd->amdContentChosen= ea->eaDefaultMailContent;	}
    else{ amd->amdContentChosen= 0;				}

    appSetOptionmenu( &(amd->amdContentOptionmenu), defaultContent );

    appOptionmenuRefreshWidth( &(amd->amdContentOptionmenu) );
    }

/************************************************************************/
/*  A find tool must be destroyed.					*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( appCloseMailDialog, w, voidamd )
    {
    AppMailDialog *	amd= (AppMailDialog *)voidamd;

    appGuiBreakDialog( &(amd->amdDialog), AQDrespCANCEL );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame for selecting a content.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appMailDialogMakeContentFrame( APP_WIDGET	parent,
						AppMailDialog *	amd )
    {
    APP_WIDGET	frame;
    APP_WIDGET	paned;

    appMakeColumnFrameInColumn( &frame, &paned, parent, amd->amdContentText );

    appMakeOptionmenuInColumn( &(amd->amdContentOptionmenu), paned );

    return frame;
    }

/************************************************************************/
/*									*/
/*  Make the form with the text widgets.				*/
/*									*/
/************************************************************************/

static void appMailDialogMakePropertyRow(	APP_WIDGET*	pRow,
						APP_WIDGET *	pLabel,
						APP_WIDGET *	pText,
						APP_WIDGET	parent,
						AppMailDialog * amd,
						const char *	labelText,
						int		textEnabled )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;
    APP_WIDGET		text;

    const int		heightResizable= 0;
    const int		columnCount= 5;
    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		textColumn= 1;
    const int		textColspan= 4;
    const int		textColumns= 50;

    row= appMakeRowInColumn( parent, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeTextInRow( &text, row, textColumn, textColspan,
						textColumns, textEnabled );

    *pRow= row; *pLabel= label; *pText= text;

    return;
    }

static void appMailDialogMakeTextForm(		APP_WIDGET	parent,
						AppMailDialog *	amd )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;

    int			fromEditable= 1;
    const int		otherEditable= 1;

    if  ( amd->amdMailFrom && amd->amdMailFrom[0] )
	{ fromEditable= 0;	}

    appMailDialogMakePropertyRow( &row, &label, &(amd->amdSubjectWidget),
			    parent, amd, amd->amdSubjectText, otherEditable );

    appMailDialogMakePropertyRow( &row, &label, &(amd->amdToWidget),
			    parent, amd, amd->amdToText, otherEditable );

    appMailDialogMakePropertyRow( &row, &label, &(amd->amdFromWidget),
			    parent, amd, amd->amdFromText, fromEditable );

    appMailDialogMakePropertyRow( &row, &label, &(amd->amdCcWidget),
			    parent, amd, amd->amdCcText, otherEditable );

    appMailDialogMakePropertyRow( &row, &label, &(amd->amdBccWidget),
			    parent, amd, amd->amdBccText, otherEditable );

    appGuiSetTypingCallbackForText( amd->amdToWidget,
					    appMailToChanged, (void *)amd );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the strip with the two buttons.				*/
/*									*/
/************************************************************************/

static void appMailDialogMakeButtonRow(		APP_WIDGET	parent,
						AppMailDialog *	amd )
    {
    const int	heightResizable= 0;

    amd->amdButtonRow= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(amd->amdSendButton), amd->amdButtonRow,
			    amd->amdSendText,
			    appMailDialogSendPushed, (void *)amd, 0, 1 );

    appMakeButtonInRow( &(amd->amdCancelButton), amd->amdButtonRow,
			    amd->amdCancelText,
			    appMailDialogCancelPushed, (void *)amd, 1, 0 );

    appGuiSetCancelButtonForDialog( &(amd->amdDialog), amd->amdCancelButton );
    }

/************************************************************************/
/*									*/
/*  Make a mail dialog.							*/
/*									*/
/************************************************************************/

static AppMailDialog * appMakeMailDialog( EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		mailOption,
					const char *		pixmapName )
    {
    AppMailDialog *	amd;
    
    APP_WIDGET		paned;
    APP_WIDGET		contentFrame;

    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;

    static int		gotMailDialogResources= 0;
    static int		gotMailErrorResources= 0;

    if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (AppMailDialog *)0;	}

    amd= (AppMailDialog *)malloc( sizeof(AppMailDialog)+
				ea->eaMailContentCount* sizeof(APP_WIDGET) );
    if  ( ! amd )
	{ XDEB(amd); return (AppMailDialog *)0;	}

    if  ( ! gotMailDialogResources )
	{
	appGuiGetResourceValues( &gotMailDialogResources, ea, amd,
				    APP_MailDialogresourceTable,
				    sizeof(APP_MailDialogresourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    if  ( gotMailErrorResources )
	{
	appGuiGetResourceValues( &gotMailErrorResources, ea,
				    APP_MailErrorMessages,
				    APP_MailErrorMessageresourceTable,
				    sizeof(APP_MailErrorMessageresourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    amd->amdApplication= ea;
    amd->amdOption= mailOption;

    appMakeVerticalDialog( &(amd->amdDialog), &paned, ea,
						appCloseMailDialog,
						(APP_DESTROY_CALLBACK_T)0,
						(void *)amd,
						ea->eaMailDialogName );

    appSetShellTitle( amd->amdDialog.adTopWidget,
					mailOption, ea->eaApplicationName );

    appMailDialogMakeTextForm( paned, amd );
    contentFrame= appMailDialogMakeContentFrame( paned, amd );
    appMailDialogMakeButtonRow( paned, amd );

    appMailDialogFillContentMenu( amd, ea );

    appGuiShowDialog( &(amd->amdDialog), ed->edToplevel.atTopWidget );

    appOptionmenuRefreshWidth( &(amd->amdContentOptionmenu) );

    return amd;
    }

/************************************************************************/
/*									*/
/*  Show the 'Send...' dialog.						*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Set the default content.					*/
/*									*/
/************************************************************************/

int appRunMailDialog(			EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		mailOption,
					const char *		pixmapName,
					const unsigned char *	documentSubject,
					void *			voiddoc )
    {
    int				rval= 0;
    int				resp= 0;

    AppMailDialog *		amd= APP_MailDialog;

    char *			mailSubject;
    char *			to;
    char *			from;
    char *			cc;
    char *			bcc;

    char *			s;

    const char * const		userName= (const char *)0;
    const char * const		password= (const char *)0;

    MailContent *		mc;
    SimpleOutputStream *	sos;

    const char *		mimeBoundary= "-----------MimeBoundary";

    /*  1  */
    if  ( ! amd )
	{
	amd= APP_MailDialog= appMakeMailDialog( ea, ed, mailOption,
								pixmapName );
	if  ( ! amd )
	    { XDEB(amd); return -1;	}

	}
    else{
	appSetShellTitle( amd->amdDialog.adTopWidget, mailOption,
						    ea->eaApplicationName );

	appGuiShowDialog( &(amd->amdDialog), ed->edToplevel.atTopWidget );
	}

    amd->amdOption= mailOption;

    /*  2  */
    if  ( ea->eaDefaultMailContent >= 0				&&
	  ea->eaDefaultMailContent < ea->eaMailContentCount	)
	{ amd->amdContentChosen= ea->eaDefaultMailContent; }
    else{
	if  ( ea->eaMailContentCount > 0 )
	    { amd->amdContentChosen= 0;	}
	}

    if  ( amd->amdMailFrom && amd->amdMailFrom[0] )
	{ appStringToTextWidget( amd->amdFromWidget, amd->amdMailFrom ); }
    else{
	from =sioSmtpGuessMailAddress();

	if  ( from )
	    { appStringToTextWidget( amd->amdFromWidget, from ); }
	}

    appMailDialogShowContent( amd );
    appMailCheckTo( amd );

    if  ( documentSubject )
	{
	appStringToTextWidget( amd->amdSubjectWidget,
					    (const char *)documentSubject );
	}

    appGuiRunDialog( &(amd->amdDialog), AQDrespNONE, ea );

    switch( amd->amdDialog.adResponse )
	{
	case AQDrespOK:
	    break;
	default:
	    LDEB(amd->amdDialog.adResponse);
	    /*FALLTHROUGH*/
	case AQDrespCANCEL:
	    appGuiHideDialog( &(amd->amdDialog) );
	    return -1;
	}

    if  ( amd->amdContentChosen < 0				||
	  amd->amdContentChosen >= ea->eaMailContentCount	)
	{ LLDEB(amd->amdContentChosen,ea->eaMailContentCount); return -1; }

    mc= ea->eaMailContents+ amd->amdContentChosen;

    mailSubject= appGetStringFromTextWidget( amd->amdSubjectWidget );
    to= appGetStringFromTextWidget( amd->amdToWidget );
    from= appGetStringFromTextWidget( amd->amdFromWidget );
    cc= appGetStringFromTextWidget( amd->amdCcWidget );
    bcc= appGetStringFromTextWidget( amd->amdBccWidget );

    s= mailSubject;
    while( isspace( *s ) )
	{ s++;	}
    if  ( ! *s )
	{
	resp= appQuestionRunOkCancelDialog( ea, amd->amdDialog.adTopWidget,
				mailOption, amd->amdSubjectMissing,
				amd->amdMsgOkText, amd->amdCancelText );

	if  ( resp != AQDrespOK )
	    { rval= -1; goto down;	}

	goto wrong;
	}

    s= to;
    while( isspace( *s ) )
	{ s++;	}
    if  ( ! *s )
	{
	resp= appQuestionRunOkCancelDialog( ea, amd->amdDialog.adTopWidget,
				mailOption, amd->amdToMissing,
				amd->amdMsgOkText, amd->amdCancelText );

	if  ( resp != AQDrespOK )
	    { rval= -1; goto down;	}

	goto wrong;
	}

    s= from;
    while( isspace( *s ) )
	{ s++;	}
    if  ( ! *s )
	{
	resp= appQuestionRunOkCancelDialog( ea, amd->amdDialog.adTopWidget,
				mailOption, amd->amdFromMissing,
				amd->amdMsgOkText, amd->amdCancelText );

	if  ( resp != AQDrespOK )
	    { rval= -1; goto down;	}

	goto wrong;
	}

    sos= sioOutSmtpOpen( amd->amdMailHost, amd->amdMailPort,
			userName, password,
			from, to, cc, bcc, mailSubject,
			mc->mcTypeSlashSubtype,
			mc->mcNeedsBoundary?mimeBoundary:(const char *)0,
			(void *)amd, appMailComplain );
    if  ( sos )
	{
	if  ( (*mc->mcWriteMail)( sos,
		mc->mcNeedsBoundary?mimeBoundary:(const char *)0, voiddoc ) )
	    { LDEB(1); rval= -1;	}

	if  ( sioOutClose( sos ) )
	    { LDEB(1); rval= -1;	}
	}
    else{ /*XDEB(sos);*/ rval= -1;	}

  down:
    appGuiHideDialog( &(amd->amdDialog) );

  wrong:

    appFreeStringFromTextWidget( mailSubject );
    appFreeStringFromTextWidget( to );
    appFreeStringFromTextWidget( from );
    appFreeStringFromTextWidget( cc );
    appFreeStringFromTextWidget( bcc );


    amd->amdOption= (APP_WIDGET)0;

    return rval;
    }
