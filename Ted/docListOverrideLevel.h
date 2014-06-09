/************************************************************************/
/*									*/
/*  Levels in the Lists of the list table of a document.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_OVERRIDE_LEVEL_H
#   define	DOC_LIST_OVERRIDE_LEVEL_H

#   include	"docListLevel.h"

typedef struct ListOverrideLevel
    {
    unsigned char	lolOverrideStartAt;
    unsigned char	lolOverrideFormat;
    DocumentListLevel	lolListLevel;
    } ListOverrideLevel;

typedef enum ListOverrideLevelProperty
    {
    LOLpropSTARTAT,
    LOLpropFORMAT,
    LOLpropLEVEL,

    LOLprop_COUNT
    } ListOverrideLevelProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListOverrideLevel(	ListOverrideLevel *		lol );
extern void docCleanListOverrideLevel(	ListOverrideLevel *		lol );

extern int docCopyListOverrideLevel(
				ListOverrideLevel *		to,
				const ListOverrideLevel *	from,
				const int *			fontMap,
				const int *			colorMap );

#   endif
