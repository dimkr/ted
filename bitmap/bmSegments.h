#   ifndef		BM_SEGMENTS_H
#   define		BM_SEGMENTS_H

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	"bitmap.h"

typedef struct DataRun
    {
    short int	drX0;
    short int	drXp;
    short int	drRepeatCount;
    } DataRun;

typedef struct SegmentEdge
    {
    struct SegmentNode *	seFrom;
    struct SegmentNode *	seTo;
    DataRun *			seRuns;
    short int			seRunCount;
    } SegmentEdge;

typedef struct SegmentNode
    {
    SegmentEdge **	snUpEdges;
    SegmentEdge **	snDownEdges;
    short int		snUpEdgeCount;
    short int		snDownEdgeCount;
    short int		snY0;
    } SegmentNode;

/************************************************************************/
/*									*/
/*  A 'segment' as discovered during the segmentation process.		*/
/*									*/
/*  NOTE that conceptually there is a similarity with the graph		*/
/*	resulting from the skeleton of a component. In this		*/
/*	implementation, the is no relationship however.			*/
/*									*/
/************************************************************************/

typedef struct BitmapSegment
    {
    short int			bsX0;
    short int			bsX1;
    short int			bsY0;
    short int			bsY1;
    short int			bsNodeCount;
    short int			bsEdgeCount;
    SegmentNode **		bsNodes;
    SegmentEdge **		bsEdges;
    } BitmapSegment;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bcComponents(	BitmapSegment ***		pSegments,
				int *				pCount,
				const unsigned char *		buffer,
				const BitmapDescription *	bd );

extern void bmFreeSegment(	BitmapSegment * bs );

extern int bmcDrawComponent(	const BitmapSegment *	bs,
				unsigned char *		buffer,
				int			col0,
				int			row0,
				int			bytesPerRow,
				int			colorEncoding );

extern void bmcStatistics(	const BitmapSegment *		bs,
				int *				pN,
				float *				pSx,
				float *				pSy,
				float *				pSxx,
				float *				pSyy,
				float *				pSxy );

#   endif	/*	BM_SEGMENTS_H	*/
