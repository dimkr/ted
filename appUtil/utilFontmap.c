/************************************************************************/
/*									*/
/*  Implements a mapping between font names as implemented by the	*/
/*  GhostScript 'Fontmap' file.						*/
/*									*/
/************************************************************************/

#   include	<appUtilConfig.h>

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<appSystem.h>
#   include	<utilFontmap.h>

#   include	<appDebugon.h>

typedef struct FontmapEntry
    {
    char *		feFrom;
    char *		feTo;
    int			feIsAlias;
    } FontmapEntry;

static FontmapEntry *	UTIL_FontmapEntries;
int			UTIL_FontmapEntryCount;

/************************************************************************/
/*									*/
/*  Get a name resulting from one of the mappings.			*/
/*									*/
/************************************************************************/

static const char *	utilFontmapGet(	const char *		from,
					const FontmapEntry *	entries,
					int			entryCount )
    {
    int			turn;

    for ( turn= 0; turn < 20; turn++ )
	{
	const FontmapEntry *	fe;
	int			i;

	fe= entries;
	for ( i= 0; i < entryCount; fe++, i++ )
	    {
	    if  ( ! strcmp( fe->feFrom, from ) )
		{ break;	}
	    }

	if  ( i >= entryCount )
	    { return (const char *)0;	}

	if  ( fe->feIsAlias )
	    { from= fe->feTo; continue;	}

	return fe->feTo;
	}

    LSDEB(turn,from);
    return (const char *)0;
    }

const char * utilFontmapGetEntry(	const char *	from )
    {
    return utilFontmapGet( from,
			UTIL_FontmapEntries, UTIL_FontmapEntryCount );
    }

/************************************************************************/
/*									*/
/*  Read the GhostScript font map.					*/
/*									*/
/*  1)  Syntax error in the Cyrillic fonts section.			*/
/*	As usual, people do not follow their own file format.		*/
/*  2)  The standard GhostScript distribution comes with a reference	*/
/*	to Fontmap.greek in Fontmap. As this file does not exist and	*/
/*	GhostScript seems to enjoy this kind of a mess, only include	*/
/*	existing files.	(This is an aspect of Macho culture.. tidiness	*/
/*	seems to be for girls and weaklings.)				*/
/*									*/
/************************************************************************/

static int utilFontmapNextItem(	FILE *	f )
    {
    int		c;

    c= getc( f );
    for (;;)
	{
	if  ( c == '%' )
	    {
	    c= getc( f );
	    while( c != EOF && c != '\n' )
		{ c= getc( f );	}

	    if  ( c == '\n' )
		{ c= getc( f );	}

	    continue;
	    }

	if  ( isspace( c ) )
	    { c= getc( f ); continue;	}

	break;
	}

    return c;
    }

int utilFontmapReadMap(	const char *	filename )
    {
    int		rval= 0;
    FILE *	f= (FILE *)0;
    int		c;

    static int	recursive= 0;

    static const char runlibfile[]= ".runlibfile";

    recursive++;

    if  ( recursive == 1 && UTIL_FontmapEntryCount > 0 )
	{ /* LDEB(UTIL_FontmapEntryCount); rval= -1; */ goto ready;	}

    if  ( recursive > 20 )
	{ LDEB(recursive); rval= -1; goto ready;	}

    f= fopen( filename, "r" );
    if  ( ! f )
	{ SXDEB(filename,f); rval= -1; goto ready;	}

    c= utilFontmapNextItem( f );
    while( c != EOF )
	{
	FontmapEntry *	fe;

	char		scratch[200+ 1];
	char		runfile[400+ 1];
	int		l;

	switch( c )
	    {
	    case ';':
		/*  1  */
		c= utilFontmapNextItem( f );
		continue;

	    case '(':
		for ( l= 0; l < 200; l++ )
		    {
		    c= getc( f );
		    if  ( c == EOF )
			{ LDEB(c); rval= -1; goto ready;	}
		    if  ( c == ')' )
			{ break;	}

		    scratch[l]= c;
		    }
		if  ( l >= 200 )
		    { LDEB(l); rval= -1; goto ready;	}
		scratch[l]= '\0';

		c= getc( f );
		if  ( c == EOF )
		    { LDEB(c); rval= -1; goto ready;	}

		while( isspace( c ) )
		    {
		    c= getc( f );
		    if  ( c == EOF )
			{ LDEB(c); rval= -1; goto ready;	}
		    }

		for ( l= 0; l < sizeof(runlibfile)- 1; l++ )
		    {
		    if  ( c != runlibfile[l] )
			{ SCDEB(runlibfile+l,c); rval= -1; goto ready; }
		    c= getc( f );
		    }

		while( isspace( c ) && c != '\n' )
		    {
		    c= getc( f );
		    if  ( c == EOF )
			{ LDEB(c); rval= -1; goto ready;	}
		    }

		if  ( c != '\n' )
		    { CDEB(c); rval= -1; goto ready; }

		if  ( appAbsoluteName( runfile, sizeof(runfile)- 1,
						scratch, 1, filename ) < 0 )
		    { SSDEB(filename,scratch); rval= -1; goto ready;	}

		/*  2  */
		if  ( ! appTestFileExists( runfile )	&&
		      utilFontmapReadMap( runfile )	)
		    { SDEB(runfile); rval= -1; goto ready;	}

		c= utilFontmapNextItem( f );
		continue;

	    case '/':
		break;

	    default:
		CDEB(c);
		rval= -1; goto ready;
	    }

	fe= realloc( UTIL_FontmapEntries,
			( UTIL_FontmapEntryCount+ 1)* sizeof(FontmapEntry) );
	if  ( ! fe )
	    { LXDEB(UTIL_FontmapEntryCount,fe); rval= -1; goto ready; }
	UTIL_FontmapEntries= fe;
	fe += UTIL_FontmapEntryCount;

	for ( l= 0; l < 200; l++ )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    if  ( isspace( c ) )
		{ break;	}

	    scratch[l]= c;
	    }
	if  ( l >= 200 )
	    { LDEB(l); rval= -1; goto ready;	}
	scratch[l]= '\0';

	fe->feFrom= strdup( scratch );
	if  ( ! fe->feFrom )
	    { XDEB(fe->feFrom); rval= -1; goto ready;	}

	while( isspace( c ) )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    }

	switch( c )
	    {
	    case '/':
		fe->feIsAlias= 1;
		break;
	    case '(':
		fe->feIsAlias= 0;
		break;
	    default:
		CDEB(c); rval= -1; goto ready;
	    }

	for ( l= 0; l < 200; l++ )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    if  ( isspace( c ) )
		{ break;	}

	    scratch[l]= c;
	    }
	if  ( l >= 200 )
	    { LDEB(l); rval= -1; goto ready;	}
	scratch[l]= '\0';

	if  ( ! fe->feIsAlias )
	    {
	    if  ( l < 1 || scratch[l-1] != ')' )
		{ SDEB(scratch); rval= -1; goto ready;	}

	    scratch[l-1]= '\0';
	    }

	fe->feTo= strdup( scratch );
	if  ( ! fe->feTo )
	    { XDEB(fe->feTo); rval= -1; goto ready;	}

	while( isspace( c ) )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    }

	if  ( c != ';' )
	    { CDEB(c); rval= -1; goto ready;	}

	UTIL_FontmapEntryCount++;
	c= utilFontmapNextItem( f );
	}

  ready:

    recursive--;

    if  ( f )
	{ fclose( f );	}

    return rval;
    }

