/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_TABLE_H
#   define	DOC_LIST_TABLE_H

#   include	"docDocumentList.h"

typedef struct DocumentListTable
    {
    DocumentList *	dltLists;
    int			dltListCount;
    } DocumentListTable;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListTable(	DocumentListTable *		dlt );
extern void docCleanListTable(	DocumentListTable *		dlt );

extern int docCopyListTable(	DocumentListTable *		to,
				const DocumentListTable *	from );

extern int docDocumentListTableAddList(	DocumentListTable *	dlt,
					const DocumentList *	dl,
					const int *		fontMap,
					const int *		colorMap );

extern int docMergeListIntoTable(	DocumentListTable *	dlt,
					const DocumentList *	dl,
					const int *		fontMap,
					const int *		colorMap );

#   endif
