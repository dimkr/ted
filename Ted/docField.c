/************************************************************************/
/*									*/
/*  Evaluate fields+ the list of kinds of fields.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Delete the previous contents of a field.				*/
/*									*/
/************************************************************************/

int docFieldReplaceContents(	int *			pStroff,
				int *			pStroffShift,
				int *			pTextAttrNr,
				BufferDocument *	bd,
				BufferItem *		paraBi,
				int			part,
				int			partCount,
				int			stroffShift,
				const unsigned char *	addedString,
				int			addedStrlen,
				void *			voidadd,
				DOC_CLOSE_OBJECT	closeObject )
    {
    int			i;
    TextParticule *	tp= paraBi->biParaParticules+ part;

    int			textAttributeNumber= tp[1].tpTextAttributeNumber;
    int			past= tp[1+partCount].tpStroff+ stroffShift;
    int			stroff= tp[1].tpStroff+ stroffShift;

    int			d= 0;

    if  ( docParaStringReplace( &d, paraBi, stroff, past,
						addedString, addedStrlen ) )
	{ LDEB(addedStrlen); return -1;	}

    if  ( partCount > 0 )
	{
	tp= paraBi->biParaParticules+ part+ 1;
	for ( i= 0; i < partCount; tp++, i++ )
	    {
	    (*closeObject)( bd, paraBi, tp, voidadd );
	    docCleanParticuleObject( bd, paraBi, tp );
	    }

	docDeleteParticules( paraBi, part+ 1, partCount );
	}

    *pStroff= stroff;
    *pStroffShift= d;
    *pTextAttrNr= textAttributeNumber;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute an individual text field in a paragraph.			*/
/*									*/
/*  NOTE: This routine is not recursive. Nested fields are handeled	*/
/*	by the caller.							*/
/*									*/
/************************************************************************/

int docRecalculateParaStringTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraBi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject )
    {
    unsigned char			scratch[500+ 1];

    int					calculated= 0;

    int					newSize;
    int					d;

    TextParticule *			tp= paraBi->biParaParticules+ part;

    int					textAttributeNumber;

    int					past;
    int					stroff;
    int					partsMade;

    int					i;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    if  ( (*fki->fkiCalculateTextString)( &calculated, &newSize, scratch,
			sizeof(scratch)- 1, bd, paraBi, part, partCount, df ) )
	{ SDEB(fki->fkiLabel); return -1;	}

    if  ( ! calculated )
	{
	*pCalculated= 0;
	*pPartShift= 0;
	/* NO! *pStroffShift= 0; */
	return 0;
	}

    if  ( tp[1].tpStrlen == newSize					&&
	  ! memcmp( paraBi->biParaString+ tp[1].tpStroff+ *pStroffShift,
						    scratch, newSize )	)
	{
	*pCalculated= 0;
	*pPartShift= 0;
	/* NO! *pStroffShift= 0; */
	return 0;
	}

    textAttributeNumber= tp[1].tpTextAttributeNumber;
    past= tp[1+partCount].tpStroff+ *pStroffShift;
    stroff= tp[1].tpStroff+ *pStroffShift;

    if  ( docParaStringReplace( &d, paraBi, stroff, past, scratch, newSize ) )
	{ LDEB(newSize); return -1;	}

    tp= paraBi->biParaParticules+ part+ 1;
    for ( i= 0; i < partCount; tp++, i++ )
	{
	(*closeObject)( bd, paraBi, tp, voidadd );
	docCleanParticuleObject( bd, paraBi, tp );
	}

    partsMade= docRedivideStringInParticules( paraBi, stroff, newSize,
				    part+ 1, partCount, textAttributeNumber );
    if  ( partsMade < partCount )
	{
	docDeleteParticules( paraBi, part+ 1+ partsMade, partCount- partsMade );
	}

    *pCalculated= 1;
    *pPartShift= partsMade- partCount;
    *pStroffShift += d;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute the text fields in a paragraph.				*/
/*									*/
/*  1)  For all fields in range...					*/
/*	NOTE that the end of the range may have been shifted by the	*/
/*	calculation of field results.					*/
/*  2)  Shift this particule by what previous calculations have changed	*/
/*	in size.							*/
/*  3)  Not the beginning of a field.. Irrelevant.			*/
/*  4)  Retrieve the field. Do some sanity checks: Only text level	*/
/*	fields are really supported.					*/
/*  5)  Count the number of particules currently in the field.		*/
/*  6)  When the field is to be recalculated.. do so.			*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*	Otherwise just shift the offsets of the particules inside the	*/
/*	field.								*/
/*  7)  When there is sufficient space inside the field to contain	*/
/*	yet another one.. go into recursion.				*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*  8)  Shift the end particule of the field.				*/
/*  9)  Set the current position in the loop to the end particule. The	*/
/*	loop increment will move to the particule after the field.	*/
/*									*/
/************************************************************************/

static int docFieldShiftStringOffsets(
				RecalculateFields *	rf,
				BufferItem *		paraBi,
				int			partFrom,
				int			partUpto,
				int			stroffShift )
    {
    return docShiftParticuleOffsets( rf->rfBd, paraBi,
					    partFrom, partUpto, stroffShift );
    }

static int docRecalculateParaTextFields(
				RecalculateFields *	rf,
				int *			pPartShift,
				int *			pStroffShift,
				BufferItem *		paraBi,
				int			part,
				int			partUpto )
    {
    BufferDocument *	bd= rf->rfBd;
    int			fieldsUpdated= 0;
    int			paraNr= -1;

    /*  1  */
    for ( part= part; part < partUpto+ *pPartShift; part++ )
	{
	TextParticule *			tp= paraBi->biParaParticules+ part;

	DocumentField *			df;
	const FieldKindInformation *	fki;

	int				endPart;
	int				partCount;
	int				stroffEnd;

	/*  2  */
	if  ( docFieldShiftStringOffsets( rf, paraBi, part, part+ 1,
							    *pStroffShift ) )
	    { LDEB(*pStroffShift);	}

	/*  3  */
	if  ( tp->tpKind != DOCkindFIELDSTART )
	    { continue;	}

	if  ( paraNr <= 0					&&
	      ( rf->rfSelBegin.epParaNr > 0	||
	        rf->rfSelEnd.epParaNr > 0	)	)
	    { paraNr= docNumberOfParagraph( paraBi );	}

	/*  4  */
	if  ( tp->tpObjectNumber < 0				||
	      tp->tpObjectNumber >= bd->bdFieldList.dflFieldCount	)
	    {
	    LLDEB(tp->tpObjectNumber,bd->bdFieldList.dflFieldCount);
	    return -1;
	    }

	df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
	if  ( df->dfKind < 0 || df->dfKind >= DOC_FieldKindCount )
	    { LLDEB(df->dfKind,DOC_FieldKindCount); continue;	}

	fki= DOC_FieldKinds+ df->dfKind;
	if  ( fki->fkiLevel != DOClevTEXT )
	    { continue;	}

	/*  5  */
	partCount= docCountParticulesInField( paraBi, part,
						    partUpto+ *pPartShift );
	if  ( partCount < 0 )
	    { SLDEB(docFieldKindStr(df->dfKind),partCount); continue;	}
	endPart= part+ 1+ partCount;
	stroffEnd= tp[partCount+1].tpStroff;

	/*  6  */
	if  ( ( fki->fkiCalculateWhen & rf->rfUpdateFlags )	&&
	      fki->fkiCalculateTextParticules			)
	    {
	    int			partShift= 0;
	    int			calculated= 0;
	    int			oldStroffShift= *pStroffShift;

	    if  ( (*fki->fkiCalculateTextParticules)( &calculated, bd,
			&partShift, pStroffShift, paraBi, part, partCount, df,
			rf->rfVoidadd, rf->rfCloseObject ) )
		{ LDEB(1); return -1;	}

	    if  ( calculated )
		{
		int	stroffShift= *pStroffShift- oldStroffShift;

		docAdjustEditPositionOffsetB( &(rf->rfSelBegin),
					paraNr, stroffEnd, stroffShift );
		docAdjustEditPositionOffsetE( &(rf->rfSelEnd),
					paraNr, stroffEnd, stroffShift );

		fieldsUpdated++; rf->rfFieldsUpdated++;
		}
	    else{
		if  ( docFieldShiftStringOffsets( rf, paraBi,
					part+ 1, endPart, *pStroffShift ) )
		    { LDEB(*pStroffShift);	}
		}

	    endPart += partShift;
	    *pPartShift += partShift;
	    }
	else{
	    if  ( docFieldShiftStringOffsets( rf, paraBi,
					part+ 1, endPart, *pStroffShift ) )
		{ LDEB(*pStroffShift);	}
	    }

	/*  7  */
	if  ( endPart- part >= 2 )
	    {
	    int			partShift= 0;
	    int			stroffShift= 0;

	    if  ( docRecalculateParaTextFields( rf,
			&partShift, &stroffShift, paraBi, part+ 1, endPart ) )
		{ LDEB(1); return -1;	}

	    endPart += partShift;
	    *pPartShift += partShift;
	    *pStroffShift += stroffShift;
	    }

	/*  8  */
	tp= paraBi->biParaParticules+ endPart;
	if  ( tp->tpKind != DOCkindFIELDEND )
	    { LDEB(tp->tpKind);	}

	tp->tpStroff += *pStroffShift;

	/*  9  */
	part= endPart;
	}

    if  ( fieldsUpdated )
	{ docInvalidateParagraphLayout( paraBi );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute the text fields in a buffer item.			*/
/*									*/
/************************************************************************/

int docRecalculateTextLevelFieldsInExternalItem(
					RecalculateFields *	rf,
					ExternalItem *		ei,
					const BufferItem *	bodySectBi,
					int			page )
    {
    int		rval= 0;
    int		ret;
    int		saveFieldsUpdated= rf->rfFieldsUpdated;

    if  ( ! ei->eiItem )
	{ return 0;	}

    if  ( ei->eiItem->biSectHeaderFooterUseForSectBi )
	{ XDEB(ei->eiItem->biSectHeaderFooterUseForSectBi); return -1;	}

    ei->eiItem->biSectHeaderFooterUseForSectBi= bodySectBi;
    ei->eiItem->biSectHeaderFooterUseForPage= page;

    rf->rfFieldsUpdated= 0;

    ret= docRecalculateTextLevelFields( rf, ei->eiItem );
    if  ( ret )
	{ LDEB(ret); rval= -1;	}

    if  ( rf->rfFieldsUpdated > 0 )
	{ ei->eiPageFormattedFor= -1; }

    rf->rfFieldsUpdated += saveFieldsUpdated;

    ei->eiItem->biSectHeaderFooterUseForSectBi= (const BufferItem *)0;

    return rval;
    }

int docRecalculateTextLevelFields(	RecalculateFields *	rf,
					BufferItem *		bi )
    {
    BufferDocument *	bd= rf->rfBd;
    int			i;

    switch( bi->biLevel )
	{
	case DOClevDOC:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docRecalculateTextLevelFields( rf, bi->biChildren[i] ) )
		    { LDEB(i); return -1;	}
		}

	    {
	    const int	page= -1;

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiFtnsep), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiFtnsepc), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiFtncn), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiAftnsep), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiAftnsepc), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}

	    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
			&(bd->bdEiAftncn), (BufferItem *)0, page )	)
		{ LDEB(1); return -1;	}
	    }

	    return 0;

	case DOClevROW:
	case DOClevCELL:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docRecalculateTextLevelFields( rf, bi->biChildren[i] ) )
		    { LDEB(i); return -1;	}
		}

	    return 0;

	case DOClevSECT:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docRecalculateTextLevelFields( rf, bi->biChildren[i] ) )
		    { LDEB(i); return -1;	}
		}

	    if  ( bi->biInExternalItem == DOCinBODY )
		{
		DocumentNote *	dn;
		const int	page= -1;

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectHeader), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectFirstPageHeader), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectLeftPageHeader), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectRightPageHeader), bi, page ) )
		    { LDEB(1); return -1;	}


		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectFooter), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectFirstPageFooter), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectLeftPageFooter), bi, page ) )
		    { LDEB(1); return -1;	}

		if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
				    &(bi->biSectRightPageFooter), bi, page ) )
		    { LDEB(1); return -1;	}

		dn= bd->bdNotes;
		for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
		    {
		    ExternalItem *	ei= &(dn->dnExternalItem);

		    if  ( dn->dnSectNr != bi->biNumberInParent )
			{ continue;	}
		    if  ( ! ei->eiItem )
			{ continue;	}

		    if  ( docRecalculateTextLevelFieldsInExternalItem( rf,
							    ei, bi, page ) )
			{ LDEB(1); return -1;	}
		    }
		}

	    return 0;

	case DOClevPARA:

	    {
	    int		partShift= 0;
	    int		stroffShift= 0;

	    if  ( docRecalculateParaTextFields( rf, &partShift, &stroffShift,
					bi, 0, bi->biParaParticuleCount ) )
		{ LDEB(1); return -1;	}
	    }

	    return 0;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  The Kinds of Fields actively supported.				*/
/*									*/
/************************************************************************/

#   define	NOT_IN_RTF	0
#   define	FIELD_IN_RTF	1

#   define	NO_DEST		0
#   define	DEST_IN_RTF	1

#   define	RESULT_READONLY	0
#   define	RESULT_EDITABLE	1

const FieldKindInformation DOC_FieldKinds[]=
{
    {
	"-UNKNOWN", (char *)0,
		DOClevANY,
		NOT_IN_RTF,
		NO_DEST,
		(CALCULATE_TEXT_PARTICULES)0,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoNEVER,
		RESULT_READONLY,
    },
    {
	"HYPERLINK", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateHyperlinkField,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoDOC_COMPLETE, /* Must Be: FIELDdoNEVER, */
		RESULT_EDITABLE,
    },
    {
	"-XE", "xe",
		DOClevTEXT,
		NOT_IN_RTF,
		DEST_IN_RTF,
		(CALCULATE_TEXT_PARTICULES)0,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoNEVER,
		RESULT_READONLY,
    },
    {
	"-TC", "tc",
		DOClevTEXT,
		NOT_IN_RTF,
		DEST_IN_RTF,
		(CALCULATE_TEXT_PARTICULES)0,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoNEVER,
		RESULT_READONLY,
    },
    {
	"-TCN", "tcn",
		DOClevTEXT,
		NOT_IN_RTF,
		DEST_IN_RTF,
		(CALCULATE_TEXT_PARTICULES)0,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoNEVER,
		RESULT_READONLY,
    },
    {
	"-BOOKMARK", (char *)0,
		DOClevTEXT,
		NOT_IN_RTF,
		NO_DEST,
		(CALCULATE_TEXT_PARTICULES)0,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoNEVER,
		RESULT_EDITABLE,
    },
    {
	"SECTION", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateSectionFieldString,
		FIELDdoDOC_FORMATTED,
		RESULT_READONLY,
    },
    {
	"SECTIONPAGES", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateSectionPagesFieldString,
		FIELDdoDOC_FORMATTED,
		RESULT_READONLY,
    },
    {
	"PAGE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculatePageFieldString,
		FIELDdoPAGE_NUMBER,
		RESULT_READONLY,
    },
    {
	"PAGEREF", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculatePagerefFieldString,
		FIELDdoDOC_FORMATTED,
		RESULT_READONLY,
    },
    {
	"NUMPAGES", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateNumpagesFieldString,
		FIELDdoDOC_FORMATTED,
		RESULT_READONLY,
    },
    {
	"REF", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateRefFieldString,
		FIELDdoDOC_COMPLETE,
		RESULT_READONLY,
    },
    {
	"CREATEDATE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocDateFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"SAVEDATE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocDateFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"PRINTDATE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocDateFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"DATE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocDateFieldString,
		FIELDdoDOC_COMPLETE,
		RESULT_READONLY,
    },
    {
	"TIME", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocDateFieldString,
		FIELDdoDOC_COMPLETE,
		RESULT_READONLY,
    },
    {
	"AUTHOR", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"COMMENTS", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"DOCCOMM", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"KEYWORDS", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"SUBJECT", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"TITLE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateDocStringFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"FILENAME", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaStringTextParticules,
		docCalculateFilenameFieldString,
		FIELDdoDOC_INFO,
		RESULT_READONLY,
    },
    {
	"SYMBOL", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateParaSymbolTextParticules,
		docCalculateSymbolFieldString,
		FIELDdoDOC_COMPLETE,
		RESULT_READONLY,
    },
    {
	"-CHFTN", (char *)0,
		DOClevTEXT,
		NOT_IN_RTF,
		NO_DEST,
		docRecalculateParaChftnTextParticules,
		docCalculateChftnFieldString,
		FIELDdoDOC_COMPLETE|FIELDdoCHFTN,
		RESULT_READONLY,
    },
    {
	"-LISTTEXT", (char *)0,
		DOClevTEXT,
		NOT_IN_RTF,
		NO_DEST,
		docRecalculateParaListtextTextParticules,
		docCalculateDocStringFieldString, /* as dummy */
		FIELDdoDOC_COMPLETE|FIELDdoLISTTEXT,
		RESULT_READONLY,
    },
    {
	"INCLUDEPICTURE", (char *)0,
		DOClevTEXT,
		FIELD_IN_RTF,
		NO_DEST,
		docRecalculateIncludePictureField,
		(CALCULATE_TEXT_STRING)0,
		FIELDdoDOC_COMPLETE,
		RESULT_READONLY,
    },
};

const int DOC_FieldKindCount=
			sizeof(DOC_FieldKinds)/sizeof(FieldKindInformation);

