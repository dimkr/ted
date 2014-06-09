/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/*  Levels in the tree correspond to the ilvl value of the paragraphs.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docListNumberTree.h"
#   include	"docListLevel.h"

#   define	LOG_TRANSACTIONS	0

/************************************************************************/
/*									*/
/*  Initialise a node.							*/
/*									*/
/************************************************************************/

static void docInitListNumberTreeNode(	ListNumberTreeNode *	lntn )
    {
    lntn->lntnParagraphNumber= -1;
    lntn->lntnLeafCount= 0;
    lntn->lntnChildren= (ListNumberTreeNode *)0;
    lntn->lntnChildCount= 0;
    lntn->lntnIsLeaf= 0;

    return;
    }

static void docCleanListNumberTreeNode(	ListNumberTreeNode *	lntn )
    {
    docFreeListNumberNodes( lntn->lntnChildren, lntn->lntnChildCount );

    return;
    }

/************************************************************************/
/*									*/
/*  Debugging:								*/
/*									*/
/*  1)  Get rid of 'not used' warning.					*/
/*									*/
/************************************************************************/

static void docDebugListNumberTreeNode(	int				indent,
					const ListNumberTreeNode *	lntn,
					int				before,
					int				after )
    {
    int			i;

    const char *	bb= "";
    const char *	aa= "";

    if  ( before >= 0				&&
	  lntn->lntnParagraphNumber >= 0	&&
	  lntn->lntnParagraphNumber <= before	)
	{ bb= "#+#"; }
    if  ( after >= 0 && lntn->lntnParagraphNumber >= after )
	{ aa= "#-#"; }

    appDebug( "%*sLEAVES= %d PARA= %d%s%s%s\n", 4* indent, "",
				    lntn->lntnLeafCount,
				    lntn->lntnParagraphNumber, bb, aa,
				    lntn->lntnIsLeaf?"*":"" );

    if  ( lntn->lntnChildCount > 0 && ! lntn->lntnChildren )
	{ LXDEB(lntn->lntnChildCount,lntn->lntnChildren); }

    if  ( lntn->lntnParagraphNumber >= 0 )
	{ before= lntn->lntnParagraphNumber;	}

    for ( i= 0; i < lntn->lntnChildCount- 1; i++ )
	{
	int		aft;

	aft= lntn->lntnChildren[i+ 1].lntnParagraphNumber;

	docDebugListNumberTreeNode( indent+ 1, &(lntn->lntnChildren[i]),
							    before, aft );

	if  ( lntn->lntnChildren[i].lntnParagraphNumber >= 0 )
	    { before= lntn->lntnChildren[i].lntnParagraphNumber;	}
	}

    if  ( i < lntn->lntnChildCount )
	{
	docDebugListNumberTreeNode( indent+ 1, &(lntn->lntnChildren[i]),
							    before, after );
	}

    return;
    }

void docLogListNumberTreeNode(		int				indent,
					const ListNumberTreeNode *	lntn )
    {
    docDebugListNumberTreeNode( indent, lntn, -1, -1 );
    return;
    }

static void docListNumberTreeLogPath(	ListNumberTreeNode **	path,
					int *			nums,
					int			level )
    {
    int		i;

    /*  1  */
    if  ( level < -1 )
	{ docListNumberTreeLogPath( path, nums, level+ 1 ); }

    appDebug( "PATH[%d]: ", level );

    for ( i= 0; i <= level; i++ )
	{
	ListNumberTreeNode *	node= path[i];

	if  ( ! node )
	    { XDEB(node); return; }
	if  ( nums[i] < 0 )
	    { LDEB(nums[i]); return; }

	if  ( nums[i] >= node->lntnChildCount )
	    {
	    appDebug( "%d[%d]. ",
			    node->lntnParagraphNumber,
			    nums[i] );
	    }
	else{
	    ListNumberTreeNode *	child;

	    child= &(node->lntnChildren[nums[i]]);

	    appDebug( "%d[%d]->%d%s ",
			    node->lntnParagraphNumber,
			    nums[i],
			    child->lntnParagraphNumber,
			    child->lntnIsLeaf?"*":"" );
	    }
	}

    appDebug( "\n" );
    }

/************************************************************************/
/*									*/
/*  Allocate nodes.							*/
/*									*/
/************************************************************************/

int docClaimListNumberTreeNodes(	ListNumberTreeNode **	pLntn,
					int *			pCount,
					int			count )
    {
    ListNumberTreeNode *	fresh;
    int				i;

    if  ( *pCount >= count )
	{ LLDEB(*pCount,count); return 0;	}

    fresh= (ListNumberTreeNode *)realloc( *pLntn,
				    count* sizeof( ListNumberTreeNode ) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    for ( i= *pCount; i < count; i++ )
	{ docInitListNumberTreeNode( &(fresh[i]) );	}

    *pLntn= fresh;
    *pCount= count;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Clean and free an array of nodes.					*/
/*									*/
/************************************************************************/

void docFreeListNumberNodes(	ListNumberTreeNode *	nodes,
				int			count )
    {
    int				i;
    ListNumberTreeNode *	lntn= nodes;

    for ( i= 0; i < count; lntn++, i++ )
	{ docFreeListNumberNodes( lntn->lntnChildren, lntn->lntnChildCount ); }

    if  ( nodes )
	{ free( nodes );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find a paragraph in the number tree.				*/
/*									*/
/*  1)  From root to leaves..						*/
/*  2)  Look for a child that is on, or past the paragraph that we look	*/
/*	for.								*/
/*  3)  Found?								*/
/*  4)  Enter the child before, or if there is none, report this as the	*/
/*	possible insertion point.					*/
/*  5)  Not found.. Report where.					*/
/*									*/
/*  path is the series of parent nodes that leads us to the node.	*/
/*  nums is the series of child node numbers if the corresponding	*/
/*	positions in 'path'. If the node is found, the value is the	*/
/*	index of the node. If nothing is found, the value is the index	*/
/*	where it would be inserted at that particular level.		*/
/*									*/
/************************************************************************/

static int docListNumberTreeFindParagraph(
					int *			pLevel,
					int *			pAfter,
					ListNumberTreeNode **	path,
					int *			nums,
					ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	node= root;
    ListNumberTreeNode *	child= (ListNumberTreeNode *)0;

    int				after= 0;
    int				level= 0;
    int				pos= 0;
    int				cmp= -1;

    /*  1  */
    while( node->lntnChildCount > 0 )
	{
	path[level]= node;

	/*  2  */
	cmp= 1;
	child= (ListNumberTreeNode *)0;
	for ( pos= 0; pos < node->lntnChildCount; pos++ )
	    {
	    child= &(node->lntnChildren[pos]);

	    cmp= paraNr- child->lntnParagraphNumber;
	    if  ( cmp <= 0 )
		{ break;	}
	    }

	/*  3  */
	if  ( cmp == 0 && child->lntnIsLeaf )
	    {
	    nums[level]= pos;
	    *pAfter= after;
	    *pLevel= level;
	    return 0;
	    }

	/*  4  */
	if  ( pos == 0 )
	    { after= 1; break;	}
	pos--;

	nums[level]= pos;
	node= &(node->lntnChildren[pos]);
	level++;
	}

    /*  5  */
    path[level]= node;
    nums[level]= 0;

    *pAfter= after;
    *pLevel= level;
    return 1;
    }

/************************************************************************/
/*									*/
/*  1)	Determine the list numbers on the path to a certain paragraph.	*/
/*  2)	Determine the list numbers on the path to the predecessor of a	*/
/*	certain paragraph.						*/
/*									*/
/************************************************************************/

/*  1  */
int docListNumberTreeGetNumberPath(	int *			nums,
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				level= 0;
    int				after= 0;

    if  ( ilvl < 0 || ilvl >= DLmaxLEVELS )
	{ LLDEB(ilvl,DLmaxLEVELS); return -1;	}

    /* LLDEB(ilvl,paraNr);docLogListNumberTreeNode(0,root); */

    if  ( docListNumberTreeFindParagraph( &level, &after,
						path, nums, root, paraNr ) )
	{
	LDEB(paraNr); docLogListNumberTreeNode(0,root);
	return -1;
	}

    if  ( level != ilvl )
	{ LLDEB(level,ilvl);	}

    return 0;
    }

/*  2  */
static int docListNumberTreePrevPath(	int *			nums,
					int *			pLevel,
					ListNumberTreeNode **	path,
					int			level )
    {
    int				ilvl= level;
    ListNumberTreeNode *	node;

    while( ilvl >= 0 && nums[ilvl] == 0 )
	{
	if  ( path[ilvl]->lntnParagraphNumber >= 0 )
	    {
	    *pLevel= ilvl- 1;
	    return 0;
	    }

	ilvl--;
	}

    if  ( ilvl < 0 )
	{ return 1;	}

    node= path[ilvl];
    nums[ilvl]--;
    node= &(node->lntnChildren[nums[ilvl]]);
    ilvl++;
    while( node->lntnChildCount > 0 )
	{
	nums[ilvl]= node->lntnChildCount- 1;
	node= &(node->lntnChildren[nums[ilvl]]);
	ilvl++;
	}

    *pLevel= ilvl- 1;
    return 0;
    }

/*  2  */
int docListNumberTreeGetPrevPath(	int *			nums,
					int *			pLevel,
					ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				level= 0;
    int				after= 0;

    /* SLDEB("PREV",paraNr);docLogListNumberTreeNode(0,root); */

    if  ( docListNumberTreeFindParagraph( &level, &after,
						path, nums, root, paraNr ) )
	{ LDEB(paraNr); return -1;	}

    return docListNumberTreePrevPath( nums, pLevel, path, level );
    }

/************************************************************************/
/*									*/
/*  Move node from one parent to another.				*/
/*									*/
/*  1)  Allocate memory.						*/
/*  2)  Open hole.							*/
/*  3)  Move nodes.							*/
/*  4)  Close hole.							*/
/*  5)  Fix counts.							*/
/*									*/
/************************************************************************/

static int docListNumberTreeMoveChildren(	ListNumberTreeNode *	to,
						int			topos,
						ListNumberTreeNode *	fr,
						int			frpos,
						int			cnt )
    {
    int		leavesMoved= 0;
    int		i;

    /*
    */
#   if LOG_TRANSACTIONS
    appDebug( "MOVE %d CHILDREN FROM %d[%d] TO %d[%d]\n", cnt,
			    fr->lntnParagraphNumber,
			    fr->lntnChildCount,
			    to->lntnParagraphNumber,
			    to->lntnChildCount );
#   endif

    /*  1  */
    if  ( docClaimListNumberTreeNodes( &(to->lntnChildren),
				       &(to->lntnChildCount),
				       to->lntnChildCount+ cnt ) )
	{ LDEB(cnt); return -1;	}

    /*  2  */
    for ( i= to->lntnChildCount- 1; i >= topos+ cnt; i-- )
	{
	to->lntnChildren[i]= to->lntnChildren[i- cnt];
	}

    /*  3  */
    for ( i= 0; i < cnt; i++ )
	{
	/*
	*/
#	if LOG_TRANSACTIONS
	appDebug( "    MOVE %d FROM %d[%d] TO %d[%d]\n",
			fr->lntnChildren[frpos+ i].lntnParagraphNumber,
			fr->lntnParagraphNumber, frpos+ i,
			to->lntnParagraphNumber, topos+ i );
#	endif

	to->lntnChildren[topos+ i]= fr->lntnChildren[frpos+ i];
	leavesMoved += to->lntnChildren[topos+ i].lntnLeafCount;
	}

    /*  4  */
    if  ( fr->lntnChildCount > frpos+ cnt )
	{
	for ( i= 0; i < cnt; i++ )
	    { fr->lntnChildren[frpos+ i]= fr->lntnChildren[frpos+ i+ cnt]; }
	}

    /*  5  */
    fr->lntnChildCount -= cnt;
    to->lntnLeafCount += leavesMoved;
    fr->lntnLeafCount -= leavesMoved;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a child in a parent node.					*/
/*									*/
/*  1)  Allocate memory.						*/
/*  2)  Open hole.							*/
/*  3)  Initialize fresh child.						*/
/*									*/
/************************************************************************/

static ListNumberTreeNode * docListNumberTreeInsertChild(
					ListNumberTreeNode *	node,
					int			pos )
    {
    int		i;

    /*  1  */
    if  ( docClaimListNumberTreeNodes( &(node->lntnChildren),
		   &(node->lntnChildCount), node->lntnChildCount+ 1 ) )
	{ LDEB(node->lntnChildCount); return (ListNumberTreeNode *)0;	}

    /*  2  */
    for ( i= node->lntnChildCount- 1; i > pos; i-- )
	{ node->lntnChildren[i]= node->lntnChildren[i-1];	}

    /*  3  */
    docInitListNumberTreeNode( &(node->lntnChildren[pos]) );

    return &(node->lntnChildren[pos]);
    }

/************************************************************************/
/*									*/
/*  Insert a paragraph in a list number tree.				*/
/*									*/
/*  1)  Find a path to the insertion point.				*/
/*  2)  If it does not go up do the desired level, extend the tree upto	*/
/*	the desired level.						*/
/*  3)  Insert the new leaf at the correct position.			*/
/*  4)  Split deeper levels if necessary.				*/
/*									*/
/************************************************************************/

int docListNumberTreeInsertParagraph(	ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];

    ListNumberTreeNode *	node= root;
    ListNumberTreeNode *	fresh= root;
    int				level= 0;
    int				after= 0;

    int				pos;
    int				res;

#   if LOG_TRANSACTIONS
    SLLDEB("ADD",ilvl,paraNr);docLogListNumberTreeNode(0,root);
#   endif

    if  ( ilvl < 0 || ilvl >= DLmaxLEVELS )
	{ LLDEB(ilvl,DLmaxLEVELS); return -1;	}

    /*  1  */
    res= docListNumberTreeFindParagraph( &level, &after,
						path, nums, root, paraNr );
    if  ( res <= 0 )
	{ LLDEB(paraNr,res); return -1;	}

    if  ( level < 0 )
	{ LDEB(level); return -1;	}

#   if LOG_TRANSACTIONS
    SDEB("@@"); docListNumberTreeLogPath( path, nums, level );
#   endif

    /*  2  */
    if  ( level < ilvl )
	{
	node= path[level];
	pos= nums[level];

	while( level < ilvl )
	    {
	    fresh= docListNumberTreeInsertChild( node, pos );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}

	    fresh->lntnParagraphNumber= -1;
	    fresh->lntnLeafCount= 0; /* ++ below */
	    fresh->lntnIsLeaf= 0;

	    level++;

	    path[level]= node= fresh;
	    nums[level]= pos= 0;
	    }
	}

    node= path[ilvl];
    pos= nums[ilvl];

    if  ( pos == 0						&&
	  after							&&
	  node->lntnChildCount > 0				&&
	  node->lntnChildren[pos].lntnParagraphNumber == -1	)
	{
	node->lntnChildren[pos].lntnParagraphNumber= paraNr;
	node->lntnChildren[pos].lntnLeafCount++;
	node->lntnChildren[pos].lntnIsLeaf= 1;
	}
    else{
	if  ( level > ilvl )
	    { pos++;	}

	/*  3  */
	fresh= docListNumberTreeInsertChild( node, pos );
	if  ( ! fresh )
	    { XDEB(fresh); return -1;	}

	fresh->lntnParagraphNumber= paraNr;
	fresh->lntnLeafCount= 1;
	fresh->lntnIsLeaf= 1;
	}

    /*  4  */
    if  ( pos > 0 )
	{
	int			lev= ilvl+ 1;

	ListNumberTreeNode *	this= &(node->lntnChildren[pos   ]);

	while( lev <= level )
	    {
	    ListNumberTreeNode *	prev= &(node->lntnChildren[pos- 1]);
	    const int			topos= 0;

	    pos= nums[lev]+ 1;
	    if  ( after )
		{ pos--;	}

	    if  ( pos >= prev->lntnChildCount )
		{ break;	}

	    if  ( docListNumberTreeMoveChildren( this, topos,
				    prev, pos, prev->lntnChildCount- pos ) )
		{ LDEB(pos); return -1;	}

	    if  ( lev < level- 1 )
		{
		fresh= docListNumberTreeInsertChild( this, 0 );
		if  ( ! fresh )
		    { XDEB(fresh); return -1;	}

		fresh->lntnParagraphNumber= -1;
		fresh->lntnLeafCount= 0; /* ++ below */
		fresh->lntnIsLeaf= 0;
		}

	    this= &(this->lntnChildren[0]);
	    node= prev;

	    lev++;
	    after= 0;
	    }
	}

#   if LOG_TRANSACTIONS
    SLLDEB("ADDDED",ilvl,paraNr);docLogListNumberTreeNode(0,root);
#   endif
    return 0;
    }


/************************************************************************/
/*									*/
/*  Delete a paragraph from the number tree.				*/
/*									*/
/*  1)  Look for the node.						*/
/*  2)  Demote the node to an internal node.				*/
/*  3)  Try to merge children into predeceding nodes.			*/
/*  4)  Look for the top level of the merge.				*/
/*  5)  Recursively merge all children into predecessor nodes.		*/
/*  6)  If no children are left, remove the node from the tree.		*/
/*  7)  Remove empty parents (unless a leaf!)				*/
/*									*/
/************************************************************************/

int docListNumberTreeDeleteParagraph(	ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];

    ListNumberTreeNode *	node;
    ListNumberTreeNode *	this;

    int				level;
    int				after;
    int				pos;

    int				i;

#   if LOG_TRANSACTIONS
    SLDEB("DEL",paraNr);docLogListNumberTreeNode(0,root);
#   endif

    /*  1  */
    if  ( docListNumberTreeFindParagraph( &level, &after,
						path, nums, root, paraNr ) )
	{ /*LDEB(paraNr);*/ return -1;	}

#   if LOG_TRANSACTIONS
    SDEB("@@"); docListNumberTreeLogPath( path, nums, level );
#   endif

    node= path[level];
    pos= nums[level];
    this= &(node->lntnChildren[pos]);

    node->lntnLeafCount--;

    /*  2  */
    this->lntnParagraphNumber= -1;
    this->lntnIsLeaf= 0;

    /*  3  */
    if  ( this->lntnChildCount > 0 && pos > 0 )
	{
	int			top= level+ 1;
	ListNumberTreeNode *	prev;
	ListNumberTreeNode *	here;

	ListNumberTreeNode *	ppth[DLmaxLEVELS+1];
	int			pnms[DLmaxLEVELS+1];

	for ( i= 0; i < top; i++ )
	    {
	    ppth[i]= path[i];
	    pnms[i]= nums[i];
	    }

	prev= &(node->lntnChildren[pos- 1]);
	here= this;

	ppth[top]= node;
	pnms[top]= pos- 1;

	/*  4  */
	while( prev && here->lntnParagraphNumber == -1 )
	    {
	    if  ( here->lntnChildCount == 0 )
		{ break;	}

	    path[top]= here;
	    nums[top]= 0;

	    ppth[top]= prev;
	    if  ( prev->lntnChildCount == 0 )
		{ pnms[top]= 0;			}
	    else{ pnms[top]= prev->lntnChildCount- 1;	}

	    here= &(here->lntnChildren[0]);
	    prev= &(prev->lntnChildren[prev->lntnChildCount- 1]);

	    top++;
	    }

	/*  5  */
	top--;
	while( top > level )
	    {
	    ListNumberTreeNode *	p= ppth[top];
	    ListNumberTreeNode *	h= path[top];
	    int				skip= 0;

	    /*
	    SLDEB("PATH",top); docListNumberTreeLogPath( path, nums, top );
	    docLogListNumberTreeNode(top,path[top]);
	    SLDEB("PPTH",top); docListNumberTreeLogPath( ppth, pnms, top );
	    docLogListNumberTreeNode(top,ppth[top]);
	    */

	    if  ( h->lntnChildCount > 0				&&
		  h->lntnChildren[0].lntnParagraphNumber < 0	)
		{ skip= 1;	}

	    if  ( docListNumberTreeMoveChildren( p, p->lntnChildCount,
				    h, 0+ skip, h->lntnChildCount- skip ) )
		{ LDEB(pos); return -1;	}

	    if  ( skip )
		{
		docCleanListNumberTreeNode( &(h->lntnChildren[0]) );
		h->lntnChildCount--;
		}

	    top--;
	    }
	}

    /*  6  */
    if  ( this->lntnChildCount == 0 )
	{
	docCleanListNumberTreeNode( this );

	node->lntnChildCount--;
	for ( i= pos; i < node->lntnChildCount; i++ )
	    { node->lntnChildren[i]= node->lntnChildren[i+ 1]; }

	/*  7  */
	while( level > 0 && node->lntnChildCount == 0 )
	    {
	    level--;

	    node= path[level];
	    pos= nums[level];
	    this= &(node->lntnChildren[pos]);

	    if  ( this->lntnChildCount != 0 )
		{ LDEB(this->lntnChildCount);	}

	    if  ( this->lntnParagraphNumber < 0	)
		{
		docCleanListNumberTreeNode( this );

		node->lntnChildCount--;
		for ( i= pos; i < node->lntnChildCount; i++ )
		    { node->lntnChildren[i]= node->lntnChildren[i+ 1]; }
		}
	    }
	}

#   if LOG_TRANSACTIONS
    SLDEB("DELETED",paraNr);docLogListNumberTreeNode(0,root);
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt references from bullets to the text because the number of	*/
/*  paragraphs changed in the document.					*/
/*									*/
/*  1)  Loop: To use less recursion.. handle the last child of a node	*/
/*	in a loop.							*/
/*  2)  Handle the other children in a recursive call.			*/
/*  3)  Skip children before the first paragraph.			*/
/*									*/
/************************************************************************/

void docShiftListTreeReferences(	ListNumberTreeNode *	root,
					int			paraFrom,
					int			paraShift )
    {
    ListNumberTreeNode *	node= root;

    /*  1  */
    for (;;)
	{
	int		pos;

	if  ( node->lntnParagraphNumber >= paraFrom )
	    {
#	    if LOG_TRANSACTIONS
	    appDebug( "SHIFT %2d+ %d -> %2d\n",
			node->lntnParagraphNumber, paraShift,
			node->lntnParagraphNumber+ paraShift );
#	    endif

	    node->lntnParagraphNumber += paraShift;
	    }

	if  ( node->lntnChildCount == 0 )
	    { break;	}

	/*  2  */
	for ( pos= 0; pos < node->lntnChildCount- 1; pos++ )
	    {
	    /*  3  */
	    if  ( node->lntnChildren[pos+ 1].lntnParagraphNumber <= paraFrom )
		{ continue;	}

	    docShiftListTreeReferences( &(node->lntnChildren[pos]),
							paraFrom, paraShift );
	    }

	node= &(node->lntnChildren[node->lntnChildCount- 1]);
	}

    return;
    }

