/************************************************************************/
/*									*/
/*  Manage tab stops.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDraw.h>

#   include	"docTabStop.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a tab stop.						*/
/*									*/
/************************************************************************/

void docInitTabStop(	TabStop *	ts )
    {
    ts->tsTwips= 0;
    ts->tsPixels= 0;
    ts->tsAlignment= DOCtaLEFT;
    ts->tsLeader= DOCtlNONE;
    ts->tsFromStyleOrList= 0;
    }

/************************************************************************/
/*									*/
/*  Initialise a tab stop List.						*/
/*									*/
/************************************************************************/

void docInitTabStopList(	TabStopList *	tsl )
    {
    tsl->tslTabStopCount= 0;
    tsl->tslTabStops= (TabStop *)0;

    return;
    }

void docCleanTabStopList(	TabStopList *	tsl )
    {
    if  ( tsl->tslTabStops )
	{ free( tsl->tslTabStops );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a tab stop into the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

int docAddTabToList(		TabStopList *		tsl,
				const TabStop *		tsNew )
    {
    TabStop *		ts;
    int			newSize;
    int			i;

    if  ( tsl->tslTabStopCount % 10 )
	{ newSize= tsl->tslTabStopCount+  1; }
    else{ newSize= tsl->tslTabStopCount+ 11; }

    newSize *= sizeof(TabStop);

    ts= (TabStop *)realloc( tsl->tslTabStops, newSize );
    if  ( ! ts )
	{ LXDEB(newSize,ts); return -1; }
    tsl->tslTabStops= ts;

    i= tsl->tslTabStopCount; ts= tsl->tslTabStops+ tsl->tslTabStopCount;
    while( i > 0 && ts[-1].tsTwips > tsNew->tsTwips )
	{ ts[0]= ts[-1]; i--; ts--;	}

    *ts= *tsNew;
    tsl->tslTabStopCount++;

    return i;
    }

int docAddTabToListPixels(	TabStopList *		tsl,
				const TabStop *		tsNew )
    {
    TabStop *		ts;
    int			newSize;
    int			i;

    if  ( tsl->tslTabStopCount % 10 )
	{ newSize= tsl->tslTabStopCount+  1; }
    else{ newSize= tsl->tslTabStopCount+ 11; }

    newSize *= sizeof(TabStop);

    ts= (TabStop *)realloc( tsl->tslTabStops, newSize );
    if  ( ! ts )
	{ LXDEB(newSize,ts); return -1; }
    tsl->tslTabStops= ts;

    i= tsl->tslTabStopCount; ts= tsl->tslTabStops+ tsl->tslTabStopCount;
    while( i > 0 && ts[-1].tsPixels > tsNew->tsPixels )
	{ ts[0]= ts[-1]; i--; ts--;	}

    *ts= *tsNew;
    tsl->tslTabStopCount++;

    return i;
    }

/************************************************************************/
/*									*/
/*  Remove a tab stop from the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

void docDeleteTabFromList(	TabStopList *		tsl,
				int			n )
    {
    TabStop *		ts;

    if  ( n < 0 || n >= tsl->tslTabStopCount )
	{ LLDEB(n,tsl->tslTabStopCount); return;	}

    tsl->tslTabStopCount--;

    ts= tsl->tslTabStops+ n;
    while( n < tsl->tslTabStopCount )
	{ ts[0]= ts[1]; ts++; n++;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Copy the list of tabs of one paragraph to another.			*/
/*									*/
/************************************************************************/

int docCopyTabStopList(		int *			pChanged,
				TabStopList *		to,
				const TabStopList *	from,
				int			pixels )
    {
    int		changed= 0;

    if  ( to->tslTabStopCount > 0 || from->tslTabStopCount > 0 )
	{
	if  ( to->tslTabStopCount != from->tslTabStopCount )
	    { changed= 1;	}

	if  ( from->tslTabStopCount == 0 )
	    { to->tslTabStopCount= 0;	}
	else{
	    TabStop *			ts;
	    TabStop *			ots;
	    TabStop *			nts;
	    int				tab;

	    ts= (TabStop *)malloc( from->tslTabStopCount* sizeof(TabStop) );
	    if  ( ! ts )
		{ LXDEB(from->tslTabStopCount,ts); return -1;	}

	    ots= to->tslTabStops;
	    nts= from->tslTabStops;
	    for ( tab= 0; tab < from->tslTabStopCount; ots++, nts++, tab++ )
		{
		if  ( to->tslTabStopCount == from->tslTabStopCount )
		    {
		    int		posDiffer;

		    if  ( pixels )
			{ posDiffer= nts->tsPixels != ots->tsPixels;	}
		    else{ posDiffer= nts->tsTwips != ots->tsTwips;	}

		    if  ( posDiffer					||
			  nts->tsAlignment != ots->tsAlignment		||
			  nts->tsLeader != ots->tsLeader		||
			  nts->tsFromStyleOrList !=
						ots->tsFromStyleOrList	)
			{
			changed= 1;
			ts[tab]= *nts;
			}
		    else{ ts[tab]= *ots;	}
		    }
		else{ ts[tab]= *nts;		}
		}

	    to->tslTabStopCount= from->tslTabStopCount;
	    if  ( to->tslTabStops )
		{ free( to->tslTabStops );	}

	    to->tslTabStops= ts;
	    }
	}

    *pChanged= changed;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare tab stop lists.						*/
/*									*/
/************************************************************************/

void docCompareTabStopLists(	int *			pDifferent,
				const TabStopList *	tsl1,
				const TabStopList *	tsl2 )
    {
    int		different= 0;

    if  ( tsl1->tslTabStopCount > 0 || tsl2->tslTabStopCount > 0 )
	{
	if  ( tsl1->tslTabStopCount != tsl2->tslTabStopCount )
	    { different= 1;	}
	else{
	    TabStop *			ts1;
	    TabStop *			ts2;
	    int				tab;

	    ts1= tsl1->tslTabStops;
	    ts2= tsl2->tslTabStops;
	    for ( tab= 0; tab < tsl2->tslTabStopCount; ts1++, ts2++, tab++ )
		{
		if  ( ts1->tsTwips != ts2->tsTwips			||
		      ts1->tsAlignment != ts2->tsAlignment		||
		      ts1->tsLeader != ts2->tsLeader			||
		      ts1->tsFromStyleOrList != ts2->tsFromStyleOrList	)
		    { different= 1; }
		}
	    }
	}

    *pDifferent= different;
    return;
    }

/************************************************************************/
/*									*/
/*  Find the position of the next tab.					*/
/*									*/
/*  1)  The automatic tab indent implied by the hanging indent.		*/
/*									*/
/************************************************************************/

int docNextTabStop(	TabStop *			pTs,
			int *				pX,
			int *				pTab,
			const TabStopList *		tsl,
			int				x0Geometry,
			int				x0TextLines,
			int				tabInterval,
			int				xPosition )
    {
    int				tab;
    const TabStop *		ts= tsl->tslTabStops;

    /*  1  */
    if  ( xPosition < x0TextLines			&&
	  ( tsl->tslTabStopCount == 0		||
	    ts->tsAlignment != DOCtaLEFT	)	)
	{ *pX= x0TextLines; *pTab= -1; return 0; }

    if  ( tsl->tslTabStopCount == 0 )
	{
	if  ( tabInterval == 0 )
	    {
	    LLDEB(tsl->tslTabStopCount,tabInterval);
	    *pX= xPosition; *pTab= -1; return 0;
	    }
	else{
	    *pTab= -1;

	    xPosition -= x0Geometry;
	    *pX= ( xPosition+ 1 )/ tabInterval;
	    *pX *= tabInterval;
	    *pX += tabInterval;
	    *pX += x0Geometry;
	    return 0;
	    }
	}
    else{
	for ( tab= 0; tab < tsl->tslTabStopCount; ts++, tab++ )
	    {
	    if  ( ts->tsTwips > xPosition- x0Geometry )
		{ break;	}
	    }

	if  ( tab >= tsl->tslTabStopCount )
	    {
	    if  ( tabInterval > 0 )
		{
		xPosition -= x0Geometry;
		*pX= ( xPosition+ 1 )/ tabInterval;
		*pX *= tabInterval;
		*pX += tabInterval;
		*pX += x0Geometry;
		*pTab= -1;

		return 0;
		}
	    else{ LDEB(tabInterval); return 1;	}
	    }

	*pTs= *ts;
	*pTab= tab;
	*pX= ts->tsTwips+ x0Geometry;
	return 0;
	}
    }

/************************************************************************/
/*									*/
/*  A new series of tab stops is returned from the ruler. It is the	*/
/*  retult of some user action on the current ruler. Merge the changes	*/
/*  into the current ruler.						*/
/*									*/
/*  In all practical situaltions, exactly one tab is moved, OR exactly	*/
/*  one tab is added OR exactly one tab is deleted. This routine is	*/
/*  written to handle any set of changes, but it tries to keep as many	*/
/*  positions in twips as possible: Pixels are less precise and		*/
/*  documents would change all the time because tabs have moved a few	*/
/*  invisible twips.							*/
/*									*/
/*  1)  If the number of tab stops has increased, claim extra memory.	*/
/*	In this way we can be sure to have enough later on.		*/
/*  2)  Look for the first tab that has a different position in terms	*/
/*	of pixels. In the mean time, copy all properties that might	*/
/*	been changed on the ruler, but keep the original twips		*/
/*	position.							*/
/*  3)  The same thing to look for the last one.			*/
/*  4)  Close the hole of missing tabs or open a hole for added ones.	*/
/*	Also Copy any properties from the new ruler, but keep the twips	*/
/*	position.							*/
/*  5)  Copy any different new tabs into the new ruler. Translate	*/
/*	pixels to twips in the mean time.				*/
/*									*/
/************************************************************************/

int docRulerMergeTabs(		TabStopList *		tsl,
				double			xfac,
				int			x0GeometryPixels,
				int			tabCount,
				const TabStop *		tabStops )
    {
    int			tab0;
    int			otab1;
    int			ntab1;

    int			twips;
    int			pixels;

    int			ntab;
    int			otab;

    /*  1  */
    if  ( tabCount > tsl->tslTabStopCount )
	{
	TabStop *	fresh;

	fresh= (TabStop *)realloc( tsl->tslTabStops,
						tabCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(tabCount,fresh); return -1;	}

	tsl->tslTabStops= fresh;
	}

    /*  2  */
    tab0= 0;
    while( tab0 < tsl->tslTabStopCount && tab0 < tabCount )
	{
	twips= tsl->tslTabStops[tab0].tsTwips;
	pixels= x0GeometryPixels+ TWIPStoPIXELS( xfac, twips );

	if  ( pixels != tabStops[tab0].tsPixels )
	    { break;	}

	tsl->tslTabStops[tab0]= tabStops[tab0];
	tsl->tslTabStops[tab0].tsTwips= twips;

	tab0++;
	}

    /*  3  */
    otab1= tsl->tslTabStopCount- 1;
    ntab1= tabCount- 1;
    while( otab1 >= tab0 && ntab1 >= tab0 )
	{
	twips= tsl->tslTabStops[otab1].tsTwips;
	pixels= x0GeometryPixels+ TWIPStoPIXELS( xfac, twips );

	if  ( pixels != tabStops[ntab1].tsPixels )
	    { break;	}

	/*  Done below
	tsl->tslTabStops[otab1]= tabStops[ntab1];
	tsl->tslTabStops[otab1].tsTwips= twips;
	*/

	otab1--; ntab1--;
	}

    /*  4  */
    if  ( tsl->tslTabStopCount < tabCount )
	{
	ntab= tabCount- 1;
	for ( otab= tsl->tslTabStopCount- 1; otab > otab1; ntab--, otab-- )
	    {
	    twips= tsl->tslTabStops[otab].tsTwips;

	    tsl->tslTabStops[ntab]= tabStops[ntab];
	    tsl->tslTabStops[ntab].tsTwips= twips;
	    }
	}
    else{
	ntab= ntab1+ 1;
	for ( otab= otab1+ 1; otab < tsl->tslTabStopCount; ntab++, otab++ )
	    {
	    twips= tsl->tslTabStops[otab].tsTwips;

	    tsl->tslTabStops[ntab]= tabStops[ntab];
	    tsl->tslTabStops[ntab].tsTwips= twips;
	    }
	}

    /*  5  */
    otab= tab0;
    for ( ntab= tab0; ntab <= ntab1; otab++, ntab++ )
	{
	pixels= tabStops[ntab].tsPixels;
	pixels -= x0GeometryPixels;
	twips= pixels/ xfac;

	tsl->tslTabStops[otab]= tabStops[ntab];
	tsl->tslTabStops[otab].tsTwips= twips;
	}

    tsl->tslTabStopCount= tabCount;

    return 0;
    }

