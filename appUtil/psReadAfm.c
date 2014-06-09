#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<locale.h>

#   include	"sioGeneral.h"
#   include	"utilDocFont.h"
#   include	"psFont.h"
#   include	"utilMatchFont.h"

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  A primitive table driven parser for AFM files.			*/
/*									*/
/************************************************************************/

#   define	AFMlenLINE	256

typedef struct AfmKeyword
    {
    char *	akString;
    int		(*akConsumer)(	SimpleInputStream *			sisAfm,
				int			valPos,
				char *			input,
				AfmFontInfo *		afi );
    } AfmKeyword;

static int psAfmConsumeLines(	SimpleInputStream *			sisAfm,
				char *			input,
				AfmFontInfo *		afi,
				AfmKeyword *		ak,
				int			keywordCount );

static int psAfmIgnore(			SimpleInputStream *, int, char *,AfmFontInfo *);
static int psAfmFontMetrics(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmFontName(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmFullName(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmNotice(			SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmVersion(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmFamilyName(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmWeight(			SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmFontBBox(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmEncodingScheme(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmMappingScheme(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmCharacterSet(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmCharacters(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmIsFixedV(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmCapHeight(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmXHeight(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmAscender(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmDescender(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartDirection(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmUnderlinePosition(	SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmUnderlineThickness(	SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmItalicAngle(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmCharWidth(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmIsFixedPitch(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartCharMetrics(	SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartKernData(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartComposites(	SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartTrackKern(		SimpleInputStream *,	int, char *,AfmFontInfo *);
static int psAfmStartKernPairs(		SimpleInputStream *,	int, char *,AfmFontInfo *);

# define psAfmStdHW psAfmIgnore
# define psAfmStdVW psAfmIgnore

static AfmKeyword	psAfmFileKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartFontMetrics",	psAfmFontMetrics,	},
    };

static AfmKeyword	psAfmMetricsKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "FontName",		psAfmFontName,		},
	{ "FullName",		psAfmFullName,		},
	{ "FamilyName",		psAfmFamilyName,	},
	{ "Weight",		psAfmWeight,		},
	{ "FontBBox",		psAfmFontBBox,		},
	{ "Version",		psAfmVersion,		},
	{ "Notice",		psAfmNotice,		},
	{ "EncodingScheme",	psAfmEncodingScheme,	},
	{ "MappingScheme",	psAfmMappingScheme,	},
	{ "EscChar",		psAfmIgnore,		},
	{ "CharacterSet",	psAfmCharacterSet,	},
	{ "Characters",		psAfmCharacters,	},
	{ "IsBaseFont",		psAfmIgnore,		},
	{ "VVector",		psAfmIgnore,		},
	{ "IsFixedV",		psAfmIsFixedV,		},
	{ "CapHeight",		psAfmCapHeight,		},
	{ "XHeight",		psAfmXHeight,		},
	{ "Ascender",		psAfmAscender,		},
	{ "Descender",		psAfmDescender,		},
	{ "StartDirection",	psAfmStartDirection,	},
	{ "UnderlinePosition",	psAfmUnderlinePosition,	},
	{ "UnderlineThickness",	psAfmUnderlineThickness,},
	{ "ItalicAngle",	psAfmItalicAngle,	},
	{ "CharWidth",		psAfmCharWidth,		},
	{ "IsFixedPitch",	psAfmIsFixedPitch,	},
	{ "StartCharMetrics",	psAfmStartCharMetrics,	},
	{ "StartKernData",	psAfmStartKernData,	},
	{ "StartComposites",	psAfmStartComposites,	},
	{ "StdHW",		psAfmStdHW,		},
	{ "StdVW",		psAfmStdVW,		},

	{ "StartKernPairs",	psAfmStartKernPairs,	},
				/****************************************/
				/*  Should not appear directly in the	*/
				/*  file but embedded between		*/
				/*  "StartKernData" and "EndKernData".	*/
				/*  As some messy afm files forget the	*/
				/*  embeding of the kerning data in a	*/
				/*  separate section, the information	*/
				/*  is accepted here as well.		*/
				/****************************************/

	{ "EndFontMetrics",	0,			},
    };

/************************************************************************/
/*									*/
/*  Get a line of input. (Essentially fgets())				*/
/*									*/
/*  3)  Remove a second carriage return. Some files really have this.	*/
/*									*/
/************************************************************************/

static char *	psGetInputLine(	char *				s,
				int *				pLen,
				int				size,
				SimpleInputStream *		sisAfm )
    {
    s= sioInGetString( s, size, sisAfm );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\n'	)
	    { s[--lineLength]= '\0'; 	}

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	/*  3  */
	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	while( lineLength > 0 && isspace( s[lineLength-1] ) )
	    { s[--lineLength]= '\0';	}

	*pLen= lineLength;
	}

    return s;
    }

/************************************************************************/

int psAfmReadAfm(		SimpleInputStream *	sisAfm,
				AfmFontInfo *		afi )
    {
    int			res;
    char		input[AFMlenLINE+1];

    res= psAfmConsumeLines( sisAfm, input, afi, psAfmFileKeywords,
				sizeof(psAfmFileKeywords)/sizeof(AfmKeyword) );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Consume a series of lines.						*/
/*									*/
/************************************************************************/

static int psAfmConsumeLines(	SimpleInputStream *	sisAfm,
				char *			input,
				AfmFontInfo *		afi,
				AfmKeyword *		keywords,
				int			keywordCount )
    {
    for (;;)
	{
	int		k;
	int		b;
	int		p;
	AfmKeyword *	ak;

	if  ( ! psGetInputLine( input, &p, AFMlenLINE, sisAfm ) )
	    { return 1;	}

	b= 0;
	while( input[b] &&   isspace( input[b] ) )
	    { b++;	}

	if  ( ! input[b] )
	    { continue;	}

	p= b;
	while( input[p] && ! isspace( input[p] ) )
	    { p++;	}
	if  ( input[p] )
	    { input[p++]= '\0';	}

	while( isspace( input[p] ) )
	    { p++;	}

	ak= keywords;
	for ( k= 0; k < keywordCount; ak++, k++ )
	    {
	    if  ( ! strcmp( input+ b, ak->akString ) )
		{ break;	}
	    }

	if  ( k >= keywordCount )
	    { SDEB(input+ b); return -1;	}
	if  ( ! ak->akConsumer )
	    { return 0;	}

	if  ( (*ak->akConsumer)( sisAfm, p, input, afi ) )
	    { SDEB(input+ b); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Ignore an input line.						*/
/*									*/
/************************************************************************/
static int	psAfmIgnore(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Consume font metrics.						*/
/*									*/
/*  1)  Not interested in the version.					*/
/*									*/
/************************************************************************/

static int psAfmFontMetrics(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    {
    return psAfmConsumeLines( sisAfm, input, afi, psAfmMetricsKeywords,
			    sizeof(psAfmMetricsKeywords)/sizeof(AfmKeyword) );
    }

/************************************************************************/
/*									*/
/*  Extract various types of values.					*/
/*									*/
/************************************************************************/

static int psAfmSaveString(	char **		pTarget,
				char *		source )
    {
    *pTarget= (char *)malloc( strlen( source )+ 1 );

    if  ( ! *pTarget )
	{ XDEB(*pTarget); return -1;	}

    strcpy( *pTarget, source );
    return 0;
    }

static int psAfmGetNumber(	double *	pNumber,
				char *		input,
				int		valPos )
    {
    char *	s= input+ valPos;
    char *	past;
    double	d;

    d= strtod( s, &past );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}

    *pNumber= d;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract various strings.						*/
/*									*/
/************************************************************************/

static int psAfmFontName(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFontName), input+ valPos ); }

static int psAfmFullName(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFullName), input+ valPos ); }

static int psAfmNotice(		SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiNotice), input+ valPos ); }

static int psAfmVersion(		SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiVersion), input+ valPos ); }

static int psAfmFamilyName(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFamilyName), input+ valPos ); }

static int	psAfmWeight(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiWeight), input+ valPos ); }

static int	psAfmEncodingScheme(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiEncodingScheme), input+ valPos ); }

/************************************************************************/
/*									*/
/*  Unimplemented..							*/
/*									*/
/************************************************************************/

static int	psAfmBBox(	int		valPos,
				char *		input,
				AfmBBox *	abb )
    {
    char *	s= input+ valPos;
    char *	past;
    double	left, bottom, right, top;

    int		done= 0;

    left= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    bottom= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    right= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    top= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    abb->abbLeft= (int)left;
    abb->abbBottom= (int)bottom;
    abb->abbRight= (int)right;
    abb->abbTop= (int)top;

    return done;
    }

static int psAfmFontBBox(	SimpleInputStream *		sisAfm,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    {
    AfmBBox	abb;
    int		res= psAfmBBox( valPos, input, &abb );
    char *	s;

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    s= input+ valPos+ res;
    while( isspace( *s ) )
	{ s++;	}
    if  ( *s )
	{ SSDEB(input,input+ valPos+ res); return -1;	}

    afi->afiFontBBox= abb;

    return 0;
    }

static int	psAfmMappingScheme(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { LDEB(1); return -1; }

static int	psAfmCharacterSet(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiCharacterSet), input+ valPos ); }

static int	psAfmCharacters(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    int		count;
    char *	past;
    char *	s= input+ valPos;

    count= strtol( s, &past, 10 );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}

    return 0;
    }

static int	psAfmIsFixedV(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmStartDirection(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmCapHeight(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiCapHeight= (int)d; return 0;
    }

static int	psAfmXHeight(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiXHeight= (int)d; return 0;
    }

static int	psAfmAscender(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiAscender= (int)d; return 0;
    }

static int	psAfmDescender(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiDescender= (int)d; return 0;
    }

static int	psAfmUnderlinePosition(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiUnderlinePosition= (int)d; return 0;
    }

static int	psAfmUnderlineThickness(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiUnderlineThickness= (int)d; return 0;
    }

static int	psAfmItalicAngle(	SimpleInputStream *		sisAfm,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    if  ( psAfmGetNumber( &(afi->afiItalicAngle), input, valPos ) )
	{ return -1;	}

    afi->afiTanItalicAngle= tan( ( M_PI* afi->afiItalicAngle ) / 180  );

    return 0;
    }

static int	psAfmCharWidth(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmIsFixedPitch(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    if  ( ! strcmp( input+ valPos, "false" ) )
	{ afi->afiIsFixedPitch= 0; return 0;	}
    if  ( ! strcmp( input+ valPos, "true" ) )
	{ afi->afiIsFixedPitch= 1; return 0;	}

    SSDEB(input,input+ valPos); return -1;
    }

static int	psAfmStartCharMetrics(	SimpleInputStream *		sisAfm,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, sisAfm ) )
	{
	AfmCharMetric *	acm;
	char *		b;
	char *		p;

	int		C= -1;
	double		WX= 0.0;
	char *		N= NULL;
	AfmBBox		abb;

	abb.abbLeft= abb.abbRight= abb.abbTop= abb.abbBottom= 0;

	if  ( ! strncmp( input, "EndCharMetrics", 14 ) )
	    {
	    b= input+ 14;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	b= input;
	while( isspace( *b ) )
	    { b++;	}

	while( *b )
	    {
	    p= b; while( *p && ! isspace( *p ) )
		{ p++;	}
	    if  ( isspace( *p ) )
		{ *(p++)= '\0';	}

	    if  ( ! strcmp( b, "C" ) )
		{
		b= p;
		C= strtol( b, &p, 10 );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "CX" ) )
		{
		b= p;
		C= strtol( b, &p, 16 );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p != ';' )
		    { SDEB(b); return -1;	}
		b= p+ 1; continue;
		}

	    if  ( ! strcmp( b, "WX" ) )
		{
		b= p;
		WX= strtod( b, &p );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "N" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		N= (char *)malloc( p- b+ 1 );
		if  ( ! N )
		    { XDEB(N); return -1;	}
		strncpy( N, b, p- b )[p- b]= '\0';
		goto nextToken;
		}

	    if  ( ! strcmp( b, "L" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "B" ) )
		{
		int	done;

		b= p;
		done= psAfmBBox( 0, b, &abb );

		if  ( done < 0 )
		    { SLDEB(b,done); return -1;	}

		p= b+ done; goto nextToken;
		}

	    SSDEB(b,N); return -1;

	    nextToken:
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p != ';' )
		    { SDEB(b); return -1;	}
		b= p+ 1;
		while( isspace( *b ) )
		    { b++;	}
		continue;
	    }

	acm= (AfmCharMetric *)realloc( afi->afiMetrics,
		    ( afi->afiMetricCount+ 1 )* sizeof( AfmCharMetric ) );
	if  ( ! acm )
	    { LXDEB(C,acm); return -1;	}
	afi->afiMetrics= acm;

	acm += afi->afiMetricCount;

	acm->acmC= C;
	if  ( C >= 0 && C < 256 )
	    { afi->afiDefaultCodeToGlyph[C]= afi->afiMetricCount; }
	acm->acmWX= (int)WX;
	acm->acmN= N;
	acm->acmKernPairs= (AfmKerningPair *)0;
	acm->acmKernPairCount= 0;
	acm->acmBBox= abb;

	afi->afiMetricCount++;
	}

    LDEB(AFMlenLINE); return -1;
    }

static int psAfmStartKernPairs(	SimpleInputStream *	sisAfm,
				int			valPos,
				char *			input,
				AfmFontInfo *		afi )
{
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, sisAfm ) )
	{
	AfmCharMetric *		acm;
	AfmKerningPair *	akp;
	char *			b;
	char *			p;

	char *	N1= (char *)0;
	char *	N2= (char *)0;
	double	XVec= 0.0;

	int	pos1;
	int	pos2;

	if  ( ! strncmp( input, "EndKernPairs", 12 ) )
	    {
	    b= input+ 12;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	b= input;
	while( isspace( *b ) )
	    { b++;	}

	if  ( ! *b || *b == '\n' )
	    { continue;	}

	while( *b )
	    {
	    p= b; while( *p && ! isspace( *p ) )
		{ p++;	}
	    if  ( isspace( *p ) )
		{ *(p++)= '\0';	}

	    if  ( ! strcmp( b, "KPX" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}
		N1= b;
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}
		N2= b;
		b= p;
		XVec= strtod( b, &p );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    SDEB(b); return -1;

	    nextToken:
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p && *p != ';' )
		    { SDEB(b); return -1;	}
		if  ( *p == ';' )
		    { b= p+ 1; }
		else{ b= p   ; }

		while( isspace( *b ) )
		    { b++;	}
		continue;
	    }

	acm= afi->afiMetrics;
	for ( pos2= 0; pos2 < afi->afiMetricCount; acm++, pos2++ )
	    {
	    if  ( acm->acmN && ! strcmp( acm->acmN, N2 ) )
		{ break;	}
	    }

	acm= afi->afiMetrics;
	for ( pos1= 0; pos1 < afi->afiMetricCount; acm++, pos1++ )
	    {
	    if  ( acm->acmN && ! strcmp( acm->acmN, N1 ) )
		{ break;	}
	    }

	if  ( pos1 >= afi->afiMetricCount || pos2 >= afi->afiMetricCount )
	    { LLDEB(pos1,pos2); return -1;	}

	akp= (AfmKerningPair *)realloc( acm->acmKernPairs,
			( acm->acmKernPairCount+ 1 )* sizeof(AfmKerningPair) );
	if  ( ! akp )
	    { LXDEB(acm->acmKernPairCount,akp); return -1;	}
	acm->acmKernPairs= akp;

	akp += acm->acmKernPairCount++;
	akp->akpPosition= pos2;
	akp->akpXVec= (int)XVec;
	}

    LDEB(AFMlenLINE); return -1;
    }

static AfmKeyword	psAfmKernDataKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartKernPairs",	psAfmStartKernPairs,	},
	{ "StartTrackKern",	psAfmStartTrackKern,	},
	{ "EndKernData",	0,			},
    };

static int	psAfmStartKernData(	SimpleInputStream *	sisAfm,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    return psAfmConsumeLines( sisAfm, input, afi, psAfmKernDataKeywords,
		    sizeof(psAfmKernDataKeywords)/sizeof(AfmKeyword) );
    }

static int	psAfmStartComposites(	SimpleInputStream *		sisAfm,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, sisAfm ) )
	{
	if  ( ! strncmp( input, "EndComposites", 13 ) )
	    { break;					}
	}

    return 0;
    }

static int	psAfmStartTrackKern(	SimpleInputStream *		sisAfm,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, sisAfm ) )
	{
	if  ( ! strncmp( input, "EndTrackKern", 13 ) )
	    { break;					}
	}

    return 0;
    }

