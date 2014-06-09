/************************************************************************/
/*									*/
/*  A range in a document.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_RANGE_H
#   define		DOC_EDIT_RANGE_H

#   include		"docEditPosition.h"

typedef struct EditRange
    {
    int			erBottomLevel;				/*  A	*/
    EditPosition	erStart;
    EditPosition	erEnd;
    } EditRange;

#   endif	/*	DOC_EDIT_RANGE_H	*/
