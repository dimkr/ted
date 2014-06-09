/************************************************************************/
/*									*/
/*  Positions in a document. To prevent crashes, all references are	*/
/*  to the paragraph number, rather than the paragraph object.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docEditPosition.h"

void docInitEditPosition(	EditPosition *	ep )
    {
    ep->epParaNr= -1;
    ep->epStroff= -1;
    }

void docAdjustEditPositionOffsetB(	EditPosition *	ep,
					int		paraNr,
					int		stroff,
					int		stroffShift )
    {
    if  ( ep->epParaNr == paraNr	&&
	  ep->epStroff >  stroff	)
	{
	ep->epStroff += stroffShift;

	if  ( ep->epStroff < stroff )
	    { ep->epStroff= stroff;	}
	}

    return;
    }

void docAdjustEditPositionOffsetE(	EditPosition *	ep,
					int		paraNr,
					int		stroff,
					int		stroffShift )
    {
    if  ( ep->epParaNr == paraNr	&&
	  ep->epStroff >= stroff	)
	{
	ep->epStroff += stroffShift;

	if  ( ep->epStroff < stroff )
	    { ep->epStroff= stroff;	}
	}

    return;
    }

void docAdjustEditPositionB(		EditPosition *	ep,
					int		paraNr,
					int		stroff,
					int		paraShift,
					int		stroffShift )
    {
    if  ( ep->epParaNr == paraNr	&&
	  ep->epStroff >  stroff	)
	{
	ep->epStroff += stroffShift;
	ep->epParaNr += paraShift;

	if  ( ep->epParaNr == paraNr	&&
	      ep->epStroff < stroff )
	    { ep->epStroff= stroff;	}

	if  ( ep->epParaNr < paraNr )
	    {
	    ep->epParaNr= paraNr;
	    ep->epStroff= 0;
	    }

	return;
	}

    if  ( ep->epParaNr > paraNr )
	{
	ep->epParaNr += paraShift;

	if  ( ep->epParaNr < paraNr )
	    {
	    ep->epParaNr= paraNr;
	    ep->epStroff= 0;
	    }
	}

    return;
    }

void docAdjustEditPositionE(		EditPosition *	ep,
					int		paraNr,
					int		stroff,
					int		paraShift,
					int		stroffShift )
    {
    if  ( ep->epParaNr == paraNr	&&
	  ep->epStroff >= stroff	)
	{
	ep->epStroff += stroffShift;
	ep->epParaNr += paraShift;

	if  ( ep->epParaNr == paraNr	&&
	      ep->epStroff < stroff )
	    { ep->epStroff= stroff;	}

	if  ( ep->epParaNr < paraNr )
	    {
	    ep->epParaNr= paraNr;
	    ep->epStroff= 0;
	    }

	return;
	}

    if  ( ep->epParaNr > paraNr )
	{
	ep->epParaNr += paraShift;

	if  ( ep->epParaNr < paraNr )
	    {
	    ep->epParaNr= paraNr;
	    ep->epStroff= 0;
	    }

	return;
	}

    return;
    }

