/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilPropMask.h>
#   include	<appDebugon.h>

#   include	"docNotesProperties.h"

/************************************************************************/
/*									*/
/*  Change note properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

void docUpdNotesProperties(
			PropertyMask *			pChgMask,
			NotesProperties *		np,
			const PropertyMask *		updMask,
			const NotesProperties *		npNew,
			int				propStartNr,
			int				propPosition,
			int				propRestart,
			int				propStyle )
    {
    if  ( PROPmaskISSET( updMask, propStartNr ) )
	{
	if  ( np->npStartNumber != npNew->npStartNumber )
	    {
	    np->npStartNumber= npNew->npStartNumber;
	    PROPmaskADD( pChgMask, propStartNr );
	    }
	}

    if  ( PROPmaskISSET( updMask, propPosition ) )
	{
	if  ( np->npPosition != npNew->npPosition )
	    {
	    np->npPosition= npNew->npPosition;
	    PROPmaskADD( pChgMask, propPosition );
	    }
	}

    if  ( PROPmaskISSET( updMask, propRestart ) )
	{
	if  ( np->npRestart != npNew->npRestart )
	    {
	    np->npRestart= npNew->npRestart;
	    PROPmaskADD( pChgMask, propRestart );
	    }
	}

    if  ( PROPmaskISSET( updMask, propStyle ) )
	{
	if  ( np->npNumberStyle != npNew->npNumberStyle )
	    {
	    np->npNumberStyle= npNew->npNumberStyle;
	    PROPmaskADD( pChgMask, propStyle );
	    }
	}

    return;
    }


/************************************************************************/
/*									*/
/*  Compare note properties.						*/
/*									*/
/************************************************************************/

void docNotesPropertyDifference(
			PropertyMask *			diffMask,
			const NotesProperties *		np1,
			const PropertyMask *		cmpMask,
			const NotesProperties *		np2,
			int				propStartNr,
			int				propPosition,
			int				propRestart,
			int				propStyle )
    {
    if  ( PROPmaskISSET( cmpMask, propStartNr ) )
	{
	if  ( np1->npStartNumber != np2->npStartNumber )
	    { PROPmaskADD( diffMask, propStartNr );	}
	}

    if  ( PROPmaskISSET( cmpMask, propPosition ) )
	{
	if  ( np1->npPosition != np2->npPosition )
	    { PROPmaskADD( diffMask, propPosition );	}
	}

    if  ( PROPmaskISSET( cmpMask, propRestart ) )
	{
	if  ( np1->npRestart != np2->npRestart )
	    { PROPmaskADD( diffMask, propRestart );	}
	}

    if  ( PROPmaskISSET( cmpMask, propStyle ) )
	{
	if  ( np1->npNumberStyle != np2->npNumberStyle )
	    { PROPmaskADD( diffMask, propStyle );	}
	}

    return;
    }

