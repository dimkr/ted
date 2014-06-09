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
/*  Follow a link.							*/
/*									*/
/*  1)  As a HACK, invoke netscape for all links with a protocol	*/
/*	different from 'file:'.						*/
/*  2)  Theoretically, mailcap/mimecap should have been consulted for	*/
/*	the application corresponding to the extension of the file.	*/
/*									*/
/************************************************************************/

static int tedSetBrowserCall(	char *			target,
				const char *		remote,
				const char *		endremote,
				const char *		direct,
				const char *		enddirect,
				const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize )
    {
    char *	to= target;

    strcpy( to, remote );
    to += strlen( to );
    memcpy( to, fileName, fileSize ); to += fileSize;
    if  ( markSize > 0 )
	{
	*(to++)= '#';
	memcpy( to, markName, markSize ); to += markSize;
	}
    strcpy( to, endremote ); to += strlen( to );

    if  ( direct )
	{
	strcpy( to, " || " ); to += strlen( to );

	strcpy( to, direct ); to += strlen( to );

	memcpy( to, fileName, fileSize ); to += fileSize;
	if  ( markSize > 0 )
	    {
	    *(to++)= '#';
	    memcpy( to, markName, markSize ); to += markSize;
	    }

	strcpy( to, enddirect ); to += strlen( to );
	}

    return to- target;
    }

/*  1  */
static int tedCallNetscape(	const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize )
    {
    char *	scratch;
    int		rval= 0;
    int		size;
    char *	to;

    const char	remote1[]= "( firefox -remote 'openUrl(";
    const char	direct1[]= "( firefox '";

    const char	remote2[]= "( mozilla -noraise -remote 'openUrl(";
    const char	direct2[]= "( mozilla '";

    const char	remote3[]= "( netscape -noraise -remote 'openUrl(";
    const char	direct3[]= "( netscape '";

    const char	remote4[]= "( kfmclient  openURL '";
    const char	endrem4[]= "' ) 2>/dev/null";

    const char	endremote[]= ",new-window)' ) 2>/dev/null";
    const char	enddirect[]= "' ) 2>/dev/null";

    /**** Prevent tricks to run arbitrary commands! */
    int		i;
    for ( i= 0; i < fileSize; i++ )
	{
	if  ( fileName[i] == '\'' )
	    { SDEB(fileName); return -1;	}
	}
    for ( i= 0; i < markSize; i++ )
	{
	if  ( markName[i] == '\'' )
	    { SDEB(markName); return -1;	}
	}
    /****/

    size= 0;

    size += sizeof(remote1)+ fileSize+ 1+ markSize;
    size += sizeof(endremote);
    size += 4;
    size += sizeof(direct1)+ fileSize+ 1+ markSize;
    size += sizeof(enddirect);

    size += 4;

    size += sizeof(remote2)+ fileSize+ 1+ markSize;
    size += sizeof(endremote);
    size += 4;
    size += sizeof(direct2)+ fileSize+ 1+ markSize;
    size += sizeof(enddirect);

    size += 4;

    size += sizeof(remote3)+ fileSize+ 1+ markSize;
    size += sizeof(endremote);
    size += 4;
    size += sizeof(direct3)+ fileSize+ 1+ markSize;
    size += sizeof(enddirect);

    size += 4;

    size += sizeof(remote4)+ fileSize+ 1+ markSize;
    size += sizeof(endrem4);

    size += 2+ 1;

    to= scratch= malloc( size );
    if  ( ! scratch )
	{ XDEB(scratch); return -1;	}

    to += tedSetBrowserCall( to, remote1, endremote, direct1, enddirect,
				    fileName, fileSize, markName, markSize );

    strcpy( to, " || " ); to += strlen( to );

    to += tedSetBrowserCall( to, remote2, endremote, direct2, enddirect,
				    fileName, fileSize, markName, markSize );

    strcpy( to, " || " ); to += strlen( to );

    to += tedSetBrowserCall( to, remote3, endremote, direct3, enddirect,
				    fileName, fileSize, markName, markSize );

    strcpy( to, " || " ); to += strlen( to );

    to += tedSetBrowserCall( to, remote4, endrem4, (char *)0, (char *)0,
				    fileName, fileSize, markName, markSize );

    strcpy( to, " &" ); to += strlen( to );

    if  ( system( scratch ) )
	{ SDEB(scratch); rval= -1;	}

    free( scratch );
    return rval;
    }

static void tedAnalyseFileNamePart(	int *		pIsFile,
					int *		pIsAbsolute,
					int *		pProtocolSize,
					const char *	fileName,
					int		fileSize )
    {
    int		isAbsolute= fileName[0] == '/';
    int		isFile;
    int		protocolSize= 0;

    if  ( isAbsolute )
	{ isFile= 1;	}
    else{
	while( protocolSize < fileSize && isalpha( fileName[protocolSize] ) )
	    { protocolSize++;	}

	/*  1  */
	if  ( protocolSize < fileSize && fileName[protocolSize] == ':' )
	    {
	    if  ( protocolSize == 4			&&
		  ! strncmp( fileName, "file", 4 )	)
		{
		fileName += 5;
		fileSize -= 5;
		isAbsolute= fileName[0] == '/';
		protocolSize= 5;
		isFile= 1;
		}
	    else{
		protocolSize= 0;
		isFile= 0;
		}
	    }
	else{ protocolSize= 0; isFile= 1;	}
	}

    *pIsFile= isFile;
    *pIsAbsolute= isAbsolute;
    *pProtocolSize= protocolSize;

    return;
    }

int tedDocFollowLink(	APP_WIDGET		option,
			EditDocument *		edFrom,
			const char *		fileName,
			int			fileSize,
			const char *		markName,
			int			markSize )
    {
    int				rval= 0;
    EditApplication *		ea= edFrom->edApplication;

    char *			slash= (char *)0;
    EditDocument *		edTo;

    char *			scratch= (char *)0;

    TedDocument *		tdFrom= (TedDocument *)edFrom->edPrivateData;
    BufferDocument *		bdFrom= tdFrom->tdDocument;
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);

    const char *		baseName= (const char *)dpFrom->dpHlinkbase;
    int				baseSize= 0;

    APP_WIDGET			relative= edFrom->edToplevel.atTopWidget;

    if  ( ! baseName )
	{ baseName= edFrom->edFilename;	}

    if  ( baseName )
	{ baseSize= strlen( baseName );	}

    if  ( fileSize > 0 )
	{
	const int	read_only= 1;
	int		fileIsFile;
	int		fileIsAbsolute;
	int		fileProtocolSize;

	tedAnalyseFileNamePart( &fileIsFile, &fileIsAbsolute, &fileProtocolSize,
							fileName, fileSize );

	if  ( ! fileIsFile )
	    {
	    return tedCallNetscape( fileName, fileSize, markName, markSize );
	    }

	fileName += fileProtocolSize;
	fileSize -= fileProtocolSize;

	if  ( baseSize > 0 )
	    { slash= strrchr( baseName, '/' ); }

	if  ( fileIsAbsolute || ! slash )
	    {
	    scratch= (char *)malloc( fileSize+ 1 );
	    if  ( ! scratch )
		{ XDEB(scratch); return -1;	}

	    strncpy( scratch, fileName, fileSize )[fileSize]= '\0';

	    edTo= appOpenDocument( ea, relative, option, read_only, scratch );
	    if  ( ! edTo )
		{ SXDEB(scratch,edTo);	}
	    }
	else{
	    int	dir= ( slash- baseName )+ 1;
	    int	len= dir+ fileSize+ 1;

	    scratch= (char *)malloc( len );
	    if  ( ! scratch )
		{ LXDEB(len,scratch); return -1;	}

	    strncpy( scratch, baseName, dir );
	    strncpy( scratch+ dir, fileName, fileSize )[fileSize]= '\0';

	    edTo= appOpenDocument( ea, relative, option, read_only, scratch );
	    if  ( ! edTo )
		{ /* SXDEB(scratch,edTo); */ rval= -1;	}
	    }
	}
    else{ edTo= edFrom;	}

    if  ( edTo && markSize > 0 )
	{ tedGoToBookmark( edTo, markName, markSize );	}

    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

int tedAppFollowLink(	APP_WIDGET		option,
			EditApplication *	ea,
			const char *		fileName,
			int			fileSize,
			const char *		markName,
			int			markSize )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFollowLink( option, ed,
				fileName, fileSize, markName, markSize );
    }

