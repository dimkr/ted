#   include	"indConfig.h"

#   include	"indlocal.h"

#   include	<appDebugoff.h>

/************************************************************************/
/*  The series of items is divided into groups.				*/
/*  Every group is preceded by a group header.				*/
/*  If the group is used, It is the number of items in the group.	*/
/*  If the group is free, It is the minus number of items in the group.	*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Walk over the allocation chain of items, and do some half heared	*/
/*  checking.								*/
/*									*/
/************************************************************************/

int	indITwalk( IND *	ind )
    {
    int	pos= 0;
    int	size;

    while( pos < ind->ind_ccount )
	{
	size=	*ITEMS(ind,pos);
	if  ( size < 0 )
	    { size= -size;	}

	if  ( pos+ size+ 1 > ind->ind_ccount )
	    {
	    appDebug( "AT %d: CORRUPTED CHAIN: %d+ %d+ 1= %d > %d\n",
		pos, pos, size, pos+size+1, ind->ind_ccount);
	    return -1;
	    }

	pos += size+ 1;
	}

    return 0;
    }

static void indITDivide(	IND *	ind,
				int	from,
				int	upto )
    {
    if  ( from < ind->ind_cfull )
	{ ind->ind_cfull= from; }

    APP_DEB(appDebug(",,,,   DIVIDE        %6d,,,,, %6d COUNT= %d\n",
		from, upto, upto- from+ 1 ));

    *ITEMS(ind,from)= from- upto+ 1;
    }

static void indITClaim( IND *	ind,
			int	old,
			int	at,
			int	n )
    {
    int *	atclas= ITEMS(ind,at);

    *atclas= n;

    /*
    if  ( at/ITsBLOCK != (at+n-1)/ITsBLOCK )
	{ appDebug("at=%d,n=%d\n",at,n); fflush(stdout); abort(); }
    */

    APP_DEB(appDebug("++++   USE  ITEMS AT %6d, COUNT= %3d\n", at, n ));

    if  ( old > 0 )
	{
	int *	olclas= ITEMS(ind,old-1);

	if  ( old- 1 < ind->ind_cfull )
	    { ind->ind_cfull= old- 1; }

	n= *olclas;
	APP_DEB(appDebug("       FREE ITEMS AT %6d, COUNT= %3d\n", old-1, n ));
	*olclas= -n;
	at++;
	while( n > 0 )
	    {
	    atclas= ITEMS(ind,at);
	    olclas= ITEMS(ind,old);
	    *atclas= *olclas;
	    at++; old++; n--;
	    }
	}
    }

/************************************************************************/
/*  (Re)allocate an items array.					*/
/************************************************************************/
int	indITalloc(	IND *	ind,
			int	old,
			int	n )
    {
    int		past;
    int		space;
    int		freec;

    int		newn;
    unsigned	sz;
    int **	new;
    int *	newpage;

    int *	clas;

    /*
    APP_DEB(appDebug( "INDITALLOC(,old=%d,n=%d)\n", old, n ));
    */

    if  ( old == 0 )
	{ LDEB(old); return -1;	}

    if  ( n < 1 || n >= 255 )
	{ LDEB(n); return -1;	}

    /********************************************************************/
    /*  Fits in old position?						*/
    /********************************************************************/
    if  ( old > 0 )
	{
	freec= old- 1;

	clas= ITEMS(ind,freec);
	if  ( *clas < 0 || *clas + freec > ind->ind_ccount )
	    { return -1;	}
	if  ( ( space= *clas ) >= n )
	    { return old;	}

	space= *clas; past= freec+ space;
	while( past < ind->ind_ccount			&&
	       space < n				)
	    {
	    clas= ITEMS(ind,past);
	    if  ( *clas >= 0 )
		{ break;	}
	    /************************************/
	    /*  No glue over block limits.	*/
	    /************************************/
	    if  ( freec/ITsBLOCK < ( past+ *clas+ 1 )/ITsBLOCK )
		{ break;	}
	    space += *clas+ 1; past  += *clas+ 1;
	    }

	if  ( space >= n )
	    {
	    APP_DEB(appDebug("BLOWUP FROM %d to %d\n", *clas, n ));
	    indITClaim( ind, -1, old, n );
	    indITDivide( ind, old+ n+ 1, past );
	    return old;
	    }
	}

    /********************************************************************/
    /*  Look for an other position, it can be fitted into.		*/
    /********************************************************************/
    if  ( ind->ind_ccount > 0 )
	{
	int	reachedEnd= 0;
	int	onlyUsed= 0;

	freec= ind->ind_cfree;

	while( ! reachedEnd || freec < ind->ind_cfree )
	    {
	    if  ( freec >= ind->ind_ccount )
		{ reachedEnd= 1; onlyUsed= 1; freec= ind->ind_cfull;	}

	    clas= ITEMS(ind,freec);
	    if  ( *clas >= 0 )
		{ past= freec+ *clas+ 1; }
	    else{ past= freec- *clas+ 1; }

	    if  ( *clas < 0 )
		{
		int	moreThanOne= 0;

		onlyUsed= 0;

		space= - *clas;

		while( past < ind->ind_ccount			&&
		       freec/ITsBLOCK == past/ITsBLOCK		&&
		       space < n				)
		    {
		    clas= ITEMS(ind,past);
		    if  ( *clas >= 0 )
			{ break;	}
		    space += - *clas+ 1; past  += - *clas+ 1;
		    moreThanOne= 1;
		    }
		if  ( space >= n )
		    {
		    APP_DEB(appDebug("USE %d OF %d\n", n, - *clas ));
		    indITClaim( ind, old, freec, n );
		    ind->ind_cfree= freec+ n+ 1;
		    if  ( space > n )
			{ indITDivide( ind, freec+ n+ 1, past );	}
		    return freec+ 1;
		    }
		else{
		    if  ( moreThanOne )
			{ indITDivide( ind, freec, past );	}
		    }
		}
	    else{
		if  ( freec+ *clas+ 1 > ind->ind_ccount )
		    {
		    appDebug( "AT %d: CORRUPTED CHAIN: %d+ %d+ 1= %d > %d\n",
			freec, freec, *clas, freec+*clas+1, ind->ind_ccount);
		    return -1;
		    }
		if  ( onlyUsed && ind->ind_cfull < past )
		    { ind->ind_cfull= past;	}
		}
	    freec= past;
	    }
	}

    /********************************************************************/
    /*  Reallocate items structure.					*/
    /********************************************************************/
    newn= ( ind->ind_ccount+ ITsBLOCK )/ITsBLOCK;
    sz= newn* sizeof(int *);
    freec= ind->ind_ccount;

    if  ( ind->ind_cpages )
	{ new= (int **) realloc( (char *)ind->ind_cpages, sz );	}
    else{ new= (int **) malloc( sz );				}
    if  ( ! new )
	{ return -1;	}
    ind->ind_cpages= new;

    newpage= (int *)malloc( ITsBLOCK* sizeof( int ) );
    if  ( ! newpage )
	{ return -1;	}
    ind->ind_cpages[newn-1]= newpage;
    ind->ind_ccount += ITsBLOCK;

    APP_DEB(appDebug( "%6d(%d*%d) ITEMS (%d nodes)\n",
	ind->ind_ccount, newn, ITsBLOCK, ind->indAllocatedNodes ));

    indITClaim( ind, old, freec, n );
    ind->ind_cfree= freec+ n+ 1;
    indITDivide( ind, freec+ n+ 1, newn* ITsBLOCK );

    return freec+ 1;
    }

void indITfree( ind, cl )
IND *	ind;
int	cl;
    {
    int *	clas= ITEMS(ind,cl-1);

    *clas= - *clas;
    }

int indITset( ind, tn, item )
IND *	ind;
int	tn;
int	item;
    {
    TrieNode *	node;
    int		l, m, r;
    int	*	oclas;
    int	*	nclas;
    int		items;

    node= NODE(ind,tn);

    l= 0;
    r= node->tn_nitem;
    m= ( l+ r )/2;

    if  ( r >= 255 )
	{ LDEB(r); return -1;	}

    if  ( r <= 0 )
	{
	node->tn_items= items= indITalloc( ind, -1, 1 );
	if  ( items < 0 )
	    { return -1;	}
	*ITEMS(ind,items)= item;
	node->tn_nitem= 1;
	}
    else{
	oclas= ITEMS(ind,node->tn_items);

	while( l < m )
	    {
	    if  ( item < oclas[m] )
		{ r= m;	}
	    else{ l= m;	}
	    m= ( l+ r )/2;
	    }
	if  ( oclas[m] == item )
	    { return 0;	}
	if  ( item > oclas[m] )
	    { m++; }
	items= indITalloc( ind, node->tn_items, node->tn_nitem+ 1 );
	if  ( items < 0 )
	    { LDEB(items); return -1;	}
	node->tn_items= items;
	r= node->tn_nitem++;
	nclas= ITEMS(ind,items);

	if  ( nclas != oclas )
	    {
	    for ( l= 0; l < r; l++ )
		{ nclas[l]= oclas[l];	}
	    }

	while( r > m )
	    { nclas[r]= nclas[r- 1]; r--; }
	nclas[m]= item;
	}

    return 0;
    }

int indITget( ind, tn, pnitem, pitems )
IND *	ind;
int	tn;
int *	pnitem;
int **	pitems;
    {
    TrieNode *	node;

    if  ( tn < 0 || tn >= ind->indAllocatedNodes )
	{ *pnitem= 0; *pitems= (int *)0; return 0;	}

    node= NODE(ind,tn);
    if  ( ! node )
	{ *pnitem= 0; *pitems= (int *)0; return 0;	}

    if  ( node->tn_nitem <= 0 )
	{ *pnitem= 0; *pitems= (int *)0; return 0;	}

    *pnitem= node->tn_nitem;
    *pitems= ITEMS(ind,node->tn_items);

    return 0;
    }
