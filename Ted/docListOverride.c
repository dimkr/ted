/************************************************************************/
/*									*/
/*  Manage list overrides.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docListOverride.h"

/************************************************************************/
/*									*/
/*  Individual list overrides.						*/
/*									*/
/************************************************************************/

void docInitListOverride(	ListOverride *	lo )
    {
    int		i;

    lo->loListID= -1;
    lo->loIndex= 0;
    lo->loListIndex= -1;
    lo->loOverrideCount= 0;

    lo->loLevelCount= 0;
    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitListOverrideLevel( &(lo->loLevels[i]) );	}

    return;
    }

void docCleanListOverride(	ListOverride *	lo )
    {
    int		i;

    for ( i= 0; i < lo->loLevelCount; i++ )
	{ docCleanListOverrideLevel( &(lo->loLevels[i]) );	}

    return;
    }

int docCopyListOverride(	ListOverride *		to,
				const ListOverride *	from,
				const int *		fontMap,
				const int *		colorMap )
    {
    int			rval= 0;

    int			i;
    ListOverrideLevel	levels[DLmaxLEVELS];

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitListOverrideLevel( &(levels[i]) );	}

    for ( i= 0; i < from->loLevelCount; i++ )
	{
	if  ( docCopyListOverrideLevel( &(levels[i]), &(from->loLevels[i]),
							fontMap, colorMap ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    for ( i= 0; i < to->loLevelCount; i++ )
	{
	docCleanListOverrideLevel( &(to->loLevels[i]) );
	/*  Not needed because of assignment below
	docInitListOverrideLevel( &(to->loLevels[i]) );
	*/
	}

    *to= *from;

    for ( i= 0; i < to->loLevelCount; i++ )
	{
	to->loLevels[i]= levels[i];
	docInitListOverrideLevel( &(levels[i]) );
	}

  ready:

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docCleanListOverrideLevel( &(levels[i]) );	}

    return rval;
    }

int docListOverrideAddLevel(	ListOverride *			lo,
				const ListOverrideLevel *	lol,
				const int *			fontMap,
				const int *			colorMap )
    {
    if  ( lo->loLevelCount < 0 || lo->loLevelCount >= DLmaxLEVELS )
	{ LLDEB(lo->loLevelCount,DLmaxLEVELS); return -1;	}

    if  ( docCopyListOverrideLevel( &(lo->loLevels[lo->loLevelCount]), lol,
							fontMap, colorMap ) )
	{ LDEB(lo->loLevelCount); return -1;	}

    lo->loLevelCount++;

    return 0;
    }

