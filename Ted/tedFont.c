/************************************************************************/
/*									*/
/*  Ted: Manipulation of font and text attributes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position or selection.		*/
/*									*/
/************************************************************************/

int tedGetDocumentAttributes(	TedDocument *		td,
				PropertyMask *		pUpdMask,
				TextAttribute *		pTaNew )
    {
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! tedHasSelection( td ) )
	{ return 1;	}

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return 1;	}

    docGetSelectionAttributes( bd, &ds, pUpdMask, pTaNew );

    return 0;
    }

int tedGetDocumentAttributeString(	char *		scratch,
					TedDocument *	td )
    {
    int				rval= 0;

    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    PropertyMask		doneMask;
    PropertyMask		updMask;
    TextAttribute		taNew;
    ExpandedTextAttribute	etaNew;

    PROPmaskCLEAR( &doneMask );
    PROPmaskCLEAR( &updMask );
    utilInitTextAttribute( &taNew );
    docInitExpandedTextAttribute( &etaNew );

    if  ( tedGetDocumentAttributes( td, &updMask, &taNew ) )
	{ rval= -1; goto ready;	}

    docExpandTextAttribute( &doneMask, &etaNew, &taNew, &updMask,
					dfl, dp->dpColors, dp->dpColorCount );

    docExpandedAttributeToString( scratch, &updMask, dfl, &etaNew );

  ready:
    docCleanExpandedTextAttribute( &etaNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the attributes of a particule.				*/
/*									*/
/************************************************************************/

int tedChangeParticuleAttributes(
				PropertyMask *			pTaAllMask,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				BufferDocument *		bd,
				BufferItem *			paraBi,
				int				partFrom,
				int				partUpto,
				const TextAttribute *		taSet,
				const PropertyMask *		taSetMask )
    {
    if  ( docChangeParticuleAttributes( pTaAllMask, bd,
						paraBi, partFrom, partUpto,
						taSet, taSetMask ) )
	{ LDEB(1); return -1;	}

    if  ( tedOpenParaScreenFonts( bd, add, sfl, paraBi, partFrom, partUpto ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
