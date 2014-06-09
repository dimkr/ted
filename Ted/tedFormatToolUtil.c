/************************************************************************/
/*									*/
/*  Utility routines for the FormatTool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Validate a dimension						*/
/*									*/
/************************************************************************/

int tedFormatValidateDimension(		int *		pNewValue,
					int *		pChanged,
					APP_WIDGET	w,
					int		oldValue )
    {
    int				changed;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( ! appGetLengthFromTextWidget( w, &oldValue, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, oldValue, UNITtyPOINTS );
	appStringToTextWidget( w, scratch );

	if  ( changed )
	    { *pNewValue= oldValue;	}

	*pChanged= changed; return 0;
	}

    return -1;
    }

int tedFormatToolGetGapWidth(		const RowProperties *	rp,
					int *			pValue,
					APP_WIDGET		w )
    {
    int				value;
    int				changed;

    int				minWidth;
    const CellProperties *	cp;
    int				col;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    value= rp->rpHalfGapWidthTwips;
    if  ( appGetLengthFromTextWidget( w, &value, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    cp= rp->rpCells;
    minWidth= cp->cpRightBoundaryTwips- rp->rpLeftIndentTwips;

    for ( col= 1; col < rp->rpCellCount; cp++, col++ )
	{
	int	cellWidth;

	cellWidth= cp[1].cpRightBoundaryTwips- cp[0].cpRightBoundaryTwips;

	if  ( minWidth > cellWidth )
	    { minWidth=  cellWidth;	}
	}

    if  ( 2* value >= minWidth )
	{
	char		scratch[50];

	value= minWidth/3;

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( w, scratch );
	appRefuseTextValue( w );
	return 1;
	}

    *pValue= value;
    return 0;
    }

int tedFormatToolGetRowLeftIndent(	const RowProperties *	rp,
					const int		pageLeftMargin,
					int *			pValue,
					APP_WIDGET		w )
    {
    int				value;
    int				changed;

    char			scratch[50];

    const int			minValue= INT_MIN;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    value= rp->rpLeftIndentTwips;
    if  ( appGetLengthFromTextWidget( w, &value, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( value < -pageLeftMargin )
	{
	value= -pageLeftMargin+ 10;

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( w, scratch );
	appRefuseTextValue( w );
	return 1;
	}

    if  ( value+ 2* rp->rpHalfGapWidthTwips >=
					rp->rpCells[0].cpRightBoundaryTwips )
	{
	value= rp->rpCells[0].cpRightBoundaryTwips- 3* rp->rpHalfGapWidthTwips;

	if  ( value < -pageLeftMargin )
	    { appRefuseTextValue( w ); return -1;	}

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( w, scratch );
	appRefuseTextValue( w );
	return 1;
	}

    *pValue= value; return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a candidate shading for a table rectangle from a shading	*/
/*  tool.								*/
/*									*/
/*  1)  Get shading with expanded colors.				*/
/*  2)  Reduce colors to palette indexes.				*/
/*  3)  Transfer to the row properties.					*/
/*									*/
/************************************************************************/

int tedFormatToolGetShading(	PropertyMask *		cpSetMask,
				RowProperties *		rp,
				int			col0,
				int			col1,
				DocumentProperties *	dp,
				ShadingTool *		st )
    {
    PropertyMask		isSetMask;

    ExpandedItemShading		eis;
    ItemShading			is;

    docInitExpandedItemShading( &eis );
    docInitItemShading( &is );

    PROPmaskCLEAR( &isSetMask );

    /*  1  */
    if  ( tedShadingToolGetShading( &eis, &isSetMask, st ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilPropMaskIsEmpty( &isSetMask ) )
	{
	PropertyMask		isDoneMask;

	PROPmaskCLEAR( &isDoneMask );

	/*  2  */
	if  ( docIndirectItemShading( &isDoneMask, &is, &isSetMask, &eis,
				    &(dp->dpColors), &(dp->dpColorCount) ) )
	    { LDEB(1); return -1;	}

	/*  3  */
	docRowSetShadingInCols( rp, col0, col1, &isSetMask, &is );

	docShadingMaskToCellMask( &isSetMask, &isSetMask );

	utilPropMaskOr( cpSetMask, cpSetMask, &isSetMask );
	}

    return 0;
    }

