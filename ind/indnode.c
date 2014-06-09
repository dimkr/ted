#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<appDebugoff.h>

/************************************************************************/
/*  Allocate a new node. In Theory, it might be advisable to look for	*/
/*  a hole in the array of nodes. As in the current version of the	*/
/*  software, no nodes are really freed. this does not really make	*/
/*  sense.								*/
/************************************************************************/
int	indTNmake( ind )
IND *	ind;
    {
    TrieNode *	node;

    if  ( ind->ind_nnode >= ind->indAllocatedNodes )
	{
	int		newm= ( ind->indAllocatedNodes+ TNsBLOCK )/ TNsBLOCK;
	unsigned	sz= newm* sizeof(TrieNode *);
	TrieNode **	new;
	TrieNode *	nwpg;

	if  ( ind->indNodePages )
	    { new= (TrieNode **) realloc( (char *)ind->indNodePages, sz );}
	else{ new= (TrieNode **) malloc( sz );				}
	if  ( ! new )
	    { return -1;	}
	ind->indNodePages= new;
	nwpg= (TrieNode *)malloc( TNsBLOCK* sizeof( TrieNode ) );
	if  ( ! nwpg )
	    { return -1;	}
	ind->indNodePages[newm-1]= nwpg;
	ind->indAllocatedNodes= newm* TNsBLOCK;
	}

    node= NODE(ind,ind->ind_nnode);
    node->tn_transitions= -1;
    node->tn_items= -1;
    node->tn_ntrans= 0;
    node->tn_nitem= 0;
    node->tn_flags= TNfUSED;
    node->tn_unused= 0;

    /*appDebug( "NODE %7d of %7d\n", ind->ind_nnode, ind->indAllocatedNodes );*/
    return ind->ind_nnode++;
    }

void indTNfree( ind, tn )
IND *	ind;
int	tn;
    {
    TrieNode *	node= NODE(ind,tn);

    if  ( node->tn_transitions >= 0 )
	{ indTLfree( ind, node->tn_transitions ); node->tn_transitions= -1; }
    if  ( node->tn_items >= 0 )
	{ indITfree( ind, node->tn_items ); node->tn_items= -1; }
    node->tn_flags= 0;
    node->tn_ntrans= 0;
    node->tn_nitem= 0;

    while( ind->ind_nnode > 0)
	{
	node= NODE(ind,ind->ind_nnode-1);

	if  ( node->tn_flags == 0	)
	    { ind->ind_nnode--;	}
	else{ break;		}
	}
    }

int indINDstep( pm, ind, tn, key )
int *			pm;
IND *			ind;
int			tn;
const unsigned char *	key;
    {
    TrieNode *	node;
    int		l, r;
    int		transitions;

    node= NODE(ind,tn);

    l= 0;
    r= node->tn_ntrans;
    *pm= ( l+ r )/2;
    transitions= node->tn_transitions;

    if  ( r <= 0 )
	{ return -1;	}

    while( l < *pm )
	{
	if  ( *key < LINK(ind,transitions+*pm)->tl_key )
	    { r= *pm;	}
	else{ l= *pm;	}
	*pm= ( l+ r )/2;
	}

    if  ( *key != LINK(ind,transitions+*pm)->tl_key )
	{ return -1;	}

    return LINK(ind,transitions+*pm)->tl_to;
    }

int indINDget( paccept, ind, tn, key )
int *			paccept;
IND *			ind;
int			tn;
const unsigned char *	key;
    {
    int		m;

    if  ( tn < 0 )
	{
	if  ( ( tn= ind->ind_start ) < 0 )
	    { return tn;	}
	}

    for (;;)
	{
	if  ( ! * key )
	    {
	    if  ( NODE(ind,tn)->tn_flags & TNfACCEPTS )
		{ *paccept= 1;	}
	    else{ *paccept= 0;	}

	    return tn;
	    }

	tn= indINDstep( &m, ind, tn, key++ );

	if  ( tn < 0 )
	    { return tn;	}
	}
    }

int indINDforget( ind, key )
IND *		ind;
unsigned char *	key;
    {
    int		accepted;
    int		tn= indINDget( &accepted, ind, ind->ind_start, key );
    TrieNode *	node;

    if  ( tn < 0 || ! accepted )
	{ return -1;	}

    node= NODE(ind,tn);

    node->tn_flags &= ~TNfACCEPTS;
    node->tn_nitem= 0;

    return 0;
    }

int indINDput( ind, key )
IND *			ind;
const unsigned char *	key;
    {
    TrieNode *	node;
    int		m;
    int		tn;

    if  ( ( tn= ind->ind_start ) < 0 )
	{
	tn= ind->ind_start= indTNmake( ind );
	if  ( tn < 0 )
	    { LDEB(tn); return -1;	}
	}

    for (;;)
	{
	if  ( tn < 0 || tn >= ind->ind_nnode )
	    {
	    appDebug("tn= %d, nnode=%d\n", tn, ind->ind_nnode);
	    return -1;
	    }
	node= NODE(ind,tn);

	/*indTLwalk(ind);*/

	if  ( ! * key )
	    { NODE(ind,tn)->tn_flags |= TNfACCEPTS; return tn;	}

	tn= indINDstep( &m, ind, tn, key );

	if  ( tn < 0 )
	    {
	    int		ntn;
	    int		transitions= node->tn_transitions;

	    if  ( node->tn_ntrans > 0 )
		{
		int	r;

		transitions= indTLalloc( ind, transitions, node->tn_ntrans+ 1 );
		if  ( transitions < 0 )
		    { appDebug( "%d trans\n", node->tn_ntrans ); return -1; }
		else{ node->tn_transitions= transitions;		  }

		if  ( *key > LINK(ind,transitions+m)->tl_key )
		    { m++; }
		ntn= indTNmake( ind );
		if  ( ntn < 0 )
		    { return -1;	}
		r= node->tn_ntrans++;
		while( r > m )
		    {
		    *LINK(ind,transitions+r)= *LINK(ind,transitions+r-1);
		    r--;
		    }
		LINK(ind,transitions+m)->tl_to= ntn;
		LINK(ind,transitions+m)->tl_key= *key;
		}
	    else{
		node->tn_transitions= transitions= indTLalloc( ind, -1, 1 );
		if  ( transitions < 0 )
		    { return -1;	}
		ntn= indTNmake( ind );
		if  ( ntn < 0 )
		    { return -1;	}
		node->tn_ntrans= 1;
		LINK(ind,transitions)->tl_to= ntn;
		LINK(ind,transitions)->tl_key= *key;
		m= 0;
		}
	    tn= LINK(ind,transitions+m)->tl_to;
	    }
	key++;
	}
    }
