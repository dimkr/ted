/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilFontEncoding.h>
#   include	<utilMatchFont.h>
#   include	<docExpandedTextAttribute.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a layout position.					*/
/*									*/
/************************************************************************/

void docInitLayoutPosition(	LayoutPosition *	lp )
    {
    lp->lpPageYTwips= 0;
    lp->lpPage= 0;
    lp->lpColumn= 0;
    lp->lpAtTopOfColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Clean and free a whole document.					*/
/*									*/
/************************************************************************/

void docFreeDocument( BufferDocument *	bd )
    {
    int		i;

    docCleanItem( bd, &(bd->bdItem) );

    for ( i= 0; i < bd->bdNoteCount; i++ )
	{ docCleanNote( bd, &(bd->bdNotes[i]) );	}

    docCleanExternalItem( bd, &(bd->bdEiFtnsep) );
    docCleanExternalItem( bd, &(bd->bdEiFtnsepc) );
    docCleanExternalItem( bd, &(bd->bdEiFtncn) );

    docCleanExternalItem( bd, &(bd->bdEiAftnsep) );
    docCleanExternalItem( bd, &(bd->bdEiAftnsepc) );
    docCleanExternalItem( bd, &(bd->bdEiAftncn) );

    docCleanStyleSheet( &(bd->bdStyleSheet) );
    docCleanFieldList( &(bd->bdFieldList) );

    docCleanDocumentProperties( &(bd->bdProperties) );

    if  ( bd->bdNotes )
	{ free( bd->bdNotes );	}

    docFreeListNumberNodes( bd->bdListNumberTrees, bd->bdListNumberTreeCount );

    utilCleanTextAttributeList( &(bd->bdTextAttributeList) );

    free( bd );
    }

/************************************************************************/
/*									*/
/*  Initialise a BufferDocument.					*/
/*									*/
/************************************************************************/

void docInitDocument(	BufferDocument *	bd )
    {
    const int		numberInParent= 0;

    utilInitTextAttributeList( &(bd->bdTextAttributeList) );

    docInitDocumentProperties( &(bd->bdProperties) );

    docInitItem( &(bd->bdItem), (BufferItem *)0, bd,
				    numberInParent, DOClevDOC, DOCinBODY );

    docInitStyleSheet( &(bd->bdStyleSheet) );
    docInitFieldList( &(bd->bdFieldList) );

    bd->bdNotes= (DocumentNote *)0;
    bd->bdNoteCount= 0;

    bd->bdListNumberTrees= (ListNumberTreeNode *)0;
    bd->bdListNumberTreeCount= 0;

    docInitExternalItem( &(bd->bdEiFtnsep) );
    docInitExternalItem( &(bd->bdEiFtnsepc) );
    docInitExternalItem( &(bd->bdEiFtncn) );

    docInitExternalItem( &(bd->bdEiAftnsep) );
    docInitExternalItem( &(bd->bdEiAftnsepc) );
    docInitExternalItem( &(bd->bdEiAftncn) );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a new document consisting of one paragraph with one empty	*/
/*  particule.								*/
/*									*/
/************************************************************************/

BufferDocument * docNewFile(	TextAttribute *			taDefault,
				const char *			defaultFont,
				int				docCharset,
				int				ansiCodepage,
				const PostScriptFontList *	psfl,
				const DocumentGeometry *	dg )
    {
    BufferDocument *		bd;
    DocumentProperties *	dp;

    PropertyMask		taNewMask;
    PropertyMask		doneMask;
    ExpandedTextAttribute	etaNew;
    TextAttribute		taNew;
    int				textAttributeNumber;

    BufferItem *		bi;

    PROPmaskCLEAR( &taNewMask );
    docInitExpandedTextAttribute( &etaNew );

    bd= (BufferDocument *)malloc( sizeof(BufferDocument) );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}
    dp= &(bd->bdProperties);

    docInitDocument( bd );
    dp->dpDocumentCharset= docCharset;
    dp->dpAnsiCodepage= ansiCodepage;
    dp->dpGeometry= *dg;

    /*  3  */
    if  ( psfl && utilAddPsFontsToDocList( &(dp->dpFontList), psfl ) )
	{ LDEB(psfl->psflFamilyCount); goto failed;	}

    if  ( docExpandedAttributeFromString( &taNewMask, &etaNew,
					    &(dp->dpFontList), defaultFont ) )
	{ SDEB(defaultFont); goto failed;	}

    if  ( ! PROPmaskISSET( &taNewMask, TApropDOC_FONT_NUMBER ) )
	{ SDEB(defaultFont); goto failed;	}

    if  ( ! PROPmaskISSET( &taNewMask, TApropFONTSIZE )	||
	  etaNew.etaFontSizeHalfPoints < 6		||
	  etaNew.etaFontSizeHalfPoints > 100		)
	{
	SLDEB(defaultFont,etaNew.etaFontSizeHalfPoints);
	etaNew.etaFontSizeHalfPoints= 24;
	PROPmaskADD( &taNewMask, TApropFONTSIZE );
	}

    utilInitTextAttribute( &taNew );

    docIndirectTextAttribute( &doneMask, &taNew, &etaNew, &taNewMask,
				    &(dp->dpColors), &(dp->dpColorCount) );

    if  ( taNew.taFontNumber >= 0				&&
	  taNew.taFontNumber < dp->dpFontList.dflFontCount	)
	{
	dp->dpDefaultFont= taNew.taFontNumber;
	dp->dpFontList.dflFonts[taNew.taFontNumber].dfUsed= 1;
	}

    textAttributeNumber= utilTextAttributeNumber(
				    &(bd->bdTextAttributeList), &taNew );
    if  ( textAttributeNumber < 0 )
	{ SLDEB(defaultFont,textAttributeNumber); goto failed;	}

    bi= docInsertItem( bd, &(bd->bdItem), -1, DOClevSECT );
    if  ( ! bi )
	{ XDEB(bi); goto failed;	}

    bi->biSectDocumentGeometry= dp->dpGeometry;

    bi= docInsertEmptyParagraph( bd, bi, textAttributeNumber );
    if  ( ! bi )
	{ XDEB(bi); goto failed;	}

    docCleanExpandedTextAttribute( &etaNew );

    *taDefault= taNew;
    return bd;

  failed:

    docCleanExpandedTextAttribute( &etaNew );

    docFreeDocument( bd );

    return (BufferDocument *)0;
    }

/************************************************************************/
/*									*/
/*  Delete a series of lines.						*/
/*									*/
/************************************************************************/

void docDeleteLines(		BufferItem *	bi,
				int		first,
				int		count )
    {
    if  ( first > bi->biParaLineCount )
	{
	LLDEB(first,bi->biParaLineCount);
	first= bi->biParaLineCount;
	}

    if  ( first+ count > bi->biParaLineCount )
	{
	LLDEB(first+count,bi->biParaLineCount);
	count= bi->biParaLineCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    bi->biParaLineCount -= count;

    while( first < bi->biParaLineCount )
	{
	bi->biParaLines[first]= bi->biParaLines[first+ count];
	first++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Remember about a line in a Text Item.				*/
/*									*/
/************************************************************************/

void docInitTextLine(		TextLine *	tl )
    {
    tl->tlStroff= 0;
    tl->tlFirstParticule= 0;
    tl->tlStrlen= 0;
    tl->tlParticuleCount= 0;
    tl->tlWordCount= 0;

    docInitLayoutPosition( &(tl->tlTopPosition) );

    tl->tlLineAscentTwips= 0;
    tl->tlLineHeightTwips= 0;
    tl->tlLineSpacingTwips= 0;

    tl->tlX0Pixels= 0;
    tl->tlX1Pixels= 0;

    tl->tlHasPageBreak= 0;

    tl->tlFrameWidthTwips= 0;
    tl->tlFrameX0Twips= 0;

    return;
    }

TextLine * docInsertTextLine(	BufferItem *	bi,
				int		line )
    {
    TextLine *		tl;
    int			newSize;

    if  ( bi->biParaLineCount % 10 )
	{ newSize= bi->biParaLineCount+  1; }
    else{ newSize= bi->biParaLineCount+ 11; }

    newSize *= sizeof(TextLine);

    tl= (TextLine *)realloc( bi->biParaLines, newSize );
    if  ( ! tl )
	{ LXDEB(bi->biParaLineCount,tl); return (TextLine *)0; }
    bi->biParaLines= tl;

    if  ( line == -1 )
	{ line= bi->biParaLineCount; }
    else{
	int		i;

	for ( i= bi->biParaLineCount; i > line; i-- )
	    { tl[i]= tl[i-1];	}
	}

    tl += line;

    docInitTextLine( tl );

    bi->biParaLineCount++;
    return tl;
    }

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

static void docCollectItemStatistics(	DocumentStatistics *	ds,
					const BufferItem *	bi )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ docCollectItemStatistics( ds, bi->biChildren[i] ); }
	    break;

	case DOClevPARA:
	    ds->dsParagraphCount++;
	    ds->dsCharacterCount += bi->biParaStrlen;
	    ds->dsLineCount += bi->biParaLineCount;

	    for ( i= 0; i < bi->biParaLineCount; i++ )
		{ ds->dsWordCount += bi->biParaLines[i].tlWordCount;	}

	    break;

	default:
	    LDEB(bi->biLevel); return;
	}

    return;
    }


void docCollectDocumentStatistics(	DocumentStatistics *	ds,
					const BufferDocument *	bd )
    {
    docInitDocumentStatistics( ds );

    docCollectItemStatistics( ds, &(bd->bdItem) );

    ds->dsPageCount= bd->bdItem.biBelowPosition.lpPage+ 1;

    return;
    }

