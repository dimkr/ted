#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<appDebugoff.h>

static void	indTLDivide( IND *, int, int );
static void indTLClaim( IND *, int, int, int );

/************************************************************************/
/*  The series of links is divided into groups.				*/
/*  Every group is preceded by a group header.				*/
/*  If the group is used, the tl_to field of the header is TLgUSED	*/
/*  If the group is free, the tl_to field of the header is TLgFREE	*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Walk over the allocation chain of links and do some half hearted	*/
/*  checking.								*/
/*									*/
/************************************************************************/

int	indTLwalk( IND *	ind )
    {
    int	pos= 0;
    TrieLink *	link;

    while( pos < ind->indAllocatedLinks )
	{
	link=	LINK(ind,pos);
	if  ( link->tl_to != TLgUSED && link->tl_to != TLgFREE )
	    {
	    appDebug( "AT %d: CORRUPTED CHAIN\n", pos );
	    indTLprint( ind, pos );
	    return -1;
	    }
	pos += link->tl_key+ 1;
	}

    return 0;
    }

static void indTLDivide( ind, from, upto )
IND *	ind;
int	from;
int	upto;
    {
    TrieLink *	link;

    if  ( from < ind->ind_lfull )
	{ ind->ind_lfull= from; }

    while( from < upto )
	{
	int	space= upto- from- 1;

	link= LINK(ind,from);

	if  ( space > 255 )
	    { space= 255;	}

	link->tl_to= TLgFREE;
	link->tl_key= space;
	APP_DEB(appDebug("       FREE LINKS AT %6d, COUNT= %3d UPTO= %6d\n",
		from, space, upto ));
	from += space+ 1;
	}
    }

static void indTLClaim( ind, old, at, n )
IND *	ind;
int	old;
int	at;
int	n;
    {
    TrieLink *	atlink= LINK(ind,at);

    atlink->tl_key= n;
    atlink->tl_to= TLgUSED;

    APP_DEB(appDebug("++++   USE  LINKS AT %6d, COUNT= %3d\n", at, n ));

    if  ( old > 0 )
	{
	TrieLink *	ollink= LINK(ind,old-1);

	if  ( old- 1 < ind->ind_lfull )
	    { ind->ind_lfull= old- 1; }

	n= ollink->tl_key;
	APP_DEB(appDebug("OLD    FREE LINKS AT %6d, COUNT= %3d\n", old-1, n ));
	ollink->tl_to= TLgFREE;
	at++;
	while( n > 0 )
	    {
	    atlink= LINK(ind,at);
	    ollink= LINK(ind,old);
	    *atlink= *ollink;
	    at++; old++; n--;
	    }
	}
    }

int	indTLalloc( ind, old, n )
IND *	ind;
int	old;
int	n;
    {
    int		past;
    int		space;
    int		lfree;

    int		newn;
    unsigned	sz;
    TrieLink **	new;
    TrieLink *	newpage;

    TrieLink *	link;

    APP_DEB(appDebug( "INDTLALLOC(,old=%d,n=%d)\n", old, n ));
    if  ( old == 0 )
	{ return -1;	}

    if  ( n < 1 || n >= 255 )
	{ appDebug("n=%d\n", n ); return -1;	}

    /********************************************************************/
    /*  Fits in old position?						*/
    /********************************************************************/
    if  ( old > 0 )
	{
	lfree= old- 1;

	link= LINK(ind,lfree);
	if  ( link->tl_to != TLgUSED )
	    { return -1;	}
	if  ( ( space= link->tl_key ) >= n )
	    { return old;	}

	space= link->tl_key; past= lfree+ space;
	while( past < ind->indAllocatedLinks			&&
	       space < n				)
	    {
	    link= LINK(ind,past);
	    if  ( link->tl_key != TLgFREE )
		{ break;	}
	    space += link->tl_key+ 1; past  += link->tl_key+ 1;
	    }

	if  ( space >= n )
	    {
	    APP_DEB(appDebug("BLOWUP FROM %d to %d\n", link->tl_key, n ));
	    indTLClaim( ind, -1, old, n );
	    indTLDivide( ind, old+ n+ 1, past );
	    return old;
	    }
	}

    /********************************************************************/
    /*  Look for an other position, it can be fitted into.		*/
    /********************************************************************/
    if  ( ind->indAllocatedLinks > 0 )
	{
	int	reachedEnd= 0;
	int	onlyUsed= 0;

	lfree= ind->ind_lfree;

	while( ! reachedEnd || lfree < ind->ind_lfree )
	    {
	    if  ( lfree >= ind->indAllocatedLinks )
		{ reachedEnd= 1; onlyUsed= 1; lfree= ind->ind_lfull;	}

	    link= LINK(ind,lfree);
	    past= lfree+ link->tl_key+ 1;

	    if  ( link->tl_to == TLgFREE )
		{
		int	moreThanOne= 0;

		onlyUsed= 0;

		space= link->tl_key;

		while( past < ind->indAllocatedLinks			&&
		       space < n				)
		    {
		    link= LINK(ind,past);
		    if  ( link->tl_key != TLgFREE )
			{ break;	}
		    space += link->tl_key+ 1; past  += link->tl_key+ 1;
		    moreThanOne= 1;
		    }
		if  ( space >= n )
		    {
		    APP_DEB(appDebug("USE %d OF %d\n", n, link->tl_key ));
		    indTLClaim( ind, old, lfree, n );
		    ind->ind_lfree= lfree+ n+ 1;
		    indTLDivide( ind, lfree+ n+ 1, past );
		    return lfree+ 1;
		    }
		else{
		    if  ( moreThanOne )
			{ indTLDivide( ind, lfree, past );	}
		    }
		}
	    else{
		if  ( link->tl_to != TLgUSED )
		    {
		    appDebug( "AT %d: CORRUPTED CHAIN\n", lfree );
		    indTLprint( ind, lfree );
		    return -1;
		    }
		if  ( onlyUsed && ind->ind_lfull < past )
		    { ind->ind_lfull= past;	}
		}
	    lfree= past;
	    }
	}

    /********************************************************************/
    /*  Reallocate links structure.					*/
    /********************************************************************/
    newn= ( ind->indAllocatedLinks+ TLsBLOCK )/TLsBLOCK;
    sz= newn* sizeof(TrieNode *);
    lfree= ind->indAllocatedLinks;

    if  ( ind->indLinkPages )
	{ new= (TrieLink **) realloc( (char *)ind->indLinkPages, sz );	}
    else{ new= (TrieLink **) malloc( sz );				}
    if  ( ! new )
	{ return -1;	}
    ind->indLinkPages= new;

    newpage= (TrieLink *)malloc( TLsBLOCK* sizeof( TrieNode ) );
    if  ( ! newpage )
	{ return -1;	}
    ind->indLinkPages[newn-1]= newpage;
    ind->indAllocatedLinks += TLsBLOCK;

    APP_DEB(appDebug( "%6d(%d*%d) LINKS (%d nodes)\n",
	ind->indAllocatedLinks, newn, TLsBLOCK, ind->indAllocatedNodes ));

    indTLClaim( ind, old, lfree, n );
    ind->ind_lfree= lfree+ n+ 1;
    indTLDivide( ind, lfree+ n+ 1, newn* TLsBLOCK );

    return lfree+ 1;
    }

void indTLfree( ind, tl )
IND *	ind;
int	tl;
    {
    TrieLink *	link= LINK(ind,tl- 1);

    link->tl_to= TLgFREE;
    }
