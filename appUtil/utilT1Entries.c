#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFont.h"
#   include	"appSystem.h"
#   include	"utilX11FontNames.h"
#   include	"utilT1Entries.h"
#   include	"utilFontmap.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get a line of input. (Essentially fgets())				*/
/*									*/
/************************************************************************/

static char *	utilT1GetInputLine(	char *		s,
					int *		pLen,
					int		size,
					FILE *		stream )
    {
    s= fgets( s, size, stream );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\n'	)
	    { s[--lineLength]= '\0'; 	}

#	if 0
	No! are unix specific files

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	/*  3  */
	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}
#	endif

	*pLen= lineLength;
	}

    return s;
    }

/************************************************************************/
/*									*/
/*  Set an X11 query format for a PostScript font.			*/
/*									*/
/************************************************************************/

static int utilT1SetXQueryFormat(	AppFontTypeface *	aft,
					const char *		x11Name,
					int			x11Length )
    {
    const AfmFontInfo *	afi= aft->aftFontInfo;
    int			enc= utilEncodingFromX11FontName( x11Name );

    if  ( enc >= 0				&&
	  ! aft->aftXQueryFormats[enc]		)
	{
	aft->aftXQueryFormats[enc]= (char *)malloc( x11Length+ 1 );
	if  ( ! aft->aftXQueryFormats[enc] )
	    { XDEB(aft->aftXQueryFormats[enc]); return -1;	}

	strncpy( aft->aftXQueryFormats[enc], x11Name, x11Length )
							    [x11Length]= '\0';
	}

    if  ( afi->afiUseFontspecificEncoding	&&
	  ! aft->aftFontSpecificQueryFormat	)
	{
	aft->aftFontSpecificQueryFormat= (char *)malloc( x11Length+ 1 );
	if  ( ! aft->aftFontSpecificQueryFormat )
	    { XDEB(aft->aftFontSpecificQueryFormat); return -1;	}

	strncpy( aft->aftFontSpecificQueryFormat, x11Name, x11Length )
							    [x11Length]= '\0';
	}

    return 0;
    }

int utilSetT1EntriesForFonts(	const char *			t1MapFile,
				int				mapNames,
				const PostScriptFontList *	psfl )
    {
    int		rval= 0;

    FILE *	xfontDir= fopen( t1MapFile, "r" );
    char	scratch[256+1];
    int		lineLength;

    int		line= 0;

    if  ( ! xfontDir )
	{ SXDEB(t1MapFile,xfontDir); return -1;	}

    while( utilT1GetInputLine( scratch, &lineLength,
					    sizeof(scratch)- 1, xfontDir ) )
	{
	int			x11Start;
	int			x11Length;
	const char *		x11Name;
	int			res;

	const AppFontFamily *	aff;
	AppFontTypeface *	aft;
	int			fam;
	int			face;

	res= utilX11SplitFontsDirLine( scratch,
					&x11Start, &x11Length, line++ );
	if  ( res < 0 )
	    { SDEB(scratch); rval= -1; continue;	}
	if  ( res > 0 )
	    { continue;	}

	x11Name= scratch+ x11Start;

	aff= psfl->psflFamilies; aft= (AppFontTypeface *)0;
	for ( fam= 0; fam < psfl->psflFamilyCount; aff++, fam++ )
	    {
	    aft= aff->affFaces;
	    for ( face= 0; face < aff->affFaceCount; aft++, face++ )
		{
		const AfmFontInfo *	afi;
		const char *		fontName;
		const char *		alias;

		afi= aft->aftFontInfo;
		fontName= alias= afi->afiFontName;

		if  ( mapNames )
		    {
		    alias= utilFontmapGetEntry( fontName );
		    if  ( ! alias )
			{ alias= fontName;	}
		    }

		if  ( ! strcmp( alias, scratch )			&&
		      utilT1SetXQueryFormat( aft, x11Name, x11Length )	)
		    { LDEB(x11Length); rval= -1; goto ready;	}
		}
	    }
	}

  ready:
    if  ( xfontDir )
	{ fclose( xfontDir );	}

    return rval;
    }

