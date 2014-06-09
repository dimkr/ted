/************************************************************************/
/*									*/
/*  A mapping from string values to pointer values.			*/
/*									*/
/*  A tree implements the mapping of strings to pointer values.		*/
/*  *   Memory management (If any) of the stored pointers is the	*/
/*	responsibility of the user.					*/
/*  *	A property of the tree decides whether the tree, or the user	*/
/*	keeps trak of the string values that are used as keys inside	*/
/*	the trees.							*/
/*									*/
/*  IMPLEMENTATION:							*/
/*  *	2-3 trees as discussed on pp 145.. of:				*/
/*	AHO, Alfred V., John E. HOPCROFT & Jeffrey D. Ullman: "The	*/
/*	desigh and analysis of Computer Algorithms", Addison and	*/
/*	Wesley, Reading MA, 1974.					*/
/*									*/
/************************************************************************/

#   include	"utilTree.h"
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A node in the tree							*/
/*									*/
/*  The node is an array of three slots and a pointer to a parent node.	*/
/*  In the root node, the pointer to the parent is NULL.		*/
/*  In internal nodes, the left most slot does not have a key value,	*/
/*  and the pointers in the slots are pointers to nodes.		*/
/*  In leaf nodes: Nodes that store the actual values, the left most	*/
/*  slot always has a key value, and the pointers in the slots refer	*/
/*  to the stored value.						*/
/*									*/
/*  Between two invocations of the routines nodes contain either	*/
/*  two or three occupied slots. During the the invocation this may be	*/
/*  temporarily different. The anomaly is repaired immediately. Only	*/
/*  the tree with exactly one stored value is an exception: The root	*/
/*  node is a leaf node that contains one stored value in the left-most	*/
/*  slot.								*/
/*									*/
/************************************************************************/

typedef struct NodeSlot
    {
    char *		nsKey;		/*  key of slot			*/
    union				/*  stored value		*/
	{
	struct TreeNode *	pt_nod;	/*  see it as a node		*/
	void *			pt_val;	/*  see it as a stored value	*/
	}		sl_ptr;
    } NodeSlot;

typedef	struct	TreeNode
    {
    NodeSlot		no_L;		/*  left   slot			*/
    NodeSlot		no_M;		/*  middle slot			*/
    NodeSlot		no_R;		/*  right  slot			*/

    struct TreeNode *	no_dn;		/*  pointer down		*/
    } TreeNode;


/************************************************************************/
/*									*/
/*  Avoid unnecessary typing						*/
/*									*/
/************************************************************************/

#   define	sl_val	sl_ptr.pt_val
#   define	sl_nod	sl_ptr.pt_nod

/************************************************************************/
/*									*/
/*  struct describing a whole tree					*/
/*									*/
/*  Unless the last action on a tree was a delete, tr_curr is the node	*/
/*  in which the current slot resides and tr_lfcu is the key of the	*/
/*  current slot.							*/
/*  After a delete the flag trCurrentDeleted is set. The slot that was	*/
/*  to be deleted is cut from the tree. And currency is supposed to be	*/
/*  half a slot before where tr_lfcu tells us it is.			*/
/*  When tr_lfcu == 0 currency is right of any slot in tr_curr		*/
/*  So when trCurrentDeleted is on, currency is on what trnxt would	*/
/*  give								*/
/*									*/
/************************************************************************/

#   define	TRsINACTIVE	0
#   define	TRsALL		1
#   define	TRsCLOSE	2

typedef struct TreeMapping
    {
    TreeNode *	tr_root;	/*  the whole structure of the tree	*/
    TreeNode *	tr_curr;	/*  the current leaf			*/
    char *	tr_lfcu;	/*  the currency (key) within tr_curr	*/
    char	trCurrentDeleted;/*  corrected currency?		*/
    char	trStatusFlag;	/*  Set by trall(), trclose()		*/
    char	trOwnKeys;	/*  If set, strdup/free keys.		*/
    } TreeMapping;

/************************************************************************/
/*									*/
/*  Memory management primitives: The indirection can be used to check	*/
/*  memory management.							*/
/*									*/
/************************************************************************/

#   define treeSaveKey( s )	strdup( s )
#   define treeFreeKey( s )	free( s )

#   define treeMakeNode()	(TreeNode *)malloc( sizeof(TreeNode) )
#   define treeFreeNode( n )	free( n )

#   define treeMakeTree()	(TreeMapping *)malloc( sizeof(TreeMapping) )
#   define treeFreeTree( t )	free( t )

/************************************************************************/
/*									*/
/*  Make a new tree							*/
/*									*/
/************************************************************************/

void * utilTreeMakeTree(	int   ownKeys )
    {
    TreeMapping *	tm= (TreeMapping *)treeMakeTree();

    if  ( ! tm )
	{ XDEB(tm); return (void *)0;	}

    tm->tr_root= (TreeNode *)0;
    tm->tr_curr= (TreeNode *)0;
    tm->tr_lfcu= (char *)0;
    tm->trCurrentDeleted= 0;
    tm->trStatusFlag= TRsINACTIVE;
    tm->trOwnKeys= ownKeys;

    return (void *)tm;
    }

/************************************************************************/
/*									*/
/*  Find the leaf in which the entry for key possibly resides		*/
/*  This is the basic 2-3 tree search function.				*/
/*									*/
/*  1)	As long we are in an internal node.				*/
/*  2)	Before middle: Enter left.					*/
/*  3)	No right: Enter middle.						*/
/*  4)	Choose between middle and right.				*/
/*									*/
/************************************************************************/

static TreeNode * treeGetLeaf(	TreeNode *	tn,
				const char *	key )
    {
    /*  1  */
    while( ! tn->no_L.nsKey )
	{
	int		cmp;

	/*  2  */
	cmp= strcmp( key, tn->no_M.nsKey );
	if  ( cmp <  0 )
	    { tn= tn->no_L.sl_nod; continue;	}

	/*  3  */
	if  ( ! tn->no_R.nsKey )
	    { tn= tn->no_M.sl_nod; continue;	}

	/*  4  */
	cmp= strcmp( key, tn->no_R.nsKey );
	if  ( cmp <  0 )
	    { tn= tn->no_M.sl_nod; continue;	}
	else{ tn= tn->no_R.sl_nod; continue;	}
	}

    return tn;
    }

/************************************************************************/
/*									*/
/*  suppose tn is a leaf, return next leaf				*/
/*									*/
/************************************************************************/

static TreeNode * treeGetNextLeaf(	TreeNode *	tn )
    {
    TreeNode *		dn;

    for (;;)
	{
	/*  leaf is root ? else step down	*/
	dn= tn->no_dn;
	if  ( ! dn )
	    { return (TreeNode *)0;	}

	/*  redescent from leftmost	*/
	if  ( dn->no_L.sl_nod == tn )
	    { tn= dn->no_M.sl_nod;	break;	}
	/*  redescent from middle	*/
	if  ( dn->no_M.sl_nod == tn )
	    {
	    /*  is there a right hand branch ?	*/
	    if  ( dn->no_R.nsKey )
		{ tn= dn->no_R.sl_nod;	break;	}
	    }
	tn= dn;
	}

    /*  walk to leftmost leaf	*/
    while( ! tn->no_L.nsKey )	{ tn= tn->no_L.sl_nod; }

    return tn;
    }

/************************************************************************/
/*									*/
/*  suppose tn is a leaf, return previous leaf				*/
/*									*/
/************************************************************************/

static TreeNode * treeGetPrevLeaf(	TreeNode *	tn )
    {
    TreeNode *		dn;

    for (;;)
	{
	/*  leaf is root ? else step down	*/
	dn= tn->no_dn;
	if  ( ! dn )
	    { return (TreeNode *)0;	}

	/*  redescent from where	*/
	if  ( dn->no_M.nsKey )
	    {
	    if  ( tn == dn->no_M.sl_nod )
		{ tn= dn->no_L.sl_nod;	break;	}

	    if  ( dn->no_R.nsKey )
		{
		if  ( tn == dn->no_R.sl_nod )
		    { tn= dn->no_M.sl_nod;	break;	}
		}
	    }
	tn= dn;
	}

    /*  walk to rightmost leaf	*/
    while( ! tn->no_L.nsKey )
	{
	if  ( tn->no_R.nsKey )
	    { tn= tn->no_R.sl_nod; continue;	}
	tn= tn->no_M.sl_nod;
	}

    return tn;
    }

/************************************************************************/
/*									*/
/*  Insert a pointer into a tree.					*/
/*									*/
/************************************************************************/

int utilTreeStoreValue(	void *		tree,
			void **		pPrevVal,
			const char **	pStoredKey,
			const char *	key,
			void *		val )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    TreeNode *		dn;
    TreeNode *		nw;
    int			cmp;
    int			where;
    int			nwkey;

    void *		prevVal= (void *)0;
    char *		savedKey= (char *)0;
    char *		childKey;

    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); return -1;	}

    if  ( ! tm->tr_root )
	{
	no= treeMakeNode();
	if  ( ! no )
	    { LDEB(no); return -1; }

	if  ( tm->trOwnKeys )
	    {
	    savedKey= treeSaveKey( key );
	    if  ( ! savedKey )
		{ XDEB(savedKey); treeFreeNode( no ); return -1;	}

	    }
	else{ savedKey= (char *)key;	}

	no->no_L.nsKey= savedKey;
	no->no_L.sl_val= val;
	no->no_M.nsKey= (char *)0;
	no->no_R.nsKey= (char *)0;
	no->no_dn= (TreeNode *)0;

	tm->tr_root= no;
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;
	tm->tr_lfcu= savedKey;
	if  ( pStoredKey )
	    { *pStoredKey= tm->tr_lfcu;	}
	if  ( pPrevVal )
	    { *pPrevVal= prevVal;	}

	return 0;
	}

    /*  get leaf with key						*/
    no= treeGetLeaf( tm->tr_root, key );

    /********************************************************************/
    /*  Try to store value in LEAF no					*/
    /*  If an insertion between two values is necessary, The stored	*/
    /*  values are shifted to the right.				*/
    /*									*/
    /*  Find out where to insert in leaf and whether key must be	*/
    /*  allocated.							*/
    /*  values of where:						*/
    /*  0: before L							*/
    /*  1: in L								*/
    /*  2: before M							*/
    /*  3: in M								*/
    /*  4: before R							*/
    /*  5: in R								*/
    /*  6: after R							*/
    /*									*/
    /********************************************************************/

    cmp= strcmp( key, no->no_L.nsKey );
    if  ( cmp <  0 )					{ nwkey= 1; where= 0; }
    else{
	if  ( cmp == 0 )				{ nwkey= 0; where= 1; }
	else{
	    if  ( ! no->no_M.nsKey )			{ nwkey= 1; where= 3; }
	    else{
		cmp= strcmp( key, no->no_M.nsKey );
		if  ( cmp <  0 )			{ nwkey= 1; where= 2; }
		else{
		    if  ( cmp == 0 )			{ nwkey= 0; where= 3; }
		    else{
			if  ( ! no->no_R.nsKey )	{ nwkey= 1; where= 5; }
			else{
			    cmp= strcmp( key, no->no_R.nsKey );
			    if  ( cmp <  0 )		{ nwkey= 1; where= 4; }
			    else{
				if  ( cmp == 0 )	{ nwkey= 0; where= 5; }
				else			{ nwkey= 1; where= 6; }
				}
			    }
			}
		    }
		}
	    }
	}

    /************************************/
    /*  allocate new key		*/
    /*  Refuse to update active trees	*/
    /************************************/
    if  ( nwkey )
	{
	if  ( tm->trStatusFlag != TRsINACTIVE )
	    { LDEB(tm->trStatusFlag); return -1;	}

	if  ( tm->trOwnKeys )
	    {
	    savedKey= treeSaveKey( key );
	    if  ( ! savedKey )
		{ XDEB(savedKey); treeFreeNode( no ); return -1;	}

	    }
	else{ savedKey= (char *)key;	}
	}

    /*  allocate overflow node	*/
    if  ( ( where % 2 ) == 0 && no->no_M.nsKey && no->no_R.nsKey )
	{
	dn= no->no_dn;
	nw= treeMakeNode();
	if  ( ! nw )
	    { XDEB(nw); return -1;	}

	nw->no_R.nsKey= (char *)0;
	nw->no_dn= dn;
	}
    else{
	nw= (TreeNode *)0;
	dn= (TreeNode *)0;
	}

    switch( where )
	{
	case 0:
	    /*  before L	*/
	    if  ( nw )
		{
		nw->no_M= no->no_R;
		nw->no_L= no->no_M;
		no->no_R.nsKey= (char *)0;
		}
	    else{
		no->no_R= no->no_M;
		}
	    no->no_M= no->no_L;
	    no->no_L.nsKey= savedKey;
	    /*FALLTHROUGH*/

	case 1:
	    /*  in L	*/
	    if  ( ! nwkey )
		{ prevVal= no->no_L.sl_val;	}
	    no->no_L.sl_val= val;
	    tm->tr_curr= no;
	    tm->trCurrentDeleted= 0;
	    tm->tr_lfcu= no->no_L.nsKey;
	    if  ( pStoredKey )
		{ *pStoredKey= tm->tr_lfcu;	}
	    if  ( pPrevVal )
		{ *pPrevVal= prevVal;	}
	    break;

	case 2:
	    /*  before M	*/
	    if  ( nw )
		{
		nw->no_M= no->no_R;
		nw->no_L= no->no_M;
		no->no_R.nsKey= (char *)0;
		}
	    else{ no->no_R= no->no_M;	}
	    no->no_M.nsKey= savedKey;
	    /*FALLTHROUGH*/

	case 3:
	    /*  in M	*/
	    if  ( nwkey )
		{ no->no_M.nsKey= savedKey;	}
	    else{ prevVal= no->no_M.sl_val;	}
	    no->no_M.sl_val= val;
	    tm->tr_curr= no;
	    tm->trCurrentDeleted= 0;
	    tm->tr_lfcu= no->no_M.nsKey;
	    if  ( pStoredKey )
		{ *pStoredKey= tm->tr_lfcu;	}
	    if  ( pPrevVal )
		{ *pPrevVal= prevVal;	}
	    break;

	case 4:
	    /*  before R	*/
	    nw->no_M= no->no_R;
	    nw->no_L.nsKey= savedKey;
	    nw->no_L.sl_val= val;
	    no->no_R.nsKey= (char *)0;
	    tm->tr_curr= nw;
	    tm->trCurrentDeleted= 0;
	    tm->tr_lfcu= savedKey;
	    if  ( pStoredKey )
		{ *pStoredKey= tm->tr_lfcu;	}
	    if  ( pPrevVal )
		{ *pPrevVal= prevVal;	}
	    break;

	case 5:
	    /*  in R	*/
	    if  ( nwkey )
		{ no->no_R.nsKey= savedKey;	}
	    else{ prevVal= no->no_R.sl_val;	}
	    no->no_R.sl_val= val;
	    tm->tr_curr= no;
	    tm->trCurrentDeleted= 0;
	    tm->tr_lfcu= no->no_R.nsKey;
	    if  ( pStoredKey )
		{ *pStoredKey= tm->tr_lfcu;	}
	    if  ( pPrevVal )
		{ *pPrevVal= prevVal;	}
	    break;

	case 6:
	    /*  after R	*/
	    nw->no_L= no->no_R;
	    nw->no_M.nsKey= savedKey;
	    nw->no_M.sl_val= val;
	    no->no_R.nsKey= (char *)0;
	    tm->tr_curr= nw;
	    tm->trCurrentDeleted= 0;
	    tm->tr_lfcu= savedKey;
	    if  ( pStoredKey )
		{ *pStoredKey= tm->tr_lfcu;	}
	    if  ( pPrevVal )
		{ *pPrevVal= prevVal;	}
	    break;

	default:
	    LDEB(where);
	    return -1;
	}

    if  ( ! nw )
	{ return 0;	}

    childKey= nw->no_L.nsKey;

    /*  descend tree and where necessary split nodes			*/
    while( dn )
	{
	NodeSlot		Hslot;

	/*  insert new node into parent. If the parent should be split	*/
	/*  continue; else return;					*/
	if  ( no == dn->no_L.sl_nod )
	    {
	    Hslot= dn->no_R;
	    dn->no_R= dn->no_M;
	    dn->no_M.nsKey= childKey;
	    dn->no_M.sl_nod= nw;
	    }
	else{
	    if  ( no == dn->no_M.sl_nod )
		{
		Hslot= dn->no_R;
		dn->no_R.nsKey= childKey;
		dn->no_R.sl_nod= nw;
		}
	    else{
		if  ( no == dn->no_R.sl_nod )
		    {
		    Hslot.nsKey= childKey;
		    Hslot.sl_nod= nw;
		    }
		else{ XDEB(no); return -1; }
		}
	    }

	if  ( ! Hslot.nsKey )
	    { return 0;	}

	/*  split node, insert information into new nodes		*/
	if  ( ! ( nw= treeMakeNode() ) )
	    { XDEB(nw); return -1; }

	/*  new values in new node	*/
	nw->no_L.nsKey= (char *)0;
	nw->no_L.sl_nod= dn->no_R.sl_nod;
	nw->no_M= Hslot;
	nw->no_R.nsKey= (char *)0;
	nw->no_dn= dn->no_dn;

	/*  connect children		*/
	nw->no_L.sl_nod->no_dn= nw;
	nw->no_M.sl_nod->no_dn= nw;

	/*  remember key of new node; adapt old node	*/
	childKey= dn->no_R.nsKey;
	dn->no_R.nsKey= (char *)0;

	/*  step down	*/
	no= dn;
	dn= no->no_dn;
	}

    /*  when we reach this place, every node in the descent of the tree	*/
    /*  was split. Make a new root					*/
    dn= treeMakeNode();
    if  ( ! dn )
	{ XDEB(dn); return -1; }

    dn->no_L.nsKey= (char *)0;
    dn->no_L.sl_nod= no;
    dn->no_M.nsKey= childKey;
    dn->no_M.sl_nod= nw;
    dn->no_R.nsKey= (char *)0;
    dn->no_R.sl_nod= (TreeNode *)0;
    dn->no_dn= (TreeNode *)0;

    /*  connect children	*/
    dn->no_L.sl_nod->no_dn= dn;
    dn->no_M.sl_nod->no_dn= dn;

    nw->no_dn= dn;
    no->no_dn= dn;
    tm->tr_root= dn;

    return 0;
    }

/************************************************************************/
/*									*/
/*  If there is any, return value stored at the first key <  key	*/
/*  If there is any, return value stored at the first key <= key	*/
/*									*/
/************************************************************************/

static void * treeGetLX(	void *		tree,
				const char **	pKey,
				const char *	key,
				int		skew )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    int			cmp;

    if  ( ! tm || ! tm->tr_root )
	{ *pKey= (char *)0; return (void *)0;	}

    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    /*  get leaf with key	*/
    no= treeGetLeaf( tm->tr_root, key );
    if  ( ! no )
	{ XDEB(no); *pKey= (char *)0; return (void *)0;	}

    cmp= strcmp( key, no->no_L.nsKey );
    if  ( cmp > skew )
	{
	if  ( no->no_M.nsKey )
	    {
	    cmp= strcmp( key, no->no_M.nsKey );
	    if  ( cmp > skew )
		{
		if  ( no->no_R.nsKey )
		    {
		    cmp= strcmp( key, no->no_R.nsKey );
		    if  ( cmp >  skew )
			{
			tm->tr_curr= no;
			tm->trCurrentDeleted= 0;
			*pKey= tm->tr_lfcu= no->no_R.nsKey;
			return no->no_R.sl_val;
			}
		    }

		tm->tr_curr= no;
		tm->trCurrentDeleted= 0;
		*pKey= tm->tr_lfcu= no->no_M.nsKey;
		return no->no_M.sl_val;
		}
	    }

	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_L.nsKey;
	return no->no_L.sl_val;
	}

    if  ( ! ( no= treeGetPrevLeaf( no ) ) )
	{ *pKey= (char *)0; return (void *)0; }

    tm->tr_curr= no;
    tm->trCurrentDeleted= 0;
    if  ( no->no_R.nsKey )
	{
	*pKey= tm->tr_lfcu= no->no_R.nsKey;
	return no->no_R.sl_val;
	}

    *pKey= tm->tr_lfcu= no->no_M.nsKey;
    return no->no_M.sl_val;
    }

void * utilTreeGetLT(	void *		tree,
			const char **	pKey,
			const char *	key )
    { return  treeGetLX( tree, pKey, key, 0 );	}

void * utilTreeGetLE(	void *		tree,
			const char **	pKey,
			const char *	key )
    { return  treeGetLX( tree, pKey, key, -1 );	}

/************************************************************************/
/*									*/
/*  If there is any, return value stored at the first key >  key	*/
/*  If there is any, return value stored at the first key >= key	*/
/*									*/
/************************************************************************/

static void * treeGetGX(	void *		tree,
				const char **	pKey,
				const char *	key,
				int		skew )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    int			cmp;

    if  ( ! tree			||
	  ! ( no= tm->tr_root ) )
	{ *pKey= (char *)0; return (void *)0;	}

    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    /*  get leaf with key	*/
    no= treeGetLeaf( no, key );

    cmp= strcmp( key, no->no_L.nsKey );
    if  ( cmp <  skew )
	{
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_L.nsKey;
	return no->no_L.sl_val;
	}

    if  ( ! no->no_M.nsKey )
	{ *pKey= (char *)0; return (void *)0;		}

    cmp= strcmp( key, no->no_M.nsKey );
    if  ( cmp <  skew )
	{
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_M.nsKey;
	return no->no_M.sl_val;
	}

    if  (   no->no_R.nsKey )
	{
	cmp= strcmp( key, no->no_R.nsKey );
	if  ( cmp <  skew )
	    {
	    tm->tr_curr= no;
	    tm->trCurrentDeleted= 0;
	    *pKey= tm->tr_lfcu= no->no_R.nsKey;
	    return no->no_R.sl_val;
	    }
	}

    if  ( ! ( no= treeGetNextLeaf( no ) ) )
	{ return (void *)0; }

    tm->tr_curr= no;
    tm->trCurrentDeleted= 0;
    *pKey= tm->tr_lfcu= no->no_L.nsKey;
    return no->no_L.sl_val;
    }

void * utilTreeGetGT(	void *		tree,
			const char **	pKey,
			const char *	key )
    { return  treeGetGX( tree, pKey, key, 0 );	}

void * utilTreeGetGE(	void *		tree,
			const char **	pKey,
			const char *	key )
    { return  treeGetGX( tree, pKey, key, 1 );	}

/************************************************************************/
/*									*/
/*  If there is any, return value stored at key.			*/
/*									*/
/************************************************************************/

void * utilTreeGetEQ(		void *		tree,
				const char **	pStoredKey,
				const char *	key )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    int			cmp;

    if  ( ! tree )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}
    no= tm->tr_root;
    if  ( ! no )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    if  ( tm->trStatusFlag == TRsCLOSE )
	{
	LDEB(tm->trStatusFlag);

	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    /*  get leaf with key	*/
    no= treeGetLeaf( no, key );
    if  ( ! no )
	{
	XDEB(no);

	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    cmp= strcmp( key, no->no_L.nsKey );
    if  ( cmp <  0 )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    if  ( cmp == 0 )
	{
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;

	if  ( pStoredKey )
	    { *pStoredKey= tm->tr_lfcu= no->no_L.nsKey;	}

	return no->no_L.sl_val;
	}

    if  ( ! no->no_M.nsKey )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    cmp= strcmp( key, no->no_M.nsKey );
    if  ( cmp <  0 )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    if  ( cmp == 0 )
	{
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;

	if  ( pStoredKey )
	    { *pStoredKey= tm->tr_lfcu= no->no_M.nsKey;	}

	return no->no_M.sl_val;
	}

    if  ( ! no->no_R.nsKey )
	{
	if  ( pStoredKey )
	    { *pStoredKey= (const char *)0;	}

	return (void *)0;
	}

    cmp= strcmp( key, no->no_R.nsKey );
    if  ( cmp == 0 )
	{
	tm->tr_curr= no;
	tm->trCurrentDeleted= 0;

	if  ( pStoredKey )
	    { *pStoredKey= tm->tr_lfcu= no->no_R.nsKey;	}

	return no->no_R.sl_val;
	}

    if  ( pStoredKey )
	{ *pStoredKey= (const char *)0;	}
    return (void *)0;
    }

/************************************************************************/
/*  fills slots in a node from an array, Is used below for the		*/
/*  redistribution of the children. Note that leaves are treated	*/
/*  differently from other nodes in two ways:				*/
/*  - Their Left hand slot gets a key.					*/
/*  - As they do not have children, these are not connected to parent	*/
/************************************************************************/

static void trfillslots(	const NodeSlot *	hs,
				TreeNode *		no,
				int			n,
				int			leaf )
    {
    if  ( leaf )
	{
	no->no_L= *(hs++);
	no->no_M= *(hs++);
	if  ( n != 3 )
	    { no->no_R.nsKey= (char *)0;	return;	}
	no->no_R= *hs;
	}
    else{
	TreeNode *	child;

	child= hs->sl_nod;
	no->no_L.sl_nod= child;	child->no_dn= no;
	no->no_L.nsKey= (char *)0;

	hs++;
	child= hs->sl_nod;
	no->no_M= *(hs++);	child->no_dn= no;

	if  ( n != 3 )
	    { no->no_R.nsKey= (char *)0;	return;	}

	child= hs->sl_nod;
	no->no_R= *hs;		child->no_dn= no;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delete an item from a tree. All gotos jump to the same label from	*/
/*  above. This label cannot be reached otherwise			*/
/*									*/
/************************************************************************/

int utilTreeDeleteEQ(		void *			tree,
				const char *		key,
				UTIL_TREE_CALLBACK	callback,
				void *			through )
    {
    const char *	storedKey;

    if  ( ! utilTreeGetEQ( tree, &storedKey, key )	&&
	  ! storedKey					)
	{ return -1;	}

    return utilTreeDeleteCurrent( tree, callback, through );
    }

int utilTreeDeleteCurrent(	void *			tree,
				UTIL_TREE_CALLBACK	callback,
				void *			through )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    TreeNode *		dn;
    TreeNode *		nh;
    char *		Lkey, * Nkey;
    int			cur;		/*  idea of currency		*/
    int			n;		/*  # of slots do distribute	*/
    static NodeSlot 	Hslot[7];	/*  slots to distribute		*/
    int			leaf= 1;	/*  working in leaf?		*/
    char *		lfcu;		/*  currency in leaf		*/

    if  ( ! tree )
	{ return -1;	}

    no= tm->tr_curr;
    lfcu= tm->tr_lfcu;
    if  ( ! no || ! lfcu || tm->trCurrentDeleted )
	{ return -1;	}

    if  ( tm->trStatusFlag != TRsINACTIVE )
	{ LDEB(tm->trStatusFlag); return -1;	}

    if  ( lfcu == no->no_L.nsKey )
	{
	/********************************************************/
	/*  left most deletion: patch pointer in parent node	*/
	/*  that refers to this slot				*/
	/********************************************************/

	if  ( callback )
	    {
	    (void) (*callback)( no->no_L.nsKey, no->no_L.sl_val, through );
	    }

	Lkey= no->no_L.nsKey;
	Nkey= no->no_M.nsKey;
	dn= no; nh= dn;
	while( ( dn= dn->no_dn ) )
	    {
	    if  ( dn->no_M.nsKey == Lkey )
		{ dn->no_M.nsKey= Nkey; break; }
	    if  ( dn->no_R.nsKey == Lkey )
		{ dn->no_R.nsKey= Nkey; break; }
	    nh= dn;
	    }
	if  ( tm->trOwnKeys )
	    { treeFreeKey( Lkey );	}
	/*  shift	*/
	no->no_L= no->no_M;
	no->no_M= no->no_R;
	no->no_R.nsKey= (char *)0;
	tm->tr_curr= no;
	tm->tr_lfcu= no->no_L.nsKey;
	tm->trCurrentDeleted= 1;

	cur= 0;
	}
    else{
	if  ( lfcu == no->no_M.nsKey )
	    {
	    if  ( callback )
		{
		(void) (*callback)( no->no_M.nsKey, no->no_M.sl_val, through );
		}

	    if  ( tm->trOwnKeys )
		{ treeFreeKey( no->no_M.nsKey );	}
	    no->no_M= no->no_R;
	    no->no_R.nsKey= (char *)0;
	    tm->tr_curr= no;
	    tm->tr_lfcu= no->no_M.nsKey;
	    tm->trCurrentDeleted= 1;
	    cur= 1;
	    }
	else{
	    if  ( callback )
		{
		(void) (*callback)( no->no_R.nsKey, no->no_R.sl_val, through );
		}

	    if  ( tm->trOwnKeys )
		{ treeFreeKey( no->no_R.nsKey );	}

	    no->no_R.nsKey= (char *)0;
	    tm->tr_lfcu= (char *)0;
	    tm->trCurrentDeleted= 1;
	    return 0;
	    }
	}

    /*  exception: root is leaf.	*/
    if  ( no == tm->tr_root )
	{
	/*  deletion of last item in tree?	*/
	if  ( ! no->no_L.nsKey )
	    {
	    treeFreeNode( no );
	    tm->tr_root= (TreeNode *)0;
	    tm->tr_curr= (TreeNode *)0;
	    tm->tr_lfcu= (char *)0;
	    }
	/*  root is leaf and less than two items is allowed.	*/
	return 0;
	}

    /*  walk down	*/
    while( ( dn= no->no_dn ) )
	{
	/*  still two children left ?					*/
	if  ( no->no_M.nsKey )		{ return 0;	}

	/*  collect all slots in Hslot					*/
	n= 0;
	nh= dn->no_L.sl_nod;
	/*  if  ( leaf && nh == no )	{ cur += n;	}		*/
	Hslot[n++]= nh->no_L;
	if  ( nh->no_M.nsKey )
	    {
	    Hslot[n++]= nh->no_M;
	    if  ( nh->no_R.nsKey )
		{ Hslot[n++]= nh->no_R;	}
	    }
	nh= dn->no_M.sl_nod;
	if  ( leaf && nh == no )	{ cur += n;	}
	if  ( leaf )
	    { Hslot[n++]= nh->no_L; }
	else{
	    Hslot[n  ].nsKey= dn->no_M.nsKey;
	    Hslot[n++].sl_ptr= nh->no_L.sl_ptr;
	    }
	if  ( nh->no_M.nsKey )
	    {
	    Hslot[n++]= nh->no_M;
	    if  ( nh->no_R.nsKey )
		{ Hslot[n++]= nh->no_R;	}
	    }
	if  ( dn->no_R.nsKey )
	    {
	    nh= dn->no_R.sl_nod;
	    if  ( leaf && nh == no )	{ cur += n;	}
	    if  ( leaf )
		{ Hslot[n++]= nh->no_L; }
	    else{
		Hslot[n  ].nsKey= dn->no_R.nsKey;
		Hslot[n++].sl_ptr= nh->no_L.sl_ptr;
		}
	    if  ( nh->no_M.nsKey )
		{
		Hslot[n++]= nh->no_M;
		if  ( nh->no_R.nsKey )
		    { Hslot[n++]= nh->no_R;	}
		}
	    }

	if  ( leaf )
	    {
	    if  ( cur < n )
		{ tm->tr_lfcu= Hslot[cur].nsKey;	}
	    else{ tm->tr_lfcu= (char *)0;		}
	    }

	switch( n )
	    {
	    case 3:	/*  from 2,2	*/
		trfillslots( Hslot   , dn->no_L.sl_nod, 3, leaf );
		if  ( leaf )		{ tm->tr_curr= dn->no_L.sl_nod; }
		treeFreeNode( dn->no_M.sl_nod );
		dn->no_M.nsKey= (char *)0;
		break;
	    case 4:	/*  from 3,2 or 2,3	*/
		trfillslots( Hslot   , dn->no_L.sl_nod, 2, leaf );
		trfillslots( Hslot+ 2, dn->no_M.sl_nod, 2, leaf );
		dn->no_M.nsKey= Hslot[2].nsKey;
		if  ( leaf )
		    {
		    if  ( cur < 2 )	{ tm->tr_curr= dn->no_L.sl_nod; }
		    else		{ tm->tr_curr= dn->no_M.sl_nod; }
		    }
		return 0;
	    case 5:	/*  from 2,2,2	*/
		trfillslots( Hslot   , dn->no_L.sl_nod, 3, leaf );
		trfillslots( Hslot+ 3, dn->no_M.sl_nod, 2, leaf );
		dn->no_M.nsKey= Hslot[3].nsKey;
		if  ( leaf )
		    {
		    if  ( cur < 3 )	{ tm->tr_curr= dn->no_L.sl_nod; }
		    else		{ tm->tr_curr= dn->no_M.sl_nod; }
		    }
		treeFreeNode( dn->no_R.sl_nod );
		dn->no_R.nsKey= (char *)0;
		break;
	    case 6:	/* from 2,2,3 or 2,3,2 or 3,2,2	*/
#		ifdef		YOU_MIGHT_DO_THIS
		trfillslots( Hslot   , dn->no_L.sl_nod, 3, leaf );
		trfillslots( Hslot+3 , dn->no_M.sl_nod, 3, leaf );
		dn->no_M.nsKey= Hslot[3].nsKey;
		if  ( leaf )
		    {
		    if  ( cur < 3 )	{ tm->tr_curr= dn->no_L.sl_nod; }
		    else		{ tm->tr_curr= dn->no_M.sl_nod; }
		    }
		treeFreeNode( dn->no_R.sl_nod );
		dn->no_R.nsKey= (char *)0;
		break;
#		else
		trfillslots( Hslot   , dn->no_L.sl_nod, 2, leaf );
		trfillslots( Hslot+ 2, dn->no_M.sl_nod, 2, leaf );
		trfillslots( Hslot+ 4, dn->no_R.sl_nod, 2, leaf );
		dn->no_M.nsKey= Hslot[2].nsKey;
		dn->no_R.nsKey= Hslot[4].nsKey;
		if  ( leaf )
		    {
		    if  ( cur < 2 )	{ tm->tr_curr= dn->no_L.sl_nod; }
		    else{
			if  ( cur < 4 )	{ tm->tr_curr= dn->no_M.sl_nod; }
			else		{ tm->tr_curr= dn->no_R.sl_nod; }
			}
		    }
		return 0;
#		endif
	    case 7:	/* from 3,3,2 or 3,2,3 or 2,3,3	*/
		trfillslots( Hslot   , dn->no_L.sl_nod, 3, leaf );
		trfillslots( Hslot+3 , dn->no_M.sl_nod, 2, leaf );
		trfillslots( Hslot+5 , dn->no_R.sl_nod, 2, leaf );
		dn->no_M.nsKey= Hslot[3].nsKey;
		dn->no_R.nsKey= Hslot[5].nsKey;
		if  ( leaf )
		    {
		    if  ( cur < 3 )	{ tm->tr_curr= dn->no_L.sl_nod; }
		    else{
			if  ( cur < 5 )	{ tm->tr_curr= dn->no_M.sl_nod; }
			else		{ tm->tr_curr= dn->no_R.sl_nod; }
			}
		    }
		return 0;
	    default:
		LDEB(n); return -1;
	    }

	no= dn; leaf= 0;
	}

    /*  more than one slot in root ?	*/
    if  ( no->no_M.nsKey )
	{ return 0;	}

    /*  no->no_L.sl_slot becomes new root. Old root is deleted	*/
    tm->tr_root= no->no_L.sl_nod;
    no->no_L.sl_nod->no_dn= (TreeNode *)0;
    treeFreeNode( no );

    return 0;
    }

/************************************************************************/
/*									*/
/*  shift currency in tree to the left and return value stored there	*/
/*									*/
/************************************************************************/

void * utilTreeGetPrevious(	void *		tree,
				const char **	pKey )
    {
    TreeMapping *	tm= (TreeMapping *)tree;

    TreeNode *		no;
    char *		lfcu;

    if  ( ! tree )
	{ *pKey= (char *)0; return (void *)0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    no= tm->tr_curr;
    if  ( ! no )
	{ *pKey= (char *)0; return (void *)0;	}

    lfcu= tm->tr_lfcu;
    if  ( lfcu )
	{
	if  ( no->no_M.nsKey )
	    {
	    if  ( lfcu == no->no_M.nsKey )
		{
		tm->trCurrentDeleted= 0;
		*pKey= tm->tr_lfcu= no->no_L.nsKey;
		return no->no_L.sl_val;
		}
	    if  ( lfcu == no->no_R.nsKey )
		{
		tm->trCurrentDeleted= 0;
		*pKey= tm->tr_lfcu= no->no_M.nsKey;
		return no->no_M.sl_val;
		}
	    }

	/*  shift to previous node	*/
	no= treeGetPrevLeaf( no );
	if  ( ! no )
	    { *pKey= (char *)0; return (void *)0;	}
	}
    /*  else simply last in this node	*/

    if  ( no->no_R.nsKey )
	{
	tm->trCurrentDeleted= 0;
	tm->tr_curr= no;
	*pKey= tm->tr_lfcu= no->no_R.nsKey;
	return no->no_R.sl_val;
	}

    if  ( no->no_M.nsKey )
	{
	tm->trCurrentDeleted= 0;
	tm->tr_curr= no;
	*pKey= tm->tr_lfcu= no->no_M.nsKey;
	return no->no_M.sl_val;
	}

    if  ( no->no_L.nsKey )
	{
	tm->trCurrentDeleted= 0;
	tm->tr_curr= no;
	*pKey= tm->tr_lfcu= no->no_L.nsKey;
	return no->no_L.sl_val;
	}

    XDEB(no->no_L.nsKey);
    *pKey= (char *)0; return (void *)0;
    }

/************************************************************************/
/*									*/
/*  shift currency in tree to the right and return value stored there	*/
/*									*/
/************************************************************************/

void * utilTreeGetNext(	void *		tree,
			const char **	pKey )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;
    char *		lfcu;

    if  ( ! tree )
	{ *pKey= (char *)0; return (void *)0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    no= tm->tr_curr;
    if  ( ! no )
	{ *pKey= (char *)0; return (void *)0;	}

    lfcu= tm->tr_lfcu;
    if  ( lfcu )
	{
	if  ( tm->trCurrentDeleted )
	    {
	    tm->trCurrentDeleted= 0;

	    *pKey= lfcu;

	    if  ( lfcu == no->no_L.nsKey )
		{ return no->no_L.sl_val;	}
	    if  ( lfcu == no->no_M.nsKey )
		{ return no->no_M.sl_val;	}

	    return no->no_R.sl_val;
	    }

	if  ( lfcu == no->no_L.nsKey && no->no_M.nsKey )
	    {
	    tm->trCurrentDeleted= 0;
	    *pKey= tm->tr_lfcu= no->no_M.nsKey;
	    return no->no_M.sl_val;
	    }

	if  ( lfcu == no->no_M.nsKey && no->no_R.nsKey )
	    {
	    tm->trCurrentDeleted= 0;
	    *pKey= tm->tr_lfcu= no->no_R.nsKey;
	    return no->no_R.sl_val;
	    }
	}

    if  ( ! ( no= treeGetNextLeaf( no ) ) )
	{ *pKey= (char *)0; return (void *)0; }

    tm->tr_curr= no;
    tm->trCurrentDeleted= 0;
    *pKey= tm->tr_lfcu= no->no_L.nsKey;
    return no->no_L.sl_val;
    }

/************************************************************************/
/*									*/
/*  return value stored on current leaf of tree				*/
/*									*/
/************************************************************************/

void * utilTreeGetCurrent(	void *		tree,
				const char **	pKey )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;
    char *		lfcu;

    if  ( ! tree )
	{ *pKey= (char *)0; return (void *)0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    no= tm->tr_curr;
    lfcu= tm->tr_lfcu;

    if  ( ! no || ! lfcu || tm->trCurrentDeleted )
	{ *pKey= (char *)0; return (void *)0;	}

    *pKey= lfcu;
    if  ( lfcu == no->no_L.nsKey )	{ return no->no_L.sl_val;	}
    if  ( lfcu == no->no_M.nsKey ) { return no->no_M.sl_val;	}
    return no->no_R.sl_val;
    }

/************************************************************************/
/*									*/
/*  return value stored on first leaf of tree				*/
/*									*/
/************************************************************************/

void * utilTreeGetFirst(	void *		tree,
				const char **	pKey )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;

    if  ( ! tree )
	{ *pKey= (char *)0; return (void *)0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    no= tm->tr_root;
    if  ( ! no )
	{ *pKey= (char *)0; return (void *)0;	}

    /*  walk to leftmost leaf	*/
    while( ! no->no_L.nsKey )
	{ no= no->no_L.sl_nod; }

    /*  set currency	*/
    tm->tr_curr= no;
    tm->trCurrentDeleted= 0;
    *pKey= tm->tr_lfcu= no->no_L.nsKey;

    return no->no_L.sl_val;
    }

/************************************************************************/
/*									*/
/*  return value stored on last leaf of tree				*/
/*									*/
/************************************************************************/

void * utilTreeGetLast(	void *		tree,
			const char **	pKey )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;

    if  ( ! tree )
	{ *pKey= (char *)0; return (void *)0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); *pKey= (char *)0; return (void *)0;	}

    no= tm->tr_root;
    if  ( ! no )
	{ *pKey= (char *)0; return (void *)0;	}

    /*  walk to rightmost leaf	*/
    while( ! no->no_L.nsKey )
	{
	if  ( no->no_R.nsKey )
	    { no= no->no_R.sl_nod;	}
	else{ no= no->no_M.sl_nod;	}
	}

    tm->tr_curr= no;

    if  ( no->no_R.nsKey )
	{
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_R.nsKey;
	return no->no_R.sl_val;
	}

    if  ( no->no_M.nsKey )
	{
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_M.nsKey;
	return no->no_M.sl_val;
	}

    if  ( no->no_L.nsKey )
	{
	/********************************************************/
	/* ONLY possible when root is leaf with one slot.	*/
	/********************************************************/
	tm->trCurrentDeleted= 0;
	*pKey= tm->tr_lfcu= no->no_L.nsKey;
	return no->no_L.sl_val;
	}

    XDEB(no->no_L.nsKey);
    *pKey= (char *)0; return (void *)0;
    }

/************************************************************************/
/*									*/
/*  call callback for all stored name, value combinations		*/
/*									*/
/************************************************************************/

int utilTreeForAll(	void *			tree,
			int			stopOnFailure,
			UTIL_TREE_CALLBACK	callback,
			void *			through )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;

    int			oldStatusFlag;
    int			rval= 0;

    if  ( ! tree )
	{ return 0;	}
    if  ( tm->trStatusFlag == TRsCLOSE )
	{ LDEB(tm->trStatusFlag); return -1;	}

    no= tm->tr_root;
    if  ( ! no )
	{ return 0;	}

    oldStatusFlag= tm->trStatusFlag;
    tm->trStatusFlag= TRsALL;

    /*  walk to leftmost leaf	*/
    while( ! no->no_L.nsKey )
	{ no= no->no_L.sl_nod; }

    while( no )	/* for all leaves	*/
	{
	if  ( (*callback)( no->no_L.nsKey, no->no_L.sl_val, through ) )
	    {
	    rval= -1;
	    if  ( stopOnFailure )
		{ break;	}
	    }

	if  ( no->no_M.nsKey )
	    {
	    if  ( (*callback)( no->no_M.nsKey, no->no_M.sl_val, through ) )
		{
		rval= -1;
		if  ( stopOnFailure )
		    { break;	}
		}

	    if  ( no->no_R.nsKey )
		{
		if  ( (*callback)( no->no_R.nsKey, no->no_R.sl_val, through ) )
		    {
		    rval= -1;
		    if  ( stopOnFailure )
			{ break;	}
		    }
		}
	    }

	no= treeGetNextLeaf( no );
	}

    tm->trStatusFlag= oldStatusFlag;

    if  ( stopOnFailure )
	{ return rval;	}
    else{ return 0;	}
    }

/************************************************************************/
/*									*/
/*  Free the whole tree. Possibly call a callback for all stored	*/
/*  values.								*/
/*									*/
/*  The function of the callback is to free the stored values without	*/
/*  having to traverse the tree twice.					*/
/*									*/
/************************************************************************/

int utilTreeFreeTree(	void *			tree,
			UTIL_TREE_CALLBACK	callback,
			void *			through )
    {
    TreeMapping *	tm= (TreeMapping *)tree;
    TreeNode *		no;
    TreeNode *		dn;

    if  ( tm->trStatusFlag != TRsINACTIVE )
	{ XDEB(tm->trStatusFlag); return -1;	}
    tm->trStatusFlag= TRsCLOSE;

    no= tm->tr_root;

    while( no )
	{
	/*  walk to leftmost leaf	*/
	while( ! no->no_L.nsKey )
	    { no= no->no_L.sl_nod; }

	if  ( callback )
	    { (*callback)( no->no_L.nsKey, no->no_L.sl_val, through );	}
	if  ( tm->trOwnKeys )
	    { treeFreeKey( no->no_L.nsKey );	}

	if  ( no->no_M.nsKey )
	    {
	    if  ( callback )
		{ (*callback)( no->no_M.nsKey, no->no_M.sl_val, through ); }
	    if  ( tm->trOwnKeys )
		{ treeFreeKey( no->no_M.nsKey );	}

	    if  ( no->no_R.nsKey )
		{
		if  ( callback )
		    { (*callback)( no->no_R.nsKey, no->no_R.sl_val, through ); }
		if  ( tm->trOwnKeys )
		    { treeFreeKey( no->no_R.nsKey );	}
		}
	    }

	for (;;)
	    {
	    dn= no->no_dn;
	    if  ( ! dn )
		{
		treeFreeNode( no );
		no= dn;
		break;
		}

	    /*  redescent from leftmost	*/
	    if  ( dn->no_L.sl_nod == no )
		{
		treeFreeNode( no );
		no= dn->no_M.sl_nod;
		break;
		}

	    /*  redescent from middle	*/
	    if  ( dn->no_M.sl_nod == no )
		{
		/*  is there a right hand branch ?	*/
		if  ( dn->no_R.nsKey )
		    { treeFreeNode( no ); no= dn->no_R.sl_nod;	break;	}
		}

	    /*  redescent from last	*/
	    treeFreeNode( no );
	    no= dn;
	    }
	}

    treeFreeTree( tree );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Utility callback: Free the values when a tree is closed.		*/
/*									*/
/************************************************************************/
/*									*/
/*ARGSUSED*/
int utilTreeFreeValue(	const char *	key,
			void *		val,
			void *		through )
    {
    if  ( val )
	{ free( val );	}
    
    return 0;
    }
