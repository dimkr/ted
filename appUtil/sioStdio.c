/************************************************************************/
/*									*/
/*  Simple io streams using the C stdio.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<sioStdio.h>
#   include	<appDebugoff.h>

static int sioStdioClose(	void *	voidf )
    { return fclose( (FILE *)voidf );	}

static int sioStdioSeek(		void *			voidf,
					long			pos )
    {
    if  ( fseek( (FILE *)voidf, pos, SEEK_SET ) )
	{ LDEB(pos); return -1;	}

    return 0;
    }

static int sioInStdioReadBytes(	void *		voidf,
				unsigned char *	buffer,
				int		count )
    { return fread( buffer, 1, count, (FILE *)voidf );	}

SimpleInputStream * sioInStdioOpen(	const char *	filename )
    {
    SimpleInputStream *	sis;
    FILE *		f= fopen( filename, "rb" );

    if  ( ! f )
	{ SXDEB(filename,f); return (SimpleInputStream *)0;	}

    sis= sioInOpen( (void *)f, sioInStdioReadBytes, sioStdioClose );

    if  ( ! sis )
	{ SXDEB(filename,sis); fclose( f ); return (SimpleInputStream *)0; }

    return sis;
    }

static int sioOutStdioWriteBytes(	void *			voidf,
					const unsigned char *	buffer,
					int			count )
    { return fwrite( buffer, 1, count, (FILE *)voidf );	}

SimpleOutputStream * sioOutStdioOpen(	const char *	filename )
    {
    SimpleOutputStream *	sos;
    FILE *			f= fopen( filename, "w" );

    if  ( ! f )
	{ SXDEB(filename,f); return (SimpleOutputStream *)0;	}

    sos= sioOutOpen( (void *)f, sioOutStdioWriteBytes,
						sioStdioSeek, sioStdioClose );

    if  ( ! sos )
	{ SXDEB(filename,sos); fclose( f ); return (SimpleOutputStream *)0; }

    return sos;
    }
