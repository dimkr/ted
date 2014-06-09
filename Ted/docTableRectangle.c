/************************************************************************/
/*									*/
/*  Book keeping on TableRectangle's					*/
/*									*/
/*  The colspan and rowspan fields are managed in a halfhearted way:	*/
/*  You need the table to do a correct administration. So their value	*/
/*  is the responsibility of the caller.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a Table Rectangle.					*/
/*									*/
/************************************************************************/

void docInitTableRectangle(	TableRectangle *	tr )
    {
    tr->trCol0= -1;
    tr->trCol1= -1;
    tr->trCol11= -1;

    tr->trRow00= -1;
    tr->trRow0= -1;
    tr->trRow1= -1;
    tr->trRow11= -1;

    tr->trCellRowspan= 0;
    tr->trCellColspan= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Intersect two TableRectangles.					*/
/*									*/
/************************************************************************/

int docIntersectTableRectangle(	TableRectangle *		tr,
				const TableRectangle *		tr1,
				const TableRectangle *		tr2 )
    {
    TableRectangle	res= *tr1;

    if  ( tr1->trRow00 != tr2->trRow00 )
	{ LLDEB(tr1->trRow00,tr2->trRow00); return 0;	}
    if  ( tr1->trRow11 != tr2->trRow11 )
	{ LLDEB(tr1->trRow11,tr2->trRow11); return 0;	}
    if  ( tr1->trCol11 != tr2->trCol11 )
	{ LLDEB(tr1->trCol11,tr2->trCol11); return 0;	}

    if  ( tr1->trCol1 < tr2->trCol0 )
	{ return 0;	}

    if  ( tr2->trCol1 < tr1->trCol0 )
	{ return 0;	}

    if  ( tr1->trRow1 < tr2->trRow0 )
	{ return 0;	}

    if  ( tr2->trRow1 < tr1->trRow0 )
	{ return 0;	}

    if  ( ! tr )
	{ return 1;	}

    if  ( tr1->trCol0 < tr2->trCol0 )
	{ res.trCol0= tr2->trCol0;	}
    else{ res.trCol0= tr1->trCol0;	}

    if  ( tr1->trRow0 < tr2->trRow0 )
	{ res.trRow0= tr2->trRow0;	}
    else{ res.trRow0= tr1->trRow0;	}

    if  ( tr1->trCol1 < tr2->trCol1 )
	{ res.trCol1= tr1->trCol1;	}
    else{ res.trCol1= tr2->trCol1;	}

    if  ( tr1->trRow1 < tr2->trRow1 )
	{ res.trRow1= tr1->trRow1;	}
    else{ res.trRow1= tr2->trRow1;	}

    res.trCellRowspan= 0;
    res.trCellColspan= 0;

    *tr= res; return 1;
    }

void docExpandTableRectangleToWholeTable(	TableRectangle *	tr )
    {
    tr->trRow0= tr->trRow00;
    tr->trRow1= tr->trRow11;
    tr->trCol0= 0;
    tr->trCol1= tr->trCol11;

    if  ( tr->trRow0 != tr->trRow1	||
	  tr->trCol0 != tr->trCol1	)
	{
	tr->trCellRowspan= 0;
	tr->trCellColspan= 0;
	}

    return;
    }

void docExpandTableRectangleToWholeRows(	TableRectangle *	tr )
    {
    tr->trCol0= 0;
    tr->trCol1= tr->trCol11;

    if  ( tr->trRow0 != tr->trRow1	||
	  tr->trCol0 != tr->trCol1	)
	{
	tr->trCellRowspan= 0;
	tr->trCellColspan= 0;
	}

    return;
    }

void docExpandTableRectangleToWholeColumns(	TableRectangle *	tr )
    {
    tr->trRow0= tr->trRow00;
    tr->trRow1= tr->trRow11;

    if  ( tr->trRow0 != tr->trRow1	||
	  tr->trCol0 != tr->trCol1	)
	{
	tr->trCellRowspan= 0;
	tr->trCellColspan= 0;
	}

    return;
    }

int docShiftTableRectangleByRows(	TableRectangle *	tr,
					int			rows )
    {
    TableRectangle	trShifted;
    TableRectangle	trTable;

    trShifted= *tr;
    trTable= *tr;

    trShifted.trRow0 += rows;
    trShifted.trRow1 += rows;

    docExpandTableRectangleToWholeTable( &trTable );

    if  ( ! docIntersectTableRectangle( &trShifted, &trShifted, &trTable ) )
	{ return -1;	}

    tr->trCellRowspan= 0;
    tr->trCellColspan= 0;

    *tr= trShifted; return 0;
    }

int docShiftTableRectangleByColumns(	TableRectangle *	tr,
					int			columns )
    {
    TableRectangle	trShifted;
    TableRectangle	trTable;

    trShifted= *tr;
    trTable= *tr;

    trShifted.trCol0 += columns;
    trShifted.trCol1 += columns;

    docExpandTableRectangleToWholeTable( &trTable );

    if  ( ! docIntersectTableRectangle( &trShifted, &trShifted, &trTable ) )
	{ return -1;	}

    tr->trCellRowspan= 0;
    tr->trCellColspan= 0;

    *tr= trShifted; return 0;
    }

