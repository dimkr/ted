/************************************************************************/
/*									*/
/*  Infrastructure for building and managing hierarchy trees.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"appTree.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Add a node to a tree.. As the last child of the current node.	*/
/*									*/
/************************************************************************/

TreeNode * appTreeAddNode(	TreeDocument *		td )
    {
    TreeNode *	tn;

    if  ( td->tdRootNode && ! td->tdCurrentNode )
	{ XXDEB(td->tdRootNode,td->tdCurrentNode); return (TreeNode *)0; }

    tn= (TreeNode *)malloc( sizeof(TreeNode) );
    if  ( ! tn )
	{ XDEB(tn); return tn;	}

    tn->tnChildCount= 0;
    tn->tnChildren= (TreeNode **)0;
    tn->tnParent= (TreeNode *)0;
    tn->tnNumberInParent= -1;

    tn->tnId= 0;
    tn->tnLabel= (char *)0;
    tn->tnLabelStrlen= 0;
    tn->tnLabelWidth= 0;
    tn->tnIconName= (char *)0;
    tn->tnPixmap= (APP_BITMAP_IMAGE)0;
    tn->tnMask= (APP_BITMAP_MASK)0;

    tn->tnIsOpen= 0;
    tn->tnX0= 0;
    tn->tnY0= 0;
    tn->tnY1= 0;

    if  ( ! td->tdRootNode )
	{ td->tdRootNode= tn;	}
    else{
	TreeNode **	fresh;
	TreeNode *	cur= td->tdCurrentNode;

	fresh= (TreeNode **)realloc( cur->tnChildren,
			    ( cur->tnChildCount+ 1 )* sizeof( TreeNode * ) );
	if  ( ! fresh )
	    {
	    LXDEB(cur->tnChildCount,fresh);
	    free( tn ); return (TreeNode *)0;
	    }

	cur->tnChildren= fresh;
	cur->tnChildren[cur->tnChildCount]= tn;
	tn->tnParent= cur;
	tn->tnNumberInParent= cur->tnChildCount++;
	}

    return tn;
    }

/************************************************************************/
/*									*/
/*  Initialise a tree document.						*/
/*									*/
/************************************************************************/

void appInitTreeDocument(	TreeDocument *	td )
    {
    td->tdRootNode= (TreeNode *)0;
    td->tdCurrentNode= (TreeNode *)0;

    td->tdFontStruct= (APP_FONT *)0;

    td->tdGridSize= 24;
    }

/************************************************************************/
/*									*/
/*  Clean a tree document.						*/
/*									*/
/************************************************************************/

void appCleanTreeDocument(	TreeDocument *	td )
    {
    if  ( td->tdRootNode )
	{ appDeleteNode( td, td->tdRootNode ); }
    }

/************************************************************************/
/*									*/
/*  Delete a node from a tree.						*/
/*									*/
/************************************************************************/

void appDeleteNode(		TreeDocument *	td,
				TreeNode *	tn )
    {
    int		i;
    TreeNode **	ch;

    ch= tn->tnChildren;
    for ( i= 0; i < tn->tnChildCount; ch++, i++ )
	{
	TreeNode *	child= *ch;

	child->tnParent= (TreeNode *)0;
	appDeleteNode( td, child );
	}

    if  ( tn->tnParent )
	{
	int		count;

	tn->tnParent->tnChildCount--;

	count= tn->tnParent->tnChildCount;
	ch= tn->tnParent->tnChildren+ tn->tnNumberInParent;

	for ( i= tn->tnNumberInParent; i < count; ch++, i++ )
	    { ch[0]= ch[1]; }
	}

    if  ( tn->tnLabel )
	{ free( tn->tnLabel );		}
    if  ( tn->tnIconName )
	{ free( tn->tnIconName );	}

    free( tn );

    return;
    }

/************************************************************************/
/*									*/
/*  Set the label/icon name of a node.					*/
/*									*/
/************************************************************************/

int appTreeSetLabel(		TreeNode *		tn,
				const char *		label )
    {
    char *	copy= (char *)0;

    if  ( label )
	{
	copy= strdup( label );
	if  ( ! copy )
	    { XDEB(copy); return -1;	}
	}

    if  ( tn->tnLabel )
	{ free( tn->tnLabel );	}

    tn->tnLabel= copy;
    if  ( tn->tnLabel )
	{ tn->tnLabelStrlen= strlen( tn->tnLabel );	}

    tn->tnLabelWidth= 0;
    return 0;
    }

int appTreeSetIconName(		TreeNode *		tn,
				const char *		iconName )
    {
    char *	copy= (char *)0;

    if  ( iconName )
	{
	copy= strdup( iconName );
	if  ( ! copy )
	    { XDEB(copy); return -1;	}
	}

    if  ( tn->tnIconName )
	{ free( tn->tnIconName );	}

    tn->tnIconName= copy;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the layout of a tree.					*/
/*									*/
/************************************************************************/

static int appTreeLayoutNode(		TreeDocument *		td,
					AppDrawingData *	add,
					EditApplication *	ea,
					TreeNode *		tn,
					int			x,
					int			y,
					int *			pY )
    {
    if  ( tn->tnIconName						&&
	  ! tn->tnPixmap						&&
	  appGetImagePixmap( ea, tn->tnIconName,
				    &(tn->tnPixmap), &(tn->tnMask) )  	)
	{ SDEB(tn->tnIconName); return -1;	}

    if  ( tn->tnLabel			&&
	  tn->tnLabelWidth == 0		&&
	  td->tdFontStruct		)
	{
	int	fontAscent;
	int	fontDescent;

	appDrawTextExtents( &(tn->tnLabelWidth), &fontAscent, &fontDescent,
		    add, td->tdFontStruct, tn->tnLabel, tn->tnLabelStrlen );
	}

    tn->tnX0= x;
    tn->tnY0= y;

    x += td->tdGridSize;
    y += td->tdGridSize;

    if  ( ! tn->tnParent || tn->tnIsOpen )
	{
	int		i;
	TreeNode **	ch;

	ch= tn->tnChildren;
	for ( i= 0; i < tn->tnChildCount; ch++, i++ )
	    {
	    TreeNode *	child= *ch;

	    if  ( appTreeLayoutNode( td, add, ea, child, x, y, &y ) )
		{ LDEB(1); return -1;	}
	    }
	}

    tn->tnY1= y;

    *pY= y; return 0;
    }

int appLayoutTree(		TreeDocument *		td,
				AppDrawingData *	add,
				EditApplication *	ea )
    {
    int		y;

    if  ( ! td->tdRootNode )
	{ return 0;	}

    return appTreeLayoutNode( td, add, ea, td->tdRootNode, 0, 0, &y );
    }

/************************************************************************/
/*									*/
/*  Drawing of trees.							*/
/*									*/
/************************************************************************/

static void appTreeSetLineMode(		TreeDocument *			td,
					AppDrawingData *		add )
    {
    static unsigned char	dot[]= { 1, 1 };

    appDrawSetForegroundColor( add, &(td->tdLineColor) );
    td->tdCurrentColor= &(td->tdLineColor);

    appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dot, sizeof( dot ) );
    }

static void appTreeUnsetLineMode(	TreeDocument *			td,
					AppDrawingData *		add )
    {
    appDrawSetForegroundColor( add, &(td->tdBlackColor) );
    td->tdCurrentColor= &(td->tdBlackColor);

    appDrawSetLineAttributes( add,
			1, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
			(const unsigned char *)0, 0 );
    }

static void appTreeOpenRect(		TreeDocument *			td,
					TreeNode *			tn,
					DocumentRectangle *		drOpen )
    {
    int		gridSize= td->tdGridSize;

    drOpen->drX0= tn->tnX0- ( 3* gridSize/ 4 )+ 1;
    drOpen->drY0= tn->tnY0+ ( 1* gridSize/ 4 )+ 1;

    drOpen->drX1= drOpen->drX0+ gridSize/2- 3;
    drOpen->drY1= drOpen->drY0+ gridSize/2- 3;

    return;
    }

static void appTreeIconRect(		TreeDocument *			td,
					TreeNode *			tn,
					DocumentRectangle *		drIcon )
    {
    int		gridSize= td->tdGridSize;

    drIcon->drX0= tn->tnX0;
    drIcon->drY0= tn->tnY0;

    drIcon->drX1= drIcon->drX0+ gridSize- 1;
    drIcon->drY1= drIcon->drY0+ gridSize- 1;

    return;
    }

static void appTreeLabelRect(		TreeDocument *			td,
					TreeNode *			tn,
					DocumentRectangle *		drLabel )
    {
    int		gridSize= td->tdGridSize;

    drLabel->drX0= tn->tnX0+ gridSize+ gridSize/ 2;
    drLabel->drY0= tn->tnY0;

    drLabel->drX1= drLabel->drX0+ tn->tnLabelWidth;
    drLabel->drY1= drLabel->drY0+ gridSize- 1;

    return;
    }

static int appDrawTreeNode(		TreeDocument *			td,
					TreeNode *			tn,
					AppDrawingData *		add,
					const DocumentRectangle *	drClip,
					int				ox,
					int				oy )
    {
    int		gridSize= td->tdGridSize;

    if  ( drClip && tn->tnX0 > drClip->drX1 )
	{
	if  ( tn->tnX0 > drClip->drX1 )
	    { return 0;	}

	if  ( tn->tnY0 > drClip->drY1 )
	    { return 0;	}

	if  ( tn->tnY1 < drClip->drY0 )
	    { return 0;	}
	}

    if  ( ( ! tn->tnParent || tn->tnIsOpen )	&&
	  tn->tnChildCount > 0			> 0 )
	{
	TreeNode *	tl= tn->tnChildren[tn->tnChildCount-1];

	if  ( td->tdCurrentColor != &(td->tdLineColor) )
	    { appTreeSetLineMode( td, add );	}

	appDrawDrawLine( add,
			tn->tnX0+ gridSize/ 2- ox, tn->tnY0+ gridSize/ 2- oy,
			tn->tnX0+ gridSize/ 2- ox, tl->tnY0+ gridSize/ 2- oy );
	}

    if  ( tn->tnParent )
	{
	if  ( td->tdCurrentColor != &(td->tdLineColor) )
	    { appTreeSetLineMode( td, add );	}

	appDrawDrawLine( add,
			tn->tnX0- gridSize/ 2- ox, tn->tnY0+ gridSize/ 2- oy,
			tn->tnX0+ gridSize/ 2- ox, tn->tnY0+ gridSize/ 2- oy );

	if  ( tn->tnChildCount > 0 )
	    {
	    DocumentRectangle	drOpen;

	    if  ( td->tdCurrentColor != &(td->tdWhiteColor) )
		{
		appDrawSetForegroundColor( add, &(td->tdWhiteColor) );
		td->tdCurrentColor= &(td->tdWhiteColor);
		}

	    appTreeOpenRect( td, tn, &drOpen );

	    appDrawFillRectangle( add,
			    drOpen.drX0- ox, drOpen.drY0- oy,
			    drOpen.drX1- drOpen.drX0+ 1,
			    drOpen.drY1- drOpen.drY0+ 1 );

	    if  ( td->tdCurrentColor != &(td->tdBlackColor) )
		{ appTreeUnsetLineMode( td, add );	}

	    appDrawDrawRectangle( add,
			    drOpen.drX0- ox, drOpen.drY0- oy,
			    drOpen.drX1- drOpen.drX0+ 1,
			    drOpen.drY1- drOpen.drY0+ 1 );

	    appDrawFillRectangle( add,
			    drOpen.drX0+ 2- ox,
			    tn->tnY0+ gridSize/ 2- oy,
			    drOpen.drY1- drOpen.drY0- 2, 1 );

	    if  ( ! tn->tnIsOpen )
		{
		appDrawFillRectangle( add,
			    tn->tnX0- gridSize/ 2- ox,
			    drOpen.drY0+ 2- oy,
			    1, drOpen.drY1- drOpen.drY0- 2 );
		}
	    }
	}

    if  ( tn->tnPixmap )
	{
	DocumentRectangle	drIcon;

	appTreeIconRect( td, tn, &drIcon );

	appDrawDrawPixmap( add, tn->tnPixmap,
					0, 0, gridSize, gridSize,
					drIcon.drX0- ox, drIcon.drY0- oy );
	}

    if  ( tn->tnLabel )
	{
	DocumentRectangle	drLabel;

	if  ( td->tdCurrentColor != &(td->tdBlackColor) )
	    { appTreeUnsetLineMode( td, add );	}

	appTreeLabelRect( td, tn, &drLabel );

	if  ( tn == td->tdCurrentNode )
	    {
	    appDrawFillRectangle( add,
			    drLabel.drX0- ox, drLabel.drY0- oy,
			    drLabel.drX1- drLabel.drX0+ 1,
			    drLabel.drY1- drLabel.drY0+ 1 );

	    if  ( td->tdCurrentColor != &(td->tdWhiteColor) )
		{
		appDrawSetForegroundColor( add, &(td->tdWhiteColor) );
		td->tdCurrentColor= &(td->tdWhiteColor);
		}
	    }

	appDrawDrawString( add, drLabel.drX0- ox,
				tn->tnY0+ ( 3* gridSize )/ 4- oy,
				tn->tnLabel, tn->tnLabelStrlen );
	}

    if  ( ! tn->tnParent || tn->tnIsOpen )
	{
	int		i;
	TreeNode **	ch;

	ch= tn->tnChildren;
	for ( i= 0; i < tn->tnChildCount; ch++, i++ )
	    {
	    TreeNode *	child= *ch;

	    if  ( appDrawTreeNode( td, child, add, drClip, ox, oy ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int appDrawTree(		TreeDocument *			td,
				AppDrawingData *		add,
				const DocumentRectangle *	drClip,
				int				ox,
				int				oy )
    {
    if  ( ! td->tdRootNode )
	{ return 0;	}

    return appDrawTreeNode( td, td->tdRootNode, add, drClip, ox, oy );
    }

/************************************************************************/
/*									*/
/*  Find the position where the tree was clicked.			*/
/*									*/
/************************************************************************/

TreeNode * appTreeFindNode(	TreeDocument *			td,
				int *				pPosition,
				int				x,
				int				y )
    {
    TreeNode *	tn= td->tdRootNode;

    while( tn && tn->tnY0 <= y && y < tn->tnY1 )
	{
	int		i;
	TreeNode **	ch;

	if  ( y < tn->tnY0+ td->tdGridSize )
	    { break;	}

	if  ( tn->tnParent && ! tn->tnIsOpen )
	    { tn= (TreeNode *)0; break;	}

	ch= tn->tnChildren;
	for ( i= 0; i < tn->tnChildCount; ch++, i++ )
	    {
	    TreeNode *	child= *ch;

	    if  ( child->tnY0 <= y && y < child->tnY1 )
		{ break;	}
	    }

	if  ( i >= tn->tnChildCount )
	    { tn= (TreeNode *)0; break;	}

	tn= tn->tnChildren[i];
	}

    if  ( tn )
	{
	DocumentRectangle	drPos;

	appTreeOpenRect( td, tn, &drPos );

	if  ( x >= drPos.drX0		&&
	      x <= drPos.drX1		&&
	      y >= drPos.drY0		&&
	      y <= drPos.drY1		)
	    { *pPosition= CLICKposOPEN; return tn;	}

	if  ( tn->tnIconName )
	    {
	    appTreeIconRect( td, tn, &drPos );

	    if  ( x >= drPos.drX0		&&
		  x <= drPos.drX1		&&
		  y >= drPos.drY0		&&
		  y <= drPos.drY1		)
		{ *pPosition= CLICKposICON; return tn;	}
	    }

	if  ( tn->tnLabel )
	    {
	    appTreeLabelRect( td, tn, &drPos );

	    if  ( x >= drPos.drX0		&&
		  x <= drPos.drX1		&&
		  y >= drPos.drY0		&&
		  y <= drPos.drY1		)
		{ *pPosition= CLICKposLABEL; return tn;	}

	    *pPosition= CLICKposSTRIP;
	    }
	}

    return tn;
    }

/************************************************************************/
/*									*/
/*  Process a mouse click in the tree.					*/
/*									*/
/************************************************************************/

int appTreeProcessMouseClick(	TreeDocument *			td,
				EditApplication *		ea,
				AppDrawingData *		add,
				APP_MOUSE_CLICK_EVENT *		bevent,
				int				ox,
				int				oy )
    {
    TreeNode *		tn;
    int			position;

    tn= appTreeFindNode( td, &position, bevent->x+ ox, bevent->y+ oy );

    if  ( ! tn )
	{ return -1;	}

    if  ( position == CLICKposOPEN )
	{
	tn->tnIsOpen= ! tn->tnIsOpen;

	appLayoutTree( td, add, ea );

	appExposeRectangle( add, 0, tn->tnY0- oy, 0, 0 );

	return 0;
	}

    if  ( position == CLICKposLABEL	||
	  position == CLICKposICON	)
	{
	if  ( td->tdCurrentNode )
	    {
	    appExposeRectangle( add, 0, td->tdCurrentNode->tnY0- oy,
							0, td->tdGridSize );
	    }

	td->tdCurrentNode= tn;
	appExposeRectangle( add, 0, td->tdCurrentNode->tnY0- oy,
							0, td->tdGridSize );

	return 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate colors and fonts for a tree.				*/
/*									*/
/************************************************************************/

int appTreeAllocateResources(		TreeDocument *		td,
					AppDrawingData *	add,
					AppColors *		ac,
					const char *		font )
    {
    if  ( appColorRgb( &(td->tdLineColor), ac, 80, 80, 80 ) )
	{ LDEB(1); return -1;	}
    if  ( appDrawBlackColor( add, &(td->tdBlackColor) ) )
	{ LDEB(1); return -1;	}
    if  ( appDrawWhiteColor( add, &(td->tdWhiteColor) ) )
	{ LDEB(1); return -1;	}

    td->tdFontStruct= appDrawOpenFont( add, font );
    if  ( ! td->tdFontStruct )
	{ SXDEB(font,td->tdFontStruct); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a rectangle of a hierarchy tree. Routine meant to be called	*/
/*  from an expose handler.						*/
/*									*/
/************************************************************************/

void appDrawTreeRectangle(	TreeDocument *		td,
				AppDrawingData *	add,
				DocumentRectangle *	drVisible,
				DocumentRectangle *	drClip,
				int			ox,
				int			oy )
    {
    DocumentRectangle		drRedraw;

    appDrawSetForegroundColor( add, &(td->tdWhiteColor) );
    td->tdCurrentColor= &(td->tdWhiteColor);

    if  ( td->tdFontStruct )
	{ appDrawSetFont( add, td->tdFontStruct ); }

    drRedraw= *drVisible;

    if  ( ! drClip							||
	  geoIntersectRectangle( &drRedraw, &drRedraw, drClip )		)
	{
	appDrawFillRectangle( add,
			    drRedraw.drX0- ox, drRedraw.drY0- oy,
			    drRedraw.drX1- drRedraw.drX0+ 1,
			    drRedraw.drY1- drRedraw.drY0+ 1 );
	}

    appDrawTree( td, add, drClip, ox, oy );

    return;
    }
