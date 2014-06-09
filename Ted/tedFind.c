/************************************************************************/
/*  Ted: Interface to the find tool.					*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<appSpellTool.h>
#   include	"docFind.h"

#   include	<appDebugon.h>

static void tedFindToolClosed(		void *		voidea )
    {
    EditApplication *	ea=	(EditApplication *)voidea;

    ea->eaFindTool= (void *)0;
    }

int tedDocFindNext(	EditDocument *	ed )
    {
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsEnd), bd,
				docFindParaFindNext, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	EditApplication *	ea= ed->edApplication;
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	if  ( ea->eaFindTool )
	    { appFindToolEnableReplace( ea->eaFindTool, 1 );	}
	}

    return ret;
    }

int tedDocFindPrev(	EditDocument *	ed )
    {
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindPrevInDocument( &dsNew, &(ds.dsBegin), bd,
				docFindParaFindPrev, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	EditApplication *	ea= ed->edApplication;
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	if  ( ea->eaFindTool )
	    { appFindToolEnableReplace( ea->eaFindTool, 1 );	}
	}

    return ret;
    }

static int tedFindToolFindNext(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFindNext( ed );
    }

static int tedFindToolFindPrev(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFindPrev( ed );
    }

int tedFindSetPattern(		EditDocument *		ed,
				const unsigned char *	pattern,
				int			useRegex )
    {
    TedDocument *		td;

    regProg *			prog;

    td= (TedDocument *)ed->edPrivateData;

    if  ( useRegex )
	{
	prog= regCompile( pattern );
	if  ( ! prog )
	    { SXDEB((char *)pattern,prog); return -1;	}
	}
    else{
	prog= regCompileEscaped( pattern );
	if  ( ! prog )
	    { SXDEB((char *)pattern,prog); return -1;	}
	}

    if  ( td->tdFindProg )
	{ free( td->tdFindProg );	}

    td->tdFindProg= (void *)prog;

    return 0;
    }

static int tedFindToolSetPattern(	void *			voidea,
					const unsigned char *	pattern,
					int			useRegex )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedFindSetPattern( ed, pattern, useRegex );
    }

void tedDocToolFind(			APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    if  ( ! ea->eaFindTool )
	{
	char *			pixmapName= "tedfind";
	APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
	APP_BITMAP_MASK		iconMask= (APP_BITMAP_MASK)0;

	TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

	if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	    { SDEB(pixmapName); return;	}

	ea->eaFindTool= appMakeFindTool( findOption,
			    ea, "tedFindTool",
			    iconPixmap, iconMask,
			    tedFindToolClosed,
			    tedFindToolFindNext,
			    tedFindToolFindPrev,
			    tedAppReplace,
			    tedFindToolSetPattern,
			    (void *)ea );
	if  ( ! ea->eaFindTool )
	    { XDEB(ea->eaFindTool); return;	}

	if  ( tar->tarFindPattern )
	    {
	    appFindToolSetPattern( ea->eaFindTool,
				    tar->tarFindPattern, tar->tarFindRegex );
	    }
	}

    appShowFindTool( ed->edToplevel.atTopWidget, ea->eaFindTool );

    if  ( ea->eaFindTool )
	{ appFindToolEnableReplace( ea->eaFindTool, 1 );	}

    return;
    }

void tedDocToolFindNext(		APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdFindProg )
	{
	tedDocToolFind( findOption, voided, voidcbs );
	return;
	}

    tedDocFindNext( ed );

    return;
    }

static int tedSpellFindNext(	void *			voidea,
				SpellCheckContext *	scc )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsEnd), bd,
					docSpellParaFindNext, (void *)scc );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int	lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	if  ( ea->eaSpellTool )
	    {
	    unsigned char	save;

	    save= dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff];
	    dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff]= '\0';

	    appSpellMakeGuesses( ea->eaSpellTool,
		    dsNew.dsBegin.dpBi->biParaString+ dsNew.dsBegin.dpStroff );

	    dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff]= save;
	    }
	}

    return ret;
    }

/************************************************************************/
/*									*/
/*  Show or create a spelling tool.					*/
/*									*/
/************************************************************************/

static void tedSpellToolClosed(		void *		voidea	)
    {
    EditApplication *	ea= (EditApplication *)voidea;

    ea->eaSpellTool= (void *)0;
    }

void tedDocToolSpell(		APP_WIDGET	spellOption,
				void *		voided,
				void *		voidcbs )
    {
    EditDocument *	ed=	(EditDocument *)voided;
    EditApplication *	ea=	ed->edApplication;

    if  ( ! ea->eaSpellTool )
	{
	char *			pixmapName= "tedspell";
	APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
	APP_BITMAP_MASK		iconMask= (APP_BITMAP_MASK)0;

	if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	    { SDEB(pixmapName); return;	}

	ea->eaSpellTool= appMakeSpellTool( spellOption, ea, "tedSpellTool",
			    iconPixmap, iconMask,
			    tedSpellToolClosed, tedSpellFindNext,
			    tedAppReplace, (void *)ea );
	}
    else{ appShowSpellTool( ea->eaSpellTool ); }

    return;
    }
