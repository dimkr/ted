/************************************************************************/
/*									*/
/*  Infrastructure for building and managing hierarchy trees.		*/
/*									*/
/************************************************************************/

#   include	<appFrame.h>

typedef struct TreeNode
    {
    /*  1  */
    int			tnChildCount;
    int			tnNumberInParent;
    struct TreeNode **	tnChildren;
    struct TreeNode *	tnParent;

    /*  2  */
    unsigned long	tnId;
    char *		tnLabel;
    char *		tnIconName;
    APP_BITMAP_IMAGE	tnPixmap;
    APP_BITMAP_MASK	tnMask;
    int			tnLabelStrlen;
    int			tnLabelWidth;

    /*  3  */
    int			tnIsOpen;
    int			tnX0;
    int			tnY0;
    int			tnY1;
    } TreeNode;

typedef struct TreeDocument
    {
    TreeNode *		tdRootNode;
    TreeNode *		tdCurrentNode;

    APP_COLOR_RGB	tdBlackColor;
    APP_COLOR_RGB	tdWhiteColor;
    APP_COLOR_RGB	tdLineColor;
    APP_COLOR_RGB *	tdCurrentColor;
    APP_FONT *		tdFontStruct;

    int			tdGridSize;
    } TreeDocument;

typedef enum ClickPosition
    {
    CLICKposSTRIP,
    CLICKposOPEN,
    CLICKposICON,
    CLICKposLABEL
    } ClickPosition;

#   define	appTreeMakeCurrent( td, tn ) (td)->tdCurrentNode= (tn)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern TreeNode * appTreeAddNode(	TreeDocument *		td );

extern void appInitTreeDocument(	TreeDocument *		td );

extern void appCleanTreeDocument(	TreeDocument *		td );

extern void appDeleteNode(		TreeDocument *		td,
					TreeNode *		tn );

extern int appTreeSetLabel(		TreeNode *		tn,
					const char *		label );

extern int appTreeSetIconName(		TreeNode *		tn,
					const char *		iconName );

extern int appLayoutTree(		TreeDocument *		td,
					AppDrawingData *	add,
					EditApplication *	ea );

extern int appDrawTree(			TreeDocument *			td,
					AppDrawingData *		add,
					const DocumentRectangle *	drClip,
					int				ox,
					int				oy );

extern TreeNode * appTreeFindNode(	TreeDocument *		td,
					int *			pPosition,
					int			x,
					int			y );

extern int appTreeProcessMouseClick( TreeDocument *		td,
				EditApplication *		ea,
				AppDrawingData *		add,
				APP_MOUSE_CLICK_EVENT *		bevent,
				int				ox,
				int				oy );

extern int appTreeAllocateResources(	TreeDocument *		td,
					AppDrawingData *	add,
					AppColors *		ac,
					const char *		font );

void appDrawTreeRectangle(	TreeDocument *		td,
				AppDrawingData *	add,
				DocumentRectangle *	drVisible,
				DocumentRectangle *	drClip,
				int			ox,
				int			oy );

