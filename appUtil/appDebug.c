/************************************************************************/
/*									*/
/*  Debugging output.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdarg.h>
#   include	<appDebug.h>

static FILE *	APP_DebugFile;

int appDebug(	const char *	format,
		... )
    {
    va_list	ap;
    int		ret;

    FILE *	debugFile= APP_DebugFile;

    if  ( ! debugFile )
	{ debugFile= stderr;	}

    va_start( ap, format );

    ret= vfprintf( debugFile, format, ap );

    va_end( ap );

    fflush( debugFile );

    return ret;
    }

int appDebugSetFile(	const char *	filename,
			const char *	mode )
    {
    FILE *	debugFile= (FILE *)0;

    if  ( filename )
	{
	debugFile= fopen( filename, mode );

	if  ( ! debugFile )
	    { SSXDEB(filename,mode,debugFile); return -1;	}
	}

    if  ( APP_DebugFile )
	{ fclose( APP_DebugFile ); APP_DebugFile= (FILE *)0;	}

    APP_DebugFile= debugFile;

    return 0;
    }
