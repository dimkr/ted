/************************************************************************/
/*									*/
/*  Paper sizes.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"appPaper.h"
#   include	"appGeoString.h"
#   include	<appDebugon.h>

typedef struct PaperSize
    {
    char *	psMnemonic;
    char *	psLabel;
    int		psWideTwips;
    int		psHighTwips;
    } PaperSize;

static PaperSize UtilPaperSizes[]=
{
    { "a4",		"A4: 210 x 297 mm",		11909,	16834,	},
    { "a5",		"A5: 149 x 210 mm",		8417,	11909,	},
    { "a6",		"A6: 105 x 149 mm",		5954,	8417,	},
    { "letter",		"Letter: 8 1/2 x 11 \"",	12240,	15840,	},
    { "legal",		"Legal: 8 1/2 x 14 \"",		12240,	20720,	},
    { "executive",	"Executive: 7 1/4 x 10 1/2 \"",	10440,	15120,	},
#   if 0
    { "a3",		"A3: 297 x 420 mm",		16834,	23818,	},
#   endif
};

/************************************************************************/
/*									*/
/*  Get information on a certain paper size.				*/
/*									*/
/************************************************************************/

int appPaperGetInformation(	int		n,
				int *		pWidth,
				int *		pHeight,
				const char **	pLabel )
    {
    PaperSize *	ps;

    if  ( n < 0								||
	  n >= (int)( sizeof(UtilPaperSizes)/sizeof(PaperSize) )	)
	{ return -1;	}

    ps= UtilPaperSizes+ n;

    if  ( pWidth )
	{ *pWidth= ps->psWideTwips;	}
    if  ( pHeight )
	{ *pHeight= ps->psHighTwips;	}
    if  ( pLabel )
	{ *pLabel= ps->psLabel;		}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to find a paper size by its size.				*/
/*									*/
/************************************************************************/

int appPaperGetBySize(		int		width,
				int		height )
    {
    unsigned	i;

    int		gw= width/200;
    int		gh= height/200;

    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	int		dw= UtilPaperSizes[i].psWideTwips- width;
	int		dh= UtilPaperSizes[i].psHighTwips- height;

	if  ( dw < gw && dw > -gw && dh < gh && dh > -gh )
	    { return i; }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Try to find a paper size by its mnemonic.				*/
/*									*/
/************************************************************************/

int appPaperGetByMnemonic(	const char *	mnemonic )
    {
    unsigned		i;

    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	if  ( ! strcmp( UtilPaperSizes[i].psMnemonic, mnemonic ) )
	    { return i; }
	}

    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	if  ( tolower( mnemonic[0] ) ==
			tolower( UtilPaperSizes[i].psMnemonic[0] )	&&
	      ! strcmp( UtilPaperSizes[i].psMnemonic+ 1, mnemonic+ 1 )	)
	    { return i; }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Try to translate a string to a paper size.				*/
/*									*/
/************************************************************************/

int appPaperFormatFromString(	int *		pFormat,
				int *		pWidth,
				int *		pHeight,
				int		unitType,
				const char *	paperString )
    {
    int		paperFormat;
    int		width;
    int		height;

    paperFormat= appPaperGetByMnemonic( paperString );
    if  ( paperFormat >= 0 )
	{
	if  ( appPaperGetInformation( paperFormat,
					&width, &height, (const char **)0 ) )
	    { SDEB(paperString); return -1;	}
	}
    else{
	if  ( appGeoRectangleFromString( paperString, unitType,
							    &width, &height ) )
	    { SDEB(paperString); return -1;	}
	}

    *pWidth= width;
    *pHeight= height;
    *pFormat= paperFormat;

    return 0;
    }
