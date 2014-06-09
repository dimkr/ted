/************************************************************************/
/*									*/
/*  Management of the lists in the list table of a document.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docDocumentList.h"

/************************************************************************/
/*									*/
/*  Individual lists.							*/
/*									*/
/************************************************************************/

void docInitDocumentList(	DocumentList *	dl )
    {
    int		i;

    dl->dlListID= -1;
    dl->dlListTemplateID= -1;
    dl->dlListHybrid= 1;
    dl->dlRestartPerSect= 0;
    dl->dlListName= (char *)0;

    dl->dlListStyleID= -1;
    dl->dlListStyleName= (char *)0;

    dl->dlLevelCount= 0;
    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitDocumentListLevel( &(dl->dlLevels[i]) );	}

    return;
    }

void docCleanDocumentList(	DocumentList *	dl )
    {
    int		i;

    if  ( dl->dlListName )
	{ free( dl->dlListName ); }
    if  ( dl->dlListStyleName )
	{ free( dl->dlListStyleName ); }

    for ( i= 0; i < dl->dlLevelCount; i++ )
	{ docCleanDocumentListLevel( &(dl->dlLevels[i]) );	}

    return;
    }

int docCopyDocumentList(	DocumentList *		to,
				const DocumentList *	from,
				const int *		fontMap,
				const int *		colorMap )
    {
    int			rval= 0;

    char *		name= (char *)0;
    char *		styleName= (char *)0;

    int			i;
    DocumentListLevel	levels[DLmaxLEVELS];

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitDocumentListLevel( &(levels[i]) );	}

    if  ( from->dlListName )
	{
	name= strdup( from->dlListName );
	if  ( ! name )
	    { XDEB(name); rval= -1; goto ready;	}
	}

    if  ( from->dlListStyleName )
	{
	styleName= strdup( from->dlListStyleName );
	if  ( ! styleName )
	    { XDEB(styleName); rval= -1; goto ready;	}
	}

    for ( i= 0; i < from->dlLevelCount; i++ )
	{
	if  ( docCopyDocumentListLevel( &(levels[i]), &(from->dlLevels[i]),
							fontMap, colorMap ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    for ( i= 0; i < to->dlLevelCount; i++ )
	{
	docCleanDocumentListLevel( &(to->dlLevels[i]) );
	/*  Not needed because of assignment below
	docInitDocumentListLevel( &(to->dlLevels[i]) );
	*/
	}

    if  ( to->dlListName )
	{ free( to->dlListName );	}
    if  ( to->dlListStyleName )
	{ free( to->dlListStyleName );	}

    *to= *from;

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitDocumentListLevel( &(to->dlLevels[i]) ); }

    for ( i= 0; i < to->dlLevelCount; i++ )
	{
	to->dlLevels[i]= levels[i];
	docInitDocumentListLevel( &(levels[i]) );
	}

    to->dlListName= name; name= (char *)0;
    to->dlListStyleName= styleName; styleName= (char *)0;

  ready:

    if  ( name )
	{ free( name );	}
    if  ( styleName )
	{ free( styleName );	}

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docCleanDocumentListLevel( &(levels[i]) );	}

    return rval;
    }

int docDocumentListAddLevel(	DocumentList *			dl,
				const DocumentListLevel *	dll,
				const int *			fontMap,
				const int *			colorMap )
    {
    if  ( dl->dlLevelCount < 0 || dl->dlLevelCount >= DLmaxLEVELS )
	{ LLDEB(dl->dlLevelCount,DLmaxLEVELS); return -1;	}

    if  ( docCopyDocumentListLevel( &(dl->dlLevels[dl->dlLevelCount]), dll,
							fontMap, colorMap ) )
	{ LDEB(dl->dlLevelCount); return -1;	}

    dl->dlLevelCount++;

    return 0;
    }

