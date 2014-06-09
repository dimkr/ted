/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine application specific default settings.			*/
/*									*/
/*  1)	Determine default code page.					*/
/*  2)	Determine default for show table grid.				*/
/*									*/
/************************************************************************/

void tedDetermineDefaultSettings(	TedAppResources *	tar )
    {
    char *		past;
    int		val;

    /*  1  */
    if  ( tar->tarDefaultAnsicpgString		&&
	  tar->tarDefaultAnsicpgInt < 0		)
	{
	val= strtol( tar->tarDefaultAnsicpgString, &past, 10 );
	if  ( past != tar->tarDefaultAnsicpgString )
	    {
	    while( *past == ' ' )
		{ past++;	}
	    }
	if  ( past != tar->tarDefaultAnsicpgString	&&
	      ! *past					)
	    { tar->tarDefaultAnsicpgInt= val;		}
	else{ SDEB(tar->tarDefaultAnsicpgString);	}
	}

    /*  2  */
    if  ( tar->tarShowTableGridString		&&
	  tar->tarShowTableGridInt == 0		)
	{
	if  ( ! strcmp( tar->tarShowTableGridString, "0" ) )
	    { tar->tarShowTableGridInt= -1;	}
	if  ( ! strcmp( tar->tarShowTableGridString, "1" ) )
	    { tar->tarShowTableGridInt=  1;	}

	if  ( tar->tarShowTableGridInt == 0 )
	    { SDEB(tar->tarShowTableGridString);	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a replace from one of the tools.				*/
/*									*/
/************************************************************************/

void tedAppReplace(	void *			voidea,
			const unsigned char *	word )
    {
    EditApplication *	ea= (EditApplication *)voidea;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;

    if  ( tedHasSelection( td )		&&
	  ! tedHasIBarSelection( td )	&&
	  td->tdCanReplaceSelection	)
	{ tedDocReplaceSelection( ed, word, strlen( (const char *)word ) ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Callback for the 'Insert File' menu option.				*/
/*									*/
/************************************************************************/

static int tedInsertFile(	void *		voided,
				APP_WIDGET	relative,
				APP_WIDGET	option,
				const char *	filename )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    int				format;
    BufferDocument *		bd;

    if  ( tedOpenDocumentFile( ea, &format, &bd, filename, relative, option ) )
	{ SDEB(filename); return -1;	}

    switch( format )
	{
	case 0:
	    if  ( tedIncludeRtfDocument( ed, bd ) )
		{ LDEB(1); docFreeDocument( bd ); return -1;	}
	    break;

	case 1:
	case 2:
	    if  ( tedIncludePlainDocument( ed, bd ) )
		{ LDEB(1); docFreeDocument( bd ); return -1;	}
	    break;

	default:
	    LDEB(format);
	    docFreeDocument( bd ); return -1;
	}
	    
    docFreeDocument( bd );

    appDocumentChanged( ed, 1 );

    return 0;
    }

void tedDocInsertFile(		APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			    ea->eaFileExtensionCount, ea->eaFileExtensions,
			    ea->eaDefaultFileFilter,
			    voided, tedInsertFile, ea );

    return;
    }

