/************************************************************************/
/*									*/
/*  Procedures to read an ispell affix file and dictionary files and	*/
/*  to build a finite automaton that recognises the same words.		*/
/*									*/
/*  Treatment of ispell affix files.					*/
/*									*/
/*  This source code is not meant as a replacement for the ispell	*/
/*  material, it is just to make things fit in with what I want to do	*/
/*  with it better.							*/
/*									*/
/*  Syntax is simplified. (No line breaks allowed in rule.)		*/
/*									*/
/************************************************************************/

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>
#   include	<charnames.h>
#   include	<appDebugon.h>
#   include	"indAffix.h"

/************************************************************************/
/* Applicability mask; macro's to use it.				*/
/************************************************************************/
typedef struct ApplicabilityMask
    {
    unsigned char	amBits[32];	/*  (256+7)/8	*/
    } ApplicabilityMask;

# define	amEmptyMask(a)	memset( (a)->amBits, 0x00, sizeof(a)->amBits )
# define	amFillMask(a)	memset( (a)->amBits, 0xff, sizeof(a)->amBits )
# define	amSet(a,v)	(a)->amBits[(v)/8] |= 1<<((v)%8)
# define	amClear(a,v)	(a)->amBits[(v)/8] &= ~(1<<((v)%8))
# define	amIsSet(a,v)	(((a)->amBits[(v)/8]&(1<<((v)%8)))!=0)

/************************************************************************/
/*  An affix rule, For simplicity a fixed maximum size for the strings	*/
/*  is assumed.								*/
/************************************************************************/
# define	AFFfrMAXLENGTH	8
# define	AFFtoMAXLENGTH	14

typedef struct AffixRule
    {
    int			arWhatSide;
    int			arShift;
    int			arCross;
    int			arMaskLength;
    int			arFromLength;
    int			arToLength;
    struct AffixRule *	arNext;
    ApplicabilityMask	arMasks[AFFfrMAXLENGTH];
    unsigned char	arToReplace[AFFfrMAXLENGTH];
    unsigned char	arReplacement[AFFtoMAXLENGTH];
    } AffixRule;

# define	AFFstrchrMAXLENGTH	3

typedef struct StringChar
    {
    unsigned char	scString[AFFstrchrMAXLENGTH+1];
    int			scLength;
    unsigned char	scMeaning;
    } StringChar;

typedef struct AffixRules
    {
    StringChar *	arStringChars;
    int			arStringCharCount;
    ApplicabilityMask	arWordChars;
    ApplicabilityMask	arBoundaryChars;
    AffixRule *		arsRules[256];

    unsigned char	arsCharKinds[256];
    unsigned char	arsCharShifts[256];
    } AffixRules;

# define BUL 300

/************************************************************************/
/*									*/
/*  Bookkeeping								*/
/*									*/
/************************************************************************/

static void affixInitRules(	AffixRules *	ars )
    {
    int		i;

    ars->arStringChars= (StringChar *)0;
    ars->arStringCharCount= 0;

    amEmptyMask( &(ars->arWordChars) );
    amEmptyMask( &(ars->arBoundaryChars) );

    for ( i= 0; i < 256; i++ )
	{ ars->arsRules[i]= (AffixRule *)0;	}

    return;
    }

static void affixFreeRules(	AffixRules *	ars )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	AffixRule *	r= ars->arsRules[i];

	while( r )
	    {
	    AffixRule *	n= r->arNext;

	    free( r ); r= n;
	    }
	}

    if  ( ars->arStringChars )
	{ free( ars->arStringChars );	}

    free( ars );

    return;
    }

void indAffixFreeRules(	void * voidars )
    {
    affixFreeRules( (AffixRules *)voidars );
    return;
    }

/************************************************************************/
/*									*/
/*  Read one affix character.						*/
/*									*/
/************************************************************************/

static int affixChar(	const AffixRules *	ars,
			const unsigned char *	buf,
			int *			pPos )
    {
    int		pos= *pPos;
    int		i;

    for ( i= 0; i < ars->arStringCharCount; i++ )
	{
	if  ( ! strncmp( buf+ pos, ars->arStringChars[i].scString,
					    ars->arStringChars[i].scLength ) )
	    {
	    *pPos += ars->arStringChars[i].scLength;
	    return ars->arStringChars[i].scMeaning;
	    }
	}

    if  ( amIsSet( &ars->arWordChars, buf[pos] ) )
	{
	(*pPos)++;

	if  ( ars->arsCharKinds[buf[pos]] & CHARisUPPER )
	    { return ars->arsCharShifts[buf[pos]];	}
	else{ return buf[pos];			}
	}

    if  ( amIsSet( &ars->arBoundaryChars, buf[pos] ) )
	{ (*pPos)++; return buf[pos]; }

    (*pPos)++; return buf[pos];
    }

int indAffixChar(	const void *		voidars,
			const unsigned char *	buf,
			int *			pPos )
    {
    return affixChar( (const AffixRules *)voidars, buf, pPos );
    }

/************************************************************************/
/*									*/
/*  Read one affix rule.						*/
/*									*/
/************************************************************************/

static AffixRule * affixNewRule(	AffixRules *		ars,
					const char *		file,
					int			line,
					int			type,
					int			shift,
					int			cross,
					const unsigned char *	buf,
					AffixRule *		next,
					const unsigned char *	charKinds,
					const unsigned char *	charShifts )
    {
    int		bufPos= 0;
    int		found;

    AffixRule *	rval= (AffixRule *)0;

    if  ( ! type )
	{ LDEB(type); goto failure;	}

    rval= (AffixRule *)malloc( sizeof(AffixRule) );
    if  ( ! rval )
	{ LDEB(rval); goto failure;	}
    rval->arNext= next;
    rval->arWhatSide= type;
    rval->arShift= shift;
    rval->arCross= cross;

    if  ( buf[bufPos] == '>' )
	{ SLSSDEB(file,line,(char *)buf,(char *)buf+bufPos);	}

    rval->arMaskLength= 0;
    while( buf[bufPos] != '>' )
	{
	if  ( rval->arMaskLength >= AFFfrMAXLENGTH )
	    {
	    LLDEB(rval->arMaskLength,AFFfrMAXLENGTH);
	    goto failure;
	    }

	if  ( buf[bufPos] == '[' )
	    {
	    if  ( buf[bufPos+1] == '^' )
		{
		amFillMask( rval->arMasks+ rval->arMaskLength );
		bufPos += 2;
		while( buf[bufPos] != ']' )
		    {
		    found= affixChar( ars, buf, &bufPos );
		    if  ( found < 0 )
			{
			SLDEB((char *)buf,found);
			SDEB((char *)buf+bufPos);
			goto failure;
			}
		    amClear( rval->arMasks+ rval->arMaskLength, found );
		    }
		bufPos++; /* skip ']' */
		}
	    else{
		if  ( ! buf[bufPos+1] )
		    { SDEB((char *)buf); goto failure; }

		amEmptyMask( rval->arMasks+ rval->arMaskLength );
		bufPos++;

		while( buf[bufPos] != ']' )
		    {
		    found= affixChar( ars, buf, &bufPos );
		    if  ( found < 0 )
			{ SLDEB((char *)buf,found); goto failure; }

		    amSet( rval->arMasks+ rval->arMaskLength, found );
		    }
		bufPos++; /* skip ']' */
		}
	    }
	else{
	    if  ( ! buf[bufPos] )
		{ SDEB((char *)buf); goto failure; }

	    if  ( buf[bufPos] == '.' )
		{
		amFillMask( rval->arMasks+ rval->arMaskLength );
		bufPos++;
		}
	    else{
		amEmptyMask( rval->arMasks+ rval->arMaskLength );
		found= affixChar( ars, buf, &bufPos );
		if  ( found < 0 )
		    {
		    SDEB((char *)buf+bufPos);
		    SLDEB((char *)buf,found);
		    goto failure;
		    }

		amSet( rval->arMasks+ rval->arMaskLength, found );
		}
	    }

	rval->arMaskLength++;

	while( isspace(buf[bufPos]) )
	    { bufPos++;	}
	}

    if  ( buf[bufPos] != '>' )
	{
	SDEB((char *)buf);
	SDEB((char *)buf+bufPos);
	goto failure;
	}

    bufPos++; /* skip '>' */

    while( isspace(buf[bufPos]) )
	{ bufPos++;	}

    if  ( ! buf[bufPos] )
	{ SLDEB((char *)buf,bufPos); goto failure; }

    if  ( buf[bufPos] == '-' )
	{
	int	tryQuote;
	int	quoted= 0;

	bufPos++;
	rval->arFromLength= 0;

	tryQuote= bufPos;
	found= affixChar( ars, buf, &tryQuote );
	if  ( found == -1 && buf[bufPos] == '"' )
	    { quoted= 1; bufPos++;	}

	for (;;)
	    {
	    if  ( rval->arFromLength >= AFFfrMAXLENGTH )
		{
		LLDEB(rval->arFromLength,AFFfrMAXLENGTH);
		goto failure;
		}

	    found= affixChar( ars, buf, &bufPos );
	    if  ( found < 0 )
		{
		if  ( isascii( buf[bufPos] ) && ! isalpha( buf[bufPos] ) )
		    {
		    if  ( ! quoted		||
			  buf[bufPos] != '"'	||
			  buf[bufPos+1] != ','	)
			{ SLDEB((char *)buf,found); goto failure; }
		    }

		found= buf[bufPos++];
		}

	    rval->arToReplace[rval->arFromLength++]= found;

	    if  ( buf[bufPos] == ',' )
		{
		bufPos++;
		while( isspace(buf[bufPos]) )
		    { bufPos++;	}
		break;
		}
	    }
	}
    else{ rval->arFromLength= 0; }

    if  ( rval->arFromLength > 0 )
	{
	int	d= 0;
	int	i;


	if  ( rval->arWhatSide == 0 )
	    { LDEB(rval->arWhatSide);	}

	if  ( rval->arWhatSide > 0 )
	    {
	    d= rval->arMaskLength- rval->arFromLength;
	    if  ( d < 0 )
		{
		for ( i= rval->arFromLength- 1; i >= rval->arMaskLength; i-- )
		    { rval->arMasks[i]= rval->arMasks[i+ d]; }

		for ( i= 0; i < -d; i++ )
		    { amFillMask( &(rval->arMasks[i]) ); }

		d= 0;
		}
	    }

	if  ( rval->arWhatSide < 0 )
	    {
	    while( rval->arMaskLength < rval->arFromLength )
		{
		amFillMask( &(rval->arMasks[rval->arMaskLength]) );
		rval->arMaskLength++;
		}

	    d= 0;
	    }

	for ( i= 0; i < rval->arFromLength; i++ )
	    {
	    amEmptyMask( &(rval->arMasks[i+ d]) );
	    amSet( &(rval->arMasks[i+ d]), rval->arToReplace[i] );
	    }
	}

    if  ( buf[bufPos] == '-' )
	{
	bufPos++;
	rval->arToLength= 0;
	}
    else{
	int	tryQuote;
	int	quoted= 0;

	tryQuote= bufPos;
	found= affixChar( ars, buf, &tryQuote );
	if  ( found == -1 && buf[bufPos] == '"' )
	    { quoted= 1; bufPos++;	}

	rval->arToLength= 0;
	while( buf[bufPos] && ! isspace( buf[bufPos] ) )
	    {
	    if  ( rval->arToLength >= AFFtoMAXLENGTH )
		{
		LLDEB(rval->arToLength,AFFtoMAXLENGTH);
		goto failure;
		}

	    found= affixChar( ars, buf, &bufPos );
	    if  ( found < 0 )
		{
		if  ( isascii( buf[bufPos] ) && ! isalpha( buf[bufPos] ) )
		    {
		    if  ( ! quoted					||
			  buf[bufPos] != '"'				||
			  ( buf[bufPos+1] && ! isspace( buf[bufPos+1] ) ) )
			{
			SDEB((char *)buf+bufPos);
			SLDEB((char *)buf,found);
			goto failure;
			}
		    }

		found= buf[bufPos++];
		}

	    rval->arReplacement[rval->arToLength++]= found;
	    }
	}

    while( isspace(buf[bufPos]) )
	{ bufPos++;	}

    if  ( buf[bufPos] )
	{ SSDEB((char *)buf,(char *)buf+bufPos); goto failure; }

    /*
    fprintf( stderr, "MaskLength=%d, arFromLength= %d, rval->arToLength=%d\n",
	    rval->arMaskLength, rval->arFromLength, rval->arToLength );
    */

    return rval;

  failure:

    if  ( rval )
	{ free( rval );	}

    return (AffixRule *)0;
    }

/************************************************************************/
/*									*/
/*  Add a string char to the set of string chars.			*/
/*									*/
/************************************************************************/

#   define	AFFscSET(f,l,s2,m) \
		strcpy( f->scString, s2 ); \
		f->scLength= l; \
		f->scMeaning= m; \

static int affixRememberStringChar(	AffixRules *		ars,
					const unsigned char *	s,
					int			l )
    {
    StringChar *	sc;

    sc= realloc( ars->arStringChars, (ars->arStringCharCount+ 1)* sizeof( StringChar ) );
    if  ( ! sc )
	{ LXDEB(ars->arStringCharCount,sc); return -1;	}

    ars->arStringChars= sc;
    sc += ars->arStringCharCount;

    if  ( l == 1 )
	{
	unsigned char scratch[2];
	scratch[0]= *s; scratch[1]= '\0';
	AFFscSET( sc, l, scratch, s[0] ); ars->arStringCharCount++; return 0;
	}

    /*  "  */
    if  ( l == 3 && ! strncmp( s, "a\\\"", l ) )
	{ AFFscSET( sc, l- 1, "a\"", ISO1_adieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "A\\\"", l ) )
	{ AFFscSET( sc, l- 1, "A\"", ISO1_Adieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "e\\\"", l ) )
	{ AFFscSET( sc, l- 1, "e\"", ISO1_edieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "E\\\"", l ) )
	{ AFFscSET( sc, l- 1, "E\"", ISO1_Edieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "i\\\"", l ) )
	{ AFFscSET( sc, l- 1, "i\"", ISO1_idieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "I\\\"", l ) )
	{ AFFscSET( sc, l- 1, "I\"", ISO1_Idieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "o\\\"", l ) )
	{ AFFscSET( sc, l- 1, "o\"", ISO1_odieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "O\\\"", l ) )
	{ AFFscSET( sc, l- 1, "O\"", ISO1_Odieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "u\\\"", l ) )
	{ AFFscSET( sc, l- 1, "u\"", ISO1_udieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "U\\\"", l ) )
	{ AFFscSET( sc, l- 1, "U\"", ISO1_Udieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "y\\\"", l ) )
	{ AFFscSET( sc, l- 1, "y\"", ISO1_ydieresis ); ars->arStringCharCount++; return 0; }

    /*  '  */
    if  ( l == 3 && ! strncmp( s, "a\\'", l ) )
	{ AFFscSET( sc, l, "a'", ISO1_aacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "A\\'", l ) )
	{ AFFscSET( sc, l, "A'", ISO1_Aacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "e\\'", l ) )
	{ AFFscSET( sc, l, "e'", ISO1_eacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "E\\'", l ) )
	{ AFFscSET( sc, l, "E'", ISO1_Eacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "i\\'", l ) )
	{ AFFscSET( sc, l, "i'", ISO1_iacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "I\\'", l ) )
	{ AFFscSET( sc, l, "I'", ISO1_Iacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "o\\'", l ) )
	{ AFFscSET( sc, l, "o'", ISO1_oacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "O\\'", l ) )
	{ AFFscSET( sc, l, "O'", ISO1_Oacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "u\\'", l ) )
	{ AFFscSET( sc, l, "u'", ISO1_uacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "U\\'", l ) )
	{ AFFscSET( sc, l, "U'", ISO1_Uacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "y\\'", l ) )
	{ AFFscSET( sc, l, "y'", ISO1_yacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "Y\\'", l ) )
	{ AFFscSET( sc, l, "Y'", ISO1_Yacute ); ars->arStringCharCount++; return 0; }

    /*  `  */
    if  ( l == 3 && ! strncmp( s, "a\\`", l ) )
	{ AFFscSET( sc, l, "a`", ISO1_agrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "A\\`", l ) )
	{ AFFscSET( sc, l, "A`", ISO1_Agrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "e\\`", l ) )
	{ AFFscSET( sc, l, "e`", ISO1_egrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "E\\`", l ) )
	{ AFFscSET( sc, l, "E`", ISO1_Egrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "i\\`", l ) )
	{ AFFscSET( sc, l, "i`", ISO1_igrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "I\\`", l ) )
	{ AFFscSET( sc, l, "I`", ISO1_Igrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "o\\`", l ) )
	{ AFFscSET( sc, l, "o`", ISO1_ograve ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "O\\`", l ) )
	{ AFFscSET( sc, l, "O`", ISO1_Ograve ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "u\\`", l ) )
	{ AFFscSET( sc, l, "u`", ISO1_ugrave ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "U\\`", l ) )
	{ AFFscSET( sc, l, "U`", ISO1_Ugrave ); ars->arStringCharCount++; return 0; }

    /*  ^  */
    if  ( l == 3 && ! strncmp( s, "a\\^", l ) )
	{ AFFscSET( sc, l, "a^", ISO1_acircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "A\\^", l ) )
	{ AFFscSET( sc, l, "A^", ISO1_Acircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "e\\^", l ) )
	{ AFFscSET( sc, l, "e^", ISO1_ecircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "E\\^", l ) )
	{ AFFscSET( sc, l, "E^", ISO1_Ecircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "i\\^", l ) )
	{ AFFscSET( sc, l, "i^", ISO1_icircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "I\\^", l ) )
	{ AFFscSET( sc, l, "I^", ISO1_Icircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "o\\^", l ) )
	{ AFFscSET( sc, l, "o^", ISO1_ocircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "O\\^", l ) )
	{ AFFscSET( sc, l, "O^", ISO1_Ocircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "u\\^", l ) )
	{ AFFscSET( sc, l, "u^", ISO1_ucircumflex ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "U\\^", l ) )
	{ AFFscSET( sc, l, "U^", ISO1_Ucircumflex ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "c,", l ) )
	{ AFFscSET( sc, l, "c,", ISO1_ccedilla ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "C,", l ) )
	{ AFFscSET( sc, l, "C,", ISO1_Ccedilla ); ars->arStringCharCount++; return 0; }

    if  ( l == 2 && ! strncmp( s, "SS", l ) )
	{ AFFscSET( sc, l, "SS", ISO1_germandbls ); ars->arStringCharCount++; return 0; }
    if  ( l == 2 && ! strncmp( s, "ss", l ) )
	{ AFFscSET( sc, l, "ss", ISO1_germandbls ); ars->arStringCharCount++; return 0; }
    if  ( l == 2 && ! strncmp( s, "sS", l ) )
	{ AFFscSET( sc, l, "sS", ISO1_germandbls ); ars->arStringCharCount++; return 0; }
    if  ( l == 2 && ! strncmp( s, "Ss", l ) )
	{ AFFscSET( sc, l, "Ss", ISO1_germandbls ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'a", l ) )
	{ AFFscSET( sc, l, "'a", ISO1_aacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'A", l ) )
	{ AFFscSET( sc, l, "'A", ISO1_Aacute ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'e", l ) )
	{ AFFscSET( sc, l, "'e", ISO1_eacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'E", l ) )
	{ AFFscSET( sc, l, "'E", ISO1_Eacute ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'i", l ) )
	{ AFFscSET( sc, l, "'i", ISO1_iacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'I", l ) )
	{ AFFscSET( sc, l, "'I", ISO1_Iacute ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'o", l ) )
	{ AFFscSET( sc, l, "'o", ISO1_oacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'O", l ) )
	{ AFFscSET( sc, l, "'O", ISO1_Oacute ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'u", l ) )
	{ AFFscSET( sc, l, "'u", ISO1_uacute ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'U", l ) )
	{ AFFscSET( sc, l, "'U", ISO1_Uacute ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\'n", l ) )
	{ AFFscSET( sc, l, "'n", ISO1_ntilde ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\'N", l ) )
	{ AFFscSET( sc, l, "'N", ISO1_Ntilde ); ars->arStringCharCount++; return 0; }

    if  ( l == 3 && ! strncmp( s, "\\\"u", l ) )
	{ AFFscSET( sc, l, "\"u", ISO1_udieresis ); ars->arStringCharCount++; return 0; }
    if  ( l == 3 && ! strncmp( s, "\\\"U", l ) )
	{ AFFscSET( sc, l, "\"U", ISO1_Udieresis ); ars->arStringCharCount++; return 0; }

    SLDEB((char *)s,l); return -1;
    }

/************************************************************************/
/*									*/
/*  Remember string chars.						*/
/*									*/
/************************************************************************/

static int	affixRememberStringChars(
				AffixRules *		ars,
				const unsigned char *	buf,
				int *			pPos )
    {
    int		pos= *pPos;

    while( buf[pos] == ' ' || buf[pos] == '\t' )
	{ pos++;	}

    while( buf[pos] )
	{
	int	before= pos;

	while( buf[pos] && buf[pos] != ' ' && buf[pos] != '\t' )
	    { pos++;	}

	if  ( affixRememberStringChar( ars, buf+ before, pos- before ) )
	    { SDEB((char *)buf); return -1;	}

	while( buf[pos] == ' ' || buf[pos] == '\t' )
	    { pos++;	}
	}

    *pPos= pos; return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a character set.						*/
/*									*/
/************************************************************************/

static int	affixGetChars(	ApplicabilityMask *	am,
				const unsigned char *	buf,
				int *			pPos	)
    {
    int		prev= -1;
    int		pos= *pPos;

    while( buf[pos] == ' ' || buf[pos] == '\t' )
	{ pos++;	}

    while( buf[pos] )
	{
	switch( buf[pos] )
	    {
	    int			cc;

	    case '"':
		pos++;
		amSet( am, buf[pos] ); pos++;
		if  ( buf[pos] != '"' )
		    { SSDEB((char *)buf,(char *)buf+pos); return -1;	}
		pos++; break;

	    case '[':
		pos++;
		while( buf[pos] != ']' )
		    {
		    if  ( ! buf[pos] )
			{ SLDEB((char *)buf,pos); return -1;	}

		    if  ( prev >= 0 && buf[pos] == '-' )
			{
			if  ( buf[pos+1] == '\\' && buf[pos+2] == 'x' )
			    {
			    sscanf( (const char *)buf+ pos+ 3, "%2x", &cc );

			    while( prev < cc )
				{ amSet( am, prev ); prev++; }

			    amSet( am, cc ); prev= -1; pos += 5;
			    continue;
			    }
			else{
			    while( prev < buf[pos+1] )
				{ amSet( am, prev ); prev++; }

			    amSet( am, buf[pos+1] ); prev= -1; pos += 2;
			    continue;
			    }
			}

		    if  ( buf[pos] == '\\' && buf[pos+1] == 'x' )
			{
			sscanf( (const char *)buf+ pos+ 2, "%2x", &cc );
			amSet( am, cc ); prev= cc; pos += 4;
			continue;
			}

		    if  ( buf[pos] == '\\' && buf[pos+1] == '-' )
			{ pos++; }

		    amSet( am, buf[pos] ); prev= buf[pos]; pos++;
		    continue;
		    }
		prev= -1; pos++; break;
	    case '\\':
		if  ( buf[pos] == '\\' && buf[pos+1] == 'x' )
		    {
		    sscanf( (const char *)buf+ pos+ 2, "%2x", &cc );
		    amSet( am, cc ); prev= cc; pos += 4;
		    break;
		    }
		amSet( am, buf[pos] );
		pos++; break;

	    default:
		amSet( am, buf[pos] );
		pos++; break;
	    }

	if  ( buf[pos] && buf[pos] != ' ' && buf[pos] != '\t' )
	    { SSDEB((char *)buf,(char *)buf+pos); return -1;	}

	while( buf[pos] == ' ' || buf[pos] == '\t' )
	    { pos++;	}
	}

    *pPos= pos; return 0;
    }

/************************************************************************/
/*									*/
/*  Read affix file.							*/
/*									*/
/************************************************************************/

void * indAffixReadFile(	const char *		filename,
				const unsigned char *	charKinds,
				const unsigned char *	charShifts )
    {
    FILE *		f= fopen( filename, "r" );
    int			pos;
    int			words;
    int			affixtype= 0;
    int			shift= 0;
    int			cross= 0;
    int			compound= 0;
    int			currentFlag= -1;
    int			l;
    int			line= 0;

    unsigned char	buf[BUL+1];
    unsigned char	word1[200+1];
    unsigned char	word2[200+1];

    AffixRule *	newRule;

    AffixRules *	ars= (AffixRules *)0;

    if  ( ! f )
	{ SXDEB(filename,f); goto failure;	}

    ars= malloc( sizeof(AffixRules) );
    if  ( ! ars )
	{ XDEB(ars); goto failure;	}

    affixInitRules( ars );

    memcpy( ars->arsCharKinds, charKinds, 256 );
    memcpy( ars->arsCharShifts, charShifts, 256 );

    while( fgets( (char *)buf, BUL, f ) )
	{
	line++;

	if  ( ! strcmp( buf, "#up\n" ) )
	    { shift=  1; continue;	}
	if  ( ! strcmp( buf, "#down\n" ) )
	    { shift= -1; continue;	}

	l= 0;
	while( l < BUL && buf[l] != '#' && buf[l] != '\n' )
	    { l++;	}
	buf[l]= '\0';

	pos= 0;
	while( buf[pos] == ' ' || buf[pos] == '\t' )
	    { pos++;	}

	if  ( ! buf[pos] )
	    { continue;	}

	words= sscanf( (char *)buf, "%s%s", word1, word2 );

	if  ( words > 0 )
	    {
	    if  ( ! strcmp( word1, "flagmarker" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "defstringtype" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "compoundwords" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "allaffixes" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "altstringtype" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "altstringchar" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "texchars" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "nroffchars" )	)
		{ continue;	}
	    if  ( ! strcmp( word1, "compoundmin" )	)
		{ continue;	}

	    if  ( ! strcmp( word1, "stringchar" )	)
		{
		l= 10;
		if  ( affixRememberStringChars( ars, buf, &l ) )
		    { SDEB((char *)buf); goto failure;	}
		continue;
		}

	    if  ( ! strcmp( word1, "boundarychars" )	)
		{
		l= 13;
		if  ( affixGetChars( &ars->arBoundaryChars, buf, &l ) )
		    { SDEB((char *)buf); goto failure;	}
		continue;
		}

	    if  ( ! strcmp( word1, "wordchars" )	)
		{
		l= 9;
		if  ( affixGetChars( &ars->arWordChars, buf, &l ) )
		    { SDEB((char *)buf); goto failure;	}
		continue;
		}

	    if  ( ! strcmp( word1, "prefixes" )	)
		{ affixtype= -1; continue;	}
	    if  ( ! strcmp( word1, "suffixes" )	)
		{ affixtype=  1; continue;	}

	    if  ( ! strcmp( word1, "flag" )	)
		{
		shift= 0; cross= 0; compound= 0;

		if  ( words > 1 )
		    {
		    unsigned char *	w2= word2;

		    if  ( w2[0] == '\\' )
			{ w2++;	}

		    if  ( w2[0] == '*' )
			{
			if  ( w2[1] == '\\' )
			    { w2++;	}

			if  ( strlen( (char *)w2 ) == 3 && w2[2] == ':' )
			    { cross= 1; currentFlag= w2[1]; continue; }
			}
		    else{
			if  ( strlen( (char *)w2 ) == 2 && w2[1] == ':' )
			    { cross= 0; currentFlag= w2[0]; continue; }
			}

		    if  ( w2[0] == '~' )
			{
			if  ( w2[1] == '\\' )
			    { w2++;	}

			if  ( strlen( (char *)w2 ) == 3 && w2[2] == ':' )
			    { compound= 1; currentFlag= w2[1]; continue; }
			}
		    else{
			if  ( strlen( (char *)w2 ) == 2 && w2[1] == ':' )
			    { compound= 0; currentFlag= w2[0]; continue; }
			}
		    }
		}
	    }

	while( ! strchr( buf, '>' ) && l < BUL )
	    {
	    if  ( ! fgets( (char *)buf+ l, BUL- l, f ) )
		{ SLDEB(filename,line); goto failure;	}

	    line++;

	    while( l < BUL && buf[l] != '#' && buf[l] != '\n' )
		{ l++;	}

	    if  ( buf[l] == '#' )
		{ buf[l]= '\0'; break; }
	    buf[l]= '\0';
	    }

	if  ( ! compound )
	    {
	    newRule= affixNewRule( ars, filename, line, affixtype, shift, cross,
				    buf+ pos, ars->arsRules[currentFlag],
				    charKinds, charShifts );
	    if  ( ! newRule )
		{ SLDEB(filename,line); SDEB((char *)buf); goto failure; }

	    ars->arsRules[currentFlag]= newRule;
	    }
	}

    fclose( f ); return ars;

  failure:

    if  ( f )
	{ fclose( f );	}
    if  ( ars )
	{ affixFreeRules( ars );	}

    return (void *)0;
    }

static int affixApplySuffixRule(	const AffixRules *	ars,
					unsigned char *		scratch,
					const unsigned char *	word,
					int			len,
					AffixRule *		ar,
					void *			ind,
					PUT_FUN			fun )
    {
    const unsigned char *	tail;
    unsigned char *		target;

    int				j;

    tail=word+ ( len- ar->arMaskLength );

    for ( j= 0; j < ar->arMaskLength; j++ )
	{
	if  ( ! amIsSet( &(ar->arMasks[j]), tail[j] ) )
	    { break;	}
	}

    if  ( j < ar->arMaskLength )
	{ return 0;	}

    target= scratch+ len- ar->arFromLength;
    memcpy( scratch, word, len- ar->arFromLength );
    memcpy( target, ar->arReplacement, ar->arToLength );
    target[ar->arToLength]= '\0';

    if  ( ar->arShift > 0 && ( ars->arsCharKinds[scratch[0]] & CHARisLOWER ) )
	{ scratch[0]= ars->arsCharShifts[scratch[0]]; }
    if  ( ar->arShift < 0 && ( ars->arsCharKinds[scratch[0]] & CHARisUPPER ) )
	{ scratch[0]= ars->arsCharShifts[scratch[0]]; }

    /* printf("%c: %s\n", rules[i], scratch ); */

    if  ( (*fun)( ind, scratch ) < 0 )
	{ SDEB((char *)scratch); return -1;	}

    return 1;
    }

static int affixApplyPrefixRule(	const AffixRules *	ars,
					unsigned char *		scratch,
					const unsigned char *	word,
					int			len,
					AffixRule *		ar,
					void *			ind,
					PUT_FUN			fun )
    {
    const unsigned char *	tail;
    unsigned char *		target;

    int				j;

    for ( j= 0; j < ar->arMaskLength; j++ )
	{
	if  ( ! amIsSet( &(ar->arMasks[j]), word[j] ) )
	    { break;	}
	}

    if  ( j <= ar->arMaskLength )
	{ return 0;	}

    target= scratch+ ar->arMaskLength;
    tail= word+ ar->arFromLength;
    memcpy( scratch, ar->arReplacement, ar->arToLength );
    memcpy( target, tail, len- ar->arFromLength );
    target[len- ar->arFromLength]= '\0';

    if  ( ar->arShift > 0 && ( ars->arsCharKinds[scratch[0]] & CHARisLOWER ) )
	{ scratch[0]= ars->arsCharShifts[scratch[0]]; }
    if  ( ar->arShift < 0 && ( ars->arsCharKinds[scratch[0]] & CHARisUPPER ) )
	{ scratch[0]= ars->arsCharShifts[scratch[0]]; }

    /* printf("%c: %s\n", rules[i], scratch ); */

    if  ( (*fun)( ind, scratch ) < 0 )
	{ SDEB((char *)scratch); return -1;	}

    return 1;
    }

static int affixApplySidedRules(	const AffixRules *	ars,
					const unsigned char *	word,
					int			len,
					const unsigned char *	rules,
					int			side,
					void *			ind,
					PUT_FUN			fun )
    {
    AffixRule *			ar;
    int				i;

    unsigned char		scratch[200+1];

    /*
    fprintf( stderr, "Apply rules: '%s'/'%s'\n", word, rules );
    */

    i= 0;
    while( rules[i] )
	{
	ar= ars->arsRules[rules[i]];

	while( ar )
	    {
	    if  ( ar->arMaskLength <= len )
		{
		if  ( side > 0 && ar->arWhatSide > 0 )
		    {
		    /*
		    printf("TRY %c: '%*.*s|%s' maskLen=%d\n",
			    rules[i],
			    len- ar->arMaskLength, len- ar->arMaskLength,
			    word,
			    word+ len- ar->arMaskLength,
			    ar->arMaskLength );
		    */

		    if  ( affixApplySuffixRule( ars, scratch, word, len,
							ar, ind, fun ) < 0 )
			{ SDEB((char *)word); return -1;	}
		    }

		if  ( side < 0 && ar->arWhatSide < 0 )
		    {
		    if  ( affixApplyPrefixRule( ars, scratch, word, len,
							ar, ind, fun ) < 0 )
			{ SDEB((char *)word); return -1;	}
		    }
		}

	    ar= ar->arNext;
	    }

	i++;
	}

    return 0;
    }

int indAffixApplyRules(	const void *		voidars,
			const unsigned char *	word,
			int			len,
			const unsigned char *	rules,
			void *			ind,
			PUT_FUN			fun )
    {
    const AffixRules *		ars= (AffixRules *)voidars;
    AffixRule *			ar;
    int				i;
    int				done;

    unsigned char		scratch[200+1];

    /*
    fprintf( stderr, "Apply rules: '%s'/'%s'\n", word, rules );
    */

    i= 0;
    while( rules[i] )
	{
	ar= ars->arsRules[rules[i]];

	while( ar )
	    {
	    if  ( ar->arMaskLength <= len )
		{
		if  ( ar->arWhatSide > 0 )
		    {
		    /*
		    printf("TRY %c: '%*.*s|%s' maskLen=%d\n",
			    rules[i],
			    len- ar->arMaskLength, len- ar->arMaskLength,
			    word,
			    word+ len- ar->arMaskLength,
			    ar->arMaskLength );
		    */

		    done= affixApplySuffixRule( ars, scratch, word, len,
							    ar, ind, fun );
		    if  ( done < 0 )
			{ SDEB((char *)word); return -1;	}

		    if  ( done == 1 && ar->arCross )
			{
			if  ( affixApplySidedRules( ars, scratch,
					strlen( (const char *)scratch ),
					rules, -ar->arWhatSide, ind, fun ) )
			    { SDEB((char *)scratch); return -1;	}
			}
		    }

		if  ( ar->arWhatSide < 0 )
		    {
		    done= affixApplyPrefixRule( ars, scratch, word, len,
							    ar, ind, fun );
		    if  ( done < 0 )
			{ SDEB((char *)word); return -1;	}

		    if  ( done == 1 && ar->arCross )
			{
			if  ( affixApplySidedRules( ars, scratch,
					strlen( (const char *)scratch ),
					rules, -ar->arWhatSide, ind, fun ) )
			    { SDEB((char *)scratch); return -1;	}
			}
		    }
		}

	    ar= ar->arNext;
	    }
	i++;
	}

    return 0;
    }

void indAffixListRules(	const void * voidars )
    {
    const AffixRules *		ars= (AffixRules *)voidars;
    AffixRule *			ar;
    int				rule;

    printf( "AFFIX RULES:\n" );

    for ( rule= 0; rule < 256; rule++ )
	{
	ar= ars->arsRules[rule];

	while( ar )
	    {
	    if  ( ar->arWhatSide > 0 )
		{ printf( "suffix flag " );	}
	    else{ printf( "prefix flag " );	}

	    if  ( ar->arCross )
		{ printf( "*" );	}

	    if  ( isprint( rule ) )
		{ printf( "%c: ", rule );	}
	    else{ printf( "%d: ", rule );	}

	    if  ( ar->arMaskLength > 0 )
		{
		}

	    printf( "\"%.*s\" -> \"%*s\"\n",
			ar->arFromLength, ar->arToReplace,
			ar->arToLength, ar->arReplacement );

	    ar= ar->arNext;
	    }
	}

    return;
    }
