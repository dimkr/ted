/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Field editing and field balancing code.				*/
/*									*/
/************************************************************************/

typedef struct FieldLevelPosition
    {
    int		flpStroff;
    int		flpParticule;
    int		flpLevel;
    int		flpRoLevel;
    } FieldLevelPosition;

static void docInitFieldLevelPosition(	FieldLevelPosition *	flp )
    {
    flp->flpStroff= -1;
    flp->flpParticule= -1;
    flp->flpLevel= -1;
    flp->flpRoLevel= -1;

    return;
    }

/************************************************************************/
/*									*/
/*  Adjust the text field level on the basis of a certain particule.	*/
/*									*/
/*  NOTE: Though only DOCkindFIELDSTART and DOCkindFIELDEND particules	*/
/*	impact on the nesting of text fields, a simple way to prevent	*/
/*	docAdjustTextFieldLevels() to be called too often would be to	*/
/*	call it for all particules except text particules.		*/
/*									*/
/************************************************************************/

static int docAdjustTextFieldLevels(
				int *				plevel,
				int *				pRoLevel,
				int				step,
				const DocumentFieldList *	dfl,
				const TextParticule *		tp )
    {
    int					done= 0;
    int					level= *plevel;
    int					roLevel= *pRoLevel;

    const DocumentField *		df;
    const FieldKindInformation *	fki;

    if  ( tp->tpKind == DOCkindFIELDSTART )
	{
	if  ( tp->tpObjectNumber < 0			||
	      tp->tpObjectNumber >= dfl->dflFieldCount	)
	    { LLDEB(tp->tpObjectNumber,dfl->dflFieldCount); goto ready;	}

	df= dfl->dflFields+ tp->tpObjectNumber;
	fki= DOC_FieldKinds+ df->dfKind;

	if  ( df->dfKind <  0			||
	      df->dfKind >= DOC_FieldKindCount	)
	    { LLDEB(df->dfKind,DOC_FieldKindCount); goto ready;	}

	if  ( fki->fkiLevel == DOClevTEXT )
	    {
	    level += step;

	    if  ( ! fki->fkiResultEditable )
		{ roLevel += step;	}

	    done= step;
	    }
	}

    if  ( tp->tpKind == DOCkindFIELDEND )
	{
	if  ( tp->tpObjectNumber < 0			||
	      tp->tpObjectNumber >= dfl->dflFieldCount	)
	    { LLDEB(tp->tpObjectNumber,dfl->dflFieldCount); goto ready;	}

	df= dfl->dflFields+ tp->tpObjectNumber;
	fki= DOC_FieldKinds+ df->dfKind;

	if  ( df->dfKind <  0			||
	      df->dfKind >= DOC_FieldKindCount	)
	    { LLDEB(df->dfKind,DOC_FieldKindCount); goto ready;	}

	if  ( fki->fkiLevel == DOClevTEXT )
	    {
	    level -= step;

	    if  ( ! fki->fkiResultEditable )
		{ roLevel -= step;	}

	    done= -step;
	    }
	}

  ready:
    *pRoLevel= roLevel;
    *plevel= level;

    return done;
    }

/************************************************************************/
/*									*/
/*  Look for the particule with the lowest text field nesting level in	*/
/*  a position.								*/
/*									*/
/*  The particule returned is a particule that starts at the lowest	*/
/*  nesting level.							*/
/*									*/
/************************************************************************/

static int docBottomParticuleForPosition(
					FieldLevelPosition *		flp,
					const DocumentFieldList *	dfl,
					const DocumentPosition *	dp,
					int				level,
					int				last )
    {
    const BufferItem *		paraBi= dp->dpBi;

    int				part0;
    int				part1;
    int				part;
    const TextParticule *	tp;

    int				levelFound= 0;
    int				roLevel= 0;

    const int			step= 1;

    if  ( docFindParticuleOfPosition( &part0, dp, 0 ) )
	{ LLDEB(dp->dpStroff,dp->dpBi->biParaStrlen); return -1;	}
    if  ( docFindParticuleOfPosition( &part1, dp, 1 ) )
	{ LLDEB(dp->dpStroff,dp->dpBi->biParaStrlen); return -1;	}

    part= part0;
    tp= paraBi->biParaParticules+ part;

    flp->flpStroff= dp->dpStroff;
    flp->flpParticule= part;
    flp->flpLevel= level;
    flp->flpRoLevel= roLevel;
    levelFound= level;

    while( part <= part1 )
	{
	if  ( tp->tpKind != DOCkindTEXT )
	    {
	    int		done;
	    
	    done= docAdjustTextFieldLevels( &level, &roLevel, step, dfl, tp );

	    if  ( done > 0 )
		{
		if  ( level -1 < levelFound			||
		      ( last && level -1 == levelFound )	)
		    {
		    flp->flpParticule= part;
		    flp->flpLevel= level- 1;
		    flp->flpRoLevel= roLevel;

		    levelFound= level- 1;
		    }
		}

	    if  ( done < 0 )
		{
		if  ( level < levelFound		||
		      ( last && level == levelFound )	)
		    {
		    if  ( part >= dp->dpBi->biParaParticuleCount- 1 )
			{ flp->flpParticule= part;	}
		    else{ flp->flpParticule= part+ 1;	}

		    flp->flpLevel= level;
		    flp->flpRoLevel= roLevel;

		    levelFound= level;
		    }
		}
	    }

	part++; tp++;
	}

    return 0;
    }

static int docFieldCalculateTextFieldLevel(
				int *				pRoLevel,
				const BufferItem *		paraBi,
				const DocumentFieldList *	dfl,
				int				level,
				int				part0,
				int				part1 )
    {
    int				part;
    const TextParticule *	tp;

    const int			step= 1;

    part= part0;
    tp= paraBi->biParaParticules+ part;
    while( part <= part1 )
	{
	if  ( tp->tpKind != DOCkindTEXT )
	    { docAdjustTextFieldLevels( &level, pRoLevel, step, dfl, tp ); }

	part++; tp++;
	}

    return level;
    }

/************************************************************************/
/*									*/
/*  Move the beginning of a selection to a certain nesting level of	*/
/*  text fields.							*/
/*									*/
/*  1)  Try to move right to find the beginning of a higher level at	*/
/*	the same string position.					*/
/*  2)  Move left to find the beginning of a lower level.		*/
/*									*/
/************************************************************************/

static void docFieldBalanceBegin(
				FieldLevelPosition *		flpBegin,
				const FieldLevelPosition *	flpEnd,
				const BufferItem *		paraBi,
				const DocumentFieldList *	dfl )
    {
    const TextParticule *	tp;
    int				part;

    const int			step= -1;

    /*  1  */
    if  ( flpBegin->flpLevel <= flpEnd->flpLevel	&&
	  flpBegin->flpRoLevel <= 0			)
	{
	int		level= flpBegin->flpLevel;
	int		roLevel= flpBegin->flpRoLevel;

	part= flpBegin->flpParticule;
	tp= paraBi->biParaParticules+ part;

	for ( part= part; part < flpEnd->flpParticule; tp++, part++ )
	    {
	    if  ( tp->tpStroff > flpBegin->flpStroff )
		{ break;	}

	    if  ( tp->tpKind != DOCkindTEXT )
		{
		docAdjustTextFieldLevels( &level, &roLevel, -step, dfl, tp );
		if  ( level < flpBegin->flpLevel )
		    { return;	}
		if  ( level == flpEnd->flpLevel )
		    {
		    flpBegin->flpParticule= part+ 1;
		    flpBegin->flpLevel= level;
		    flpBegin->flpRoLevel= roLevel;
		    return;
		    }
		}
	    }

	return;
	}

    /*  2  */
    part= flpBegin->flpParticule- 1;
    tp= paraBi->biParaParticules+ part;

    for ( part= part; part >= 0; tp--, part-- )
	{
	if  ( tp->tpKind != DOCkindTEXT )
	    {
	    docAdjustTextFieldLevels( &(flpBegin->flpLevel),
				    &(flpBegin->flpRoLevel), step, dfl, tp );

	    if  ( flpBegin->flpLevel <= flpEnd->flpLevel	&&
		  flpBegin->flpRoLevel <= 0			)
		{ break;	}
	    }
	}

    if  ( part < 0 )
	{ LDEB(part); return;	}

    tp= paraBi->biParaParticules+ part;

    flpBegin->flpParticule= part;
    flpBegin->flpStroff= tp->tpStroff;

    return;
    }

/************************************************************************/
/*									*/
/*  Move the end of a selection to a certain nesting level of text	*/
/*  fields.								*/
/*									*/
/*  1)  Find the first text particule that contains the end position.	*/
/*  2)  Move right until the nesting level reaches the bottom level	*/
/*	of the nested fields.						*/
/*									*/
/************************************************************************/

static void docFieldBalanceEnd(	const FieldLevelPosition *	flpBegin,
				FieldLevelPosition *		flpEnd,
				const BufferItem *		paraBi,
				const DocumentFieldList *	dfl )
    {
    const TextParticule *	tp;
    int				part;

    const int			step= 1;

    if  ( flpEnd->flpLevel <= flpBegin->flpLevel	&&
	  flpEnd->flpRoLevel <= 0			)
	{
	int		level= flpEnd->flpLevel;
	int		roLevel= flpEnd->flpRoLevel;

	part= flpEnd->flpParticule;
	tp= paraBi->biParaParticules+ part;

	for ( part= part; part > flpBegin->flpParticule; tp--, part-- )
	    {
	    if  ( tp->tpStroff < flpEnd->flpStroff )
		{ break;	}

	    if  ( tp->tpKind != DOCkindTEXT )
		{
		docAdjustTextFieldLevels( &level, &roLevel, -step, dfl, tp );
		if  ( level < flpEnd->flpLevel )
		    { return;	}
		if  ( level == flpBegin->flpLevel )
		    {
		    flpEnd->flpParticule= part- 1;
		    flpEnd->flpLevel= level;
		    flpEnd->flpRoLevel= roLevel;
		    return;
		    }
		}
	    }

	return;
	}

    part= flpEnd->flpParticule+ 1;
    tp= paraBi->biParaParticules+ part;

    for ( part= flpEnd->flpParticule+ 1;
			    part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != DOCkindTEXT )
	    {
	    docAdjustTextFieldLevels( &(flpEnd->flpLevel),
					&(flpEnd->flpRoLevel), step, dfl, tp );

	    if  ( flpEnd->flpLevel <= flpBegin->flpLevel	&&
		  flpEnd->flpRoLevel <= 0			)
		{ break;	}
	    }
	}

    if  ( part >= paraBi->biParaParticuleCount )
	{
	flpEnd->flpParticule= paraBi->biParaParticuleCount;
	flpEnd->flpStroff= paraBi->biParaStrlen;

	if  ( part > paraBi->biParaParticuleCount )
	    {LLLDEB(part,flpBegin->flpParticule,paraBi->biParaParticuleCount);}

	return;
	}

    tp= paraBi->biParaParticules+ part;

    flpEnd->flpParticule= part;
    flpEnd->flpStroff= tp->tpStroff+ tp->tpStrlen;

    return;
    }

/************************************************************************/
/*									*/
/*  Balance a selection such that it does not partially cover any	*/
/*  field.								*/
/*									*/
/*  A)  Determine particule numbers for the least nested particules	*/
/*	at the beginning and the end of the selection.			*/
/*  B)  Determine actual values of the text field nesting levels.	*/
/*  C)  And look for the matching particules at the same level.		*/
/*									*/
/************************************************************************/

int docBalanceFieldSelection(	int *			pBeginMoved,
				int *			pEndMoved,
				const BufferDocument *	bd,
				DocumentPosition *	dpBegin,
				DocumentPosition *	dpEnd )
    {
    int				beginMoved= 0;
    int				endMoved= 0;

    FieldLevelPosition		flpBegin;
    FieldLevelPosition		flpEnd;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    /*  A  */
    docInitFieldLevelPosition( &flpBegin );
    docInitFieldLevelPosition( &flpEnd );

    if  ( docBottomParticuleForPosition( &flpBegin, dfl, dpBegin, 0, 0 ) )
	{ LDEB(1); goto ready;	}
    if  ( docBottomParticuleForPosition( &flpEnd, dfl, dpEnd, 0, 1 ) )
	{ LDEB(1); goto ready;	}

    /*  B  */
    flpBegin.flpRoLevel= 0;
    flpBegin.flpLevel= docFieldCalculateTextFieldLevel( &(flpBegin.flpRoLevel),
					dpBegin->dpBi, dfl,
					0, 0, flpBegin.flpParticule- 1 );
    if  ( flpBegin.flpLevel < 0 || flpBegin.flpRoLevel < 0 )
	{ LLDEB(flpBegin.flpLevel,flpBegin.flpRoLevel);	}

    if  ( dpBegin->dpBi == dpEnd->dpBi )
	{
	/*  B  */
	flpEnd.flpRoLevel= flpBegin.flpRoLevel;
	flpEnd.flpLevel= docFieldCalculateTextFieldLevel( &(flpEnd.flpRoLevel),
						dpEnd->dpBi, dfl,
						flpBegin.flpLevel,
						flpBegin.flpParticule,
						flpEnd.flpParticule- 1 );

	if  ( flpEnd.flpLevel < 0 || flpEnd.flpRoLevel < 0 )
	    { LLDEB(flpEnd.flpLevel,flpEnd.flpRoLevel);	}

	if  ( flpBegin.flpLevel >= flpEnd.flpLevel )
	    {
	    docFieldBalanceEnd( &flpBegin, &flpEnd, dpEnd->dpBi, dfl );
	    docFieldBalanceBegin( &flpBegin, &flpEnd, dpBegin->dpBi, dfl );
	    }
	else{
	    docFieldBalanceBegin( &flpBegin, &flpEnd, dpBegin->dpBi, dfl );
	    docFieldBalanceEnd( &flpBegin, &flpEnd, dpEnd->dpBi, dfl );
	    }
	}
    else{
	FieldLevelPosition		flpPara;

	/*  B  */
	flpEnd.flpRoLevel= 0;
	flpEnd.flpLevel= docFieldCalculateTextFieldLevel( &(flpEnd.flpRoLevel),
						dpEnd->dpBi, dfl,
						0, 0,
						flpEnd.flpParticule- 1 );
	if  ( flpEnd.flpLevel < 0 || flpEnd.flpRoLevel < 0 )
	    { LLDEB(flpEnd.flpLevel,flpEnd.flpRoLevel);	}

	docInitFieldLevelPosition( &flpPara );
	flpPara.flpStroff= dpBegin->dpBi->biParaStrlen;
	flpPara.flpParticule= dpBegin->dpBi->biParaParticuleCount;
	flpPara.flpLevel= 0;
	flpPara.flpRoLevel= 0;
	docFieldBalanceBegin( &flpBegin, &flpPara, dpBegin->dpBi, dfl );

	docInitFieldLevelPosition( &flpPara );
	flpPara.flpStroff= 0;
	flpPara.flpParticule= 0;
	flpPara.flpLevel= 0;
	flpPara.flpRoLevel= 0;
	if  ( flpEnd.flpStroff < dpEnd->dpBi->biParaStrlen )
	    {
	    docFieldBalanceEnd( &flpPara, &flpEnd, dpEnd->dpBi, dfl );
	    }
	}

    if  ( dpBegin->dpStroff != flpBegin.flpStroff )
	{ dpBegin->dpStroff= flpBegin.flpStroff; beginMoved= 1; }
    if  ( dpEnd->dpStroff != flpEnd.flpStroff )
	{ dpEnd->dpStroff= flpEnd.flpStroff; endMoved= 1; }

  ready:

    *pBeginMoved= beginMoved;
    *pEndMoved= endMoved;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Are we in a field?.							*/
/*									*/
/*  1)  Return the particule number of the particule that was checked.	*/
/*									*/
/************************************************************************/

int docPositionInField(		int *				pPart,
				const DocumentPosition *	dp,
				const BufferDocument *		bd )
    {
    FieldLevelPosition		flp;
    const int			startLevel= 0;
    const int			last= 0;
    const int			part0= 0;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    /*  A  */
    docInitFieldLevelPosition( &flp );

    if  ( docBottomParticuleForPosition( &flp, dfl, dp, startLevel, last ) )
	{ LDEB(1); return 0;	}

    flp.flpRoLevel= 0;
    flp.flpLevel= docFieldCalculateTextFieldLevel( &(flp.flpRoLevel),
		    dp->dpBi, dfl, startLevel, part0, flp.flpParticule- 1 );
    if  ( flp.flpLevel < 0 || flp.flpRoLevel < 0 )
	{ LLDEB(flp.flpLevel,flp.flpRoLevel);	}

    /*  1  */
    if  ( pPart )
	{ *pPart= flp.flpParticule;	}

    return flp.flpLevel > 0;
    }

/************************************************************************/
/*									*/
/*  Insert the particule that terminates a field.			*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert end particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertEndParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpEnd,
				int				part1,
				int *				pEndPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttributeNumber;

    BufferItem *	bi= dpEnd->dpBi;

    /*  1  */
    tp= bi->biParaParticules+ part1;
    if  ( tp->tpStrlen > 0				&&
	  dpEnd->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part1++; tp++;	}

    if  ( part1 < bi->biParaParticuleCount )
	{ textAttributeNumber= tp[ 0].tpTextAttributeNumber; }
    else{ textAttributeNumber= tp[-1].tpTextAttributeNumber; }

    /*  2  */
    if  ( part1 < bi->biParaParticuleCount	&&
	  tp->tpStroff != dpEnd->dpStroff	)
	{
	int	len= tp->tpStroff+ tp->tpStrlen- dpEnd->dpStroff;

	tp->tpStrlen= dpEnd->dpStroff- tp->tpStroff;

	tp= docInsertTextParticule( bi, part1+ 1,
		    dpEnd->dpStroff, len, tp->tpKind, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); return -1;	}

	part1++;
	}

    /*  3  */
    tp= docInsertTextParticule( bi, part1,
		    dpEnd->dpStroff, 0, DOCkindFIELDEND, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (PropertyMask *)0,
						bd, bi, part1, part1+ 1,
						taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    *pEndPart= part1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the particule that starts a field.				*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert start particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertStartParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpStart,
				int				part0,
				int *				pStartPart,
				int *				pEndPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttributeNumber;

    int			part1= *pEndPart;

    BufferItem *	bi= dpStart->dpBi;

    /*  1  */
    tp= bi->biParaParticules+ part0;
    if  ( tp->tpStrlen > 0					&&
	  dpStart->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part0++; tp++;	}

    if  ( part0 < bi->biParaParticuleCount )
	{ textAttributeNumber= tp[ 0].tpTextAttributeNumber; }
    else{ textAttributeNumber= tp[-1].tpTextAttributeNumber; }

    /*  2  */
    if  ( part0 < bi->biParaParticuleCount	&&
	  tp->tpStroff != dpStart->dpStroff	)
	{
	int		stroff= tp->tpStroff;
	int		len= dpStart->dpStroff- tp->tpStroff;

	tp->tpStrlen= tp->tpStroff+ tp->tpStrlen- dpStart->dpStroff;
	tp->tpStroff= dpStart->dpStroff;

	tp= docInsertTextParticule( bi, part0,
				stroff, len, tp->tpKind, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); return -1;	}

	if  ( part1 >= part0 )
	    { part1++;	}

	part0++;
	}

    /*  3  */
    tp= docInsertTextParticule( bi, part0,
		dpStart->dpStroff, 0, DOCkindFIELDSTART, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (PropertyMask *)0, bd,
						bi, part0, part0+ 1,
						taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    if  ( part1 >= part0 )
	{ part1++;	}

    *pStartPart= part0; *pEndPart= part1; return 0;
    }

/************************************************************************/
/*									*/
/*  Surround a selected range of text by a text level field.		*/
/*									*/
/************************************************************************/

int docSurroundTextSelectionByField(
				DocumentField **		pDf,
				BufferDocument *		bd,
				int *				pStartPart,
				int *				pEndPart,
				const DocumentSelection *	ds,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    int				level= 0;
    int				roLevel= 0;
    int				part0;
    int				part1;

    DocumentField *		df;
    int				fieldNumber;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ XXDEB(ds->dsBegin.dpBi,ds->dsEnd.dpBi); return -1;	}

    df= docClaimField( &fieldNumber, &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    if  ( docFindParticuleOfPosition( &part0, &(ds->dsBegin), 1 ) )
	{ LDEB(ds->dsBegin.dpStroff); return -1;	}
    if  ( docFindParticuleOfPosition( &part1, &(ds->dsEnd), 0 ) )
	{ LDEB(ds->dsEnd.dpStroff); return -1;	}

    level= docFieldCalculateTextFieldLevel( &roLevel,
				ds->dsBegin.dpBi, dfl, level, part0, part1 );
    if  ( level != 0 )
	{
	LLLDEB(part0,part1,level);
	docListItem(0,ds->dsBegin.dpBi);
	return -1;
	}

    /*  find end, split?, insert end particule */
    if  ( docFieldInsertEndParticule( bd, &(ds->dsEnd),
					    part1, &part1,
					    fieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    /*  find begin, split?, insert start particule */
    if  ( docFieldInsertStartParticule( bd, &(ds->dsBegin),
					    part0, &part0, &part1,
					    fieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilPropMaskIsEmpty( taSetMask ) )
	{
	PropertyMask	taDoneMask;

	PROPmaskCLEAR( &taDoneMask );

	if  ( docChangeParticuleAttributes( &taDoneMask, bd,
						ds->dsBegin.dpBi,
						part0, part1,
						taSet, taSetMask ) )
	    { LDEB(1);	}
	}

    *pDf= df;
    *pStartPart= part0;
    *pEndPart= part1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Count the number of particules inside a text level field. The	*/
/*  begin and end particules are not counted.				*/
/*									*/
/*  1)  Do some sanity checks on the proper nesting of fields.. Just	*/
/*	to have a safe feeling.						*/
/*									*/
/************************************************************************/

int docCountParticulesInField(		const BufferItem *	bi,
					int			part,
					int			partUpto )
    {
    int				fieldLevel= 0;
    int				partCount= 0;
    int				fieldNumber;

    const TextParticule *	tp;

    tp= bi->biParaParticules+ part;
    if  ( tp->tpKind != DOCkindFIELDSTART )
	{ LDEB(tp->tpKind); return -1;	}
    fieldNumber= tp->tpObjectNumber;

    tp++; part++;
    while( part < partUpto )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    /*  1  */
	    if  ( tp->tpObjectNumber == fieldNumber )
		{ LLDEB(tp->tpObjectNumber,fieldNumber);	}

	    fieldLevel++;
	    }

	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    if  ( tp->tpObjectNumber == fieldNumber )
		{
		/*  1 
		if  ( fieldLevel != 0 )
		    { LDEB(fieldLevel);	}
		*/

		return partCount;
		}

	    fieldLevel--;
	    }

	partCount++, part++; tp++;
	}

    /*  1  */
    LLDEB(fieldLevel,fieldNumber);
    docListItem(0,bi);
    return -1;
    }

/************************************************************************/
/*									*/
/*  Find the special field at the head of a paragraph in a list.	*/
/*									*/
/************************************************************************/

int docParaHeadFieldKind(	const BufferItem *	paraBi,
				const BufferDocument *	bd )
    {
    int		fieldKind= -1;

    if  ( paraBi->biInExternalItem == DOCinFOOTNOTE	||
	  paraBi->biInExternalItem == DOCinENDNOTE	)
	{
	const int		stroff= 0;
	DocumentNote *		dn;
	DocumentPosition	dp;

	if  ( docGetNote( &dn, bd, paraBi, stroff ) < 0 )
	    { LDEB(stroff); return -1;	}

	if  ( docFirstPosition( &dp, dn->dnExternalItem.eiItem ) )
	    { LDEB(1); return -1;	}

	if  ( dp.dpBi == paraBi )
	    { fieldKind= DOCfkCHFTN;	}
	}

    if  ( paraBi->biParaListOverride > 0 )
	{ fieldKind= DOCfkLISTTEXT;	}

    return fieldKind;
    }

int docDelimitParaHeadField(	int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				const BufferItem *	paraBi,
				const BufferDocument *	bd )
    {
    int				part;
    const TextParticule *	tp;

    int		fieldNr= -1;
    int		partBegin= -1;
    int		partEnd= -1;
    int		stroffBegin= -1;
    int		stroffEnd= -1;

    int		fieldKind;

    fieldKind= docParaHeadFieldKind( paraBi, bd );
    if  ( fieldKind < 0 )
	{ return 1;	}

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	const DocumentField *	df;

	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
	    if  ( df->dfKind == fieldKind )
		{
		partBegin= part;
		stroffBegin= tp->tpStroff;
		fieldNr= tp->tpObjectNumber;
		}
	    }

	if  ( tp->tpKind == DOCkindFIELDEND	&&
	      tp->tpObjectNumber == fieldNr	)
	    { partEnd= part; stroffEnd= tp->tpStroff; break;	}
	}

    if  ( partBegin < 0 )
	{ return 1;	}
    if  ( partEnd <= partBegin )
	{ LLDEB(partBegin,partEnd); return -1;	}

    *pFieldNr= fieldNr;
    *pPartBegin= partBegin;
    *pPartEnd= partEnd;
    *pStroffBegin= stroffBegin;
    *pStroffEnd= stroffEnd;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the special field at the head of a numbered paragraph.	*/
/*  (buller), or of a foot/endnote.					*/
/*									*/
/*  0)  Insert a text particule at the head of the paragraph as a	*/
/*	temporary field value.						*/
/*  1)  Allocate a field.						*/
/*  2)  Insert start particule.						*/
/*  3)  Insert end particule.						*/
/*  4)  Make sure there is at least one particule after the field.	*/
/*									*/
/************************************************************************/

int docInsertParaHeadField(	int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				BufferItem *		paraBi,
				BufferDocument *	bd,
				int			fieldKind,
				int			textAttributeNumber )
    {
    int				fieldNumber;
    DocumentField *		df;
    TextParticule *		tpField;
    TextParticule *		tpText;

    int				stroffShift= 0;

    const int			head= 0;
    const int			len= 0;
    const int			stroffHead= 0;

    int				wasEmpty= ( paraBi->biParaStrlen == 0 );

    /*  4  */
    if  ( docParaStringReplace( &stroffShift, paraBi,
		    stroffHead, stroffHead, (const unsigned char *)"?", 1 ) )
	{ LDEB(paraBi->biParaStrlen); return -1; }

    if  ( paraBi->biParaParticuleCount > 0 && wasEmpty )
	{
	tpText= paraBi->biParaParticules;
	if  ( tpText->tpKind != DOCkindTEXT )
	    { SDEB(docKindStr(tpText->tpKind)); return -1;	}

	tpText->tpStrlen= 1;

	if  ( docShiftParticuleOffsets( bd, paraBi, head+ 1,
				paraBi->biParaParticuleCount, stroffShift ) )
	    { LDEB(stroffShift); }
	}
    else{
	tpText= docInsertTextParticule( paraBi, head,
			    stroffHead, 1, DOCkindTEXT, textAttributeNumber );
	if  ( ! tpText )
	    { LXDEB(paraBi->biParaParticuleCount,tpText); return -1; }

	if  ( docShiftParticuleOffsets( bd, paraBi, head+ 1,
				paraBi->biParaParticuleCount, stroffShift ) )
	    { LDEB(stroffShift); }
	}

    /*  1  */
    df= docClaimField( &fieldNumber, &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}
    df->dfKind= DOCfkUNKNOWN;

    /*  2  */
    tpField= docInsertTextParticule( paraBi, head, stroffHead, len,
				    DOCkindFIELDSTART, textAttributeNumber );
    if  ( ! tpField )
	{ XDEB(tpField); return -1;	}
    tpField->tpObjectNumber= fieldNumber;

    /*  3  */
    tpField= docInsertTextParticule( paraBi, head+ 2, stroffHead+ 1, len,
				    DOCkindFIELDEND, textAttributeNumber );
    if  ( ! tpField )
	{ XDEB(tpField); return -1;	}
    tpField->tpObjectNumber= fieldNumber;

    df->dfKind= fieldKind;

    /*  4  */
    if  ( paraBi->biParaParticuleCount == head+ 3 )
	{
	tpText= docInsertTextParticule( paraBi, head+ 3,
		    paraBi->biParaStrlen, 0, DOCkindTEXT, textAttributeNumber );
	if  ( ! tpText )
	    { LXDEB(paraBi->biParaParticuleCount,tpText); return -1; }
	}

    *pFieldNr= fieldNumber;
    *pPartBegin= head;
    *pPartEnd= head+ 2;
    *pStroffBegin= stroffHead;
    *pStroffEnd= stroffHead+ 1;

    return 0;
    }

