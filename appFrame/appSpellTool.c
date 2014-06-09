/************************************************************************/
/*									*/
/*  A spelling tool.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<appDebugon.h>

#   include	<errno.h>

#   include	"appSpellTool.h"
#   include	<appSystem.h>
#   include	<appFrame.h>

#   include	<charnames.h>
#   include	<ind.h>

#   ifdef USE_MOTIF
#	include	<X11/keysym.h>
#   endif

#   ifdef __VMS
    static const char * const		APPSPELL_IndexExtension= "IND";
#   else
    static const char * const		APPSPELL_IndexExtension= "ind";
#   endif

/************************************************************************/
/*									*/
/*  Resources for the spell tool.					*/
/*									*/
/************************************************************************/

typedef struct AppSpellToolResources
    {
    char *	astrDictionary;
    char *	astrLearn;
    char *	astrForget;
    char *	astrGuesses;
    char *	astrIgnore;
    char *	astrFindNext;
    char *	astrGuess;
    char *	astrCorrect;
    char *	astrPrivateDictionaries;
    char *	astrSystemDictionaries;
    char *	astrDirNoAccess;
    char *	astrNoSuchDir;
    char *	astrDirNoDicts;
    char *	astrNoDicts;
    char *	astrLower;
    char *	astrClose;
    char *	astrSysDictNoAccess;
    char *	astrPrivDictDirNotMade;
    char *	astrPrivDictNoAccess;
    char *	astrPrivDictWrongFormat;
    } AppSpellToolResources;

static AppConfigurableResource APP_SpellToolResourceTable[]=
    {
    /************************/
    /*  Spell Tool.		*/
    /************************/
    APP_RESOURCE( "spellToolDictTitle",
		offsetof(AppSpellToolResources,astrDictionary),
		"Dictionary" ),
    APP_RESOURCE( "spellToolLearn",
		offsetof(AppSpellToolResources,astrLearn),
		"Learn" ),
    APP_RESOURCE( "spellToolForget",
		offsetof(AppSpellToolResources,astrForget),
		"Forget" ),
    APP_RESOURCE( "spellToolGuesses",
		offsetof(AppSpellToolResources,astrGuesses),
		"Guesses" ),
    APP_RESOURCE( "spellToolIgnore",
		offsetof(AppSpellToolResources,astrIgnore),
		"Ignore" ),
    APP_RESOURCE( "spellToolFindNext",
		offsetof(AppSpellToolResources,astrFindNext),
		"Find Next" ),
    APP_RESOURCE( "spellToolGuess",
		offsetof(AppSpellToolResources,astrGuess),
		"Guess" ),
    APP_RESOURCE( "spellToolCorrect",
		offsetof(AppSpellToolResources,astrCorrect),
		"Correct" ),
    APP_RESOURCE( "spellToolNoDicts",
		offsetof(AppSpellToolResources,astrNoDicts),
		"None" ),

    APP_RESOURCE( "spellToolLower",
		offsetof(AppSpellToolResources,astrLower),
		"Lower" ),
    APP_RESOURCE( "spellToolClose",
		offsetof(AppSpellToolResources,astrClose),
		"Close" ),

#   ifdef __VMS
    APP_RESOURCE( "spellToolPrivateDicts",
		offsetof(AppSpellToolResources,astrPrivateDictionaries),
		"_Dictionaries" ),
#   else
    APP_RESOURCE( "spellToolPrivateDicts",
		offsetof(AppSpellToolResources,astrPrivateDictionaries),
		".Dictionaries" ),
#   endif
    APP_RESOURCE( "spellToolSystemDicts",
		offsetof(AppSpellToolResources,astrSystemDictionaries),
		INDDIR ),

    APP_RESOURCE( "spellToolDirNoAccess",
		offsetof(AppSpellToolResources,astrDirNoAccess),
		"Could not access directory for dictionaries." ),
    APP_RESOURCE( "spellToolDirNoSuchDir",
		offsetof(AppSpellToolResources,astrNoSuchDir),
		"This directory could not be found." ),
    APP_RESOURCE( "spellToolDirNoDicts",
		offsetof(AppSpellToolResources,astrDirNoDicts),
		"No dictionaries were found in this directory." ),
    APP_RESOURCE( "spellToolSysDictNoAccess",
		offsetof(AppSpellToolResources,astrSysDictNoAccess),
		"Could not read system dictionary." ),
    APP_RESOURCE( "spellToolPrivDictDirNotMade",
		offsetof(AppSpellToolResources,astrPrivDictDirNotMade),
		"Could not make private directory." ),
    APP_RESOURCE( "spellToolPrivDictNoAccess",
		offsetof(AppSpellToolResources,astrPrivDictNoAccess),
		"Could not read private dictionary." ),
    APP_RESOURCE( "spellToolPrivDictWrongFormat",
		offsetof(AppSpellToolResources,astrPrivDictWrongFormat),
		"Private dictionary has an illegal format." ),
    };

/************************************************************************/
/*									*/
/*  Represents a spell tool.						*/
/*									*/
/************************************************************************/
#   define	FILEL	400

typedef struct AppSpellCheckContext
    {
    SpellCheckContext	asccSpellCheckContext;
    APP_WIDGET		asccMenuWidget;
    } AppSpellCheckContext;

typedef struct AppSpellTool
    {
    EditApplication *		astApplication;

    AppSpellToolResources *	astResources;

    APP_WIDGET			astTopWidget;
    APP_WIDGET			astMainWidget;

    AppOptionmenu		astDictionaryOptionmenu;
    APP_WIDGET			astGuessList;
    APP_WIDGET			astWordText;

    APP_WIDGET			astLearnButton;
    APP_WIDGET			astForgetButton;
    APP_WIDGET			astCorrectButton;
    APP_WIDGET			astGuessButton;
    APP_WIDGET			astIgnoreButton;
    APP_WIDGET			astLowerButton;
    APP_WIDGET			astCloseButton;

    AppToolDestroy		astDestroy;
    SpellToolFindNext		astFindNext;
    SpellToolCorrect		astCorrect;
    void *			astTarget;

    AppSpellCheckContext *	astDictionaryContexts;
    int				astDictionaryCount;

    int				astCurrentDictionary;

    char *			astPrivateDictionaries;
    char *			astSystemDictionaries;
    } AppSpellTool;

static FILE *	appSpellOpenDictionary(	const char *	prefix,
					AppSpellTool *	ast,
					int		read_only );

/************************************************************************/
/*									*/
/*  Open index files for a certain dictionary.				*/
/*									*/
/************************************************************************/

static int appSpellToolOpenIndices(	SpellCheckContext *	scc,
					AppSpellTool *		ast )
    {
    char *		systemdicts= ast->astSystemDictionaries;

    char		scratch[FILEL];
#   ifdef __VMS
    char *		format= "%s.%s";
#   else
    char *		format= "%s/%s.%s";
#   endif
       
    FILE *		privateDict;

#   ifdef __VMS
    sprintf( scratch, format,
			scc->sccDictionaryPrefix, APPSPELL_IndexExtension );
#   else
    sprintf( scratch, format, systemdicts,
			scc->sccDictionaryPrefix, APPSPELL_IndexExtension );
#   endif

    scc->sccStaticInd= indRead( scratch, 1 );

    if  ( ! scc->sccStaticInd )
	{
	AppSpellToolResources *	astr= ast->astResources;

	appQuestionRunSubjectErrorDialog( ast->astApplication,
			ast->astTopWidget, (APP_WIDGET)0,
			scratch, astr->astrSysDictNoAccess );
	return -1;
	}

    privateDict= appSpellOpenDictionary( scc->sccDictionaryPrefix, ast, 1 );

    if  ( privateDict )
	{
	if  ( indReadPrivateDictionary( privateDict,
				&scc->sccLearntInd, &(scc->sccForgotInd) ) )
	    {
	    AppSpellToolResources *	astr= ast->astResources;

	    appQuestionRunSubjectErrorDialog( ast->astApplication,
			ast->astTopWidget, (APP_WIDGET)0,
			scc->sccDictionaryPrefix,
			astr->astrPrivDictWrongFormat );
	    }

	fclose( privateDict );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt buttons to different situations..				*/
/*									*/
/************************************************************************/

static void appSpellToolSomethingFound(	AppSpellTool *	ast,
					int		yes_no )
    {
    appGuiEnableWidget( ast->astLearnButton, yes_no );
    appGuiEnableWidget( ast->astIgnoreButton, yes_no );

    return;
    }

static void appSpellToolGotAlternative(	AppSpellTool *	ast,
					int		yes_no )
    {
    appGuiEnableWidget( ast->astForgetButton, yes_no );
    appGuiEnableWidget( ast->astCorrectButton, yes_no );
    appGuiEnableWidget( ast->astGuessButton, yes_no );

    return;
    }

/************************************************************************/
/*									*/
/*  Look for the next unknown word.					*/
/*									*/
/************************************************************************/

static void appSpellToolFindNext(	AppSpellTool *	ast )
    {
    AppSpellCheckContext *	ascc;
    SpellCheckContext *		scc;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}

    if  ( ! ast->astFindNext < 0 )
	{ XDEB(ast->astFindNext); return;	}

    ascc= ast->astDictionaryContexts+ ast->astCurrentDictionary;
    scc= &(ascc->asccSpellCheckContext);

    appStringToTextWidget( ast->astWordText, "" );
    appGuiEmptyListWidget( ast->astGuessList );

    if  ( ! scc->sccStaticInd			&&
	  appSpellToolOpenIndices( scc, ast )	)
	{ return;	}

    if  ( ! (*ast->astFindNext)( ast->astTarget, scc ) )
	{ appSpellToolSomethingFound( ast, 1 );	}
    else{ appSpellToolSomethingFound( ast, 0 );	}

    appSpellToolGotAlternative( ast, 0 );
    }

/************************************************************************/
/*									*/
/*  'Learn' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolLearnPushed, w, voidast )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;
    unsigned char *		word;
    AppSpellCheckContext *	ascc;
    SpellCheckContext *		scc;
    FILE *			privateDict;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}

    ascc= ast->astDictionaryContexts+ ast->astCurrentDictionary;
    scc= &(ascc->asccSpellCheckContext);

    if  ( ! scc->sccStaticInd			&&
	  appSpellToolOpenIndices( scc, ast )	)
	{ return;	}

    if  ( ! scc->sccLearntInd )
	{
	scc->sccLearntInd= indMake();
	if  ( ! scc->sccLearntInd )
	    { XDEB(scc->sccLearntInd); return;	}
	}

    privateDict= appSpellOpenDictionary( scc->sccDictionaryPrefix, ast, 0 );
    if  ( ! privateDict )
	{ return;	}

    word= (unsigned char *)appGetStringFromTextWidget( ast->astWordText );

    if  ( indLearnWord( privateDict,
			    scc->sccLearntInd, scc->sccForgotInd, word ) )
	{ SDEB((char *)word);	}

    appFreeStringFromTextWidget( (char *)word );

    fclose( privateDict );

#   if 1
    appSpellToolFindNext( ast );
#   else
    appGuiEnableWidget( ast->astLearnButton, 0 );
    appGuiEnableWidget( ast->astForgetButton, 1 );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  'Forget' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolForgetPushed, w, voidast )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;
    unsigned char *		word;
    AppSpellCheckContext *	ascc;
    SpellCheckContext *		scc;
    FILE *			privateDict;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}

    ascc= ast->astDictionaryContexts+ ast->astCurrentDictionary;
    scc= &(ascc->asccSpellCheckContext);

    if  ( ! scc->sccStaticInd			&&
	  appSpellToolOpenIndices( scc, ast )	)
	{ return;	}

    if  ( ! scc->sccForgotInd )
	{
	scc->sccForgotInd= indMake();
	if  ( ! scc->sccForgotInd )
	    { XDEB(scc->sccForgotInd); return;	}
	}

    privateDict= appSpellOpenDictionary( scc->sccDictionaryPrefix, ast, 0 );
    if  ( ! privateDict )
	{ SXDEB(scc->sccDictionaryPrefix,privateDict); return;	}

    word= (unsigned char *)appGetStringFromTextWidget( ast->astWordText );

    if  ( indForgetWord( privateDict,
			    scc->sccLearntInd, scc->sccForgotInd, word ) )
	{ SDEB((char *)word);	}

    appFreeStringFromTextWidget( (char *)word );

    fclose( privateDict );

    appGuiEnableWidget( ast->astForgetButton, 0 );
    appGuiEnableWidget( ast->astLearnButton, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find Next' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolFindNextPushed, w, voidast )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;

    appSpellToolFindNext( ast );

    return;
    }

/************************************************************************/
/*									*/
/*  'Ignore' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolIgnorePushed, w, voidast )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;
    unsigned char *		word;
    AppSpellCheckContext *	ascc;
    SpellCheckContext *		scc;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}

    ascc= ast->astDictionaryContexts+ ast->astCurrentDictionary;
    scc= &(ascc->asccSpellCheckContext);

    if  ( ! scc->sccLearntInd )
	{
	scc->sccLearntInd= indMake();
	if  ( ! scc->sccLearntInd )
	    { XDEB(scc->sccLearntInd); return;	}
	}

    word= (unsigned char *)appGetStringFromTextWidget( ast->astWordText );

    if  ( indMoveWord( scc->sccForgotInd, scc->sccLearntInd, word ) )
	{ SDEB((char *)word); return;	}

    appSpellToolFindNext( ast );

    appFreeStringFromTextWidget( (char *)word );

    return;
    }

/************************************************************************/
/*									*/
/*  The user typed something in the Coreection, Turn on the the		*/
/*  'Correct' button.							*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appSpellCorrectionTyped, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;

    appSpellToolGotAlternative( ast, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  'Correct' button has been pushed.					*/
/*  Or a double click on the listbox with guesses.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolCorrect, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;
    char *		guess;

    if  ( ! ast->astCorrect < 0 )
	{ XDEB(ast->astCorrect); return;	}

    guess= appGetStringFromTextWidget( ast->astWordText );

    (*ast->astCorrect)( ast->astTarget, (unsigned char *)guess );

    appFreeStringFromTextWidget( guess );

    appSpellToolFindNext( ast );

    return;
    }

/************************************************************************/
/*  The 'Guess' button has been pushed.					*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellGuessButtonPushed, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;
    char *		word;

    word= appGetStringFromTextWidget( ast->astWordText );

    appSpellMakeGuesses( voidast, (unsigned char *)word );

    appFreeStringFromTextWidget( word );

    return;
    }

/************************************************************************/
/*  A guess in the list has been selected.				*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appSpellGuessChosen, w, voidast, voidlcs )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;

    char *			text;

    text= appGuiGetStringFromListCallback( w, voidlcs );
    if  ( text )
	{
	appStringToTextWidget( ast->astWordText, text );
	appFreeStringFromListCallback( text );
	}

    appSpellToolGotAlternative( ast, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row of buttons for the Spell Tool.				*/
/*									*/
/************************************************************************/

static void appSpellToolMakeButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					char *			leftLabel,
					char *			rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					AppSpellTool *		ast )
    {
    APP_WIDGET		row;

    APP_WIDGET		leftButton;
    APP_WIDGET		rightButton;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &leftButton, row, leftLabel,
				leftCallback, (void *)ast, 0, showAsDefault );
    appMakeButtonInRow( &rightButton, row, rightLabel,
				rightCallback, (void *)ast, 1, showAsDefault );

    *pRow= row; *pLeftButton= leftButton, *pRightButton= rightButton;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the dictionary part of the spelling tool.			*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellMakeDictionaryFrame( APP_WIDGET	parent,
					AppSpellToolResources * astr,
					AppSpellTool *		ast	)
    {
    APP_WIDGET		frame;
    APP_WIDGET		paned;

    APP_WIDGET		buttonRow;

    appMakeColumnFrameInColumn( &frame, &paned,
					    parent, astr->astrDictionary );

    appMakeOptionmenuInColumn( &(ast->astDictionaryOptionmenu), paned );

    appSpellToolMakeButtonRow( &buttonRow, paned,
		&(ast->astLearnButton), &(ast->astForgetButton),
		astr->astrLearn, astr->astrForget,
		appSpellToolLearnPushed, appSpellToolForgetPushed,
		ast );

    return frame;
    }

/************************************************************************/
/*									*/
/*  A spell tool must be destroyed.					*/
/*									*/
/************************************************************************/

static void appDestroySpellTool(	AppSpellTool *	ast )
    {
    int			i;

    for ( i= 0; i < ast->astDictionaryCount; i++ )
	{
	indCleanSpellCheckContext( &(ast->astDictionaryContexts[i].
						    asccSpellCheckContext) );
	}

    if  ( ast->astDictionaryContexts )
	{ free( ast->astDictionaryContexts );	}

    if  ( ast->astDestroy )
	{ (*ast->astDestroy)( ast->astTarget );	}

    appDestroyShellWidget( ast->astTopWidget );

    free( ast );

    return;
    }

static APP_CLOSE_CALLBACK_H( appCloseSpellTool, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;

    appDestroySpellTool( ast );

    return;
    }

static APP_BUTTON_CALLBACK_H( appLowerSpellTool, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;
    APP_WIDGET		shell= ast->astTopWidget;

    appGuiLowerShellWidget( shell );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSpellToolCloseButtonPushed, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;

    appDestroySpellTool( ast );

    return;
    }

#   ifdef USE_MOTIF
static void appSpellToolCancel(		APP_WIDGET		w,
					void *			voidast,
					XEvent *		event,
					Boolean *		pRefused )
    {
    if  ( event->type == KeyRelease )
	{
	XKeyEvent *		kevent= &(event->xkey);
	AppSpellTool *		ast= (AppSpellTool *)voidast;

	if  ( XKeysymToKeycode( XtDisplay(w), XK_Escape ) == kevent->keycode )
	    { appDestroySpellTool( ast ); *pRefused= 0; return; }
	}

    *pRefused= 1; return;
    }
#   endif

/************************************************************************/
/*									*/
/*  Obtain the list of dictionaries.					*/
/*									*/
/*  The initialistation of the character classification should be	*/
/*  based on some kind of knowledge, NOT on the name of the language.	*/
/*									*/
/*  The font of the List the Text widgets should be adapted to the	*/
/*  character set of the language. Now things look ridiculous.		*/
/*									*/
/************************************************************************/

static int appSpellAddDictionary(	const char *		filename,
					void *			voidast )
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;
    AppSpellCheckContext *	fresh;
    char *			name;

#   ifdef __VMS
    char *			semicolon;
#   endif

    const char *	s;
    int			l;

    s= strrchr( filename, '/' );
    if  ( s )
	{ s++;		}
    else{ s= filename;	}

#   ifdef __VMS
    semicolon= strrchr( s , ';' );
    if  ( semicolon )
	{ *semicolon= '\0';	}
#   endif

    l= strlen( s );
    if  ( l == 0 )
	{ LDEB(l); return 0;	}

    if  ( s[l- 4] != '.' || strcmp( s+ l- 3, APPSPELL_IndexExtension ) )
	{ SSDEB(filename,s+ l- 4); return 0;	}

    fresh= (AppSpellCheckContext *)realloc( ast->astDictionaryContexts,
		(ast->astDictionaryCount+ 1)*sizeof(AppSpellCheckContext) );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;	}
    ast->astDictionaryContexts= fresh;

    name= (char *)malloc( l- 3 );
    if  ( ! name )
	{ LXDEB(l,name); return -1;       }
    strncpy( name, s, l- 4 )[l- 4]= '\0';

    fresh += ast->astDictionaryCount++;

    indInitSpellCheckContext( &(fresh->asccSpellCheckContext) );
    fresh->asccSpellCheckContext.sccDictionaryPrefix= name;
    fresh->asccMenuWidget= (APP_WIDGET)0;

    /********************************************/
    /*  Wrong.. But concentrated in one place.	*/
    /********************************************/
    if  ( ! strcmp( name, "Czech" )	||
	  ! strcmp( name, "Polish" )	||
	  ! strcmp( name, "Hungarian" )	||
	  ! strcmp( name, "Slovak" )	)
	{
	indSpellIso2CharacterKinds( &(fresh->asccSpellCheckContext) );
	return 0;
	}

    if  ( ! strcmp( name, "Russian" )	)
	{
	indSpellIso5CharacterKinds( &(fresh->asccSpellCheckContext) );
	return 0;
	}

    if  ( ! strcmp( name, "Greek" )	)
	{
	indSpellIso7CharacterKinds( &(fresh->asccSpellCheckContext) );
	return 0;
	}

    indSpellIso1CharacterKinds( &(fresh->asccSpellCheckContext) );

    return 0;
    }

static int appSpellGetDictionaries(	AppSpellTool *		ast,
					AppSpellToolResources *	astr )
    {
    char *		systemdicts= ast->astSystemDictionaries;

    if  ( appForAllFiles( systemdicts, APPSPELL_IndexExtension,
					(void *)ast, appSpellAddDictionary ) )
	{ SDEB(systemdicts); return -1;	}

    if  ( ast->astDictionaryCount == 0 )
	{
	if  ( appTestDirectory( systemdicts ) )
	    {
	    appQuestionRunSubjectErrorDialog( ast->astApplication,
			(APP_WIDGET)0, (APP_WIDGET)0,
			systemdicts, astr->astrNoSuchDir );
	    }
	else{
	    if  ( appTestFileReadable( systemdicts ) )
		{
		appQuestionRunSubjectErrorDialog( ast->astApplication,
			    (APP_WIDGET)0, (APP_WIDGET)0,
			    systemdicts, astr->astrDirNoAccess );
		}
	    else{
		appQuestionRunSubjectErrorDialog( ast->astApplication,
			    (APP_WIDGET)0, (APP_WIDGET)0,
			    systemdicts, astr->astrDirNoDicts );
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  A dictionary has been selected.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appSpellDictionaryChosen, w, voidast )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;
    int			i;

    i= appGuiGetOptionmenuItemIndex( &(ast->astDictionaryOptionmenu), w );
    if  ( i < 0 || i >= ast->astDictionaryCount )
	{ LLDEB(i,ast->astDictionaryCount); return;	}

    ast->astCurrentDictionary= i;

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the list of dictionaries.					*/
/*									*/
/************************************************************************/

static void appSpellFillDictionaryMenu(		AppSpellTool *		ast,
						AppSpellToolResources *	astr )
    {
    int				i;
    AppSpellCheckContext *	ascc;

    appEmptyOptionmenu( &(ast->astDictionaryOptionmenu) );

    ascc= ast->astDictionaryContexts;
    for ( i= 0; i < ast->astDictionaryCount; ascc++, i++ )
	{
	ascc->asccMenuWidget= appAddItemToOptionmenu(
			    &(ast->astDictionaryOptionmenu),
			    ascc->asccSpellCheckContext.sccDictionaryPrefix,
			    appSpellDictionaryChosen, (void *)ast );
	}

    if  ( ast->astDictionaryCount == 0 )
	{
	(void) appAddItemToOptionmenu( &(ast->astDictionaryOptionmenu),
			    astr->astrNoDicts,
			    appSpellDictionaryChosen, (void *)ast );

	appGuiEnableWidget( ast->astTopWidget, 0 );
	}

    appSetOptionmenu( &(ast->astDictionaryOptionmenu), 0 );
    ast->astCurrentDictionary= 0;

    appOptionmenuRefreshWidth( &(ast->astDictionaryOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Make the listbox with guesses.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellGuessList(	APP_WIDGET		parent,
					AppSpellToolResources * astr,
					AppSpellTool *		ast )
    {
    APP_WIDGET		label;
    APP_WIDGET		list;

    const int		visibleItems= 6;

    appMakeLabelInColumn( &label, parent, astr->astrGuesses );

    appGuiMakeListInColumn( &list, parent, visibleItems,
					appSpellGuessChosen, (void *)ast );

    return list;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the spelling Tool.				*/
/*									*/
/************************************************************************/

static void appSpellMakeButtonRows(	APP_WIDGET		spellForm,
					AppSpellToolResources * astr,
					AppSpellTool *		ast )
    {
    APP_WIDGET		buttonRow;

    APP_WIDGET		findNextButton;

    appSpellToolMakeButtonRow( &buttonRow, spellForm,
		&(ast->astIgnoreButton), &(findNextButton),
		astr->astrIgnore, astr->astrFindNext,
		appSpellToolIgnorePushed, appSpellToolFindNextPushed,
		ast );

    appSpellToolMakeButtonRow( &buttonRow, spellForm,
		&(ast->astGuessButton), &(ast->astCorrectButton),
		astr->astrGuess, astr->astrCorrect,
		appSpellGuessButtonPushed, appSpellToolCorrect,
		ast );

    appSpellToolMakeButtonRow( &buttonRow, spellForm,
		&(ast->astLowerButton), &(ast->astCloseButton),
		astr->astrLower, astr->astrClose,
		appLowerSpellTool, appSpellToolCloseButtonPushed,
		ast );

    return;
    }

/************************************************************************/
/*									*/
/*  make a spell tool.							*/
/*									*/
/************************************************************************/

void * appMakeSpellTool(	APP_WIDGET		spellOption,
				EditApplication *	ea,
				const char *		widgetName,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				AppToolDestroy		destroy,
				SpellToolFindNext	findNext,
				SpellToolCorrect	correct,
				void *			target )
    {
    AppSpellTool *	ast;
    APP_WIDGET		dictionaryFrame;
    const int		userResizable= 1;

    static AppSpellToolResources	astr;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&astr,
					APP_SpellToolResourceTable,
					sizeof(APP_SpellToolResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    ast= (AppSpellTool *)malloc( sizeof(AppSpellTool) );
    if  ( ! ast )
	{ XDEB(ast); return (void *)0;	}

    ast->astApplication= ea;

    ast->astResources= &astr;

    ast->astDestroy= destroy;
    ast->astFindNext= findNext;
    ast->astCorrect= correct;
    ast->astTarget= target;
    ast->astPrivateDictionaries= astr.astrPrivateDictionaries;
    ast->astSystemDictionaries= astr.astrSystemDictionaries;

    ast->astDictionaryContexts= (AppSpellCheckContext *)0;
    ast->astDictionaryCount= 0;
    ast->astCurrentDictionary= -1;

    appInitOptionmenu( &(ast->astDictionaryOptionmenu) );

    appSpellGetDictionaries( ast, &astr );

    appMakeVerticalTool( &(ast->astTopWidget), &(ast->astMainWidget), ea,
	    iconPixmap, iconMask,
	    widgetName, userResizable, spellOption,
	    appCloseSpellTool, (void *)ast );

    dictionaryFrame= appSpellMakeDictionaryFrame( ast->astMainWidget,
								&astr, ast );
    ast->astGuessList= appSpellGuessList( ast->astMainWidget, &astr, ast );

#   ifdef USE_MOTIF
    XtAddCallback( ast->astGuessList, XmNdefaultActionCallback,
					appSpellToolCorrect, (void *)ast );
#   endif

    appMakeTextInColumn( &(ast->astWordText), ast->astMainWidget, 0, 1 );

    appGuiSetTypingCallbackForText( ast->astWordText,
				    appSpellCorrectionTyped, (void *)ast );

    appSpellMakeButtonRows( ast->astMainWidget, &astr, ast );

#   ifdef USE_MOTIF
    XtInsertEventHandler( ast->astGuessList, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astWordText, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astLearnButton, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astForgetButton, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astCorrectButton, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astGuessButton, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
    XtInsertEventHandler( ast->astIgnoreButton, KeyReleaseMask, False,
				appSpellToolCancel, (void *)ast, XtListHead );
#   endif

    appSpellFillDictionaryMenu( ast, &astr );

    appShowShellWidget( ast->astTopWidget );

    appOptionmenuRefreshWidth( &(ast->astDictionaryOptionmenu) );

    appSpellToolSomethingFound( ast, 0 );
    appSpellToolGotAlternative( ast, 0 );

    return (void *)ast;
    }

/************************************************************************/
/*  Draw a spell tool to front.						*/
/************************************************************************/
void appShowSpellTool(		void *	voidast	)
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;

    appStringToTextWidget( ast->astWordText, "" );
    appGuiEmptyListWidget( ast->astGuessList );

    appShowShellWidget( ast->astTopWidget );

    appSpellToolSomethingFound( ast, 0 );
    appSpellToolGotAlternative( ast, 0 );

    }

void appSpellMakeGuesses(	void *			voidast,
				const unsigned char *	word	)
    {
    AppSpellTool *		ast= (AppSpellTool *)voidast;
    SpellGuessContext		sgc;
    AppSpellCheckContext *	ascc;
    SpellCheckContext *		scc;

    int				limit;
    int				i;

    static IndGuessList		igl;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}

    ascc= ast->astDictionaryContexts+ ast->astCurrentDictionary;
    scc= &(ascc->asccSpellCheckContext);

    if  ( ! scc->sccStaticInd			&&
	  appSpellToolOpenIndices( scc, ast )	)
	{ return;	}

    sgc.sgcGuessList= &igl;
    sgc.sgcCheckContext= scc;

    indCleanGuessList( &igl );
    indInitGuessList( &igl );

    appGuiEmptyListWidget( ast->astGuessList );
    appStringToTextWidget( ast->astWordText, (const char *)word );

    if  ( scc->sccStaticInd )
	{
	indGuessWord( scc->sccStaticInd, word, &sgc,
		    (const GuessSubstitution *)0, 0,
		    scc->sccCharKinds, scc->sccCharShifts );
	}

    if  ( scc->sccLearntInd )
	{
	indGuess( scc->sccLearntInd, word, &sgc, INDhASIS,
		    (const GuessSubstitution *)0, 0,
		    scc->sccCharKinds, scc->sccCharShifts );
	}

    indSortGuesses( &igl );

    if  ( igl.iglGuessCount > 6 )
	{
	limit= ( igl.iglGuesses[0].igsScore+
		    2* igl.iglGuesses[igl.iglGuessCount-1].igsScore )/ 3;
	}
    else{ limit= 0;	}

    for ( i= 0; i < igl.iglGuessCount; i++ )
	{
	if  ( igl.iglGuesses[i].igsScore < limit )
	    { break;	}

	appGuiAddValueToListWidget( ast->astGuessList,
				    (const char *)igl.iglGuesses[i].igsWord );
	}

    appSpellToolGotAlternative( ast, 0 );
    }

/************************************************************************/
/*									*/
/*  Open a private dictionary.						*/
/*									*/
/************************************************************************/

static FILE *	appSpellOpenDictionary(	const char *	prefix,
					AppSpellTool *	ast,
					int		read_only )
    {
    char *		dir= ast->astPrivateDictionaries;
    char		scratch[FILEL+ 1];
    char		home[FILEL+ 1];
    int			len;

    FILE *		f;

    char *		mode;

    len= appHomeDirectory( home, FILEL );
    if  ( len < 0 )
	{ LDEB(len); return (FILE *)0;	}

    if  ( dir[0] == '/' )
	{ strcpy( scratch, dir );			}
    else{
#	ifdef __VMS
	sprintf( scratch, "%.*s.%s]", len- 1, home, dir );
#	else
	sprintf( scratch, "%s/%s", home, dir );
#	endif
	}

    if  ( appTestDirectory( scratch ) )
	{
	if  ( read_only )
	    { return (FILE *)0;	}

	if  ( appMakeDirectory( scratch ) )
	    {
	    AppSpellToolResources *	astr= ast->astResources;

	    appQuestionRunSubjectErrorDialog( ast->astApplication,
			ast->astTopWidget, (APP_WIDGET)0,
			scratch, astr->astrPrivDictDirNotMade );

	    return (FILE *)0;
	    }
	}

    if  ( dir[0] == '/' )
	{ sprintf( scratch, "%s/%s.changes", dir, prefix );		}
    else{
#	ifdef __VMS
	char *	vms_file= strchr( prefix, ']' );

	sprintf( scratch, "%.*s.%s%s.changes", len- 1, home, dir, vms_file );
#	else
	sprintf( scratch, "%s/%s/%s.changes", home, dir, prefix );
#	endif
	}

    if  ( read_only )
	{ mode= "r";	}
    else{ mode= "a";	}

    f= fopen( scratch, mode );
    if  ( ! f && ! read_only )
	{ SLDEB(scratch,errno); return (FILE *)0; }

    return f;
    }

void appEnableSpellTool(	void *	voidast,
				int	enabled )
    {
    AppSpellTool *	ast= (AppSpellTool *)voidast;

    appGuiEnableWidget( ast->astMainWidget, enabled != 0 );

    return;
    }
