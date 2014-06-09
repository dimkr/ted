#   ifndef		DOC_LAYOUT_POSITION_H
#   define		DOC_LAYOUT_POSITION_H

/************************************************************************/
/*									*/
/*  Used to layout lines of text.					*/
/*									*/
/*  A0 paper is 4* 30 cm high: 28.3465* 120* 20= 68031.6 Twips.		*/
/*  So an unsigned short limits us to A1 paper.				*/
/*									*/
/************************************************************************/

typedef struct LayoutPosition
    {
    unsigned short	lpPageYTwips;
    unsigned short	lpPage;
    unsigned char	lpColumn;
    unsigned char	lpAtTopOfColumn;
    } LayoutPosition;

# define DOC_SAME_POSITION( lp1, lp2 ) ( \
		    (lp1)->lpPageYTwips == (lp2)->lpPageYTwips	&& \
		    (lp1)->lpPage == (lp2)->lpPage		&& \
		    (lp1)->lpColumn == (lp2)->lpColumn		)

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitLayoutPosition(	LayoutPosition *	lp );

#   endif	/*	DOC_LAYOUT_POSITION_H	*/
