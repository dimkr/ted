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

#   include	"docListTable.h"

void docInitListTable(		DocumentListTable *		dlt )
    {
    dlt->dltLists= (DocumentList *)0;
    dlt->dltListCount= 0;

    return;
    }

void docCleanListTable(		DocumentListTable *		dlt )
    {
    int		i;

    for ( i= 0; i < dlt->dltListCount; i++ )
	{ docCleanDocumentList( &(dlt->dltLists[i]) );	}

    if  ( dlt->dltLists )
	{ free( (void *)dlt->dltLists );	}

    return;
    }

int docCopyListTable(		DocumentListTable *		to,
				const DocumentListTable *	from )
    {
    int			i;

    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;

    if  ( to->dltListCount > from->dltListCount )
	{
	for ( i= from->dltListCount; i < to->dltListCount; i++ )
	    { docCleanDocumentList( &(to->dltLists[i]) );	}

	to->dltListCount= from->dltListCount;
	}

    if  ( to->dltListCount < from->dltListCount )
	{
	DocumentList *	fresh;

	fresh= realloc( to->dltLists,
				from->dltListCount* sizeof(DocumentList) );
	if  ( ! fresh )
	    { LXDEB(from->dltListCount,fresh); return -1;	}

	to->dltLists= fresh;

	while( to->dltListCount < from->dltListCount )
	    { docInitDocumentList( &(to->dltLists[to->dltListCount++]) ); }
	}

    for ( i= 0; i < from->dltListCount; i++ )
	{
	if  ( docCopyDocumentList( &(to->dltLists[i]), &(from->dltLists[i]),
							fontMap, colorMap ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a list to the listtable.					*/
/*									*/
/************************************************************************/

int docDocumentListTableAddList( DocumentListTable *		dlt,
				const DocumentList *		dl,
				const int *			fontMap,
				const int *			colorMap )
    {
    DocumentList *	fresh;

    fresh= (DocumentList *)realloc( dlt->dltLists,
			    (dlt->dltListCount+ 1)* sizeof(DocumentList) );
    if  ( ! fresh )
	{ LXDEB(dlt->dltListCount,fresh); return -1;	}
    dlt->dltLists= fresh;

    fresh += dlt->dltListCount;

    docInitDocumentList( fresh );
    if  ( dl && docCopyDocumentList( fresh, dl, fontMap, colorMap ) )
	{ LDEB(dlt->dltListCount); return -1;	}

    dlt->dltListCount++;
    return 0;
    }

int docMergeListIntoTable(	DocumentListTable *		dlt,
				const DocumentList *		dlNew,
				const int *			fontMap,
				const int *			colorMap )
    {
    int			oldCount= dlt->dltListCount;
    DocumentList *	dl;

    int		i;

    dl= dlt->dltLists;
    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	int		level;

	if  ( dl->dlListID != dlNew->dlListID )
	    { continue;	}
	if  ( dl->dlListTemplateID != dlNew->dlListTemplateID )
	    { LLDEB(dl->dlListTemplateID,dlNew->dlListTemplateID);	}
	if  ( dl->dlListStyleID != dlNew->dlListStyleID )
	    { LLDEB(dl->dlListTemplateID,dlNew->dlListTemplateID);	}

	for ( level= dl->dlLevelCount; level < dlNew->dlLevelCount; level++ )
	    {
	    if  ( docDocumentListAddLevel( dl, dlNew->dlLevels+ level,
							fontMap, colorMap ) )
		{ LDEB(level); return -1;	}
	    }

	return i;
	}

    if  ( docDocumentListTableAddList( dlt, dlNew, fontMap, colorMap ) )
	{ LDEB(dlt->dltListCount); return -1;	}

    return oldCount;
    }
