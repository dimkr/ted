/************************************************************************/
/*									*/
/*  Save a BufferDocument into a plain text file.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docBuf.h"

#   include	<appDebugon.h>

typedef struct PlainWritingContext
    {
    int		pwcFold;
    int		pwcEndClosed;
    int		pwcNoteRefCount;
    int		pwcNoteDefCount;
    } PlainWritingContext;

static void docInitPlainWritingContext(	PlainWritingContext *	pwc )
    {
    pwc->pwcFold= 0;
    pwc->pwcEndClosed= 1;
    pwc->pwcNoteRefCount= 0;
    pwc->pwcNoteDefCount= 0;
    }

static int docPlainSaveParaItem(	SimpleOutputStream *		sos,
					const BufferDocument *		bd,
					const BufferItem *		bi,
					const DocumentSelection *	ds,
					PlainWritingContext *		pwc )
    {
    TextParticule *		tp;
    unsigned char *		s;
    int				pos= 0;

    int				lineCount= 0;
    int				paragraphUnderlined= 1;

    int				part= 0;
    int				stroff= 0;
    int				upto= 0;

    const DocumentField *	df;

    if  ( ds && ds->dsBegin.dpBi == bi )
	{
	const int	lastOne= 1;

	stroff= ds->dsBegin.dpStroff;

	if  ( docFindParticuleOfPosition( &part, &(ds->dsBegin), lastOne ) )
	    { LDEB(stroff); return -1;	}
	}

    tp= bi->biParaParticules+ part;
    s= bi->biParaString+ stroff;

    if  ( ds && ds->dsEnd.dpBi == bi )
	{ upto= ds->dsEnd.dpStroff;	}
    else{ upto= bi->biParaStrlen;	}

    while( stroff < upto && part < bi->biParaParticuleCount )
	{
	TextAttribute		ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	switch( tp->tpKind )
	    {
	    case DOCkindTAB:
		sioOutPutCharacter( '\t', sos );
		s++; stroff++;
		pos= 8* ( pos+ 8 )/ 8;
		paragraphUnderlined= 0;
		break;

	    case DOCkindTEXT:
		if  ( pwc->pwcFold && pos > 8 && pos+ tp->tpStrlen >= 72 )
		    {
		    sioOutPutCharacter( '\n', sos );
		    lineCount++; pos= 0;

		    if  ( bi->biParaFirstIndentTwips < -9	&&
			  bi->biParaLeftIndentTwips > 9		)
			{
			sioOutPutCharacter( '\t', sos ); pos= 8;
			paragraphUnderlined= 0;
			}
		    }

		if  ( ! ta.taTextIsUnderlined )
		    { paragraphUnderlined= 0;	}

		while( stroff < upto && stroff < tp->tpStroff+ tp->tpStrlen )
		    {
#		    if 0
		    if  ( ! paragraphUnderlined && ta.taTextIsUnderlined )
			{
			sioOutPutCharacter( '_', sos );
			sioOutPutCharacter( '\b', sos );
			}

		    if  ( ta.taFontIsBold )
			{
			sioOutPutCharacter( *s, sos );
			sioOutPutCharacter( '\b', sos );
			}
#		    endif

		    sioOutPutCharacter( *s, sos );
		    s++; stroff++; pos++;
		    }

		break;

	    case DOCkindLINEBREAK:
		sioOutPutCharacter( '\n', sos );
		pos= 0; paragraphUnderlined= 0;
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		break;

	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		sioOutPutCharacter( '\n', sos );
		sioOutPutCharacter( '\f', sos );
		pos= 0; paragraphUnderlined= 0;
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		break;

	    case DOCkindFIELDSTART:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

		if  ( df->dfKind == DOCfkPAGEREF )
		    {
		    int		count;

		    count= docCountParticulesInField( bi, part,
						    bi->biParaParticuleCount );

		    count++;
		    tp += count; part += count;
		    stroff= tp[1].tpStroff; s= bi->biParaString+ stroff;
		    break;
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;
		    char	scratch[20+1];

		    count= docCountParticulesInField( bi, part,
						    bi->biParaParticuleCount );

		    if  ( bi->biInExternalItem == DOCinBODY )
			{
			sprintf( scratch, "[%d]", pwc->pwcNoteRefCount+ 1 );
			pwc->pwcNoteRefCount++;
			}
		    else{
			sprintf( scratch, "[%d]", pwc->pwcNoteDefCount+ 1 );
			pwc->pwcNoteDefCount++;
			}

		    sioOutPutString( scratch, sos );

		    tp += count; part += count;
		    stroff= tp[1].tpStroff; s= bi->biParaString+ stroff;
		    }
		else{
		    s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		    }
		break;

	    default:
		LDEB(tp->tpKind);
		/*FALLTHROUGH*/
	    case DOCkindNOTE:
	    case DOCkindOBJECT:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		break;
	    }

	 part++; tp++;
	 }

    if  ( pwc->pwcEndClosed && stroff == bi->biParaStrlen )
	{ sioOutPutCharacter( '\n', sos ); lineCount++;	}

#   if 0
    if  ( lineCount == 1	&&
	  pos > 0		&&
	  paragraphUnderlined	)
	{
	stroff= 0;
	if  ( ds && ds->dsBegin.dpBi == bi )
	    { stroff= ds->dsBegin.dpStroff; }

	while( stroff < upto )
	    { sioOutPutCharacter( '-', sos ); stroff++;	}

	sioOutPutCharacter( '\n', sos );
	}
#   endif

    return 0;
    }

static int docPlainSaveRowItem(	SimpleOutputStream *		sos,
				const BufferDocument *		bd,
				const BufferItem *		rowBi,
				const DocumentSelection *	ds,
				PlainWritingContext *		pwc )
    {
    int			col;

    for ( col= 0; col < rowBi->biChildCount; col++ )
	{
	int		para;
	BufferItem *	cellBi= rowBi->biChildren[col];

	if  ( ds && docCompareItemPositions( cellBi, ds->dsBegin.dpBi ) < 0 )
	    { continue;	}

	if  ( ds && docCompareItemPositions( cellBi, ds->dsEnd.dpBi ) > 0 )
	    { continue;	}

	if  ( ! ds						||
	      ( ( ds->dsCol0 < 0 || col >= ds->dsCol0 )	&&
		( ds->dsCol1 < 0 || col <= ds->dsCol1 )	)	)
	    {
	    for ( para= 0; para < cellBi->biChildCount; para++ )
		{
		BufferItem *	paraBi= cellBi->biChildren[para];

		if  ( ds && docCompareItemPositions( paraBi,
						    ds->dsBegin.dpBi ) < 0 )
		    { continue;	}

		if  ( ds && docCompareItemPositions( paraBi,
						    ds->dsEnd.dpBi ) > 0 )
		    { continue;	}

		if  ( docPlainSaveParaItem( sos, bd, paraBi, ds, pwc ) )
		    { LLDEB(col,para); return -1;	}
		}
	    }
	}

    return 0;
    }

static int docPlainSaveItem(	SimpleOutputStream *		sos,
				const BufferDocument *		bd,
				const BufferItem *		bi,
				const DocumentSelection *	ds,
				PlainWritingContext *		pwc )
    {
    int		i;

    if  ( ds )
	{
	if  ( docCompareItemPositions( bi, ds->dsBegin.dpBi ) < 0	)
	    { return 0;	}

	if  ( docCompareItemPositions( bi, ds->dsEnd.dpBi ) > 0	)
	    { return 0;	}
	}

    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevCELL:
	rowAsGroup:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docPlainSaveItem( sos, bd, bi->biChildren[i], ds, pwc ) )
		    { LDEB(i); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{ goto rowAsGroup;	}

	    if  ( docPlainSaveRowItem( sos, bd, bi, ds, pwc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docPlainSaveParaItem( sos, bd, bi, ds, pwc ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }


int docPlainSaveDocument(	SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				fold,
				int				closed )
    {
    const BufferItem *		bi= &bd->bdItem;

    PlainWritingContext		pwc;

    docInitPlainWritingContext( &pwc );

    pwc.pwcFold= fold;
    pwc.pwcEndClosed= closed;

    if  ( ds )
	{
	ExternalItem *	ei;
	BufferItem *	bodySectBi;

	bi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! bi )
	    { XDEB(bi); return -1;	}
	}

    if  ( docPlainSaveItem( sos, bd, bi, ds, &pwc ) )
	{ LDEB(bi->biLevel); return -1; }

    if  ( ! ds && pwc.pwcNoteRefCount > 0 )
	{
	int		i;
	DocumentNote *	dn;

	sioOutPutString( "\n-----\n\n", sos );

	dn= bd->bdNotes;
	for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	    {
	    ExternalItem *	ei;

	    if  ( dn->dnParaNr < 0 )
		{ continue;	}

	    ei= &(dn->dnExternalItem);;
	    if  ( ! ei->eiItem )
		{ XDEB(ei->eiItem); continue;	}

	    if  ( docPlainSaveItem( sos, bd, ei->eiItem, ds, &pwc ) )
		{ LDEB(bi->biLevel); return -1; }
	    }

	if  ( pwc.pwcNoteDefCount != pwc.pwcNoteRefCount )
	    { LLDEB(pwc.pwcNoteDefCount,pwc.pwcNoteRefCount);	}
	}

    return 0;
    }
