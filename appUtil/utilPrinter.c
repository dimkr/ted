/************************************************************************/
/*									*/
/*  Utility functions for printing.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"utilPrinter.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find out what printers are available on the system.			*/
/*									*/
/*  0)  Compare for qsort()ing by name.					*/
/*  1)  BSD/Linux style.						*/
/*  2)  AIX style. ( Thanks to Marc Selis from axi.nl )			*/
/*  3)  SysV/LPRng style.						*/
/*									*/
/*  NOTE: Before the CUPS, all commands were tried. Now that CUPS	*/
/*	implements them both on the same machine, the first one that	*/
/*	returns any printers is assumed to return them all.		*/
/*									*/
/************************************************************************/

/*  0  */
static int utilPrinterCompareNames(	const void *	vpd1,
					const void *	vpd2 )
    {
    const PrintDestination *	pd1= (const PrintDestination *)vpd1;
    const PrintDestination *	pd2= (const PrintDestination *)vpd2;

    return strcmp( pd1->pdPrinterName, pd2->pdPrinterName );
    }

/*  2,3 */
static int utilPrinterSetLpCommand(	PrintDestination *	pd,
					const char *		name,
					int			l )
    {
    pd->pdCommand= (char *)malloc( l+ 12 );
    if  ( ! pd->pdCommand )
	{ LXDEB(l,pd->pdCommand); return -1;	}
    sprintf( pd->pdCommand, "lp -s -d '%s'", name );
    pd->pdPrintKind= APPprinterPIPE;
    pd->pdPrinterName= (char *)malloc( l+ 1 );
    if  ( ! pd->pdPrinterName )
	{ LXDEB(l,pd->pdPrinterName); return -1;	}

    pd->pdCommandLength= strlen( pd->pdCommand );
    pd->pdPercentCount= 0;

    strcpy( pd->pdPrinterName, name );

    return 0;
    }

/*  1  */
static int utilPrinterGetLprPrinters(	const char *		command,
					int *			pDefaultPrinter,
					int *			pPrinterCount,
					PrintDestination **	pDestinations )
    {
    char			scratch[250+1];

    int				l;
    FILE *			f;
    int				found= 0;

    PrintDestination *		fresh;
    int				count= *pPrinterCount;
    const char *		defaultPrinterName;

    f= popen( command, "r" );
    if  ( ! f )
	{ return 0;	}

    while( fgets( scratch, 250, f ) )
	{
	l= strlen( scratch );
	if  ( l > 0 && scratch[l-1] == '\n' )
	    { scratch[l-1]= '\0';	}

	if  ( scratch[0] == '\t' || scratch[l-2] != ':' )
	    { continue;	}

	scratch[l-2]= '\0';

	fresh= (PrintDestination *)realloc( *pDestinations,
				    (count+ 1)*sizeof( PrintDestination ) );
	if  ( ! fresh )
	    { XDEB(fresh); return -1;	}
	*pDestinations= fresh;

	fresh += count;

	fresh->pdCommand= (char *)malloc( l+ 23 );
	if  ( ! fresh->pdCommand )
	    { LXDEB(l,fresh->pdCommand); return -1;	}
#	if 0
	Has the advantage that there is no maximum on the file size.
	Has the disadvantage that it does not work on SuSE Linux 6.0
	sprintf( fresh->pdCommand, "lpr -r -s -P '%s' '%s'",
							scratch, "%f" );
#	else
	sprintf( fresh->pdCommand, "lpr -r -P '%s' '%s'",
							scratch, "%f" );
#	endif
	fresh->pdPrintKind= APPprinterTMPFILE;
	fresh->pdPrinterName= strdup( scratch );
	if  ( ! fresh->pdPrinterName )
	    { LXDEB(l,fresh->pdPrinterName); return -1;	}

	fresh->pdCommandLength= strlen( fresh->pdCommand );
	fresh->pdPercentCount= 1;

	found++; count++;
	}

    pclose( f );

    qsort( *pDestinations, count, sizeof(PrintDestination),
						    utilPrinterCompareNames );

    defaultPrinterName= getenv( "PRINTER" );
    if  ( defaultPrinterName )
	{
	PrintDestination *	pd= *pDestinations;

	for ( l= count- found; l < count; pd++, l++ )
	    {
	    if  ( ! strcmp( defaultPrinterName, pd->pdPrinterName ) )
		{ *pDefaultPrinter= l; break;	}
	    }
	}

    *pPrinterCount= count;
    return found;
    }

/*  2  */
static int utilPrinterGetAixPrinters(	const char *		command,
					int *			pDefaultPrinter,
					int *			pPrinterCount,
					PrintDestination **	pDestinations )
    {
    char			scratch[250+1];

    int				l;
    FILE *			f;
    int				found= 0;

    PrintDestination *		fresh;
    int				count= *pPrinterCount;
    const char *		defaultPrinterName;

    f= popen( command, "r" );
    if  ( ! f )
	{ return 0;	}

    if  ( fgets( scratch, 250, f ) && fgets( scratch, 250, f ) )
	{
	while( fgets( scratch, 250, f ) )
	    {
	    char *	s= strchr( scratch, ' ' );
	    if  ( ! s )
		{ continue;	}
	    *s= '\0'; l= s- scratch;
    
	    fresh= (PrintDestination *)realloc( *pDestinations,
				    (count+ 1)*sizeof( PrintDestination ) );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}
	    *pDestinations= fresh;
    
	    if  ( utilPrinterSetLpCommand( fresh+ count, scratch, l ) )
		{ LDEB(l); return -1;	}
    
	    found++; count++;
	    }
	}

    pclose( f );

    qsort( *pDestinations, count, sizeof(PrintDestination),
						    utilPrinterCompareNames );

    defaultPrinterName= getenv( "LPDEST" );
    if  ( defaultPrinterName )
	{
	PrintDestination *	pd= *pDestinations;

	for ( l= count- found; l < count; pd++, l++ )
	    {
	    if  ( ! strcmp( defaultPrinterName, pd->pdPrinterName ) )
		{ *pDefaultPrinter= l; break;	}
	    }
	}

    *pPrinterCount= count;
    return found;
    }

/*  3  */
static int utilPrinterGetLpPrinters(	const char *		command,
					int *			pDefaultPrinter,
					int *			pPrinterCount,
					PrintDestination **	pDestinations )
    {
    char			scratch[250+1];

    int				l;
    FILE *			f;
    int				found= 0;

    PrintDestination *		fresh;
    int				count= *pPrinterCount;
    const char *		defaultPrinterName;

    /*  2  */
    f= popen( command, "r" );
    if  ( ! f )
	{ return 0;	}

    while( fgets( scratch, 250, f ) )
	{
	char *	s= strchr( scratch, ' ' );
	if  ( ! s )
	    { continue;	}
	*s= '\0'; l= s- scratch;

	fresh= (PrintDestination *)realloc( *pDestinations,
				(count+ 1)*sizeof( PrintDestination ) );
	if  ( ! fresh )
	    { XDEB(fresh); return -1;	}
	*pDestinations= fresh;

	if  ( utilPrinterSetLpCommand( fresh+ count, scratch, l ) )
	    { LDEB(l); return -1;	}

	found++; count++;
	}

    pclose( f );

    qsort( *pDestinations, count, sizeof(PrintDestination),
						    utilPrinterCompareNames );

    defaultPrinterName= getenv( "LPDEST" );
    if  ( defaultPrinterName )
	{
	PrintDestination *	pd= *pDestinations;

	for ( l= count- found; l < count; pd++, l++ )
	    {
	    if  ( ! strcmp( defaultPrinterName, pd->pdPrinterName ) )
		{ *pDefaultPrinter= l; break;	}
	    }
	}

    *pPrinterCount= count;
    return found;
    }

static int utilPrinterSetCustom(	PrintDestination *	pd,
					const char *		customCommand,
					const char *		customName )
    {
    const char *	p;

    pd->pdCommand= strdup( customCommand );
    pd->pdPrinterName= strdup( customName );

    pd->pdCommandLength= strlen( customCommand );
    pd->pdPercentCount= 0;

    if  ( ! pd->pdCommand && ! pd->pdPrinterName )
	{ XXDEB(pd->pdCommand,pd->pdPrinterName); return -1; }

    p= customCommand;
    while( *p )
	{
	if  ( p[0] == '%' && p[1] == 'f' )
	    { pd->pdPercentCount++;	}
	p++;
	}

    if  ( pd->pdPercentCount > 0 )
	{ pd->pdPrintKind= APPprinterTMPFILE;	}
    else{ pd->pdPrintKind= APPprinterPIPE;	}

    return 0;
    }

int utilPrinterGetPrinters(	int *			pPrinterCount,
				int *			pDefaultPrinter,
				PrintDestination **	pDestinations,
				const char *		customCommand,
				const char *		customName,
				const char *		customCommand2,
				const char *		customName2 )
    {
    PrintDestination *		pd= (PrintDestination *)0;
    int				count= 0;
    int				defaultPrinter= -1;
    int				extra= 0;

    /*  1  */
    if  ( count == 0 )
	{
	utilPrinterGetLprPrinters(  "( lpc status ) 2>/dev/null",
						&defaultPrinter, &count, &pd );
	}

    /*  1b  */
    if  ( count == 0 )
	{
	utilPrinterGetLprPrinters(  "( /usr/sbin/lpc status ) 2>/dev/null",
						&defaultPrinter, &count, &pd );
	}

    /*  2  */
    if  ( count == 0 )
	{
	utilPrinterGetAixPrinters(  "( enq -As ) 2>/dev/null",
						&defaultPrinter, &count, &pd );
	}

    /*  3  */
    if  ( count == 0 )
	{
	utilPrinterGetLpPrinters(  "( lpstat -a ) 2>/dev/null",
						&defaultPrinter, &count, &pd );
	}

    if  ( defaultPrinter < 0 && count > 0 )
	{ defaultPrinter= 0;	}

    if  ( customName || customCommand )
	{
	if  ( ! customName || ! customCommand )
	    { XXDEB(customName,customCommand);	}
	else{ extra++;				}
	}

    if  ( customName2 || customCommand2 )
	{
	if  ( ! customName2 || ! customCommand2 )
	    { XXDEB(customName2,customCommand2);	}
	else{ extra++;					}
	}

    if  ( extra > 0 )
	{
	PrintDestination *	fresh;

	fresh= (PrintDestination *)realloc( pd,
				(count+ extra)*sizeof( PrintDestination ) );
	if  ( ! fresh )
	    { LLXDEB(count,extra,fresh); return -1;	}
	pd= fresh;
	}

    if  ( customName && customCommand )
	{
	if  ( utilPrinterSetCustom( pd+ count, customCommand, customName ) )
	    { LLDEB(extra,count); return -1;		}
	else{ defaultPrinter= count++;			}
	}

    if  ( customName2 && customCommand2 )
	{
	if  ( utilPrinterSetCustom( pd+ count, customCommand2, customName2 ) )
	    { LLDEB(extra,count); return -1;		}
	else{ count++;					}
	}

    *pPrinterCount= count;
    *pDefaultPrinter= defaultPrinter;
    *pDestinations= pd;

    return 0;
    }
