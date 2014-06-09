/************************************************************************/
/*  Really Quit, Really Close dialogs.					*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appFrame.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Run a 'Really Close' dialog.					*/
/*									*/
/************************************************************************/

void appRunReallyCloseDialog(	APP_WIDGET			option,
				EditDocument *			ed )
    {
    EditApplication *		ea= ed->edApplication;
    int				rcc;

    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    rcc= appQuestionRunSubjectYesNoCancelDialog( ea,
						ed->edToplevel.atTopWidget,
						option,
						ed->edTitle,
						afmr->afmrReallyCloseQuestion,
						afmr->afmrReallyCloseSaveIt,
						afmr->afmrReallyCloseDontSave,
						afmr->afmrReallyCloseCancel );

    switch( rcc )
	{
	case AQDrespYES:
	    if  ( ! ea->eaSaveDocument )
		{ XDEB(ea->eaSaveDocument); return;	}

	    if  ( ! ed->edFilename )
		{
#		ifdef USE_MOTIF
		appDocFileSaveAs( option, (void *)ed, (void *)0 ); return;
#		endif
#		ifdef USE_GTK
		appDocFileSaveAs( (GtkMenuItem *)option, (void *)ed ); return;
#		endif
		XDEB(ed->edFilename); return;
		}
	    else{
		const int	interactive= 1;

		if  ( ! appDocSaveDocumentByName( ed, option, interactive,
					    ed->edFormat, ed->edFilename ) )
		    { appDocumentChanged( ed, 0 ); }
		}

	    if  ( ! ed->edHasBeenChanged )
		{ appCloseDocument( ea, ed );	}
	    break;

	case AQDrespNO:
	    appCloseDocument( ea, ed );
	    break;
	case AQDrespCANCEL:
	    break;
	default:
	    LDEB(rcc); break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Run a 'Really Quit' dialog.						*/
/*									*/
/************************************************************************/

int appRunReallyQuitDialog(	APP_WIDGET			option,
				APP_WIDGET			relative,
				EditApplication *		ea )
    {
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    return appQuestionRunYesNoCancelDialog( ea,
					relative,
					option,
					afmr->afmrReallyQuitQuestion,
					afmr->afmrReallyQuitReview,
					afmr->afmrReallyQuitAnyway,
					afmr->afmrReallyQuitCancel );
    }
