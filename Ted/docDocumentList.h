/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/*  Refer to page 21 of the RTF 1.7 document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_LIST_H
#   define	DOC_DOCUMENT_LIST_H

#   include	"docListLevel.h"

typedef struct DocumentList
    {
    int			dlListID;
    int			dlListTemplateID;
    int			dlListHybrid;
    int			dlRestartPerSect;
    char *		dlListName;

    int			dlListStyleID;
    char *		dlListStyleName;

    DocumentListLevel	dlLevels[DLmaxLEVELS];
    int			dlLevelCount;
    } DocumentList;

typedef enum DocumentListProperty
    {
    DLpropLISTID,
    DLpropTEMPLATEID,
    DLpropHYBRID,
    DLpropRESTARTHDN,
    DLpropSTYLEID,
    DLpropSTYLENAME,

    DLpropNAME,

    DLpropLEVELS,

    DLprop_COUNT
    } DocumentListProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentList(	DocumentList *		dl );
extern void docCleanDocumentList(	DocumentList *		dl );

extern int docCopyDocumentList(	DocumentList *		to,
				const DocumentList *	from,
				const int *		fontMap,
				const int *		colorMap );

extern int docDocumentListAddLevel(
				DocumentList *			dl,
				const DocumentListLevel *	dll,
				const int *			fontMap,
				const int *			colorMap );

#   endif
