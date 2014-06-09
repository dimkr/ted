/************************************************************************/
/*									*/
/*  Printing related functionality.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<locale.h>
#   include	<unistd.h>

#   include	<appSystem.h>
#   include	<sioPipe.h>
#   include	<sioFd.h>
#   include	<sioStdout.h>
#   include	<sioStdio.h>

#   include	<appPaper.h>
#   include	"appFrame.h"

#   include	<appDebugon.h>

#   ifdef HAVE_MKSTEMP
#	define USE_MKSTEMP 1
#   else
#	define USE_MKSTEMP 0
#   endif

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/*  1)  Avoid user oriented numeric formatting in a PostScript file,	*/
/*	that is meant to be read by computers.				*/
/*  2)  Use mkstemp to make the name of a temporary file. Though there	*/
/*	is no reason here, we simply conform to the fashionable		*/
/*	paranoia to make 'gcc' shut up.					*/
/*									*/
/************************************************************************/

int appCallPrintFunction(	SimpleOutputStream *		sos,
				const PrintJob *		pj,
				const PrintGeometry *		pg )
    {
    EditApplication *	ea= pj->pjApplication;
    int			rval= 0;

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    if  ( (*ea->eaPrintDocument)( sos, pj, pg ) )
	{ LDEB(1); rval= -1;	}

    /*  1  */
    setlocale( LC_NUMERIC, "" );

    return rval;
    }

/*  2  */
# if USE_MKSTEMP

static int appPrintMakeTempfile(	int *			pFd,
					char *			scratchName )
    {
    int				fd;

    strcpy( scratchName, "/tmp/tprXXXXXX" );
    fd= mkstemp( scratchName );
    if  ( fd < 0 )
	{ SLDEB(scratchName,fd); return -1;	}

    *pFd= fd;
    return 0;
    }

static int appPrintOpenTempfile(	SimpleOutputStream **	pSos,
					int			fd,
					const char *		scratchName )
    {
    SimpleOutputStream *	sos;

    sos= sioOutFdOpen( fd );
    if  ( ! sos )
	{ SLXDEB(scratchName,fd,sos); return -1; }

    *pSos= sos; return 0;
    }

static int appPrintCloseTempfile(	int			fd,
					SimpleOutputStream *	sos )
    {
    int		rval= 0;

    if  ( sos && sioOutClose( sos ) )
	{ XDEB(sos); rval= -1;	}

    if  ( fd >= 0 && close( fd ) )
	{ LDEB(fd); rval= -1;	}

    return rval;
    }

# else

static int appPrintMakeTempfile(	int *			pFd,
					char *			scratchName )
    {
    int				fd;

    tmpnam( scratchName );
    fd= -1;

    *pFd= fd;
    return 0;
    }

static int appPrintOpenTempfile(	SimpleOutputStream **	pSos,
					int			fd,
					const char *		scratchName )
    {
    SimpleOutputStream *	sos;

    sos= sioOutStdioOpen( scratchName );
    if  ( ! sos )
	{ SXDEB(scratchName,sos); return -1;	}

    *pSos= sos; return 0;
    }

static int appPrintCloseTempfile(	int			fd,
					SimpleOutputStream *	sos )
    {
    int		rval= 0;

    if  ( sos && sioOutClose( sos ) )
	{ XDEB(sos); rval= -1;	}

    return rval;
    }

# endif

static char * appDocBuildTmpfileCommand(	const char *	command,
						int		commandLen,
						const char *	fileName,
						int		fileCount )
    {
    char *		rval;
    char *		to;
    const char *	from;

    int			fileLen;

    fileLen= strlen( fileName );

    to= rval= malloc( commandLen+ fileCount* fileLen+ 1 );
    if  ( ! rval )
	{ LXDEB(commandLen,rval); return (char *)0; }
    from= command;

    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'f' )
	    {
	    strcpy( to, fileName ); to += fileLen;
	    from += 2; continue;
	    }

	*(to++)= *(from++);
	}

    *to= '\0';

    return rval;
    }

int appPrintDocument(	int				printer,
			const PrintJob *		pj,
			const PrintGeometry *		pg )
    {
    EditApplication *		ea= pj->pjApplication;
    PrintDestination *		pd= ea->eaPrintDestinations+ printer;
    char			scratchName[L_tmpnam+1];
    char *			command= (char *)0;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    int				fd= -1;

    switch( pd->pdPrintKind )
	{
	case APPprinterPIPE:
	    {
	    sos= sioOutPipeOpen( pd->pdCommand );

	    if  ( ! sos )
		{ SXDEB(pd->pdCommand,sos); return -1;	}

	    appCallPrintFunction( sos, pj, pg );

	    if  ( sioOutClose( sos ) )
		{ SDEB(pd->pdCommand); return -1;	}
	    }

	    return 0;

	case APPprinterTMPFILE:

	    if  ( appPrintMakeTempfile( &fd, scratchName ) )
		{ LDEB(1); return -1;	}

	    command= appDocBuildTmpfileCommand( 
				pd->pdCommand, pd->pdCommandLength,
				scratchName, pd->pdPercentCount );
	    if  ( ! command )
		{
		LXDEB(pd->pdCommandLength,command);
		appPrintCloseTempfile( fd, sos );
		return -1;
		}

	    if  ( appPrintOpenTempfile( &sos, fd, scratchName ) )
		{
		SLDEB(scratchName,fd);
		appPrintCloseTempfile( fd, sos );
		free( command );
		return -1;
		}

	    appCallPrintFunction( sos, pj, pg );

	    if  ( appPrintCloseTempfile( fd, sos ) )
		{
		SLDEB(scratchName,fd);
		free( command );
		return -1;
		}

	    system( command );
	    free( command );
	    return 0;

	case APPprinterTOFILE:
	default:
	    LDEB(pd->pdPrintKind);
	    return -1;
	}
    }

static char * appDocBuildFaxCommand(		const char *	faxCommand,
						int		commandLen,
						const char *	fileName,
						const char *	faxNumber,
						char *		title,
						int		fileCount,
						int		faxCount,
						int		titleCount )
    {
    char *		command;
    const char *	s;
    char *		to;

    int			fileLen;
    int			faxLen;
    int			titleLen;

    if  ( ! title )
	{ title= "";	}

    fileLen= strlen( fileName );
    faxLen= strlen( faxNumber );
    titleLen= strlen( title );

    to= command= (char *)malloc( commandLen+
					fileCount* fileLen+
					faxCount* faxLen+
					titleCount* titleLen+ 1 );
    if  ( ! command )
	{ XDEB(command); return (char *)0;	}

    s= faxCommand;
    while( *s )
	{
	if  ( s[0] == '%' && s[1] == 'f' )
	    {
	    strcpy( to, fileName ); to += fileLen;
	    s += 2; continue;
	    }

	if  ( s[0] == '%' && s[1] == 'n' )
	    {
	    strcpy( to, faxNumber ); to += faxLen;
	    s += 2; continue;
	    }

	if  ( s[0] == '%' && s[1] == 't' )
	    {
	    strcpy( to, title ); to += titleLen;
	    s += 2; continue;
	    }

	*(to++)= *(s++);
	}
    *to= '\0';

    return command;
    }

/************************************************************************/
/*									*/
/*  Fill a PrintJob for an EditDocument.				*/
/*									*/
/************************************************************************/

void appPrintJobForEditDocument(	PrintJob *		pj,
					EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    pj->pjPrivateData= ed->edPrivateData;
    pj->pjFormat= ed->edFormat;
    pj->pjDrawingData= &(ed->edDrawingData);
    pj->pjApplication= ea;
    pj->pjTitle= ed->edTitle;

    return;
    }

void appApplicationSettingsToPrintGeometry(
					PrintGeometry *		pg,
					EditApplication *	ea )
    {
    pg->pgUsePostScriptFilters= ea->eaUsePostScriptFilters;
    pg->pgUsePostScriptIndexedImages= ea->eaUsePostScriptIndexedImages;

    pg->pgSkipEmptyPages= ea->eaSkipEmptyPages;
    pg->pgSkipBlankPages= ea->eaSkipBlankPages;
    pg->pgOmitHeadersOnEmptyPages= ea->eaOmitHeadersOnEmptyPages;

    return;
    }

/************************************************************************/
/*									*/
/*  Fax a document.							*/
/*									*/
/************************************************************************/

int appFaxDocument(	EditDocument *			ed,
			const char *			faxNumber,
			const PrintGeometry *		pg )
    {
    EditApplication *		ea= ed->edApplication;
    int				fileCount= 0;
    int				faxCount= 0;
    int				titleCount= 0;

    int				commandLen;

    char			scratchName[L_tmpnam+1];

    char *			command;

    int				fd= -1;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    PrintJob			pj;

    if  ( ! ea->eaFaxCommand )
	{ XDEB(ea->eaFaxCommand); return -1;	}

    appPrintJobForEditDocument( &pj, ed );

    {
    const char * s= ea->eaFaxCommand;

    while( *s )
	{
	if  ( s[0] == '%' && s[1] == 'f' )
	    { fileCount++; s += 2; continue;	}

	if  ( s[0] == '%' && s[1] == 'n' )
	    { faxCount++; s += 2; continue;	}

	if  ( s[0] == '%' && s[1] == 't' )
	    { titleCount++; s += 2; continue;	}

	s++;
	}

    commandLen= s- ea->eaFaxCommand;
    }

    if  ( faxCount == 0 )
	{ LDEB(faxCount); return -1;	}

    if  ( fileCount > 0 )
	{
	if  ( appPrintMakeTempfile( &fd, scratchName ) )
	    { LDEB(1); return -1;	}

	command= appDocBuildFaxCommand( ea->eaFaxCommand, commandLen,
					scratchName, faxNumber, ed->edTitle,
					fileCount, faxCount, titleCount );

	if  ( ! command )
	    {
	    LXDEB(commandLen,command);
	    appPrintCloseTempfile( fd, sos );
	    return -1;
	    }

	if  ( appPrintOpenTempfile( &sos, fd, scratchName ) )
	    {
	    SLDEB(scratchName,fd);
	    appPrintCloseTempfile( fd, sos );
	    free( command );
	    return -1;
	    }

	appCallPrintFunction( sos, &pj, pg );

	if  ( appPrintCloseTempfile( fd, sos ) )
	    {
	    SLDEB(scratchName,fd);
	    free( command );
	    return -1;
	    }

	system( command );
	}
    else{
	scratchName[0]= '\0';

	command= appDocBuildFaxCommand( ea->eaFaxCommand, commandLen,
					scratchName, faxNumber, ed->edTitle,
					fileCount, faxCount, titleCount );

	sos= sioOutPipeOpen( command );
	if  ( ! sos )
	    { SXDEB(command,sos); free( command ); return -1;	}

	appCallPrintFunction( sos, &pj, pg );

	if  ( sioOutClose( sos ) )
	    { SDEB(command); return -1;	}
	}

    free( command );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a print job for a command line call.				*/
/*									*/
/************************************************************************/

static int appPrintJobForCommand(	PrintJob *		pj,
					AppDrawingData *	add,
					EditApplication *	ea,
					const char *		fromName )
    {
    double		screenPixelsPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    if  ( ea->eaMakePrivateData	)
	{
	pj->pjPrivateData= (*ea->eaMakePrivateData)();
	if  ( ! pj->pjPrivateData )
	    { XDEB(pj->pjPrivateData); return -1; }
	}

    if  ( (*ea->eaOpenDocument)( ea, pj->pjPrivateData, &(pj->pjFormat),
					    ea->eaToplevel.atTopWidget,
					    (APP_WIDGET)0, fromName ) )
	{ SDEB(fromName); return -1; }

    if  ( ea->eaToplevel.atTopWidget )
	{
	appGetFactors( ea, &screenPixelsPerMM, &verPixPerMM, &xfac, &yfac );

	appSetDrawingEnvironment( add, ea->eaMagnification, xfac,
			    screenPixelsPerMM,
			    &(ea->eaPostScriptFontList),
			    ea->eaToplevel.atTopWidget );

	add->addPageGapPixels= (int)( ea->eaPageGapMM* verPixPerMM );
	}
    else{
	add->addPostScriptFontList= &(ea->eaPostScriptFontList);
	}

    pj->pjDrawingData= add;
    pj->pjApplication= ea;
    pj->pjTitle= fromName;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initial setup for command line printing.				*/
/*									*/
/************************************************************************/

static void appPrintFinishCommandRun(	EditApplication *	ea,
					PrintJob *		pj,
					AppDrawingData *	add )
    {
    (*ea->eaFreeDocument)( pj->pjPrivateData, pj->pjFormat, add );

    appCleanDrawingData( add );

    return;
    }

static int appPrintStartCommandRun(	EditApplication *	ea,
					PrintJob *		pj,
					PrintGeometry *		pg,
					AppDrawingData *	add,
					const char *		paperString,
					const char *		fromName )
    {
    pg->pgSheetGeometry= ea->eaDefaultDocumentGeometry;

    if  ( paperString )
	{
	DocumentGeometry *	dg= &(pg->pgSheetGeometry);
	int			paperFormat;

	if  ( appPaperFormatFromString( &paperFormat,
					    &(dg->dgPageWideTwips),
					    &(dg->dgPageHighTwips),
					    ea->eaUnitInt, paperString ) )
	    { SDEB(paperString); return -1;	}
	}

    if  ( appPrintJobForCommand( pj, add, ea, fromName ) )
	{ SDEB(fromName); return -1;	}

    if  ( ea->eaSuggestNup )
	{ (*ea->eaSuggestNup)( pg, pj->pjPrivateData );	}

    if  ( (*ea->eaLayoutDocument)( pj->pjPrivateData, pj->pjFormat, add,
						    &(pg->pgSheetGeometry) ) )
	{
	SDEB(fromName);
	appPrintFinishCommandRun( ea, pj, add );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Command line Print To File entrypoint.				*/
/*									*/
/************************************************************************/

int appPrintToFile(	EditApplication *	ea,
			const char *		fromName,
			const char *		toName,
			const char *		paperString )
    {
    int				rval= 0;
    AppDrawingData		add;

    SimpleOutputStream *	sos;

    PrintGeometry		pg;
    PrintJob			pj;

    utilInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );
    appInitDrawingData( &add );

    if  ( ! strcmp( fromName, toName ) )
	{ SSDEB(fromName,toName); rval= -1; goto ready;	}

    if  ( appPrintStartCommandRun( ea, &pj, &pg, &add, paperString, fromName ) )
	{ SDEB(fromName); rval= -1; goto ready;	}

    if  ( ! strcmp( toName, "-" ) )
	{ sos= sioOutStdoutOpen();		}
    else{ sos= sioOutStdioOpen( toName );	}

    if  ( ! sos )
	{
	SXDEB(toName,sos);
	appPrintFinishCommandRun( ea, &pj, &add );
	rval= -1; goto ready;
	}

    if  ( appCallPrintFunction( sos, &pj, &pg ) )
	{ SDEB(fromName); rval= -1;	}

    sioOutClose( sos );

    appPrintFinishCommandRun( ea, &pj, &add );

  ready:
    utilCleanPrintGeometry( &pg );
    return rval;
    }

/************************************************************************/
/*									*/
/*  Command line Print To Printer entrypoint.				*/
/*									*/
/************************************************************************/

int appPrintToPrinter(	EditApplication *	ea,
			const char *		fromName,
			const char *		toName,
			const char *		paperString )
    {
    int			rval= 0;
    AppDrawingData	add;

    int			printer;

    PrintGeometry	pg;
    PrintJob		pj;

    utilInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );
    appInitDrawingData( &add );

    if  ( ! ea->eaPrintDestinationsCollected	&&
	  appGetPrintDestinations( ea )		)
	{ LDEB(1); rval= -1; goto ready;	}

    printer= ea->eaDefaultPrintDestination;

    if  ( toName )
	{
	int				i;
	const PrintDestination *	pd;

	pd= ea->eaPrintDestinations;
	for ( i= 0; i < ea->eaPrintDestinationCount; pd++, i++ )
	    {
	    if  ( ! strcmp( toName, pd->pdPrinterName ) )
		{ printer= i; break;	}
	    }

	if  ( i >= ea->eaPrintDestinationCount )
	    { SDEB(toName); rval= -1; goto ready;	}
	}

    if  ( printer < 0 )
	{ LDEB(printer); rval= -1; goto ready;	}

    if  ( appPrintStartCommandRun( ea, &pj, &pg, &add, paperString, fromName ) )
	{ SDEB(fromName); rval= -1; goto ready;	}

    if  ( appPrintDocument( printer, &pj, &pg ) )
	{ SDEB(fromName); rval= -1;	}

    appPrintFinishCommandRun( ea, &pj, &add );

  ready:
    utilCleanPrintGeometry( &pg );
    return rval;
    }

