/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docListOverrideTable.h"

void docInitListOverrideTable(		ListOverrideTable *		lot )
    {
    lot->lotOverrides= (ListOverride *)0;
    lot->lotOverrideCount= 0;

    return;
    }

void docCleanListOverrideTable(		ListOverrideTable *		lot )
    {
    int		i;

    for ( i= 0; i < lot->lotOverrideCount; i++ )
	{ docCleanListOverride( &(lot->lotOverrides[i]) );	}

    if  ( lot->lotOverrides )
	{ free( (void *)lot->lotOverrides );	}

    return;
    }

int docCopyListOverrideTable(	ListOverrideTable *		to,
				const ListOverrideTable *	from )
    {
    int			i;

    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;

    if  ( to->lotOverrideCount > from->lotOverrideCount )
	{
	for ( i= from->lotOverrideCount; i < to->lotOverrideCount; i++ )
	    { docCleanListOverride( &(to->lotOverrides[i]) );	}

	to->lotOverrideCount= from->lotOverrideCount;
	}

    if  ( to->lotOverrideCount < from->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= realloc( to->lotOverrides,
				from->lotOverrideCount* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(from->lotOverrideCount,fresh); return -1;	}

	to->lotOverrides= fresh;

	while( to->lotOverrideCount < from->lotOverrideCount )
	    {
	    docInitListOverride( &(to->lotOverrides[to->lotOverrideCount++]) );
	    }
	}

    for ( i= 0; i < from->lotOverrideCount; i++ )
	{
	if  ( docCopyListOverride( &(to->lotOverrides[i]),
				&(from->lotOverrides[i]), fontMap, colorMap ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a list to the listtable.					*/
/*									*/
/************************************************************************/

int docListOverrideTableAddOverride(	ListOverrideTable *	lot,
					const ListOverride *	lo,
					int			idx,
					const int *		fontMap,
					const int *		colorMap )
    {
    if  ( idx >= lot->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= (ListOverride *)realloc( lot->lotOverrides,
				    (idx+ 1)* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(idx,fresh); return -1;	}

	lot->lotOverrides= fresh;

	fresh += lot->lotOverrideCount;
	while( lot->lotOverrideCount <= idx )
	    {
	    docInitListOverride( fresh );
	    lot->lotOverrideCount++; fresh++;
	    }
	}

    if  ( lo							&&
	  docCopyListOverride( lot->lotOverrides+ idx, lo,
					    fontMap, colorMap )	)
	{ LDEB(lot->lotOverrideCount); return -1;	}
    lot->lotOverrides[idx].loIndex= idx;
    lot->lotOverrides[idx].loListIndex= -1;

    return 0;
    }

extern int docMergeListOverrideIntoTable(
					ListOverrideTable *	lot,
					const ListOverride *	loNew,
					const int *		fontMap,
					const int *		colorMap )
    {
    int			idx= lot->lotOverrideCount;

    if  ( idx < 1 )
	{ idx=  1;	}

    if  ( loNew->loIndex > 0					&&
	  loNew->loIndex < lot->lotOverrideCount		&&
	  lot->lotOverrides[loNew->loIndex].loIndex ==
					    loNew->loIndex	&&
	  lot->lotOverrides[loNew->loIndex].loListID ==
					    loNew->loListID	)
	{ return loNew->loIndex;	}

    if  ( docListOverrideTableAddOverride( lot, loNew, idx,
							fontMap, colorMap ) )
	{ LLDEB(idx,lot->lotOverrideCount); return -1;	}

    return idx;
    }

