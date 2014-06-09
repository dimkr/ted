/************************************************************************/
/*									*/
/*  Color allocation, mainly for bitmap images.				*/
/*									*/
/************************************************************************/

#   ifndef	BMCOLOR_H
#   define	BMCOLOR_H

/************************************************************************/
/*									*/
/*  An RGB8 Color.							*/
/*  The most usual way to display images on a computer screen.		*/
/*									*/
/*  An RGB16 Color.							*/
/*  Used to match X11's way of handling colors.				*/
/*									*/
/*  NOTE that the structs use native types so the 8/16 indication is	*/
/*	more a suggestion than an implementation reality.		*/
/*									*/
/************************************************************************/

typedef struct	RGB8Color
    {
    unsigned char	rgb8Red;
    unsigned char	rgb8Green;
    unsigned char	rgb8Blue;
    unsigned char	rgb8Alpha;
    } RGB8Color;

typedef struct	RGB16Color
    {
    unsigned short	rgb16Red;
    unsigned short	rgb16Green;
    unsigned short	rgb16Blue;
    unsigned char	rgb16Alpha;
    } RGB16Color;

/************************************************************************/
/*									*/
/*  For color approximation, anti aliasing and dithering		*/
/*									*/
/************************************************************************/

typedef struct AllocatorColor
    {
    RGB16Color		acColorValues;
    unsigned long	acColorNumber;
    unsigned char	acAllocated;
#			define		AC_UNALLOCATED		0
#			define		AC_ALLOCATED		1
#			define		AC_CALCULATED		2
#			define		AC_COPIED		3
    } AllocatorColor;

#   define acRed	acColorValues.rgb16Red
#   define acGreen	acColorValues.rgb16Green
#   define acBlue	acColorValues.rgb16Blue

struct ColorAllocator;

typedef int (*SystemAllocator)(		AllocatorColor *	ac,
					struct ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b );

typedef void (*SystemCleanup)(		struct ColorAllocator *	ca );

typedef struct ColorAllocator
    {
    AllocatorColor	ca222Colors[64];
    AllocatorColor *	caColors;
    int			caColorCount;
    int			caDepth;
    unsigned int	caRedApproxShift;
    unsigned int	caGreenApproxShift;
    unsigned int	caBlueApproxShift;
    unsigned int	caRedPixelShift;
    unsigned int	caGreenPixelShift;
    unsigned int	caBluePixelShift;
    unsigned int	caRedMask;
    unsigned int	caGreenMask;
    unsigned int	caBlueMask;

    AllocatorColor	caColorLastAllocated;

    int			caAllocationType;
#			define		CA_UNKNOWN		0
#			define		CA_INDEX_ALLOC		1
#			define		CA_CALCULATED		2
#			define		CA_ALLOCATOR		3
    void *		caSystemPrivate;
    SystemAllocator	caSystemAllocator;
    SystemCleanup	caSystemCleanup;

    } ColorAllocator;

/************************************************************************/
/*									*/
/*  Macros for indexing colors in an array of colors, usually in an	*/
/*  ColorAllocator structure.						*/
/*									*/
/************************************************************************/

#   define	C222(r,g,b)	( ( ( (r) >> 2 ) & 0x30 ) | \
				  ( ( (g) >> 4 ) & 0x0c ) | \
				  ( ( (b) >> 6 ) & 0x03 ) )

#   define bmColorRgbDirect( ac, ca, r, g, b )			\
    {								\
    unsigned int	rr= (r) >> (ca)->caRedApproxShift;	\
    unsigned int	gg= (g) >> (ca)->caGreenApproxShift;	\
    unsigned int	bb= (b) >> (ca)->caBlueApproxShift;	\
    \
    (ac)->acRed=   ( 65535* rr )/ (ca)->caRedMask;		\
    (ac)->acGreen= ( 65535* gg )/ (ca)->caGreenMask;		\
    (ac)->acBlue=  ( 65535* bb )/ (ca)->caBlueMask;		\
    \
    (ac)->acColorNumber= ( rr << (ca)->caRedPixelShift ) +	\
		 ( gg << (ca)->caGreenPixelShift )	+	\
		 ( bb << (ca)->caBluePixelShift )	;	\
    }


/************************************************************************/
/*									*/
/*  Compare RGB8Colors.							*/
/*									*/
/************************************************************************/

# define bmRGB8ColorsDiffer( a, b ) ( \
			    (a)->rgb8Red != (b)->rgb8Red	|| \
			    (a)->rgb8Green != (b)->rgb8Green	|| \
			    (a)->rgb8Blue != (b)->rgb8Blue	|| \
			    (a)->rgb8Alpha != (b)->rgb8Alpha	)

/************************************************************************/
/*									*/
/*  Routine declarations:						*/
/*									*/
/************************************************************************/

extern void bmSetCalculatedShifts(	ColorAllocator *	ca,
					unsigned long		redMask,
					unsigned long		greenMask,
					unsigned long		blueMask );

extern void bmInitColorAllocator(	ColorAllocator *	ca );
extern void bmCleanColorAllocator(	ColorAllocator *	ca );

extern int bmFindNearestColorRgb(	AllocatorColor *	acRet,
					const ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b );

extern int bmAllocateAllocatorColors(	ColorAllocator *	ca,
					int			count );

extern int bmInsertColor(		RGB8Color **		pColors,
					int *			pColorCount,
					int			avoidZero,
					int			maxSize,
					const RGB8Color *	rgb8 );

extern void bmInitRGB8Color(		RGB8Color *		rgb8 );

extern int bmRgbHue(		const RGB8Color *	rgb8 );
extern int bmRgbSaturation(	const RGB8Color *	rgb8 );
extern int bmRgbIntensity(	const RGB8Color *	rgb8 );

#   endif	/*  BMCOLOR_H  */
