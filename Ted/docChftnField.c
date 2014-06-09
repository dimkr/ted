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

#   include	<utilBase26.h>
#   include	<utilRoman.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Format the value of a 'chftn' field.				*/
/*									*/
/*  I.E. the text of the reference to the note in the body of the text,	*/
/*	or the identical sign at the begnning of the note.		*/
/*									*/
/************************************************************************/

int docFormatChftnField(	unsigned char *			target,
				int				targetSize,
				const DocumentProperties *	dp,
				int				noteNumber,
				int				extItKind )
    {
    const NotesProperties *	np;

    switch( extItKind )
	{
	case DOCinFOOTNOTE:
	    np= &(dp->dpFootnoteProperties);
	    break;

	case DOCinENDNOTE:
	    np= &(dp->dpEndnoteProperties);
	    break;

	default:
	    SDEB(docExternalKindStr(extItKind)); return -1;
	}

    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{ noteNumber += np->npStartNumber- 1;	}

    switch( np->npNumberStyle )
	{
	case FTNstyleNAR:
	    sprintf( (char *)target, "%d", noteNumber+ 1 );
	    break;

	case FTNstyleNALC:
	    if  ( utilBase26String( (char *)target, targetSize,
							noteNumber+ 1, 0 ) )
		{ LDEB(noteNumber); return -1;	}
	    break;

	case FTNstyleNAUC:
	    if  ( utilBase26String( (char *)target, targetSize,
							noteNumber+ 1, 1 ) )
		{ LDEB(noteNumber); return -1;	}
	    break;

	case FTNstyleNRLC:
	    if  ( utilRomanString( (char *)target, targetSize,
							noteNumber+ 1, 0 ) )
		{ sprintf( (char *)target, "NRLC:%d", noteNumber+ 1 );	}
	    break;

	case FTNstyleNRUC:
	    if  ( utilRomanString( (char *)target, targetSize,
							noteNumber+ 1, 1 ) )
		{ sprintf( (char *)target, "NRUC:%d", noteNumber+ 1 );	}
	    break;

	case FTNstyleNCHI:
	default:
	    LDEB(np->npNumberStyle);
	    sprintf( (char *)target, "(%d)", noteNumber+ 1 );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docCalculateChftnFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfChftn )
    {
    DocumentNote *	dn;
    TextParticule *	tp;

    if  ( paraBi->biInExternalItem == DOCinBODY			&&
	  part+ 1+ partCount+ 1 < paraBi->biParaParticuleCount	)
	{
	int	notePart= part+ 1+ partCount+ 1;

	tp= paraBi->biParaParticules+ notePart;

	if  ( tp->tpKind != DOCkindNOTE )
	    {
	    /* LSDEB(notePart,docKindStr(tp->tpKind)); */
	    *pCalculated= 0;
	    return 0;
	    }

	if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
	    {
	    int paraNr= docNumberOfParagraph( paraBi );

	    LLDEB(paraNr,tp->tpStroff);
	    *pCalculated= 0; return 0;
	    }

	if  ( docFormatChftnField( target, targetSize,
					    &(bd->bdProperties),
					    dn->dnNoteNumber,
					    dn->dnExternalItemKind ) )
	    { LDEB(dn->dnExternalItemKind); *pCalculated= 0; return 0; }

	*pNewSize= strlen( (char *)target );
	*pCalculated= 1;
	return 0;
	}

    if  ( paraBi->biInExternalItem == DOCinFOOTNOTE	||
	  paraBi->biInExternalItem == DOCinENDNOTE	)
	{
	tp= paraBi->biParaParticules;

	if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
	    { LDEB(tp->tpStroff); *pCalculated= 0; return 0; }

	if  ( docFormatChftnField( target, targetSize,
					    &(bd->bdProperties),
					    dn->dnNoteNumber,
					    dn->dnExternalItemKind ) )
	    { LDEB(dn->dnExternalItemKind); *pCalculated= 0; return 0; }

	*pNewSize= strlen( (char *)target );
	*pCalculated= 1;
	return 0;
	}

    LLDEB(paraBi->biInExternalItem,part);
    *pCalculated= 0; return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docRecalculateParaChftnTextParticules(
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
    int			ret;

    TextAttribute	taSet;
    PropertyMask	taSetMask;
    PropertyMask	taDoneMask;


    ret= docRecalculateParaStringTextParticules( pCalculated, bd,
			    pPartShift, pStroffShift, paraBi, part, partCount,
			    df, voidadd, closeObject );

    if  ( ret )
	{ LDEB(ret); return ret;	}

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );
    PROPmaskCLEAR( &taDoneMask );

    taSet.taSuperSub= DOCfontSUPERSCRIPT;
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    if  ( docChangeParticuleAttributes( &taDoneMask, bd,
					paraBi, part+ 1,
					part+ 1+ partCount+ *pPartShift,
					&taSet, &taSetMask ) )
	{ LDEB(1);	}

    if  ( ! utilPropMaskIsEmpty( &taDoneMask ) )
	{ *pCalculated= 1;	}

    return ret;
    }
