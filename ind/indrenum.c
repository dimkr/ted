#   include	"indConfig.h"

#   include	"indlocal.h"

#   include	<appDebugoff.h>

/************************************************************************/
/*  Renumber a node in an index, and all the nodes that are reachable	*/
/*  from it.								*/
/*  1)	Already done: Ok.						*/
/*  2)	Allocate new node.						*/
/*  3)	Renumber descendants.						*/
/************************************************************************/

static int indRenumberNode(	int	tn,
				IND *	oldIndex,
				IND *	newIndex,
				int *	newNumbers )
    {
    TrieNode *	oldNode;
    TrieNode *	newNode;
    int		i;

    /*  1  */
    if  ( newNumbers[tn] >= 0 )
	{ return newNumbers[tn];	}

    /*  2  */
    newNumbers[tn]= indTNmake( newIndex );
    if  ( newNumbers[tn] < 0 )
	{ LDEB(newNumbers[tn]); return newNumbers[tn];	}

    /*  3  */
    oldNode= NODE(oldIndex,tn);
    newNode= NODE(newIndex,newNumbers[tn]);

    if  ( oldNode->tn_ntrans > 0 )
	{
	int newTransitions= indTLalloc( newIndex, -1, oldNode->tn_ntrans );

	if  ( newTransitions < 0 )
	    { LDEB(newTransitions); return -1;	}

	newNode->tn_ntrans= oldNode->tn_ntrans;
	newNode->tn_transitions= newTransitions;

	for ( i= 0; i < oldNode->tn_ntrans; i++ )
	    {
	    TrieLink *	oldLink= LINK(oldIndex,oldNode->tn_transitions+ i);
	    int		newTo;
	    TrieLink *	newLink= LINK(newIndex,newTransitions+ i);

	    newTo= indRenumberNode(
			    oldLink->tl_to, oldIndex, newIndex, newNumbers );
	    if  ( newTo < 0 )
		{ LDEB(newTo); return -1; }

	    newLink->tl_key= oldLink->tl_key;
	    newLink->tl_to= newTo;
	    }
	}
    else{
	newNode->tn_ntrans= 0;
	newNode->tn_transitions= -1;
	}

    if  ( oldNode->tn_nitem > 0 )
	{
	int *	oldItems= ITEMS(oldIndex,oldNode->tn_items);
	int *	newItems;
	int	items;

	items= indITalloc( newIndex, -1, oldNode->tn_nitem );
	if  ( items < 0 )
	    { LDEB(items); return -1;	}

	newItems= ITEMS(newIndex,items);
	for ( i= 0; i < oldNode->tn_nitem; i++ )
	    { newItems[i]= oldItems[i]; }

	newNode->tn_nitem= oldNode->tn_nitem;
	newNode->tn_items= items;
	}
    else{
	newNode->tn_nitem= 0;
	newNode->tn_items= -1;
	}

    newNode->tn_flags= oldNode->tn_flags;

    return newNumbers[tn];
    }

/************************************************************************/
/*  Renumber the states in an index. This can make the locality of	*/
/*  reference somewhat better.						*/
/************************************************************************/
IND *	indINDrenumber( IND * ind )
    {
    int		tn;
    int *	newNumbers= (int *)0;

    IND *	rval= indINDmake( 0 );
    if  ( ! rval )
	{ XDEB(rval); return rval;	}

    newNumbers= (int *)malloc( ind->ind_nnode* sizeof(int) );
    if  ( ! newNumbers )
	{ XDEB(newNumbers); indINDfree( rval ); return (IND *)0; }

    for ( tn= 0; tn < ind->ind_nnode; tn++ )
	{ newNumbers[tn]= -1;	}

    rval->ind_start= indRenumberNode( ind->ind_start, ind, rval, newNumbers );
    if  ( rval->ind_start < 0 )
	{
	LDEB(rval->ind_start);
	free( (char *)newNumbers ); indINDfree( rval ); return (IND *)0;
	}

    free( (char *)newNumbers );

    return rval;
    }
