/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXTERNAL_ITEM_H
#   define	DOC_EXTERNAL_ITEM_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentField;

/************************************************************************/
/*									*/
/*  Kinds of external items.						*/
/*									*/
/************************************************************************/

typedef enum WhatPages
    {
    PAGES_FIRST_PAGE= 0,
    PAGES_SUBSEQUENT_PAGES,
    PAGES_ALL_PAGES,
    PAGES_ODD_PAGES,
    PAGES_EVEN_PAGES,
    PAGES__COUNT
    } WhatPages;

typedef enum DOC_ItemRoot
    {
    DOCinUNKNOWN= 0,

    DOCinBODY,

    DOCinSECT_HEADER,
    DOCinFIRST_HEADER,
    DOCinLEFT_HEADER,
    DOCinRIGHT_HEADER,

    DOCinSECT_FOOTER,
    DOCinFIRST_FOOTER,
    DOCinLEFT_FOOTER,
    DOCinRIGHT_FOOTER,

    DOCinFOOTNOTE,
    DOCinENDNOTE,

    DOCinFTNSEP,
    DOCinFTNSEPC,
    DOCinFTNCN,

    DOCinAFTNSEP,
    DOCinAFTNSEPC,
    DOCinAFTNCN,

    DOCinSHPTXT,

    DOCin_COUNT
    } DOC_ItemRoot;

extern const int DOC_HeaderScopes[PAGES__COUNT];
extern const int DOC_FooterScopes[PAGES__COUNT];

/************************************************************************/
/*									*/
/*  A structure describing an external item.				*/
/*									*/
/************************************************************************/

typedef struct ExternalItem
    {
    struct BufferItem *	eiItem;
    int			eiPageFormattedFor;
    int			eiY0UsedTwips;
    int			eiY1UsedTwips;
    int			eiY0ReservedTwips;
    int			eiY1ReservedTwips;

    int			eiPageSelectedUpon;
    } ExternalItem;

/************************************************************************/
/*									*/
/*  A footnote/endnote.							*/
/*									*/
/*  1)  Though the notes are stored with the section, the paragraph	*/
/*	number is relative to the body of the document as a whole.	*/
/*									*/
/************************************************************************/

typedef struct DocumentNote
    {
    ExternalItem	dnExternalItem;
    short		dnNoteNumber;
    short		dnReferringPage;
    int			dnSectNr;
    int			dnParaNr;
    int			dnStroff;
    int			dnReferringColumn;
    unsigned char	dnExternalItemKind;
    unsigned char	dnAutoNumber;
    } DocumentNote;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitExternalItem(	ExternalItem *		ei );

extern void docCleanExternalItem(	struct BufferDocument *	bd,
					ExternalItem *		ei );

#   endif
