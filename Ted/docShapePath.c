/************************************************************************/
/*									*/
/*  Standard shape forms.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	"docShape.h"

#   include	<appDebugon.h>

/************************************************************************/

# define sva(x) sizeof(x)/sizeof(ShapeVertex), x 

static ShapeVertex SV_33[]=
    {
	{ 0, 0 },
	{ 1, 0 },
	{ 1, 1 },
    };
const ShapePath SP_33= { "SP_33", 1, 1, 0, sva(SV_33) };


static ShapeVertex SV_34[]=
    {
	{ 0, 0 },
	{ 1, 0 },
	{ 1, 1 },
	{ 2, 1 },
    };
const ShapePath SP_34= { "SP_34", 2, 1, 0, sva(SV_34) };


static ShapeVertex SV_LINE[]=
    {
	{ 0, 0 },
	{ 1, 1 },
    };
const ShapePath SP_LINE= { "SP_LINE", 1, 1, 0, sva(SV_LINE) };


static ShapeVertex SV_DIAMOND[]=
    {
	{ 1, 0 },
	{ 2, 1 },
	{ 1, 2 },
	{ 0, 1 },
    };
const ShapePath SP_DIAMOND= { "SP_DIAMOND", 2, 2, 1, sva(SV_DIAMOND) };


static ShapeVertex SV_RECTANGLE[]=
    {
	{ 0, 0 },
	{ 1, 0 },
	{ 1, 1 },
	{ 0, 1 },
    };
const ShapePath SP_RECTANGLE= { "SP_RECTANGLE", 1, 1, 1, sva(SV_RECTANGLE) };


static ShapeVertex SV_ISOSCELES_TRIANGLE[]=
    {
	{ 1, 0 },
	{ 2, 2 },
	{ 0, 2 },
    };
const ShapePath SP_ISOSCELES_TRIANGLE=
	{ "SP_ISOSCELES_TRIANGLE", 2, 2, 1, sva(SV_ISOSCELES_TRIANGLE) };


static ShapeVertex SV_FLOW_CHART_MERGE[]=
    {
	{ 0, 0 },
	{ 2, 0 },
	{ 1, 2 },
    };
const ShapePath SP_FLOW_CHART_MERGE=
	{ "SP_FLOW_CHART_MERGE", 2, 2, 1, sva(SV_FLOW_CHART_MERGE) };


static ShapeVertex SV_RIGHT_TRIANGLE[]=
    {
	{ 0, 0 },
	{ 1, 1 },
	{ 0, 1 },
    };
const ShapePath SP_RIGHT_TRIANGLE=
	{ "SP_RIGHT_TRIANGLE", 1, 1, 1, sva(SV_RIGHT_TRIANGLE) };


static ShapeVertex SV_PARALLELOGRAM[]=
    {
	{ 1, 0 },
	{ 4, 0 },
	{ 3, 1 },
	{ 0, 1 },
    };
const ShapePath SP_PARALLELOGRAM=
	{ "SP_PARALLELOGRAM", 4, 1, 1, sva(SV_PARALLELOGRAM) };


static ShapeVertex SV_TRAPEZOID[]=
    {
	{ 0, 0 },
	{ 4, 0 },
	{ 3, 1 },
	{ 1, 1 },
    };
const ShapePath SP_TRAPEZOID= { "SP_TRAPEZOID", 4, 1, 1, sva(SV_TRAPEZOID) };


static ShapeVertex SV_HEXAGON[]=
    {
	{ 1, 0 },	/*  0  */
	{ 3, 0 },	/*  1  */
	{ 4, 1 },	/*  2  */
	{ 3, 2 },	/*  3  */
	{ 1, 2 },	/*  4  */
	{ 0, 1 },	/*  5  */
    };
const ShapePath SP_HEXAGON= { "SP_HEXAGON", 4, 2, 1, sva(SV_HEXAGON) };


static ShapeVertex SV_OCTAGON[]=
    {
	{ 1, 0 },	/*  0  */
	{ 3, 0 },	/*  1  */
	{ 4, 1 },	/*  2  */
	{ 4, 3 },	/*  3  */
	{ 3, 4 },	/*  4  */
	{ 1, 4 },	/*  5  */
	{ 0, 3 },	/*  6  */
	{ 0, 1 },	/*  7  */
    };
const ShapePath SP_OCTAGON= { "SP_OCTAGON", 4, 4, 1, sva(SV_OCTAGON) };


static ShapeVertex SV_PLUS_SIGN[]=
    {
	{ 1, 0 },	/*  0  */
	{ 3, 0 },	/*  1  */
	{ 3, 1 },	/*  2  */
	{ 4, 1 },	/*  3  */
	{ 4, 3 },	/*  4  */
	{ 3, 3 },	/*  5  */
	{ 3, 4 },	/*  6  */
	{ 1, 4 },	/*  7  */
	{ 1, 3 },	/*  8  */
	{ 0, 3 },	/*  9  */
	{ 0, 1 },	/* 10  */
	{ 1, 1 },	/* 11  */
    };
const ShapePath SP_PLUS_SIGN= { "SP_PLUS_SIGN", 4, 4, 1, sva(SV_PLUS_SIGN) };


static ShapeVertex SV_PENTAGON[]=
    {
	{  5,  0 },	/*  0  */
	{ 10,  1 },	/*  1  */
	{  8,  3 },	/*  2  */
	{  2,  3 },	/*  3  */
	{  0,  1 },	/*  4  */
    };
const ShapePath SP_PENTAGON= { "SP_PENTAGON", 10, 3, 1, sva(SV_PENTAGON) };


static ShapeVertex SV_STAR[]=
    {
	{  5,  0 },	/*  0  */
	{  6,  4 },	/*  1  */
	{ 10,  4 },	/*  2  */
	{  7,  6 },	/*  3  */
	{  8, 10 },	/*  4  */
	{  5,  7 },	/*  5  */
	{  2, 10 },	/*  6  */
	{  3,  6 },	/*  7  */
	{  0,  4 },	/*  8  */
	{  4,  4 },	/*  9  */
    };
const ShapePath SP_STAR= { "SP_STAR", 10, 10, 1, sva(SV_STAR) };


static ShapeVertex SV_SEAL4[]=
    {
	{  5,  0 },	/*  0  */
	{  6,  4 },	/*  1  */
	{ 10,  5 },	/*  2  */
	{  6,  6 },	/*  3  */
	{  5, 10 },	/*  4  */
	{  4,  6 },	/*  5  */
	{  0,  5 },	/*  6  */
	{  4,  4 },	/*  7  */
    };
const ShapePath SP_SEAL4= { "SP_SEAL4", 10, 10, 1, sva(SV_SEAL4) };


static ShapeVertex SV_ARROW[]=
    {
	{  0,  1 },	/*  0  */
	{  3,  1 },	/*  1  */
	{  3,  0 },	/*  2  */
	{  4,  2 },	/*  3  */
	{  3,  4 },	/*  4  */
	{  3,  3 },	/*  5  */
	{  0,  3 },	/*  6  */
    };
const ShapePath SP_ARROW= { "SP_ARROW", 4, 4, 1, sva(SV_ARROW) };


static ShapeVertex SV_NOTCHED_RIGHT_ARROW[]=
    {
	{  0,  1 },	/*  0  */
	{  6,  1 },	/*  1  */
	{  6,  0 },	/*  2  */
	{  8,  2 },	/*  3  */
	{  6,  4 },	/*  4  */
	{  6,  3 },	/*  5  */
	{  0,  3 },	/*  6  */
	{  1,  2 },	/*  7  */
    };
const ShapePath SP_NOTCHED_RIGHT_ARROW=
	{ "SP_NOTCHED_RIGHT_ARROW", 8, 4, 1, sva(SV_NOTCHED_RIGHT_ARROW) };


static ShapeVertex SV_HOME_PLATE[]=
    {
	{  0,  0 },	/*  0  */
	{  3,  0 },	/*  1  */
	{  4,  1 },	/*  2  */
	{  3,  2 },	/*  3  */
	{  0,  2 },	/*  4  */
    };
const ShapePath SP_HOME_PLATE=
	{ "SP_HOME_PLATE", 4, 2, 1, sva(SV_HOME_PLATE) };


static ShapeVertex SV_CHEVRON[]=
    {
	{  0,  0 },	/*  0  */
	{  3,  0 },	/*  1  */
	{  4,  2 },	/*  2  */
	{  3,  4 },	/*  3  */
	{  0,  4 },	/*  4  */
	{  1,  2 },	/*  5  */
    };
const ShapePath SP_CHEVRON= { "SP_CHEVRON", 4, 4, 1, sva(SV_CHEVRON) };


static ShapeVertex SV_LEFT_ARROW[]=
    {
	{  1,  0 },	/*  0  */
	{  1,  1 },	/*  1  */
	{  4,  1 },	/*  2  */
	{  4,  3 },	/*  3  */
	{  1,  3 },	/*  4  */
	{  1,  4 },	/*  5  */
	{  0,  2 },	/*  6  */
    };
const ShapePath SP_LEFT_ARROW= { "SP_LEFT_ARROW", 4, 4, 1, sva(SV_LEFT_ARROW) };


static ShapeVertex SV_RIGHT_ARROW_CALLOUT[]=
    {
	{  0,  0 },	/*  0  */
	{ 10,  0 },	/*  1  */
	{ 10, 12 },	/*  2  */
	{ 12, 12 },	/*  3  */
	{ 12, 10 },	/*  4  */
	{ 15, 15 },	/*  5  */
	{ 12, 20 },	/*  6  */
	{ 12, 18 },	/*  7  */
	{ 10, 18 },	/*  8  */
	{ 10, 30 },	/*  9  */
	{  0, 30 },	/* 10  */
    };
const ShapePath SP_RIGHT_ARROW_CALLOUT=
	{ "SP_RIGHT_ARROW_CALLOUT", 15, 30, 1, sva(SV_RIGHT_ARROW_CALLOUT) };


static ShapeVertex SV_LEFT_ARROW_CALLOUT[]=
    {
	{  5,  0 },	/*  0  */
	{ 15,  0 },	/*  1  */
	{ 15, 30 },	/*  2  */
	{  5, 30 },	/*  3  */
	{  5, 18 },	/*  4  */
	{  3, 18 },	/*  5  */
	{  3, 20 },	/*  6  */
	{  0, 15 },	/*  7  */
	{  3, 10 },	/*  8  */
	{  3, 12 },	/*  9  */
	{  5, 12 },	/* 10  */
    };
const ShapePath SP_LEFT_ARROW_CALLOUT=
	{ "SP_LEFT_ARROW_CALLOUT", 15, 30, 1, sva(SV_LEFT_ARROW_CALLOUT) };


static ShapeVertex SV_UP_ARROW_CALLOUT[]=
    {
	{ 15,  0 },	/*  0  */
	{ 20,  3 },	/*  1  */
	{ 18,  3 },	/*  2  */
	{ 18,  5 },	/*  3  */
	{ 30,  5 },	/*  4  */
	{ 30, 15 },	/*  5  */
	{  0, 15 },	/*  6  */
	{  0,  5 },	/*  7  */
	{ 12,  5 },	/*  8  */
	{ 12,  3 },	/*  9  */
	{ 10,  3 },	/* 10  */
    };
const ShapePath SP_UP_ARROW_CALLOUT=
	{ "SP_UP_ARROW_CALLOUT", 30, 15, 1, sva(SV_UP_ARROW_CALLOUT) };


static ShapeVertex SV_DOWN_ARROW_CALLOUT[]=
    {
	{  0,  0 },	/*  0  */
	{ 30,  0 },	/*  1  */
	{ 30, 10 },	/*  2  */
	{ 18, 10 },	/*  3  */
	{ 18, 12 },	/*  4  */
	{ 20, 12 },	/*  5  */
	{ 15, 15 },	/*  6  */
	{ 10, 12 },	/*  7  */
	{ 12, 12 },	/*  8  */
	{ 12, 10 },	/*  9  */
	{  0, 10 },	/* 10  */
    };
const ShapePath SP_DOWN_ARROW_CALLOUT=
	{ "SP_DOWN_ARROW_CALLOUT", 30, 15, 1, sva(SV_DOWN_ARROW_CALLOUT) };


static ShapeVertex SV_LEFT_RIGHT_ARROW_CALLOUT[]=
    {
	{ 15,  0 },	/*  0  */
	{ 45,  0 },	/*  1  */
	{ 45, 12 },	/*  2  */
	{ 48, 12 },	/*  3  */
	{ 48, 10 },	/*  4  */
	{ 60, 15 },	/*  5  */
	{ 48, 20 },	/*  6  */
	{ 48, 18 },	/*  7  */
	{ 45, 18 },	/*  8  */
	{ 45, 30 },	/*  9  */
	{ 15, 30 },	/* 10  */
	{ 15, 18 },	/* 11  */
	{ 12, 18 },	/* 12  */
	{ 12, 20 },	/* 13  */
	{  0, 15 },	/* 14  */
	{ 12, 10 },	/* 15  */
	{ 12, 12 },	/* 16  */
	{ 15, 12 },	/* 17  */
    };
const ShapePath SP_LEFT_RIGHT_ARROW_CALLOUT=
	{
	"SP_LEFT_RIGHT_ARROW_CALLOUT",
	60, 30, 1, sva(SV_LEFT_RIGHT_ARROW_CALLOUT)
	};


static ShapeVertex SV_UP_DOWN_ARROW_CALLOUT[]=
    {
	{ 15,  0 },	/*  0  */
	{ 20, 12 },	/*  1  */
	{ 18, 12 },	/*  2  */
	{ 18, 15 },	/*  3  */
	{ 30, 15 },	/*  4  */
	{ 30, 45 },	/*  5  */
	{ 18, 45 },	/*  6  */
	{ 18, 48 },	/*  7  */
	{ 20, 48 },	/*  8  */
	{ 15, 60 },	/*  9  */
	{ 10, 48 },	/* 10  */
	{ 12, 48 },	/* 11  */
	{ 12, 45 },	/* 12  */
	{  0, 45 },	/* 13  */
	{  0, 15 },	/* 14  */
	{ 12, 15 },	/* 15  */
	{ 12, 12 },	/* 16  */
	{ 10, 12 },	/* 17  */
    };
const ShapePath SP_UP_DOWN_ARROW_CALLOUT=
	{
	"SP_UP_DOWN_ARROW_CALLOUT",
	30, 60, 1, sva(SV_UP_DOWN_ARROW_CALLOUT)
	};


static ShapeVertex SV_QUAD_ARROW_CALLOUT[]=
    {
	{  8,  0 },	/*  0  */
	{ 10,  2 },	/*  1  */
	{  9,  2 },	/*  2  */
	{  9,  4 },	/*  3  */
	{ 12,  4 },	/*  4  */
	{ 12,  7 },	/*  5  */
	{ 14,  7 },	/*  6  */
	{ 14,  6 },	/*  7  */
	{ 16,  8 },	/*  8  */
	{ 14, 10 },	/*  9  */
	{ 14,  9 },	/* 10  */
	{ 12,  9 },	/* 11  */
	{ 12, 12 },	/* 12  */
	{  9, 12 },	/* 13  */
	{  9, 14 },	/* 14  */
	{ 10, 14 },	/* 15  */
	{  8, 16 },	/* 16  */
	{  6, 14 },	/* 17  */
	{  7, 14 },	/* 18  */
	{  7, 12 },	/* 19  */
	{  4, 12 },	/* 20  */
	{  4,  9 },	/* 21  */
	{  2,  9 },	/* 22  */
	{  2, 10 },	/* 23  */
	{  0,  8 },	/* 24  */
	{  2,  6 },	/* 25  */
	{  2,  7 },	/* 26  */
	{  4,  7 },	/* 27  */
	{  4,  4 },	/* 28  */
	{  7,  4 },	/* 29  */
	{  7,  2 },	/* 30  */
	{  6,  2 },	/* 31  */
    };
const ShapePath SP_QUAD_ARROW_CALLOUT=
	{ "SP_QUAD_ARROW_CALLOUT", 16, 16, 1, sva(SV_QUAD_ARROW_CALLOUT) };


static ShapeVertex SV_LEFT_RIGHT_ARROW[]=
    {
	{  1,  1 },	/*  0  */
	{  3,  1 },	/*  1  */
	{  3,  0 },	/*  2  */
	{  4,  2 },	/*  3  */
	{  3,  4 },	/*  4  */
	{  3,  3 },	/*  5  */
	{  1,  3 },	/*  6  */
	{  1,  4 },	/*  7  */
	{  0,  2 },	/*  8  */
	{  1,  0 },	/*  9  */
    };
const ShapePath SP_LEFT_RIGHT_ARROW=
	{ "SP_LEFT_RIGHT_ARROW", 4, 4, 1, sva(SV_LEFT_RIGHT_ARROW) };


static ShapeVertex SV_UP_ARROW[]=
    {
	{  2,  0 },	/*  0  */
	{  4,  1 },	/*  1  */
	{  3,  1 },	/*  2  */
	{  3,  4 },	/*  3  */
	{  1,  4 },	/*  4  */
	{  1,  1 },	/*  5  */
	{  0,  1 },	/*  6  */
    };
const ShapePath SP_UP_ARROW= { "SP_UP_ARROW", 4, 4, 1, sva(SV_UP_ARROW) };


static ShapeVertex SV_DOWN_ARROW[]=
    {
	{  1,  0 },	/*  0  */
	{  3,  0 },	/*  1  */
	{  3,  3 },	/*  2  */
	{  4,  3 },	/*  3  */
	{  2,  4 },	/*  4  */
	{  0,  3 },	/*  5  */
	{  1,  3 },	/*  6  */
    };
const ShapePath SP_DOWN_ARROW=
	{ "SP_DOWN_ARROW", 4, 4, 1, sva(SV_DOWN_ARROW) };


static ShapeVertex SV_UP_DOWN_ARROW[]=
    {
	{  2,  0 },	/*  0  */
	{  4,  1 },	/*  1  */
	{  3,  1 },	/*  2  */
	{  3,  3 },	/*  3  */
	{  4,  3 },	/*  4  */
	{  2,  4 },	/*  5  */
	{  0,  3 },	/*  6  */
	{  1,  3 },	/*  7  */
	{  1,  1 },	/*  8  */
	{  0,  1 },	/*  9  */
    };
const ShapePath SP_UP_DOWN_ARROW=
	{ "SP_UP_DOWN_ARROW", 4, 4, 1, sva(SV_UP_DOWN_ARROW) };


static ShapeVertex SV_QUAD_ARROW[]=
    {
	{ 15,  0 },	/*  0  */
	{ 20,  6 },	/*  1  */
	{ 18,  6 },	/*  2  */
	{ 18, 12 },	/*  3  */
	{ 24, 12 },	/*  4  */
	{ 24, 10 },	/*  5  */
	{ 30, 15 },	/*  6  */
	{ 24, 20 },	/*  7  */
	{ 24, 18 },	/*  8  */
	{ 18, 18 },	/*  9  */
	{ 18, 24 },	/* 10  */
	{ 20, 24 },	/* 11  */
	{ 15, 30 },	/* 12  */
	{ 10, 24 },	/* 13  */
	{ 12, 24 },	/* 14  */
	{ 12, 18 },	/* 15  */
	{  6, 18 },	/* 16  */
	{  6, 20 },	/* 17  */
	{  0, 15 },	/* 18  */
	{  6, 10 },	/* 19  */
	{  6, 12 },	/* 20  */
	{ 12, 12 },	/* 21  */
	{ 12,  6 },	/* 22  */
	{ 10,  6 },	/* 23  */
    };
const ShapePath SP_QUAD_ARROW=
	{ "SP_QUAD_ARROW", 30, 30, 1, sva(SV_QUAD_ARROW) };


static ShapeVertex SV_LEFT_RIGHT_UP_ARROW[]=
    {
	{ 15,  0 },	/*  0  */
	{ 20,  3 },	/*  1  */
	{ 18,  3 },	/*  2  */
	{ 18,  5 },	/*  3  */
	{ 24,  5 },	/*  4  */
	{ 24,  4 },	/*  5  */
	{ 30,  6 },	/*  6  */
	{ 24,  8 },	/*  7  */
	{ 24,  7 },	/*  8  */
	{  6,  7 },	/*  9  */
	{  6,  8 },	/* 10  */
	{  0,  6 },	/* 11  */
	{  6,  4 },	/* 12  */
	{  6,  5 },	/* 13  */
	{ 12,  5 },	/* 14  */
	{ 12,  3 },	/* 15  */
	{ 10,  3 },	/* 16  */
    };
const ShapePath SP_LEFT_RIGHT_UP_ARROW=
	{ "SP_LEFT_RIGHT_UP_ARROW", 30, 8, 1, sva(SV_LEFT_RIGHT_UP_ARROW) };


static ShapeVertex SV_LEFT_UP_ARROW[]=
    {
	{  6,  0 },	/*  0  */
	{  8,  3 },	/*  1  */
	{  7,  3 },	/*  2  */
	{  7,  7 },	/*  3  */
	{  3,  7 },	/*  4  */
	{  3,  8 },	/*  5  */
	{  0,  6 },	/*  6  */
	{  3,  4 },	/*  7  */
	{  3,  5 },	/*  8  */
	{  5,  5 },	/*  9  */
	{  5,  3 },	/* 10  */
	{  4,  3 },	/* 11  */
    };
const ShapePath SP_LEFT_UP_ARROW=
	{ "SP_LEFT_UP_ARROW", 8, 8, 1, sva(SV_LEFT_UP_ARROW) };


static ShapeVertex SV_BENT_UP_ARROW[]=
    {
	{  6,  0 },	/*  0  */
	{  8,  1 },	/*  1  */
	{  7,  1 },	/*  2  */
	{  7,  3 },	/*  3  */
	{  0,  3 },	/*  4  */
	{  0,  2 },	/*  5  */
	{  5,  2 },	/*  6  */
	{  5,  1 },	/*  7  */
	{  4,  1 },	/*  8  */
    };
const ShapePath SP_BENT_UP_ARROW=
	{ "SP_BENT_UP_ARROW", 8, 3, 1, sva(SV_BENT_UP_ARROW) };


static ShapeVertex SV_FLOW_CHART_PUNCHED_CARD[]=
    {
	{ 1, 0 },
	{ 4, 0 },
	{ 4, 4 },
	{ 0, 4 },
	{ 0, 1 },
    };
const ShapePath SP_FLOW_CHART_PUNCHED_CARD=
	{
	"SP_FLOW_CHART_PUNCHED_CARD",
	4, 4, 1, sva(SV_FLOW_CHART_PUNCHED_CARD)
	};


static ShapeVertex SV_FLOW_CHART_MANUAL_INPUT[]=
    {
	{ 4, 0 },
	{ 4, 4 },
	{ 0, 4 },
	{ 0, 1 },
    };
const ShapePath SP_FLOW_CHART_MANUAL_INPUT=
	{
	"SP_FLOW_CHART_MANUAL_INPUT",
	4, 4, 1, sva(SV_FLOW_CHART_MANUAL_INPUT)
	};


static ShapeVertex SV_FLOW_CHART_OFF_PAGE_CONNECTOR[]=
    {
	{ 0, 0 },
	{ 4, 0 },
	{ 4, 3 },
	{ 2, 4 },
	{ 0, 3 },
    };
const ShapePath SP_FLOW_CHART_OFF_PAGE_CONNECTOR=
	{
	"SP_FLOW_CHART_OFF_PAGE_CONNECTOR",
	4, 4, 1, sva(SV_FLOW_CHART_OFF_PAGE_CONNECTOR)
	};


static ShapeVertex SV_WEDGE_RECT_CALLOUT[]=
    {
	{ 0, 0 },
	{ 4, 0 },
	{ 4, 4 },
	{ 2, 4 },
	{ 0, 5 },
	{ 1, 4 },
	{ 0, 4 },
    };
const ShapePath SP_WEDGE_RECT_CALLOUT=
	{
	"SP_WEDGE_RECT_CALLOUT",
	4, 4, 1, sva(SV_WEDGE_RECT_CALLOUT)
	};


