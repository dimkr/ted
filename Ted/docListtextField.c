/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Calculate the string for a listtext field.				*/
/*  The caller (below) makes the particules.				*/
/*									*/
/*  1)  The actual number						*/
/*  2)  The space for the separator.					*/
/*									*/
/************************************************************************/

static int docCalculateListtextParaString(
				unsigned char *			target,
				int				maxsize,
				int				ilvl,
				const int *			numberPath,
				const int *			startPath,
				const int *			formatPath,
				const DocumentListLevel *	dll )
    {
    int		done;

    /*  1  */
    done= docListLevelFormatParagraphNumber( target, maxsize, (int *)0, 0,
			    ilvl, numberPath, startPath, formatPath, dll );
    if  ( done < 0 )
	{ LDEB(done); return done;	}

    target += done;

    /*  2  */
    if  ( dll->dllFollow == DOCllfTAB || dll->dllFollow == DOCllfSPACE )
	{
	if  ( done+ 1 >= maxsize )
	    { LLDEB(done,maxsize); }

	*(target++)= ' '; done++;
	target[0]= '\0';
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Evaluate Listtext fields for bulleted and numbered lists &c.	*/
/*									*/
/*  1)  Paranoia: check list number.					*/
/*  2)  Get override and number tree.					*/
/*  3)  Link override to list if nexessary.				*/
/*  4)  Paranoia again: Is it an existing list?				*/
/*  5)  Obtain the path in the number tree. ( E.G: 13.1.18.11 )		*/
/*  6)  Calculate number or bullet text.				*/
/*  7)  Replace old field contents with the new string.			*/
/*  8)  Calculate attribute number for added text.			*/
/*  9)  Insert text particule.						*/
/*  10) Insert tab.							*/
/*									*/
/************************************************************************/

int docRecalculateParaListtextTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraBi,
				int				part,
				int				partCount,
				DocumentField *			dfListtext,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject )
    {
    const ParagraphProperties *	pp= &(paraBi->biParaProperties);

    int				ilvl= pp->ppListLevel;

    ListOverride *		lo;
    DocumentList *		dl;
    ListNumberTreeNode *	root;

    int				paraNr= docNumberOfParagraph( paraBi );

    int				startPath[DLmaxLEVELS+1];
    int				formatPath[DLmaxLEVELS+1];
    const DocumentListLevel *	dll= (const DocumentListLevel *)0;
    int				numberPath[DLmaxLEVELS+1];

    unsigned char		addedString[200+1];
    int				addedStrlen;

    int				stroff;
    int				stroffShift;
    int				textAttributeNumber;
    int				sepAttributeNumber;

    int				partsMade= 0;
    int				makeSep= 0;
    int				sepKind= DOCkindTAB;

    TextAttribute		ta;
    PropertyMask		doneMask;

    TextParticule *		tp;

    utilInitTextAttribute( &ta );

    if  ( pp->ppListOverride <= 0 )
	{ LDEB(pp->ppListOverride); *pCalculated= 0; return 0; }

    /*  1,2,3,4  */
    if  ( docGetListOfParagraph( &lo, &root, &dl, pp->ppListOverride, bd ) )
	{ LLDEB(pp->ppListOverride,ilvl); *pCalculated= 0; return 0; }

    if  ( docListGetFormatPath( startPath, formatPath, &dll, ilvl, dl, lo ) )
	{ LLDEB(pp->ppListOverride,ilvl); *pCalculated= 0; return 0; }

    /*  5  */
    if  ( docListNumberTreeGetNumberPath( numberPath, root, ilvl, paraNr ) )
	{ LLDEB(pp->ppListOverride,ilvl); *pCalculated= 0; return 0; }

    /*  6  */
    addedStrlen= docCalculateListtextParaString(
				    addedString, sizeof(addedString)- 1, ilvl,
				    numberPath, startPath, formatPath, dll );
    if  ( addedStrlen < 0 )
	{ LLDEB(ilvl,addedStrlen); *pCalculated= 0; return 0;	}

    /*  7  */
    if  ( docFieldReplaceContents( &stroff,
			    &stroffShift, &textAttributeNumber,
			    bd, paraBi, part, partCount,
			    *pStroffShift, addedString, addedStrlen,
			    voidadd, closeObject ) )
	{ LDEB(1); return -1;	}

    if  ( stroff != 0 )
	{ LDEB(stroff);	}

    /*  8  */
    utilGetTextAttributeByNumber( &ta,
		    &(bd->bdTextAttributeList), textAttributeNumber );
    utilUpdateTextAttribute( &doneMask, &ta, &(dll->dllTextAttribute),
						&(dll->dllTextAttributeMask) );
    textAttributeNumber=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &ta );

    sepAttributeNumber= textAttributeNumber;

    if  ( dll->dllFollow != DOCllfNONE )
	{
	int		changed= 0;

	if  ( PROPmaskISSET( &(dll->dllTextAttributeMask),
						TApropTEXTUNDERLINED )	&&
	      ta.taTextIsUnderlined					)
	    { ta.taTextIsUnderlined= 0; changed= 1;	}

	if  ( PROPmaskISSET( &(dll->dllTextAttributeMask),
						TApropSTRIKETHROUGH )	&&
	      ta.taHasStrikethrough					)
	    { ta.taHasStrikethrough= 0; changed= 1;	}

	if  ( changed )
	    {
	    sepAttributeNumber=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &ta );
	    }
	}

    /*  9  */
    if  ( dll->dllFollow == DOCllfTAB )
	{ makeSep= 1; sepKind= DOCkindTAB;	}

    if  ( dll->dllFollow == DOCllfSPACE			&&
	  sepAttributeNumber != textAttributeNumber	)
	{ makeSep= 1; sepKind= DOCkindTEXT;	}

    tp= docInsertTextParticule( paraBi, part+ 1,
					    stroff, addedStrlen- makeSep,
					    DOCkindTEXT, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); *pCalculated= 0; return 0;	}
    partsMade++;

    /*  10  */
    if  ( makeSep )
	{
	tp= docInsertTextParticule( paraBi, part+ 2,
					    stroff+ addedStrlen- makeSep, 1,
					    sepKind, sepAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); *pCalculated= 0; return 0;	}
	partsMade++;
	}

    *pCalculated= 1;
    *pPartShift= partsMade- partCount;
    *pStroffShift += stroffShift;

    return 0;
    }

