/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_OVERRIDE_TABLE_H
#   define	DOC_LIST_OVERRIDE_TABLE_H

#   include	"docListOverride.h"

typedef struct ListOverrideTable
    {
    ListOverride *	lotOverrides;
    int			lotOverrideCount;
    } ListOverrideTable;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListOverrideTable(	ListOverrideTable *		lot );
extern void docCleanListOverrideTable(	ListOverrideTable *		lot );

extern int docCopyListOverrideTable(	ListOverrideTable *		to,
					const ListOverrideTable *	from );

extern int docListOverrideTableAddOverride(
				ListOverrideTable *		lot,
				const ListOverride *		lo,
				int				idx,
				const int *			fontMap,
				const int *			colorMap );

extern int docMergeListOverrideIntoTable(
				ListOverrideTable *		lot,
				const ListOverride *		lo,
				const int *			fontMap,
				const int *			colorMap );

#   endif
