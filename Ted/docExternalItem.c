/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
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
/*  Kinds of Headers/Footers.						*/
/*									*/
/************************************************************************/

const int DOC_HeaderScopes[PAGES__COUNT]=
    {
    DOCinFIRST_HEADER,
    DOCinSECT_HEADER,
    DOCinSECT_HEADER,
    DOCinRIGHT_HEADER,
    DOCinLEFT_HEADER,
    };

const int DOC_FooterScopes[PAGES__COUNT]=
    {
    DOCinFIRST_FOOTER,
    DOCinSECT_FOOTER,
    DOCinSECT_FOOTER,
    DOCinRIGHT_FOOTER,
    DOCinLEFT_FOOTER,
    };

/************************************************************************/
/*									*/
/*  Initialise an ExternalItem.						*/
/*									*/
/************************************************************************/

void docInitExternalItem(	ExternalItem *	ei )
    {
    ei->eiItem= (BufferItem *)0;
    ei->eiPageFormattedFor= -1;

    ei->eiY0UsedTwips= 0;
    ei->eiY1UsedTwips= 0;
    ei->eiY0ReservedTwips= 0;
    ei->eiY1ReservedTwips= 0;

    ei->eiPageSelectedUpon= -1;

    return;
    }

void docCleanExternalItem(	BufferDocument *	bd,
				ExternalItem *		ei )
    {
    if  ( ei->eiItem )
	{ docFreeItem( bd, ei->eiItem );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a new 'External Item' E.G. A section Header/Footer or a	*/
/*  Footnote/Endnote.							*/
/*									*/
/************************************************************************/

BufferItem * docMakeExternalItem(
			    BufferDocument *		bd,
			    const SelectionScope *	ss,
			    const SectionProperties *	sp )
    {
    BufferItem *	bi= (BufferItem *)malloc( sizeof(BufferItem) );

    const int		numberInParent= 0;

    if  ( ! bi )
	{ XDEB(bi); return (BufferItem *)0;	}

    docInitItem( bi, (BufferItem *)0, bd, numberInParent,
					DOClevSECT, ss->ssInExternalItem );

    if  ( docCopySectionProperties( &(bi->biSectProperties), sp ) )
	{
	LDEB(1);
	docFreeItem( bd, bi );
	return (BufferItem *)0;
	}

    bi->biSectBreakKind= DOCsbkNONE;

    bi->biSectSelectionScope= *ss;

    return bi;
    }

/************************************************************************/
/*									*/
/*  Make an external item and insert an empty paragraph into it.	*/
/*									*/
/************************************************************************/

BufferItem * docMakeExternalparagraph(
			BufferDocument *		bd,
			ExternalItem *			ei,
			const BufferItem *		bodyBi,
			int				textAttributeNumber,
			int				noteIndex,
			int				inExtIt )
    {
    BufferItem *		paraBi= (BufferItem *)0;

    SectionProperties		spDef;

    int				numberInParent= -1;
    SelectionScope		ss;

    docInitSelectionScope( &ss );
    docInitSectionProperties( &spDef );

    switch( inExtIt )
	{
	case DOCinBODY:
	    LDEB(inExtIt); goto ready;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    while( bodyBi && bodyBi->biLevel != DOClevSECT )
		{ bodyBi= bodyBi->biParent;	}
	    if  ( ! bodyBi )
		{ XDEB(bodyBi); goto ready;	}

	    numberInParent= bodyBi->biNumberInParent;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:

	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    break;

	default:
	    LDEB(inExtIt); return (BufferItem *)0;
	}


    ss.ssInExternalItem= inExtIt;
    ss.ssSectNrExternalTo= numberInParent;
    ss.ssNoteArrayIndex= noteIndex;

    ei->eiItem= docMakeExternalItem( bd, &ss, &spDef );
    if  ( ! ei->eiItem )
	{ XDEB(ei->eiItem); goto ready;	}

    ei->eiPageFormattedFor= -1;
    ei->eiPageSelectedUpon= -1;

    paraBi= docInsertEmptyParagraph( bd, ei->eiItem, textAttributeNumber );
    if  ( ! paraBi )
	{
	XDEB(paraBi);
	docCleanExternalItem( bd, ei );
	docInitExternalItem( ei );
	goto ready;
	}

  ready:

    docCleanSectionProperties( &spDef );

    return paraBi;
    }

