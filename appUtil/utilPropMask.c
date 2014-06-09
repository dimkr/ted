/************************************************************************/
/*									*/
/*  Property mask operations.						*/
/*  Most is done by macros in the header file.				*/
/*									*/
/************************************************************************/

#   include	<utilPropMask.h>
#   include	<appDebugon.h>

void utilPropMaskFill(		PropertyMask *	pm,
				int		c )
    {
    int			prop;

    for ( prop= 0; prop < c; prop++ )
	{ PROPmaskADD( pm, prop ); }

    return;
    }

int utilPropMaskIsEmpty(	const PropertyMask *	pm )
    {
    int		byte;

    if  ( ! pm )
	{ XDEB(pm); return 1;	}

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{
	if  ( pm->pmBits[byte] )
	    { return 0;	}
	}

    return 1;
    }

void utilPropMaskOr(		PropertyMask *		pm0,
				const PropertyMask *	pm1,
				const PropertyMask *	pm2 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= pm1->pmBits[byte] | pm2->pmBits[byte]; }

    return;
    }

void utilPropMaskAnd(		PropertyMask *		pm0,
				const PropertyMask *	pm1,
				const PropertyMask *	pm2 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= pm1->pmBits[byte] & pm2->pmBits[byte]; }

    return;
    }

void utilPropMaskNot(		PropertyMask *		pm0,
				const PropertyMask *	pm1 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= ~pm1->pmBits[byte];	}

    return;
    }

