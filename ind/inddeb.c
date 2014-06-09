#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<appDebugoff.h>

/************************************************************************/
/*  Print a list of Nodes with their transitions.			*/
/************************************************************************/
void indTLprint( ind, tl )
IND *	ind;
int	tl;
    {
    TrieLink *	link= LINK(ind,tl);

    if  ( link->tl_to == TLgFREE )
	{ appDebug("\tLINK %6d FREE %d slots\n", tl, link->tl_key ); return; }
    if  ( link->tl_to == TLgUSED )
	{ appDebug("\tLINK %6d FREE %d slots\n", tl, link->tl_key ); return; }
    appDebug("\tLINK %6d '%c' -> %6d\n",
	tl, link->tl_key?link->tl_key:'#', link->tl_to );
    }

void indTNprint( ind, tn )
IND *	ind;
int	tn;
    {
    TrieNode *	node= NODE(ind,tn);
    int		transitions= node->tn_transitions;
    int		tl;

    appDebug( "NODE %6d: %3d transitions AT %6d %s %d items at %d\n",
	tn, node->tn_ntrans, transitions,
	(node->tn_flags&TNfACCEPTS)?"ACCEPTS":"INTERN",
	node->tn_nitem, node->tn_items );

    for ( tl= 0; tl < node->tn_ntrans; tl++ )
	{ indTLprint(ind,transitions+ tl); }

    if  ( node->tn_flags & TNfACCEPTS && node->tn_nitem > 0 )
	{
	int	nitem;
	int *	items;
	int	i;

	/*
	indITget( ind, tn, &nitem, &items );
	*/
	indGetItems( (void *)ind, tn, &nitem, &items );

	for ( i= 0; i < nitem; i++ )
	    { appDebug( "\t ITEM %7d\n", items[i] ); }
	}
    }

void indINDprint( ind )
IND *	ind;
    {
    int	tn;

    appDebug( "INDEX: root= %d, %d nodes, %d links\n",
	ind->ind_start, ind->ind_nnode, ind->indAllocatedLinks );

    for ( tn= 0; tn < ind->ind_nnode; tn++ )
	{ indTNprint( ind, tn ); }
    }

/************************************************************************/
/*									*/
/*  Count the different length of deterministic path in the automaton.	*/
/*									*/
/************************************************************************/

void indINDcount( IND *	ind )
    {
    int		tn;
    int		l;

    static int lengths[200];

    for ( tn= 0; tn < ind->ind_nnode; tn++ )
	{
	TrieNode *	node= NODE(ind,tn);

	if  ( node->tn_ntrans == 1 )
	    {
	    int		transitions= node->tn_transitions;
	    TrieLink *	link= LINK(ind,transitions);

	    l= 0;
	    while( node->tn_ntrans == 1 )
		{
		node= NODE(ind,link->tl_to); l++;
		transitions= node->tn_transitions;
		link= LINK(ind,transitions);
		}
	    lengths[l]++;
	    }
	}

    for ( l= 1; l < 199; l++ )
	{
	if  ( lengths[l] > 0 )
	    { appDebug( "%3d: %5d\n", l, lengths[l]- lengths[l+1] ); }
	}
    }

/************************************************************************/
/*  Print all possible words stored in a trie.				*/
/*  NOTE: The approach is naive and loops on cyclic structures.		*/
/************************************************************************/
void	indDump( ind, tn, level )
IND *	ind;
int	tn;
int	level;
    {
    static char	outbuf[400];
    TrieNode *	node;
    int		i;
    int		n;

    if  ( tn < 0 )
	{ return;	}
    node= NODE(ind,tn);
    n= node->tn_ntrans;

    if  ( node->tn_flags & TNfACCEPTS )
	{ outbuf[level]= '\0'; appDebug( "> %s\n", outbuf ); }

    for ( i= 0; i < n; i++ )
	{
	outbuf[level]= LINK(ind,node->tn_transitions+i)->tl_key;
	indDump( ind, LINK(ind,node->tn_transitions+i)->tl_to, level+ 1 );
	}
    }
