/************************************************************************/
/*									*/
/*  Maniputaion of file names.						*/
/*									*/
/*  The routines do not consult the file system to find out whether	*/
/*  strings refer to a particular kind of thing on the file system.	*/
/*  The current directory is however retrieved to resolve relative	*/
/*  names.								*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appSystem.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return a pointer to the beginning of the extension part in a file	*/
/*  name. The pointer points IN the input string.			*/
/*									*/
/************************************************************************/

const char * appFileExtensionOfName(	const char *	filename )
    {
    const char *	slash;
    const char *	dot;

    slash= strrchr( filename, '/' );
    if  ( slash )
	{ dot= strrchr( slash+ 1, '.' );	}
    else{ dot= strrchr( filename, '.' );	}

    if  ( ! dot )
	{ return dot;		}
    else{ return dot+ 1;	}
    }

const char * appRelativeName(	const char *	filename )
    {
    const char *	slash;

    slash= strrchr( filename, '/' );
    if  ( slash )
	{ return slash+ 1;	}
    else{ return filename;	}
    }

/************************************************************************/
/*									*/
/*  Translate a file name to an absolute name.				*/
/*									*/
/*  a)  Utility routine: append a path fragment to an already existing	*/
/*	piece of path.							*/
/*  b)  Make some attempts to remove silly intermediate fragments	*/
/*	from the path. This can give unexpected results when there are	*/
/*	symbolic links to directories, or the ../ go below the root of	*/
/*	the file system. The results of not removing ./ and ../ can be	*/
/*	as unexpected, but has the advantage not to attach any meaning	*/
/*	to the structure of file names in application code.		*/
/*									*/
/*  1)  If the name is an absolute one, just copy it to the		*/
/*	destination.							*/
/*  2)  See whether the file the name has to be relative to has a	*/
/*	directory part. If so, determine its length, including the	*/
/*	slash that separates it from the relative part.			*/
/*  3)  If the file the name has to be relative to is an absolute one,	*/
/*	append the relative name to the absolute one.			*/
/*  4)  Both names are relative.. Determine the current directory.	*/
/*  5)  Append a final slash to the name of the current directory.	*/
/*  6)  Build one absolute name of the collected path elements.		*/
/*									*/
/************************************************************************/

/*  a  */
static int utilFileNameCatenate(	char *		path,
					int		pathLen,
					int		addSlash,
					const char *	relative,
					int		relLen )
    {
#   ifndef __VMS

    /*  b  */
    while( pathLen > 0 )
	{
	if  ( ! strncmp( relative, "./", 2 ) )
	    {
	    relative += 2;
	    relLen -= 2;
	    continue;
	    }

	if  ( ! strncmp( relative, "../", 3 ) )
	    {
	    int			lastSlash= -1;
	    int			i;

	    for ( i= 0; i < pathLen- 1; i++ )
		{
		if  ( path[i] == '/' )
		    { lastSlash= i;	}
		}

	    if  ( lastSlash < 0 )
		{ break; }

	    pathLen= lastSlash+ 1;
	    relative += 3;
	    relLen -= 3;
	    continue;
	    }

	break;
	}
#   endif

    /*
    if  ( pathLen > 0 && path[pathLen- 1] != '/' )
	{ path[pathLen++]= '/';	}
    */
    if  ( addSlash )
	{ path[pathLen++]= '/';	}

    memcpy( path+ pathLen, relative, relLen );
    path[pathLen+ relLen]= '\0';

    return pathLen+ relLen;
    }

int appAbsoluteName(	char *		absolute,
			int		len,
			const char *	filename,
			int		relativeIsFile,
			const char *	nameRelativeTo )
    {
    int			fileLen= 0;
    int			rootLen= 0;
    int			relLen= 0;
    int			absLen;
    int			addSlash= 0;

    const char *	slash;

    fileLen= strlen( filename );

    /*  1  */
    if  ( filename[0] == '/' )
	{
	if  ( fileLen > len )
	    { SLDEB(filename,len); return -1;	}

	strcpy( absolute, filename );
	return fileLen;
	}

    /*  2  */
    if  ( nameRelativeTo )
	{
	if  ( relativeIsFile )
	    {
	    slash= strrchr( nameRelativeTo, '/' );

	    if  ( slash )
		{ relLen= slash- nameRelativeTo+ 1;	}
	    }
	else{
	    addSlash= 1;
	    relLen= strlen( nameRelativeTo );
	    }
	}

    /*  3  */
    if  ( relLen > 0 && nameRelativeTo[0] == '/' )
	{
	if  ( relLen+ addSlash+ fileLen > len )
	    { LLLDEB(relLen,fileLen,len); return -1;	}

	strncpy( absolute, nameRelativeTo, relLen );
	absLen= relLen;
	absLen= utilFileNameCatenate( absolute, absLen, addSlash,
							filename, fileLen );
	return absLen;
	}

    /*  4  */
    rootLen= appCurrentDirectory( absolute, len- fileLen- 2 );
    if  ( rootLen < 1 )
	{ LSDEB(rootLen,filename); return -1;	}

    if  ( rootLen+ 2 >= len )
	{ LLDEB(rootLen,len); return -1;	}

#   ifndef __VMS
    /*  5  */
    if  ( rootLen > 1 )
	{
	absolute[rootLen++]= '/';
	absolute[rootLen  ]= '\0';
	}
#   endif

    if  ( rootLen+ relLen+ fileLen > len )
	{ LLLLDEB(rootLen,relLen,fileLen,len); return -1;	}

    /*  6  */
    absLen= rootLen;
    if  ( relLen > 0 )
	{
	absLen= utilFileNameCatenate( absolute, absLen, 0,
						    nameRelativeTo, relLen );
	}

    absLen= utilFileNameCatenate( absolute, absLen, 0,
						    filename, fileLen );
    return absLen;
    }

# if 0

static void xx( const char *	filename,
		const char *	fileRelativeTo )
    {
    char	absolute[1000+1];

    if  ( appAbsoluteName( absolute, sizeof( absolute )- 1,
					    filename, fileRelativeTo ) < 0 )
	{ SSDEB( filename, fileRelativeTo );		}
    else{ SSSDEB( filename, fileRelativeTo, absolute );	}

    return;
    }

void xxx()
    {

    xx( "in.gif", "out.rtf" );
    xx( "../in.gif", "out.rtf" );
    xx( "../../in.gif", "out.rtf" );

    xx( "in.gif", "../out.rtf" );
    xx( "in.gif", "../../out.rtf" );
    xx( "../in.gif", "../../out.rtf" );

    xx( "/in.gif", "../../out.rtf" );
    xx( "/tmp/in.gif", "../../out.rtf" );

    xx( "in.gif", "/out.rtf" );
    xx( "in.gif", "/etc/out.rtf" );
    xx( "../in.gif", "/etc/out.rtf" );
    xx( "../../in.gif", "/etc/out.rtf" );

    return;
    }

# endif

