/************************************************************************/
/*									*/
/*  Calculate 'Hyperlink' fields.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Evaluate a 'HYPERLINK' field.					*/
/*									*/
/*  Just make the contents blue as a compatibility hack with previous	*/
/*  versions.								*/
/*									*/
/************************************************************************/

int docRecalculateHyperlinkField(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraBi,
				int				partFrom,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject )
    {
    const int				partUpto= partFrom+ partCount+ 2;

    TextAttribute			taSet;
    PropertyMask			taSetMask;
    PropertyMask			taDoneMask;

    DocumentProperties *		dp= &(bd->bdProperties);
    int					blue;
    RGB8Color				rgb8Blue;

    if  ( ! getenv( "TED_HYPERLINKS_BLUE" ) )
	{ *pCalculated= 0; return 0;	}

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );
    PROPmaskCLEAR( &taDoneMask );

    bmInitRGB8Color( &rgb8Blue );
    rgb8Blue.rgb8Red= 0;
    rgb8Blue.rgb8Green= 0;
    rgb8Blue.rgb8Blue= 255;

    blue= docAllocateDocumentColor( dp, &rgb8Blue );
    if  ( blue < 0 )
	{ LDEB(blue); return -1;	}

    taSet.taTextColorNumber= blue;
    taSet.taTextIsUnderlined= 1;
    PROPmaskADD( &taSetMask, TApropTEXT_COLOR );
    PROPmaskADD( &taSetMask, TApropTEXTUNDERLINED );

    if  ( docChangeParticuleAttributes( &taDoneMask, bd,
						paraBi, partFrom, partUpto,
						&taSet, &taSetMask ) )
	{ LDEB(1); return -1;	}

    if  ( utilPropMaskIsEmpty( &taDoneMask ) )
	{ *pCalculated= 0;	}
    else{ *pCalculated= 1;	}

    /*
    *pPartShift += 0;
    *pStroffShift += 0;
    */

    return 0;
    }

