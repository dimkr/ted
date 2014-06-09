/************************************************************************/
/*									*/
/*  Positions in a document. To prevent crashes, all references are	*/
/*  to the paragraph number, rather than the paragraph object.		*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_POSITION_H
#   define		DOC_EDIT_POSITION_H

typedef struct EditPosition
    {
    int		epParaNr;
    int		epStroff;
    } EditPosition;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitEditPosition(		EditPosition *	ep );

extern void docAdjustEditPositionOffsetB(	EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		stroffShift );

extern void docAdjustEditPositionOffsetE(	EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		stroffShift );

extern void docAdjustEditPositionB(		EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		paraShift,
						int		stroffShift );

extern void docAdjustEditPositionE(		EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		paraShift,
						int		stroffShift );

#   endif	/*	DOC_EDIT_POSITION_H	*/
