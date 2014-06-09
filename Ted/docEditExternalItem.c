/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Copy a note from one paragraph to another.				*/
/*									*/
/************************************************************************/

int docCopyNote(		DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				TextParticule *		tpTo,
				const BufferItem *	biFrom,
				const TextParticule *	tpFrom )
    {
    const BufferDocument *	bdFrom= dcj->dcjBdFrom;
    BufferDocument *		bdTo= dcj->dcjBdTo;

    DocumentNote *		dnTo;
    DocumentNote *		dnFrom;
    BufferItem *		sectBiTo;
    int				noteIndex;

    if  ( biTo->biInExternalItem != DOCinBODY )
	{
	LDEB(biTo->biInExternalItem);
	tpTo->tpKind= DOCkindTAB;
	return 0;
	}

    sectBiTo= biTo;
    while( sectBiTo && sectBiTo->biLevel != DOClevSECT )
	{ sectBiTo= sectBiTo->biParent;	}
    if  ( ! sectBiTo )
	{ XDEB(sectBiTo); return -1;	}

    if  ( docGetNote( &dnFrom, bdFrom, biFrom, tpFrom->tpStroff ) < 0 )
	{ LDEB(tpFrom->tpStroff); return -1;	}

    noteIndex= docInsertNote( &dnTo, bdTo, biTo,
				tpTo->tpStroff, dnFrom->dnAutoNumber );
    if  ( noteIndex < 0 )
	{ LLDEB(tpTo->tpStroff,noteIndex); return -1;	}

    if  ( docGetNote( &dnFrom, bdFrom, biFrom, tpFrom->tpStroff ) < 0 )
	{ LDEB(tpFrom->tpStroff); return -1;	}

    if  ( dnFrom->dnExternalItem.eiItem )
	{
	SelectionScope	ssRoot;

	docInitSelectionScope( &ssRoot );

	ssRoot.ssInExternalItem= dnFrom->dnExternalItemKind;
	ssRoot.ssSectNrExternalTo= sectBiTo->biNumberInParent;
	ssRoot.ssNoteArrayIndex= noteIndex;

	dnTo->dnExternalItem.eiItem= docCopySectItem( dcj, eo, &ssRoot,
			    (BufferItem *)0, 0,
			    dnFrom->dnExternalItem.eiItem );

	if  ( ! dnTo->dnExternalItem.eiItem )
	    { XDEB(dnTo->dnExternalItem.eiItem); return -1;	}
	}

    dnTo->dnExternalItemKind= dnFrom->dnExternalItemKind;

    switch( dnTo->dnExternalItemKind )
	{
	case DOCinFOOTNOTE:
	    if  ( docCheckNoteSeparatorItem( bdTo, DOCinFTNSEP ) )
		{ LDEB(dnTo->dnExternalItemKind); return -1;	}
	    break;

	case DOCinENDNOTE:
	    if  ( docCheckNoteSeparatorItem( bdTo, DOCinAFTNSEP ) )
		{ LDEB(dnTo->dnExternalItemKind); return -1;	}
	    break;

	default:
	    LDEB(dnFrom->dnExternalItemKind);
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a shape.							*/
/*									*/
/*  1)  Children.							*/
/*  2)  Vertices.							*/
/*  2)  Wrap polygon.							*/
/*  4)  Segment information						*/
/*  5)  Text box content.						*/
/*									*/
/************************************************************************/

DrawingShape * docCopyDrawingShape(
				DocumentCopyJob *	dcj,
				EditOperation *		eo,
				const DrawingShape *	from )
    {
    DrawingShape *	rval= (DrawingShape *)0;
    DrawingShape *	to;

    to= (DrawingShape *)malloc( sizeof(DrawingShape) );
    if  ( ! to )
	{ XDEB(to); goto ready;	}

    *to= *from;
    docInitDrawingShapeAllocated( to );

    /*  1  */
    to->dsChildren= malloc( from->dsChildCount* sizeof(DrawingShape *) );
    if  ( ! to->dsChildren )
	{ LXDEB(from->dsChildCount,to->dsChildren); goto ready;	}

    /*  2  */
    to->dsVertices= malloc( from->dsVertexCount* sizeof(ShapeVertex *) );
    if  ( ! to->dsVertices )
	{ LXDEB(from->dsVertexCount,to->dsVertices); goto ready;	}

    /*  3  */
    to->dsWrapPolygonVertices= malloc(
			from->dsWrapPolygonVertexCount* sizeof(ShapeVertex *) );
    if  ( ! to->dsWrapPolygonVertices )
	{
	LXDEB(from->dsWrapPolygonVertexCount,to->dsWrapPolygonVertices);
	goto ready;
	}

    /*  4  */
    to->dsSegmentInfos= malloc( from->dsSegmentInfoCount* sizeof(int) );
    if  ( ! to->dsSegmentInfos )
	{ LXDEB(from->dsSegmentInfoCount,to->dsSegmentInfos); goto ready; }

    /*  1  */
    while( to->dsChildCount < from->dsChildCount )
	{
	to->dsChildren[to->dsChildCount]= docCopyDrawingShape( dcj, eo,
					from->dsChildren[to->dsChildCount] );
	if  ( ! to->dsChildren[to->dsChildCount] )
	    {
	    LXDEB(to->dsChildCount,to->dsChildren[to->dsChildCount]);
	    goto ready;
	    }
	to->dsChildCount++;
	}

    /*  2  */
    while( to->dsVertexCount < from->dsVertexCount )
	{
	to->dsVertices[to->dsVertexCount]= from->dsVertices[to->dsVertexCount];
	to->dsVertexCount++;
	}

    /*  3  */
    while( to->dsWrapPolygonVertexCount < from->dsWrapPolygonVertexCount )
	{
	to->dsWrapPolygonVertices[to->dsWrapPolygonVertexCount]=
		    from->dsWrapPolygonVertices[to->dsWrapPolygonVertexCount];
	to->dsWrapPolygonVertexCount++;
	}

    /*  4  */
    while( to->dsSegmentInfoCount < from->dsSegmentInfoCount )
	{
	to->dsSegmentInfos[to->dsSegmentInfoCount]=
				from->dsSegmentInfos[to->dsSegmentInfoCount];
	to->dsSegmentInfoCount++;
	}

    /*  5  */
    if  ( from->dsExternalItem.eiItem )
	{
	to->dsExternalItem.eiItem= docCopySectItem( dcj, eo,
			&(from->dsExternalItem.eiItem->biSectSelectionScope),
			(BufferItem *)0, 0,
			from->dsExternalItem.eiItem );

	if  ( ! to->dsExternalItem.eiItem )
	    { XDEB(to->dsExternalItem.eiItem); goto ready;	}
	}

    rval= to; to= (DrawingShape *)0; /* steal */

  ready:

    if  ( to )
	{ docFreeDrawingShape( dcj->dcjBdTo, to ); }

    return rval;
    }

