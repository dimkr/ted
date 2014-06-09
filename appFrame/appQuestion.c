/************************************************************************/
/*									*/
/*  Utility routines for making remarks and posing questions.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appFrame.h>

#   include	<appDebugon.h>

typedef struct QuestionContextResources
    {
    char *	qcrYesText;
    char *	qcrNoText;
    char *	qcrCancelText;
    char *	qcrOkText;
    } QuestionContextResources;

typedef struct QuestionContext
    {
    AppDialog				qcDialog;
    APP_WIDGET				qcPaned;

    APP_WIDGET				qcQuestionWidget;
    APP_WIDGET				qcSubjectWidget;

    APP_WIDGET				qcYesButton;
    APP_WIDGET				qcNoButton;
    APP_WIDGET				qcCancelButton;
    APP_WIDGET				qcOkButton;

    const QuestionContextResources *	qcResources;
    } QuestionContext;

/************************************************************************/
/*									*/
/*  Resource table for various button texts.				*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_QuestionResourceTable[]=
{
    APP_RESOURCE( "messageYesText",
		offsetof(QuestionContextResources,qcrYesText),	"Yes" ),
    APP_RESOURCE( "messageNoText",
		offsetof(QuestionContextResources,qcrNoText),	"No" ),
    APP_RESOURCE( "messageCancelText",
		offsetof(QuestionContextResources,qcrCancelText),"Cancel" ),
    APP_RESOURCE( "messageOkText",
		offsetof(QuestionContextResources,qcrOkText),	"Ok" ),
};

/************************************************************************/
/*									*/
/*  Run a Yes/No/Cancel dialog.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appQuestionOkButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    appGuiBreakDialog( &(qc->qcDialog), AQDrespOK );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionYesButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    appGuiBreakDialog( &(qc->qcDialog), AQDrespYES );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionNoButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    appGuiBreakDialog( &(qc->qcDialog), AQDrespNO );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionCancelButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    appGuiBreakDialog( &(qc->qcDialog), AQDrespCANCEL );
    return;
    }

/************************************************************************/
/*									*/
/*  Make a Ok form with one default button.				*/
/*  Make a Yes/No/Cancel form with three buttons.			*/
/*									*/
/************************************************************************/

static APP_WIDGET appQuestionMakeOkRow(	APP_WIDGET		parent,
					QuestionContext *	qc,
					const char *		okText )
    {
    APP_WIDGET		buttonRow;
    const int		heightResizable= 0;
    const int		showAsDefault= 1;

    buttonRow= appMakeRowInColumn( parent, 1, heightResizable );

    appMakeButtonInRow( &(qc->qcOkButton), buttonRow, okText,
		    appQuestionOkButtonPushed, (void *)qc, 0, showAsDefault );

    return buttonRow;
    }

static APP_WIDGET appQuestionMakeYesNoCancelRow( APP_WIDGET	parent,
					QuestionContext *	qc,
					char *			yesText,
					char *			noText,
					char *			cancelText )
    {
    APP_WIDGET		buttonRow;
    const int		heightResizable= 0;
    const int		showYesAsDefault= 1;
    const int		showOtherAsDefault= 0;

    buttonRow= appMakeRowInColumn( parent, 3, heightResizable );

    appMakeButtonInRow( &(qc->qcYesButton), buttonRow, yesText,
				appQuestionYesButtonPushed,
				(void *)qc, 0, showYesAsDefault );

    appMakeButtonInRow( &(qc->qcNoButton), buttonRow, noText,
				appQuestionNoButtonPushed,
				(void *)qc, 1, showOtherAsDefault );

    appMakeButtonInRow( &(qc->qcCancelButton), buttonRow, cancelText,
				appQuestionCancelButtonPushed,
				(void *)qc, 2, showOtherAsDefault );

    appGuiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcYesButton );
    appGuiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcCancelButton );

    return buttonRow;
    }

static APP_WIDGET appQuestionMakeOkCancelRow( APP_WIDGET	parent,
					QuestionContext *	qc,
					char *			okText,
					char *			cancelText )
    {
    APP_WIDGET		buttonRow;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    buttonRow= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(qc->qcOkButton), buttonRow, okText,
				appQuestionOkButtonPushed,
				(void *)qc, 0, showAsDefault );

    appMakeButtonInRow( &(qc->qcCancelButton), buttonRow, cancelText,
				appQuestionCancelButtonPushed,
				(void *)qc, 1, showAsDefault );

    appGuiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcOkButton );
    appGuiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcCancelButton );

    return buttonRow;
    }


static APP_WIDGET appQuestionMakeYesNoRow( APP_WIDGET		parent,
					QuestionContext *	qc,
					char *			yesText,
					char *			noText )
    {
    APP_WIDGET		buttonRow;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    buttonRow= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(qc->qcYesButton), buttonRow, yesText,
					    appQuestionYesButtonPushed,
					    (void *)qc, 0, showAsDefault );

    appMakeButtonInRow( &(qc->qcNoButton), buttonRow, noText,
					    appQuestionNoButtonPushed,
					    (void *)qc, 1, showAsDefault );

    appGuiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcYesButton );
    appGuiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcNoButton );

    return buttonRow;
    }

/************************************************************************/
/*									*/
/*  Make the framework of a Question Dialog.				*/
/*									*/
/************************************************************************/

static APP_DESTROY_CALLBACK_H( appFreeQuestionContext, w, voidqc )
    { free( voidqc ); return;	}

static int appMakeQuestionDialog( 	EditApplication *	ea,
					QuestionContext **	pQc,
					APP_WIDGET *		pPaned )
    {
    APP_WIDGET		paned;

    QuestionContext *	qc;

    static int				gotResources= 0;
    static QuestionContextResources	qcr;

    qc= (QuestionContext *)malloc( sizeof(QuestionContext) );
    if  ( ! qc )
	{ XDEB(qc); return -1;	}

    qc->qcQuestionWidget= (APP_WIDGET)0;
    qc->qcSubjectWidget= (APP_WIDGET)0;
    qc->qcYesButton= (APP_WIDGET)0;
    qc->qcNoButton= (APP_WIDGET)0;
    qc->qcCancelButton= (APP_WIDGET)0;
    qc->qcOkButton= (APP_WIDGET)0;
    qc->qcResources= &qcr;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&qcr,
				    APP_QuestionResourceTable,
				    sizeof(APP_QuestionResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    appMakeVerticalDialog( &(qc->qcDialog), &paned, ea,
						(APP_CLOSE_CALLBACK_T)0,
						appFreeQuestionContext,
						(void *)qc,
						ea->eaMessageDialogName );

    *pQc= qc;
    *pPaned= paned;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a subject and a question in a form.				*/
/*									*/
/************************************************************************/

static void appQuestionMakeSubjectQuestion( QuestionContext *	qc,
					APP_WIDGET		paned,
					const char *		subject,
					const char *		question )
    {
    appMakeLabelInColumn( &(qc->qcSubjectWidget), paned, subject );
    appMakeLabelInColumn( &(qc->qcQuestionWidget), paned, question );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a subject and a question in a form.				*/
/*									*/
/************************************************************************/

static void appQuestionMakeQuestion(	QuestionContext *	qc,
					APP_WIDGET		paned,
					const char *		question )
    {
    appMakeLabelInColumn( &(qc->qcQuestionWidget), paned, question );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the question/subject of a question dialog.			*/
/*									*/
/************************************************************************/

static void appQuestionChangeSubject(	QuestionContext *	qc,
					const char *		subject )
    {
    if  ( ! qc->qcSubjectWidget )
	{ XDEB(qc->qcSubjectWidget); return;	}

    appGuiChangeLabelText( qc->qcSubjectWidget, subject );
    }

static void appQuestionChangeQuestion(	QuestionContext *	qc,
					const char *		question )
    {
    if  ( ! qc->qcQuestionWidget )
	{ XDEB(qc->qcQuestionWidget); return;	}

    appGuiChangeLabelText( qc->qcQuestionWidget, question );
    }

static void appQuestionChangeOkText(	QuestionContext *	qc,
					const char *		okText )
    { appGuiChangeButtonText( qc->qcOkButton, okText );	}

static void appQuestionChangeYesText(	QuestionContext *	qc,
					const char *	yesText )
    { appGuiChangeButtonText( qc->qcYesButton, yesText );	}

static void appQuestionChangeNoText(	QuestionContext *	qc,
					const char *		noText )
    { appGuiChangeButtonText( qc->qcNoButton, noText );	}

static void appQuestionChangeCancelText( QuestionContext *	qc,
					const char *		cancelText )
    { appGuiChangeButtonText( qc->qcCancelButton, cancelText );	}


static void appQuestionRunDialog(	EditApplication *	ea,
					QuestionContext *	qc,
					APP_WIDGET		relative,
					APP_WIDGET		option )
    {
    appSetShellTitle( qc->qcDialog.adTopWidget,
					option, ea->eaApplicationName );

    appGuiShowDialog( &(qc->qcDialog), relative );

    appGuiRunDialog( &(qc->qcDialog), AQDrespNONE, ea );

    appGuiHideDialog( &(qc->qcDialog) );

    return;
    }

static int appQuestionRunOkDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					QuestionContext *	qc )
    {
    if  ( ! qc->qcOkButton )
	{ XDEB(qc->qcOkButton); return -1;	}

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyYesNoCancelDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						QuestionContext * qc,
						const char *	yesText,
						const char *	noText,
						const char *	cancelText )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeYesText( qc, yesText?yesText:qcr->qcrYesText );
    appQuestionChangeNoText( qc, noText?noText:qcr->qcrNoText );
    appQuestionChangeCancelText( qc, cancelText?cancelText:qcr->qcrCancelText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyOkCancelDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						QuestionContext * qc,
						const char *	okText,
						const char *	cancelText )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeOkText( qc, okText?okText:qcr->qcrOkText );
    appQuestionChangeCancelText( qc, cancelText?cancelText:qcr->qcrCancelText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyYesNoDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						QuestionContext * qc,
						const char *	yesText,
						const char *	noText )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeYesText( qc, yesText?yesText:qcr->qcrYesText );
    appQuestionChangeNoText( qc, noText?noText:qcr->qcrNoText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

int appQuestionRunSubjectYesNoCancelDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						const char *	subject,
						const char *	question,
						const char *	yesText,
						const char *	noText,
						const char *	cancelText )
    {
    int				rval;

    static QuestionContext *	subjectYesNoCancelContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, question );
	return AQDrespCANCEL;
	}

    if  ( ! subjectYesNoCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectYesNoCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= subjectYesNoCancelContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectYesNoCancelContext, paned,
							subject, question );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeYesNoCancelRow( paned,
				    subjectYesNoCancelContext,
				    qcr->qcrYesText,
				    qcr->qcrNoText,
				    qcr->qcrCancelText );
	}
    else{
	appQuestionChangeSubject( subjectYesNoCancelContext, subject );
	appQuestionChangeQuestion( subjectYesNoCancelContext, question );
	}

    rval= appQuestionRunAnyYesNoCancelDialog( ea, relative, option,
					subjectYesNoCancelContext,
					yesText, noText, cancelText );

    appDestroyShellWidget( subjectYesNoCancelContext->qcDialog.adTopWidget );
    subjectYesNoCancelContext= (QuestionContext *)0;

    return rval;
    }

int appQuestionRunYesNoCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question,
					const char *		yesText,
					const char *		noText,
					const char *		cancelText )
    {
    int			rval;

    static QuestionContext *	yesNoCancelContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    if  ( ! yesNoCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &yesNoCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= yesNoCancelContext->qcResources;

	appQuestionMakeQuestion( yesNoCancelContext, paned, question );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeYesNoCancelRow( paned,
					yesNoCancelContext,
					qcr->qcrYesText,
					qcr->qcrNoText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( yesNoCancelContext, question );
	}

    rval= appQuestionRunAnyYesNoCancelDialog( ea, relative, option,
					yesNoCancelContext,
					yesText, noText, cancelText );

    appDestroyShellWidget( yesNoCancelContext->qcDialog.adTopWidget );
    yesNoCancelContext= (QuestionContext *)0;

    return rval;
    }

void appQuestionRunSubjectErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    static QuestionContext *	subjectErrorContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return;
	}

    if  ( ! subjectErrorContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectErrorContext, &paned ) )
	    { LDEB(1); return;	}

	qcr= subjectErrorContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectErrorContext, paned,
							subject, message );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeOkRow( paned, subjectErrorContext,
							qcr->qcrOkText );
	}
    else{
	const QuestionContextResources *	qcr= subjectErrorContext->qcResources;

	appQuestionChangeSubject( subjectErrorContext, subject );
	appQuestionChangeQuestion( subjectErrorContext, message );
	appQuestionChangeOkText( subjectErrorContext,
					    qcr->qcrOkText );
	}

    appQuestionRunOkDialog( ea, relative, option, subjectErrorContext );

    appDestroyShellWidget( subjectErrorContext->qcDialog.adTopWidget );
    subjectErrorContext= (QuestionContext *)0;
    }

int appQuestionRunOkCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question,
					const char *		okText,
					const char *		cancelText )
    {
    int			rval;

    static QuestionContext *	okCancelContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    if  ( ! okCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &okCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= okCancelContext->qcResources;

	appQuestionMakeQuestion( okCancelContext, paned, question );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeOkCancelRow( paned,
					okCancelContext,
					qcr->qcrOkText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( okCancelContext, question );
	}

    rval= appQuestionRunAnyOkCancelDialog( ea, relative, option,
					okCancelContext, okText, cancelText );

    appDestroyShellWidget( okCancelContext->qcDialog.adTopWidget );
    okCancelContext= (QuestionContext *)0;

    return rval;
    }

int appQuestionRunSubjectOkCancelDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message,
					const char *		okText,
					const char *		cancelText )
    {
    int			rval;

    static QuestionContext *	subjectOkCancelContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return AQDrespCANCEL;
	}

    if  ( ! subjectOkCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectOkCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= subjectOkCancelContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectOkCancelContext, paned,
							subject, message );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeOkCancelRow( paned,
					subjectOkCancelContext,
					qcr->qcrOkText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( subjectOkCancelContext, message );
	appQuestionChangeSubject( subjectOkCancelContext, subject );
	}

    rval= appQuestionRunAnyOkCancelDialog( ea, relative, option,
				subjectOkCancelContext, okText, cancelText );

    appDestroyShellWidget( subjectOkCancelContext->qcDialog.adTopWidget );
    subjectOkCancelContext= (QuestionContext *)0;

    return rval;
    }

int appQuestionRunSubjectYesNoDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message,
					const char *		yesText,
					const char *		noText )
    {
    int			rval;

    static QuestionContext *	subjectYesNoContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return AQDrespFAILURE;
	}

    if  ( ! subjectYesNoContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectYesNoContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= subjectYesNoContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectYesNoContext, paned,
							subject, message );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeYesNoRow( paned,
					subjectYesNoContext,
					qcr->qcrYesText,
					qcr->qcrNoText );
	}
    else{
	appQuestionChangeQuestion( subjectYesNoContext, message );
	appQuestionChangeSubject( subjectYesNoContext, subject );
	}

    rval= appQuestionRunAnyYesNoDialog( ea, relative, option,
				    subjectYesNoContext, yesText, noText );

    appDestroyShellWidget( subjectYesNoContext->qcDialog.adTopWidget );
    subjectYesNoContext= (QuestionContext *)0;

    return rval;
    }

void appQuestionRunErrorDialog(	EditApplication *	ea,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const char *		message )
    {
    static QuestionContext *	errorContext;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, message );
	return;
	}

    if  ( ! errorContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	APP_WIDGET				buttonRow;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &errorContext, &paned ) )
	    { LDEB(1); return;	}

	qcr= errorContext->qcResources;

	appQuestionMakeQuestion( errorContext, paned, message );

	appGuiInsertSeparatorInColumn( &sep, paned );

	buttonRow= appQuestionMakeOkRow( paned,
				    errorContext, qcr->qcrOkText );
	}
    else{
	const QuestionContextResources *	qcr= errorContext->qcResources;

	appQuestionChangeQuestion( errorContext, message );
	appQuestionChangeOkText( errorContext, qcr->qcrOkText );
	}

    appQuestionRunOkDialog( ea, relative, option, errorContext );

    appDestroyShellWidget( errorContext->qcDialog.adTopWidget );
    errorContext= (QuestionContext *)0;

    return;
    }
