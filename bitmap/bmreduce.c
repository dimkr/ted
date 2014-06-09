#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Color reduction.							*/
/*									*/
/************************************************************************/

#   define	LOG_CUTS	0
#   define	LOG_PALETTE	0

typedef struct HistogramEntry
    {
    int			heCount;
    RGB8Color		heColor;
    } HistogramEntry;

#   define	heRed	heColor.rgb8Red
#   define	heGreen	heColor.rgb8Green
#   define	heBlue	heColor.rgb8Blue

typedef struct	HashBucket
    {
    HistogramEntry	hbHistogramEntry;
    int			hbNumber;
    struct HashBucket *	hbNext;
    } HashBucket;

#   define	hbCount	hbHistogramEntry.heCount
#   define	hbRed	hbHistogramEntry.heColor.rgb8Red
#   define	hbGreen	hbHistogramEntry.heColor.rgb8Green
#   define	hbBlue	hbHistogramEntry.heColor.rgb8Blue

typedef struct ColorBox
    {
    int		cbHistogramOffset;
    int		cbColorCount;
    int		cbPixelCount;
    int		cbCutNumber;

    int		cbRMin;
    int		cbRMax;
    int		cbGMin;
    int		cbGMax;
    int		cbBMin;
    int		cbBMax;
    } ColorBox;

typedef struct CutNode
    {
    unsigned int	cnLeft;
    unsigned int	cnRight;
    unsigned char	cnLeftMaximum;
    unsigned char	cnComponent;
    } CutNode;

#   define	CN_LEAF		0
#   define	CN_RED		1
#   define	CN_GREEN	2
#   define	CN_BLUE		3

typedef struct ColorReducer
    {
    HashBucket **		crHashTable;
    HistogramEntry *		crHistogram;
    RGB8Color *			crPalette;
    CutNode *			crCutNodes;
    int				crColorsCounted;
    } ColorReducer;

/************************************************************************/
/*									*/
/*  Color hash varia.							*/
/*									*/
/************************************************************************/

#define HASH_SIZE 6553

#define ppm_hash(r,g,b)	((( (int) (r) * 33023 +    \
			    (int) (g) * 30013 +    \
			    (int) (b) * 27011 ) & 0x7fffffff) % HASH_SIZE )

static void bmFreeColorHash(	HashBucket **	hashTable	)
    {
    int			i;

    for ( i= 0; i < HASH_SIZE; i++ )
	{
	HashBucket *	hashBucket= hashTable[i];

	while( hashBucket )
	    {
	    HashBucket *	nx= hashBucket->hbNext;
	    free( (char *)hashBucket );
	    hashBucket= nx;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a new hash item: Convenience routine.			*/
/*									*/
/************************************************************************/

static int bmInsertHash(	HashBucket **	hashTable,
				int		hash,
				int		number,
				int		r,
				int		g,
				int		b )
    {
    HashBucket *	hashBucket= (HashBucket *)malloc( sizeof(HashBucket) );

    if  ( ! hashBucket )
	{ XDEB(hashBucket); return -1;	}

    hashBucket->hbNext= hashTable[hash];
    hashTable[hash]= hashBucket;

    hashBucket->hbCount= 1;
    hashBucket->hbNumber= number;

    hashBucket->hbRed= r;
    hashBucket->hbGreen= g;
    hashBucket->hbBlue= b;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Utility functions: Manage a color reducer.				*/
/*									*/
/************************************************************************/

static void bmInitColorReducer(	ColorReducer *	cr )
    {
    cr->crHashTable= (HashBucket **)0;
    cr->crHistogram= (HistogramEntry *)0;
    cr->crPalette= (RGB8Color *)0;
    cr->crCutNodes= (CutNode *)0;

    return;
    }

static void bmCleanColorReducer( ColorReducer *	cr )
    {
    if  ( cr->crPalette )
	{ free( cr->crPalette );		}
    if  ( cr->crHistogram )
	{ free( cr->crHistogram );		}
    if  ( cr->crCutNodes )
	{ free( cr->crCutNodes );		}
    if  ( cr->crHashTable )
	{ bmFreeColorHash( cr->crHashTable );	}

    return;
    }

static int bmAllocateReducerPalette(	ColorReducer *	cr,
					int		count )
    {
    RGB8Color *	fresh;

    fresh= (RGB8Color *)realloc( cr->crPalette, count* sizeof(RGB8Color) );
    if  ( ! fresh )
	{ LLDEB(count,fresh); return -1;	}

    cr->crPalette= fresh;
    memset( cr->crPalette, 0, count* sizeof(RGB8Color) );

    return 0;
    }

static int bmAllocateCutNodes(		ColorReducer *	cr,
					int		colorCount )
    {
    CutNode *	fresh;

    fresh= (CutNode *)realloc( cr->crCutNodes, 2* colorCount* sizeof(CutNode) );
    if  ( ! fresh )
	{ LLDEB(colorCount,fresh); return -1;	} 

    cr->crCutNodes= fresh;
    memset( cr->crCutNodes, 0, 2* colorCount* sizeof(CutNode) );

    return 0;
    }

static int bmAllocateHistogram(		ColorReducer *	cr,
					int		colorCount )
    {
    HistogramEntry *	fresh;

    fresh= (HistogramEntry *)realloc( cr->crHistogram,
					colorCount* sizeof(HistogramEntry) );
    if  ( ! fresh )
	{ LLDEB(colorCount,fresh); return -1; }

    cr->crHistogram= fresh;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine median.							*/
/*									*/
/************************************************************************/

static int bmHistRedMedian(	int *			pPixelCount,
				int *			pColorCount,
				int			pixelCount,
				HistogramEntry *	histogram,
				int			colorCount,
				unsigned int		cMin,
				unsigned int		cMax )
    {
    int		cMed;

    int		lt= 0;
    int		eq= 0;
    int		gt= 0;

    cMax++;

    cMed= ( cMin+ cMax )/ 2;
    for (;;)
	{
	int			i;
	const HistogramEntry *	he;

	lt= 0; eq= 0; gt= 0;

	he= histogram;
	for ( i= 0; i < colorCount; he++, i++ )
	    {
	    if  ( he->heRed > cMed )
		{ gt += he->heCount; continue; }
	    if  ( he->heRed < cMed )
		{ lt += he->heCount; continue; }

	    eq += he->heCount; continue;
	    }

	if  ( lt+ eq < pixelCount/ 2 )
	    {
	    cMin= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	if  ( eq+ gt < pixelCount/ 2 )
	    {
	    cMax= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	break;
	}

    if  ( lt+ eq < eq+ gt )
	{ pixelCount= lt+ eq;		}
    else{ cMed--; pixelCount= lt;	}

    lt= 0; gt= colorCount- 1;
    while( gt > lt )
	{
	while( gt > lt && histogram[lt].heColor.rgb8Red <= cMed )
	    { lt++;	}
	while( gt > lt && histogram[gt].heColor.rgb8Red >  cMed )
	    { gt--;	}

	if  ( gt > lt )
	    {
	    HistogramEntry	swap;

	    swap= histogram[lt];
	    histogram[lt]= histogram[gt];
	    histogram[gt]= swap;
	    }
	}

    while( lt < colorCount && histogram[lt].heColor.rgb8Red <= cMed )
	{ lt++;	}

    *pPixelCount= pixelCount; *pColorCount= lt; return cMed;
    }

static int bmHistGreenMedian(	int *			pPixelCount,
				int *			pColorCount,
				int			pixelCount,
				HistogramEntry *	histogram,
				int			colorCount,
				unsigned int		cMin,
				unsigned int		cMax )
    {
    int		cMed;

    int		lt= 0;
    int		eq= 0;
    int		gt= 0;

    cMax++;

    cMed= ( cMin+ cMax )/ 2;
    for (;;)
	{
	int			i;
	const HistogramEntry *	he;

	lt= 0; eq= 0; gt= 0;

	he= histogram;
	for ( i= 0; i < colorCount; he++, i++ )
	    {
	    if  ( he->heGreen > cMed )
		{ gt += he->heCount; continue; }
	    if  ( he->heGreen < cMed )
		{ lt += he->heCount; continue; }

	    eq += he->heCount; continue;
	    }

	if  ( lt+ eq < pixelCount/ 2 )
	    {
	    cMin= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	if  ( eq+ gt < pixelCount/ 2 )
	    {
	    cMax= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	break;
	}

    if  ( lt+ eq < eq+ gt )
	{ pixelCount= lt+ eq;		}
    else{ cMed--; pixelCount= lt;	}

    lt= 0; gt= colorCount- 1;
    while( gt > lt )
	{
	while( gt > lt && histogram[lt].heColor.rgb8Green <= cMed )
	    { lt++;	}
	while( gt > lt && histogram[gt].heColor.rgb8Green >  cMed )
	    { gt--;	}

	if  ( gt > lt )
	    {
	    HistogramEntry	swap;

	    swap= histogram[lt];
	    histogram[lt]= histogram[gt];
	    histogram[gt]= swap;
	    }
	}

    while( lt < colorCount && histogram[lt].heColor.rgb8Green <= cMed )
	{ lt++;	}

    *pPixelCount= pixelCount; *pColorCount= lt; return cMed;
    }

static int bmHistBlueMedian(	int *			pPixelCount,
				int *			pColorCount,
				int			pixelCount,
				HistogramEntry *	histogram,
				int			colorCount,
				unsigned int		cMin,
				unsigned int		cMax )
    {
    int		cMed;

    int		lt= 0;
    int		eq= 0;
    int		gt= 0;

    cMax++;


    cMed= ( cMin+ cMax )/ 2;
    for (;;)
	{
	int			i;
	const HistogramEntry *	he;

	lt= 0; eq= 0; gt= 0;

	he= histogram;
	for ( i= 0; i < colorCount; he++, i++ )
	    {
	    if  ( he->heBlue > cMed )
		{ gt += he->heCount; continue; }
	    if  ( he->heBlue < cMed )
		{ lt += he->heCount; continue; }

	    eq += he->heCount; continue;
	    }

	if  ( lt+ eq < pixelCount/ 2 )
	    {
	    cMin= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	if  ( eq+ gt < pixelCount/ 2 )
	    {
	    cMax= cMed;
	    cMed= ( cMin+ cMax )/ 2;
	    continue;
	    }

	break;
	}

    if  ( lt+ eq < eq+ gt )
	{ pixelCount= lt+ eq;		}
    else{ cMed--; pixelCount= lt;	}

    lt= 0; gt= colorCount- 1;
    while( gt > lt )
	{
	while( gt > lt && histogram[lt].heColor.rgb8Blue <= cMed )
	    { lt++;	}
	while( gt > lt && histogram[gt].heColor.rgb8Blue >  cMed )
	    { gt--;	}

	if  ( gt > lt )
	    {
	    HistogramEntry	swap;

	    swap= histogram[lt];
	    histogram[lt]= histogram[gt];
	    histogram[gt]= swap;
	    }
	}

    while( lt < colorCount && histogram[lt].heColor.rgb8Blue <= cMed )
	{ lt++;	}

    *pPixelCount= pixelCount; *pColorCount= lt; return cMed;
    }

/************************************************************************/
/*									*/
/*  Determine the range of the colors in a box.				*/
/*									*/
/************************************************************************/

static void bmDelimitColorBox(		ColorBox *		cb,
					const HistogramEntry *	histogram )
    {
    const HistogramEntry *	he;
    int				i;

    if  ( cb->cbColorCount == 0 )
	{ LDEB(cb->cbColorCount); return;	}

    he= histogram+ cb->cbHistogramOffset;
    cb->cbRMin= cb->cbRMax= he->heRed;
    cb->cbGMin= cb->cbGMax= he->heGreen;
    cb->cbBMin= cb->cbBMax= he->heBlue;

    he++;

    for ( i= 1; i < cb->cbColorCount; he++, i++ )
	{
	if  ( cb->cbRMin > he->heRed )
	    { cb->cbRMin=  he->heRed;	}
	if  ( cb->cbRMax < he->heRed )
	    { cb->cbRMax=  he->heRed;	}

	if  ( cb->cbGMin > he->heGreen )
	    { cb->cbGMin=  he->heGreen; }
	if  ( cb->cbGMax < he->heGreen )
	    { cb->cbGMax=  he->heGreen; }

	if  ( cb->cbBMin > he->heBlue )
	    { cb->cbBMin=  he->heBlue; }
	if  ( cb->cbBMax < he->heBlue )
	    { cb->cbBMax=  he->heBlue; }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the median for splitting a box.				*/
/*									*/
/************************************************************************/

static void bmSplitBox(	HistogramEntry *	histogram,
			const ColorBox *	cbParent,
			ColorBox *		cbLow,
			ColorBox *		cbHigh,
			int *			pLeftMaximum,
			int *			pComponent )
    {
    int				rDif= cbParent->cbRMax- cbParent->cbRMin;
    int				gDif= cbParent->cbGMax- cbParent->cbGMin;
    int				bDif= cbParent->cbBMax- cbParent->cbBMin;

    histogram += cbParent->cbHistogramOffset;

    *cbLow= *cbParent;
    *cbHigh= *cbParent;

    if  ( 77* rDif > 150* gDif && 77* rDif >  29* bDif )
	{
	cbLow->cbRMax= bmHistRedMedian( &(cbLow->cbPixelCount),
				    &(cbLow->cbColorCount),
				    cbParent->cbPixelCount, histogram,
				    cbParent->cbColorCount,
				    cbParent->cbRMin, cbParent->cbRMax );

	cbHigh->cbRMin= cbLow->cbRMax+ 1;
	cbHigh->cbColorCount= cbParent->cbColorCount- cbLow->cbColorCount;
	cbHigh->cbPixelCount= cbParent->cbPixelCount- cbLow->cbPixelCount;
	cbHigh->cbHistogramOffset= cbParent->cbHistogramOffset+
							cbLow->cbColorCount;
	*pLeftMaximum= cbLow->cbRMax;
	*pComponent= CN_RED;

#	if  LOG_CUTS
	appDebug( "CUT RED   at %3d..%3d..%3d "
			    "%4d+ %4d= %4dC %6d+ %6d= %6dP\n",
			    cbParent->cbRMin, cbLow->cbRMax, cbParent->cbRMax,
			    cbLow->cbColorCount, cbHigh->cbColorCount,
			    cbParent->cbColorCount,
			    cbLow->cbPixelCount, cbHigh->cbPixelCount,
			    cbParent->cbPixelCount );
#	endif
	}
    else{
	if  ( 150* gDif > 29* bDif )
	    {
	    cbLow->cbGMax= bmHistGreenMedian( &(cbLow->cbPixelCount),
				    &(cbLow->cbColorCount),
				    cbParent->cbPixelCount, histogram,
				    cbParent->cbColorCount,
				    cbParent->cbGMin, cbParent->cbGMax );
	    cbHigh->cbGMin= cbLow->cbGMax+ 1;
	    cbHigh->cbColorCount= cbParent->cbColorCount- cbLow->cbColorCount;
	    cbHigh->cbPixelCount= cbParent->cbPixelCount- cbLow->cbPixelCount;
	    cbHigh->cbHistogramOffset= cbParent->cbHistogramOffset+
							cbLow->cbColorCount;
	    *pLeftMaximum= cbLow->cbGMax;
	    *pComponent= CN_GREEN;

#	    if  LOG_CUTS
	    appDebug( "CUT GREEN at %3d..%3d..%3d "
			    "%4d+ %4d= %4dC %6d+ %6d= %6dP\n",
			    cbParent->cbGMin, cbLow->cbGMax, cbParent->cbGMax,
			    cbLow->cbColorCount, cbHigh->cbColorCount,
			    cbParent->cbColorCount,
			    cbLow->cbPixelCount, cbHigh->cbPixelCount,
			    cbParent->cbPixelCount );
#	    endif
	    }
	else{
	    cbLow->cbBMax= bmHistBlueMedian( &(cbLow->cbPixelCount),
				    &(cbLow->cbColorCount),
				    cbParent->cbPixelCount, histogram,
				    cbParent->cbColorCount,
				    cbParent->cbBMin, cbParent->cbBMax );

	    cbHigh->cbBMin= cbLow->cbBMax+ 1;
	    cbHigh->cbColorCount= cbParent->cbColorCount- cbLow->cbColorCount;
	    cbHigh->cbPixelCount= cbParent->cbPixelCount- cbLow->cbPixelCount;
	    cbHigh->cbHistogramOffset= cbParent->cbHistogramOffset+
							cbLow->cbColorCount;
	    *pLeftMaximum= cbLow->cbBMax;
	    *pComponent= CN_BLUE;

#	    if  LOG_CUTS
	    appDebug( "CUT BLUE  at %3d..%3d..%3d "
			    "%4d+ %4d= %4dC %6d+ %6d= %6dP\n",
			    cbParent->cbBMin, cbLow->cbBMax, cbParent->cbBMax,
			    cbLow->cbColorCount, cbHigh->cbColorCount,
			    cbParent->cbColorCount,
			    cbLow->cbPixelCount, cbHigh->cbPixelCount,
			    cbParent->cbPixelCount );
#	    endif
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find a collection of colors using 'median cut'.			*/
/*									*/
/************************************************************************/

static int bmFindCuts(		ColorBox *		boxes,
				CutNode *		nodes,
				HistogramEntry *	histo,
				int			colorCount,
				int			pixelCount,
				int			maxcolors )
    {
    int				i;
    int				boxCount;
    int				nodeCount= 0;

    boxes[0].cbHistogramOffset= 0;
    boxes[0].cbColorCount= colorCount;
    boxes[0].cbPixelCount= pixelCount;
    boxes[0].cbCutNumber= 0;
    boxCount= 1;

    nodes[0].cnComponent= CN_LEAF;
    nodes[0].cnLeftMaximum= 0;		/*  Irrelevant !	*/
    nodes[0].cnLeft= 0;		/*  Both to this color.	*/
    nodes[0].cnRight= 0;		/*  Both to this color.	*/
    nodeCount= 1;

    while( boxCount < maxcolors )
	{
	int		biggestBox;
	int		leftMaximum;
	int		cutComponent;

	ColorBox	cbSplit;

	for ( i= 0; i < boxCount; i++ )
	    {
	    if  ( boxes[i].cbColorCount > 1 )
		{ break;	}
	    }
	if  ( i >= boxCount )
	    { /* LLDEB(i,boxCount); */ break;	}
	biggestBox= i;
	for ( i= biggestBox+ 1; i < boxCount; i++ )
	    {
	    if  ( boxes[i].cbColorCount > 1				&&
		  boxes[i].cbPixelCount > boxes[biggestBox].cbPixelCount )
		{ biggestBox= i;	}
	    }

	bmDelimitColorBox( boxes+ biggestBox, histo );

	cbSplit= boxes[biggestBox];
	bmSplitBox( histo,
				&cbSplit, boxes+ biggestBox, boxes+ boxCount,
				&leftMaximum, &cutComponent );

	/****************/
	/*  Cut Nodes	*/
	/****************/
	i= cbSplit.cbCutNumber;
	nodes[i].cnComponent= cutComponent;
	nodes[i].cnLeftMaximum= leftMaximum;
	nodes[i].cnLeft= nodeCount;
	nodes[i].cnRight= nodeCount+ 1;

	nodes[nodeCount].cnComponent= CN_LEAF;
	nodes[nodeCount].cnLeftMaximum= 0;
	nodes[nodeCount].cnLeft= biggestBox;
	nodes[nodeCount].cnRight= biggestBox;

	nodes[nodeCount+1].cnComponent= CN_LEAF;
	nodes[nodeCount+1].cnLeftMaximum= 0;
	nodes[nodeCount+1].cnLeft= boxCount;
	nodes[nodeCount+1].cnRight= boxCount;

	/****************/
	/*  Boxes	*/
	/****************/
	boxes[biggestBox].cbCutNumber= nodeCount;
	boxes[boxCount].cbCutNumber= nodeCount+ 1;

	nodeCount += 2; boxCount++;
	}

    return boxCount;
    }

static int bmFindColors(	ColorReducer *		cr,
				int			colorCount,
				int			pixelCount,
				int			maxcolors )
    {
    ColorBox *			boxes;
    const ColorBox *		cb;
    int				i;
    int				j;
    int				boxCount;

    boxes= (ColorBox *)malloc( maxcolors* sizeof(ColorBox) );
    if  ( ! boxes )
	{ LLDEB(maxcolors,boxes); return -1; }

    boxCount= bmFindCuts( boxes, cr->crCutNodes, cr->crHistogram,
					colorCount, pixelCount, maxcolors );

    cb= boxes;
    for ( i= 0; i < boxCount; cb++, i++ )
	{
	long			sR= 0;
	long			sG= 0;
	long			sB= 0;
	long			N= 0;

	HistogramEntry *	he= cr->crHistogram+ cb->cbHistogramOffset;

	for ( j= 0; j < boxes[i].cbColorCount; j++, he++ )
	    {
	    sR += he->heCount* he->heRed;
	    sG += he->heCount* he->heGreen;
	    sB += he->heCount* he->heBlue;
	    N  += he->heCount;
	    }

	cr->crPalette[i].rgb8Red= sR/ N;
	cr->crPalette[i].rgb8Green= sG/ N;
	cr->crPalette[i].rgb8Blue= sB/ N;

#	if LOG_PALETTE
	appDebug( "BOX %3d: "
			"R %3d..%3d..%3d "
			"G %3d..%3d..%3d "
			"B %3d..%3d..%3d "
			"%4dC %6ldP\n",

			i,
			cb->cbRMin, cr->crPalette[i].rgb8Red, cb->cbRMax,
			cb->cbGMin, cr->crPalette[i].rgb8Green, cb->cbGMax,
			cb->cbBMin, cr->crPalette[i].rgb8Blue, cb->cbBMax,
			boxes[i].cbColorCount, N );
#	endif
	}

    free( boxes );

    return boxCount;
    }

/************************************************************************/
/*									*/
/*  Various translation routines.					*/
/*									*/
/************************************************************************/

static int bmReduceAllocateColorCut(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ColorReducer *	cr= (ColorReducer *)ca->caSystemPrivate;
    const CutNode *	cn= cr->crCutNodes;
    int			i;

    i= 0;
    for (;;)
	{
	switch( cn[i].cnComponent )
	    {
	    case CN_RED:
		if  ( r <= cn[i].cnLeftMaximum )
		    { i= cn[i].cnLeft;	}
		else{ i= cn[i].cnRight;	}
		continue;

	    case CN_GREEN:
		if  ( g <= cn[i].cnLeftMaximum )
		    { i= cn[i].cnLeft;	}
		else{ i= cn[i].cnRight;	}
		continue;

	    case CN_BLUE:
		if  ( b <= cn[i].cnLeftMaximum )
		    { i= cn[i].cnLeft;	}
		else{ i= cn[i].cnRight;	}
		continue;

	    case CN_LEAF:
		i= cn[i].cnLeft;

		ac->acRed= 257* cr->crPalette[i].rgb8Red;
		ac->acGreen= 257* cr->crPalette[i].rgb8Green;
		ac->acBlue= 257* cr->crPalette[i].rgb8Blue;
		ac->acColorNumber= i;
		return 0;

	    default:
		LDEB(cn[i].cnComponent); return -1;
	    }
	}
    }

static int bmReduceAllocateColorHash(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ColorReducer *	cr= (ColorReducer *)ca->caSystemPrivate;
    int			hash;
    HashBucket *	hashBucket;
    int			i;

    hash= ppm_hash( r, g, b );
    hashBucket= cr->crHashTable[hash];
    while( hashBucket )
	{
	if  ( hashBucket->hbRed == r	&&
	      hashBucket->hbGreen == g	&&
	      hashBucket->hbBlue == b	)
	    { break;	}

	hashBucket= hashBucket->hbNext;
	}

    if  ( ! hashBucket )
	{ XDEB(hashBucket); i= 0;	}
    else{ i= hashBucket->hbNumber;	}

    ac->acRed= 257* cr->crPalette[i].rgb8Red;
    ac->acGreen= 257* cr->crPalette[i].rgb8Green;
    ac->acBlue= 257* cr->crPalette[i].rgb8Blue;
    ac->acColorNumber= i;

    return 0;
    }

static void bmReduceCleanupAllocator(	ColorAllocator *	ca )
    {
    ColorReducer *	cr= (ColorReducer *)ca->caSystemPrivate;

    if  ( cr )
	{
	bmCleanColorReducer( cr );
	free( cr );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Hash the colors in a 24 bit image. Do not return more than a	*/
/*  given number of colors.						*/
/*									*/
/*  1)  Make a hash table for the colors.				*/
/*									*/
/************************************************************************/

static int bmHashColors24(	HashBucket ***		pHashTable,
				int *			pColorCount,
				int			pixelsHigh,
				int			pixelsWide,
				int			bytesPerRow,
				const unsigned char *	bufIn,
				int			maxcolors,
				unsigned int		mask		)
    {
    unsigned int		r;
    unsigned int		g;
    unsigned int		b;

    int				row;
    int				col;

    HashBucket **		hashTable;
    HashBucket *		hashBucket;
    int				hash;

    int				colorCount= 0;

    const unsigned char *	from;

    /*  1  */
    hashTable= (HashBucket **)malloc( HASH_SIZE* sizeof(HashBucket *) );
    if  ( ! hashTable )
	{ XDEB(hashTable); return -1;	}
    for ( row= 0; row < HASH_SIZE; row++ )
	{ hashTable[row]= (HashBucket *)0;	}

    for ( row= 0; row < pixelsHigh; row++ )
	{
	from= bufIn+ row* bytesPerRow;

	for ( col= 0; col < pixelsWide; col++ )
	    {
	    r= *(from++) & mask; g= *(from++) & mask; b= *(from++) & mask;
	    hash= ppm_hash( r, g, b );

	    if  ( hash < 0 || hash >= HASH_SIZE )
		{ LLDEB(hash,HASH_SIZE); return -1; }

	    hashBucket= hashTable[hash];
	    while( hashBucket )
		{
		if  ( hashBucket->hbRed == r	&&
		      hashBucket->hbGreen == g	&&
		      hashBucket->hbBlue == b	)
		    { break;	}
		hashBucket= hashBucket->hbNext;
		}
	    if  ( hashBucket )
		{ hashBucket->hbCount++; continue;	}

	    if  ( bmInsertHash( hashTable, hash, colorCount++, r, g, b ) )
		{ LDEB(1); bmFreeColorHash( hashTable ); return -1; }

	    if  ( colorCount > maxcolors )
		{ bmFreeColorHash( hashTable ); return 1; }
	    }
	}

    *pColorCount= colorCount;
    *pHashTable= hashTable;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert a color hash table to a histogram.				*/
/*									*/
/************************************************************************/

static int bmHashToHistogram(	ColorReducer *		cr,
				int			colorCount )
    {
    HashBucket *		hashBucket;

    int				row;
    int				i;

    row= 0;
    for ( i= 0; i < HASH_SIZE; i++ )
	{
	hashBucket= cr->crHashTable[i];

	while( hashBucket )
	    {
	    cr->crHistogram[row++]= hashBucket->hbHistogramEntry;
	    hashBucket= hashBucket->hbNext;
	    }
	}

    return 0;
    }

static void bmHashToPalette(	ColorReducer *		cr )
    {
    HashBucket *		hashBucket;

    int				i;

    for ( i= 0; i < HASH_SIZE; i++ )
	{
	hashBucket= cr->crHashTable[i];

	while( hashBucket )
	    {
	    cr->crPalette[hashBucket->hbNumber]=
					hashBucket->hbHistogramEntry.heColor;

	    hashBucket= hashBucket->hbNext;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Reduce the number of colors in an image to at most 'maxcolors'.	*/
/*									*/
/*  0)  Allocate the palette.						*/
/*  1)  Make a hash table for the colors.				*/
/*  2)  Store and count all the colors in the hash table.		*/
/*  3)  If the number of colors is below the maximum, fill the palette	*/
/*	with the colors in the hash table.				*/
/*  4)  Else translate the hash table to a histogram.			*/

/*  4)  Allocate memory for the palette and for the tree that will be	*/
/*	used to classify the colors.					*/
/*  5)  Classify colors: Yields a palette and a tree.			*/
/*  6)  Allocate and initialise output.					*/
/*  7)  Translate to representative of box.				*/
/*									*/
/************************************************************************/

int bmColorReduce(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				maxcolors )
    {
    int				rval= 0;

    int				row;
    int				i;

    unsigned char		mask;

    ColorAllocator		ca;
    ColorReducer *		cr;

    int				colorCount= 0;
    int				colorsFound;

    int				bitsPerPixel;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 0;

    bmInitColorAllocator( &ca );

    cr= malloc( sizeof( ColorReducer ) );
    if  ( ! cr )
	{ XDEB(cr); rval= -1; goto ready;	}

    bmInitColorReducer( cr );

    ca.caSystemPrivate= (void *)cr;
    ca.caAllocationType= CA_ALLOCATOR;
    ca.caSystemCleanup= bmReduceCleanupAllocator;

    if  ( maxcolors > 256 )
	{ LDEB(maxcolors); rval= -1; goto ready;	}

    if  ( maxcolors <= 16 )
	{ bitsPerPixel= 4;	}
    else{ bitsPerPixel= 8;	}

    /*  0  */
    if  ( bmAllocateReducerPalette( cr, maxcolors ) )
	{ LDEB(maxcolors); rval= -1; goto ready;	}

    /*  1, 2  */
    switch( bdIn->bdBitsPerPixel )
	{
	case 24:
	    for ( i= 0; i < 8; i++ )
		{
		mask= ~( 0xff >> ( 8- i ) );

		row= bmHashColors24( &(cr->crHashTable), &colorCount,
				bdIn->bdPixelsHigh, bdIn->bdPixelsWide,
				bdIn->bdBytesPerRow, bufferIn, 32768, mask );
		if  ( row < 0 )
		    { LDEB(row); rval= -1; goto ready;	}
		if  ( row == 0 )
		    { break;				}
		}
	    break;

	default:
	    LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;
	}

    /*  3  */
    if  ( mask == 0xff && colorCount <= maxcolors )
	{
	bmHashToPalette( cr );

	colorsFound= colorCount;
	}
    else{
	/*  4  */
	if  ( bmAllocateHistogram( cr, colorCount ) )
	    { LDEB(colorCount); rval= -1; goto ready;	}

	if  ( bmHashToHistogram( cr, colorCount ) )
	    { LDEB(colorCount); rval= -1; goto ready;	}

	if  ( bmAllocateCutNodes( cr, maxcolors ) )
	    { LDEB(maxcolors); rval= -1; goto ready;	}

	/*  5  */
	colorsFound= bmFindColors( cr, colorCount,
			bdIn->bdPixelsHigh* bdIn->bdPixelsWide, maxcolors );

	if  ( colorsFound < 1 )
	    { LLDEB(maxcolors,colorsFound); rval= -1; goto ready; }
	}

    /*  6  */
    bdOut->bdPixelsWide= bdIn->bdPixelsWide;
    bdOut->bdPixelsHigh= bdIn->bdPixelsHigh;

    bdOut->bdBitsPerSample= bdIn->bdBitsPerSample;
    bdOut->bdSamplesPerPixel= 3;
    bdOut->bdBitsPerPixel= bitsPerPixel;
    bdOut->bdBytesPerRow= ( bdOut->bdBitsPerPixel* bdOut->bdPixelsWide+ 7 )/8;
    bdOut->bdBufferLength= bdOut->bdPixelsHigh* bdOut->bdBytesPerRow;

    bdOut->bdXResolution= bdIn->bdXResolution;
    bdOut->bdYResolution= bdIn->bdYResolution;
    bdOut->bdUnit= bdIn->bdUnit;

    bdOut->bdColorEncoding= BMcoRGB8PALETTE;
    bdOut->bdColorCount= colorsFound;

    bdOut->bdHasAlpha= 0;

    *pBufOut= malloc( bdOut->bdBufferLength );
    if  ( ! *pBufOut )
	{ LLDEB( bdOut->bdBufferLength,*pBufOut); rval= -1; goto ready; }

    /*  7  */
    if  ( cr->crCutNodes )
	{ ca.caSystemAllocator= bmReduceAllocateColorCut;	}
    else{ ca.caSystemAllocator= bmReduceAllocateColorHash;	}

    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
				dither, *pBufOut, bufferIn, bdOut, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bdOut->bdRGB8Palette= cr->crPalette; /*  steal */
    cr->crPalette= (RGB8Color *)0;

  ready:

    bmCleanColorAllocator( &ca ); /* also frees cr */

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a color allocator to be used with a certain palette.		*/
/*									*/
/************************************************************************/

int bmSetColorAllocatorForPaletteImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd )
    {
    int				rval= 0;

    int				i;
    ColorReducer *		cr= (ColorReducer *)0;
    ColorBox *			boxes= (ColorBox *)0;
    int *			map= (int *)0;
    int				boxCount;

    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
	{ LLDEB(bd->bdColorEncoding,BMcoRGB8PALETTE); rval= -1; goto ready; }

    cr= malloc( sizeof( ColorReducer ) );
    if  ( ! cr )
	{ XDEB(cr); rval= -1; goto ready;	}

    bmInitColorReducer( cr );

    ca->caSystemPrivate= (void *)cr;
    ca->caAllocationType= CA_ALLOCATOR;
    ca->caSystemCleanup= bmReduceCleanupAllocator;

    if  ( bmAllocateReducerPalette( cr, bd->bdColorCount ) )
	{ LDEB(bd->bdColorCount); rval= -1; goto ready;	}

    if  ( bmAllocateCutNodes( cr, bd->bdColorCount ) )
	{ LDEB(bd->bdColorCount); rval= -1; goto ready;	}

    if  ( bmAllocateHistogram( cr, bd->bdColorCount ) )
	{ LDEB(bd->bdColorCount); rval= -1; goto ready;	}

    boxes= (ColorBox *)malloc( bd->bdColorCount* sizeof(ColorBox) );
    if  ( ! boxes )
	{ LLDEB(bd->bdColorCount,boxes); return -1; }

    map= (int *)malloc( bd->bdColorCount* sizeof(int) );
    if  ( ! map )
	{ LLDEB(bd->bdColorCount,map); return -1; }

    for ( i= 0; i < bd->bdColorCount; i++ )
	{
	cr->crPalette[i]= bd->bdRGB8Palette[i];

	cr->crHistogram[i].heColor= bd->bdRGB8Palette[i];
	cr->crHistogram[i].heCount= 1;

	map[i]= -1;
	}

    boxCount= bmFindCuts( boxes, cr->crCutNodes, cr->crHistogram,
							    bd->bdColorCount,
							    bd->bdColorCount,
							    bd->bdColorCount );

    if  ( boxCount != bd->bdColorCount )
	{ LLDEB(boxCount,bd->bdColorCount); rval= -1; goto ready;	}

    ca->caSystemAllocator= bmReduceAllocateColorCut;

    for ( i= 0; i < bd->bdColorCount; i++ )
	{
	AllocatorColor	ac;

	bmReduceAllocateColorCut( &ac, ca,
				    bd->bdRGB8Palette[i].rgb8Red,
				    bd->bdRGB8Palette[i].rgb8Green,
				    bd->bdRGB8Palette[i].rgb8Blue );

	if  ( ac.acColorNumber < 0 || ac.acColorNumber >= bd->bdColorCount )
	    { LLDEB(i,ac.acColorNumber);	}

	map[ac.acColorNumber]= i;
	}

    for ( i= 0; i < 2* bd->bdColorCount; i++ )
	{
	if  ( cr->crCutNodes[i].cnComponent != CN_LEAF )
	    { continue;	}

	cr->crCutNodes[i].cnLeft=map[cr->crCutNodes[i].cnLeft];
	}

#   if 0
    for ( i= 0; i < bd->bdColorCount; i++ )
	{
	AllocatorColor	ac;

	bmReduceAllocateColorCut( &ac, ca,
				    bd->bdRGB8Palette[i].rgb8Red,
				    bd->bdRGB8Palette[i].rgb8Green,
				    bd->bdRGB8Palette[i].rgb8Blue );

	if  ( ac.acColorNumber != i )
	    {
	    LLDEB(i,ac.acColorNumber);
	    LDEB(bd->bdRGB8Palette[i].rgb8Red);
	    LDEB(bd->bdRGB8Palette[i].rgb8Green);
	    LDEB(bd->bdRGB8Palette[i].rgb8Blue);
	    }
	}
#   endif

  ready:

    if  ( boxes )
	{ free( boxes );	}
    if  ( map )
	{ free( map );	}

    return rval;
    }

