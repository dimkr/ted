#   include	"bitmapConfig.h"

#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	"bmputrow.h"
#   include	<appDebugon.h>

# if 0

Perspective mapping of a pixmap image:
======================================

We have a pixmap with its origin in P.
The width in pixels of the pixmap is w, the height h.
Its u (column) direction is along a vector N.
Its v (row) direction is along a vector M.

The observer is at a (0,0,E_z).

The pixel at (col,row) physically is at point (u,v) in 2D space,
that in its turn is at point (x,y,z) in 3D space.

x= P_x+ ( col* N_x )/ w+ ( row* M_x )/ h;
y= P_y+ ( col* N_y )/ w+ ( row* M_y )/ h;
z= P_z+ ( col* N_z )/ w+ ( row* M_z )/ h;

Projected on the output image, this gives

X= x/ ( (z/-E_z)+ 1 )
Y= y/ ( (z/-E_z)+ 1 )

Now.. For quality texture mapping, we need to go in the opposite direction.

x=P_x+ N_x/w* col+ M_x/h* row;
y=P_y+ N_y/w* col+ M_y/h* row;
z/-E_z= P_z/-E_z+ N_z/(-E_z*w)* col+ M_z/(-E_z*h)* row;

Simplify constants:
N_x= N_x/w, N_y= N_y/w, M_x= M_x/h, M_y= M_y/h,
P_z= P_z/-E_z, N_z= N_z/(-E_z*w), M_z= M_z/(-E_z*h),

Substitute expressions for x,y,z in those for X,Y:
X= ( P_x+ N_x* col+ M_x* row )/ ( P_z+ N_z* col+ M_z* row+ 1 )
Y= ( P_y+ N_y* col+ M_y* row )/ ( P_z+ N_z* col+ M_z* row+ 1 )

Multiply by denominators:
P_z* X+ N_z* X* col+ M_z* X* row+ X= P_x+ N_x* col+ M_x* row;
P_z* Y+ N_z* Y* col+ M_z* Y* row+ Y= P_y+ N_y* col+ M_y* row;

Rearrange to look like simple linear equations:
( N_z* X- N_x )* col+ ( M_z* X- M_x )* row= P_x- P_z* X- X;
( N_z* Y- N_y )* col+ ( M_z* Y- M_y )* row= P_y- P_z* Y- Y;

Determinant:
det=
( N_z* X- N_x )* ( M_z* Y- M_y )- ( N_z* Y- N_y )* ( M_z* X- M_x )=

[ N_z* M_z* X* Y+	-N_z* X* M_y+	-N_x* M_z* Y+	N_x* M_y ]-
[ N_z* M_z* X* Y+	-N_z* Y* M_x+	-N_y* M_z* X+	N_y* M_x ]=

det=	[ N_y* M_z- N_z* M_y ]* X+
	[ N_z* M_x- N_x* M_z ]* Y+
	[ N_x* M_y- N_y* M_x ]

Numerators for the solution of the linear equations:

nmc=
( P_x- (P_z+1)* X )* ( M_z* Y- M_y )- ( P_y- (P_z+1)* Y )* ( M_z* X- M_x )=

[ P_x* M_z* Y+	-P_x* M_y+	-(P_z+1)* X* M_z* Y+	(P_z+1)* X* M_y	]-
[ P_y* M_z* X+	-P_y* M_x,	-(P_z+1)* Y* M_z* X+	(P_z+1)* Y* M_x ]=

nmc=	[ (P_z+1)* M_y - P_y* M_z ]* X+
	[ P_x* M_z - (P_z+1)* M_x ]* Y+
	[ P_y* M_x- P_x* M_y ].

nmr=
( P_y- (P_z+1)* Y )* ( N_z* X- N_x )- ( P_x- (P_z+1)* X )* ( N_z* Y- N_y )=

[ P_y* N_z* X+	-P_y* N_x+	-(P_z+1)* Y* N_z* X+	(P_z+1)* Y* N_x ]-
[ P_x* N_z* Y+	-P_x* N_y+	-(P_z+1)* X* N_z* Y+	(P_z+1)* X* N_y ]=

nmr=	[ P_y* N_z- (P_z+1)* N_y ]* X+
	[ (P_z+1)* N_x- P_x* N_z ]* Y+
	[ P_x* N_y- P_y* N_x ].

And finally: row= nmr/det, column= nmc/det.

# endif

# define SHOW_PROJECTION	1
# define SHOW_COLUMNS		1

# if SHOW_COLUMNS

static void bmShowSlope(	int				label,
				int				n,
				double				Y,
				int				rowOut,
				const BitmapDescription *	bdOut,
				const double *			A_x,
				const double *			A_y,
				double				dx,
				double				dy )
    {
    appDebug( "%c=%d: Y= %7.1f (%7.1f,%7.1f) -> (%7.1f,%7.1f)"
						" dx/dy= %7.1f/%-7.1f\n",
		    label, n, Y, A_x[n], A_y[n], A_x[n+1], A_y[n+1], dx, dy );

    appDebug(  "     row %6d [%7.1f,%7.1f] -> [%7.1f,%7.1f]\n",
					rowOut, A_x[n], A_y[n], A_x[n+1],
					bdOut->bdPixelsHigh- A_y[n+1] );
    }

static void bmShowArray(	int				label,
				const BitmapDescription *	bdOut,
				double *			A_x,
				double *			A_y )
    {
    int		i;

    for ( i= 0; i < 4; i++ )
	{
	appDebug( "%c[%d]= (%8.1f,%8.1f) @[%8.1f,%8.1f]\n",
		    label, i, A_x[i], A_y[i],
		    A_x[i], bdOut->bdPixelsHigh- A_y[i] );
	}

    }

# endif

/************************************************************************/
/*									*/
/*  Find the quandrangle in the output where pixels from the input will	*/
/*  be contained in.							*/
/*									*/
/************************************************************************/

static int bmMapSetBorders(	double			L_x[4],
				double			L_y[4],
				double			R_x[4],
				double			R_y[4],
				const double		Q_x[4],
				const double		Q_y[4],
				int			top,
				int			bot )
    {
    int		l;
    int		r;

    int		nx;
    int		pr;
    int		ll;
    int		rr;

    int		dir;

    for ( l= 0; l < 4; l++ )
	{
	L_x[l]= Q_x[bot];
	L_y[l]= Q_y[bot];
	R_x[l]= Q_x[bot];
	R_y[l]= Q_y[bot];
	}

    /**/
    l= r= 0;
    nx= pr= ll= rr= top;

    l++; ll= pr--; pr= ( pr+ 4 ) % 4;
    r++; rr= nx++; nx= ( nx    ) % 4;

    L_x[0]= R_x[0]= Q_x[top];
    L_y[0]= R_y[0]= Q_y[top];

    dir= 0;
    if  ( Q_x[pr] <= Q_x[ll] && Q_y[pr] <= Q_y[ll] )
	{ dir= -1;	}
    if  ( Q_x[nx] <= Q_x[rr] && Q_y[nx] <= Q_y[rr] )
	{ dir=  1;	}

    if  ( dir == 0 )
	{ LDEB(dir); return -1;	}

    if  ( dir < 0 )
	{
	while( Q_y[pr] <= Q_y[ll] )
	    {
	    L_x[l]= Q_x[pr];
	    L_y[l]= Q_y[pr];

	    l++; ll= pr--; pr= ( pr+ 4 ) % 4;
	    }

	while( Q_y[nx] <= Q_y[rr] )
	    {
	    R_x[r]= Q_x[nx];
	    R_y[r]= Q_y[nx];

	    r++; rr= nx++; nx= ( nx    ) % 4;
	    }
	}
    else{
	while( Q_y[nx] <= Q_y[ll] )
	    {
	    L_x[l]= Q_x[nx];
	    L_y[l]= Q_y[nx];

	    l++; ll= nx++; nx= ( nx    ) % 4;
	    }

	while( Q_y[pr] <= Q_y[rr] )
	    {
	    R_x[r]= Q_x[pr];
	    R_y[r]= Q_y[pr];

	    r++; rr= pr--; pr= ( pr+ 4 ) % 4;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Actually execute the texture mapping operation. (Or the inverse)	*/
/*									*/
/*  7)  For all output rows..						*/
/*  8)  Determine the left and right most intersections of the		*/
/*	projection of the input rectangle with the scan line.		*/
/*  9)  Collect input from the input image.				*/
/*  10) Emit is to the output image.					*/
/*									*/
/************************************************************************/

static int bmMapPixels(		unsigned char *			bufferOut,
				const BitmapDescription *	bdOut,
				const unsigned char *		bufferIn,
				const BitmapDescription *	bdIn,
				int				row0,
				int				rowP,
				int				top,
				const double			Q_y[4],
				const FillJob *			fj,
				PutScreenRow			putRow,
				GetSourceRow			getRow,
				const double			L_x[4],
				const double			L_y[4],
				const double			R_x[4],
				const double			R_y[4],
				double				nmr_x,
				double				nmr_y,
				double				nmr_c,
				double				nmc_x,
				double				nmc_y,
				double				nmc_c,
				double				det_x,
				double				det_y,
				double				det_c )
    {
    int			l;
    double		dxl;
    double		dyl;

    int			r;
    double		dxr;
    double		dyr;

    int			rowOut;

    double		det;
    double		nmr;
    double		nmc;

    /*  6  */
    l= 0;
    dxl= ( L_x[l+1]- L_x[l] );
    dyl= ( L_y[l+1]- L_y[l] );

    r= 0;
    dxr= ( R_x[r+1]- R_x[r] );
    dyr= ( R_y[r+1]- R_y[r] );

#   if SHOW_COLUMNS
    bmShowSlope( 'L', l, Q_y[top], row0, bdOut, L_x, L_y, dxl, dyl );
    bmShowSlope( 'R', r, Q_y[top], row0, bdOut, R_x, R_y, dxr, dyr );
#   endif

    for ( rowOut= row0; rowOut < rowP; rowOut++ )
	{
	unsigned char *	to= bufferOut+ rowOut* bdOut->bdBytesPerRow;

	double		X0;
	double		XP;
	double		Y;

	int		col0;
	int		colP;
	int		colOut;

	bmInitColorRow( fj->fjThisRow+ 1, bdOut->bdPixelsWide );

	Y= bdOut->bdPixelsHigh- rowOut;

	/*  7  */
	while( Y < L_y[l+1] )
	    {
	    l++;
	    dxl= ( L_x[l+1]- L_x[l] );
	    dyl= ( L_y[l+1]- L_y[l] );

#	    if SHOW_COLUMNS
	    bmShowSlope( 'L', l, Y, rowOut, bdOut, L_x, L_y, dxl, dyl );
#	    endif
	    }

	while( Y < R_y[r+1] )
	    {
	    r++;
	    dxr= ( R_x[r+1]- R_x[r] );
	    dyr= ( R_y[r+1]- R_y[r] );

#	    if SHOW_COLUMNS
	    bmShowSlope( 'R', r, Y, rowOut, bdOut, R_x, R_y, dxr, dyr );
#	    endif
	    }

	if  ( dyl == 0.0 )
	    {
	    if  ( L_x[l] <= L_x[l+1] )
		{ X0= L_x[l];	}
	    else{ X0= L_x[l+1];	}
	    }
	else{ X0= L_x[l]+ ( ( Y- L_y[l] )* dxl )/ dyl;	}

	if  ( dyr == 0.0 )
	    {
	    if  ( R_x[r] >= R_x[r+1] )
		{ XP= R_x[r];	}
	    else{ XP= R_x[r+1];	}
	    }
	else{ XP= R_x[r]+ ( ( Y- R_y[r] )* dxr )/ dyr;	}

	if  ( X0 < 0 )
	    { X0=  0;	}
	if  ( X0 > bdOut->bdPixelsWide )
	    { X0=  bdOut->bdPixelsWide;	}

	if  ( XP < 0 )
	    { XP=  0;	}
	if  ( XP > bdOut->bdPixelsWide )
	    { XP=  bdOut->bdPixelsWide;	}

	col0= floor( X0 );
	colP= floor( XP );

	det= X0* det_x+ Y* det_y+ det_c;
	nmr= X0* nmr_x+ Y* nmr_y+ nmr_c;
	nmc= X0* nmc_x+ Y* nmc_y+ nmc_c;

	for ( colOut= col0; colOut < colP; colOut++ )
	    {
	    int				rowIn;
	    int				colIn;

	    rowIn= bdIn->bdPixelsHigh- nmr/ det;
	    colIn= nmc/ det;

#	    if  SHOW_COLUMNS
	    if  ( rowIn <  0				||
		  rowIn >= bdIn->bdPixelsHigh		||
		  colIn <  0				||
		  colIn >= bdIn->bdPixelsWide		)
		{ LLLLLDEB(rowOut,col0,colP,rowIn,colIn);	}
#	    endif

	    /*  9  */
	    (*getRow)( fj->fjThisRow+ 1, colOut,
				bufferIn+ rowIn* bdIn->bdBytesPerRow,
				colIn, colIn+ 1, bdIn );

	    det += det_x;
	    nmr += nmr_x;
	    nmc += nmc_x;
	    }

	/*  10  */
	if  ( (*putRow)( to, fj, fj->fjThisRow+ 1 ) )
	    { LDEB(rowOut); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the source cow,column,divisor contributions by the target row	*/
/*  and column.								*/
/*									*/
/*  1)  Apply the simplification of the constants.			*/
/*  2)  Apply the correction for the x,y position of the eye=camera.	*/
/*									*/
/************************************************************************/

static void bmTextureMapBwdCoefficients(
				    double *			pNmr_x,
				    double *			pNmr_y,
				    double *			pNmr_c,
				    double *			pNmc_x,
				    double *			pNmc_y,
				    double *			pNmc_c,
				    double *			pDet_x,
				    double *			pDet_y,
				    double *			pDet_c,
				    const BitmapDescription *	bd,
				    double			P_x,
				    double			P_y,
				    double			P_z,
				    double			N_x,
				    double			N_y,
				    double			N_z,
				    double			M_x,
				    double			M_y,
				    double			M_z,
				    double			E_x,
				    double			E_y,
				    double			E_z )
    {
    /*  1  */
    double	w= bd->bdPixelsWide;
    double	h= bd->bdPixelsHigh;
    double	kh= -E_z* h;
    double	kw= -E_z* w;

    *pDet_x= ( N_y/ w  )* ( M_z/ kh )-		( N_z/ kw )* ( M_y/ h  );
    *pDet_y= ( N_z/ kw )* ( M_x/ h  )-		( N_x/ w  )* ( M_z/ kh );
    *pDet_c= ( N_x/ w  )* ( M_y/ h  )-		( N_y/ w  )* ( M_x/ h  );

    *pNmc_x= ( P_z/ -E_z+ 1 )* ( M_y/ h )-	( P_y- E_y )* ( M_z/ kh );
    *pNmc_y= ( P_x- E_x )* ( M_z/ kh )-		( P_z/ -E_z+ 1 )* ( M_x/ h  );
    *pNmc_c= ( P_y- E_y )* ( M_x/ h  )-		( P_x- E_x )* ( M_y/ h  );

    *pNmr_x= ( P_y- E_y )* ( N_z/ kw )-		( P_z/ -E_z+ 1 )* ( N_y/ w  );
    *pNmr_y= ( P_z/ -E_z+ 1 )* ( N_x/ w  )-	( P_x- E_x )* ( N_z/ kw );
    *pNmr_c= ( P_x- E_x )* ( N_y/ w  )-		( P_y- E_y )* ( N_x/ w  );

    /*  2  */
    *pDet_c -= E_y* *pDet_y;
    *pNmc_c -= E_y* *pNmc_y;
    *pNmr_c -= E_y* *pNmr_y;

    *pDet_c -= E_x* *pDet_x;
    *pNmc_c -= E_x* *pNmc_x;
    *pNmr_c -= E_x* *pNmr_x;

    return;
    }

/************************************************************************/
/*									*/
/*  Perform a texture mapping along a perspective projection.		*/
/*									*/
/*  Meaning of the parameters:						*/
/*									*/
/*  ca		ColorAllocator to fill the output image.		*/
/*  swap*	Provisions for weird bitmap formats. Just passed	*/
/*		through.						*/
/*  dither	Dither when colors are approximated?			*/
/*  bufferOut	Store image bytes here.					*/
/*  bufferIn	Source of input bytes.					*/
/*  bdOut	Describes the format of the output image.		*/
/*  bdIn	Describes the format of the input image.		*/
/*  E_x,E_y,E_z	The position of the 'eye' or lens opening of the	*/
/*		observer. Typically E_z is negative. Good values to	*/
/*		start with are:						*/
/*		E_x= 0.5						*/
/*		E_y= 0.5* bdOut->bdPixelsHigh/bdOut->bdPixelsWide	*/
/*		E_z= -1.0						*/
/*  P_x,P_y,P_z	The position of the lower left corner of the input	*/
/*		image in 3D space.					*/
/*  N_x,N_y,N_z	The direction in 3D space of the rows in the input	*/
/*		image. [And length]					*/
/*  M_x,M_y,M_z	The direction in 3D space of the columns in the input	*/
/*		image. [And length] As this relates to geometry,	*/
/*		the columns are oriented from bottop to top.		*/
/*									*/
/*  The observer is always looking in the direction of the Z axis.	*/
/*  The output image always is in the z=0 plane with the lower left	*/
/*  corner in x=0,y=0,z=0.						*/
/*  To get different projections, change the point of view in stead of	*/
/*  the position of the output image.					*/
/*									*/
/*  ALL double parameters use the width of the output image as their	*/
/*  unit.								*/
/*									*/
/*  Use the simple 'Perfect' mapping approach.				*/
/*									*/
/*  1)  Initialize data structures.					*/
/*  2)	The unit for the double parameters is the width of the output	*/
/*	image. Adapt to the actual size of the image.			*/
/*  4)	Project the corners of the input image onto the output image,	*/
/*	and determine the range of rows that can be reached by the	*/
/*	input image.							*/
/*  5)  Determine the orientation of the reachable part of the image.	*/
/*  6)  Calculate the factors of X and Y and the constants that		*/
/*	contribute to the determinant and the numerators. Note that in	*/
/*	the middle of the derivation, the constants were simplified.	*/
/*	The Fixup for E_x or E_y != 0 is not in the derivation.		*/
/*  6a) Compensate for the translation caused by E_x != 0 or E_y != 0.	*/
/*  7)  For all output rows..						*/
/*  8)  Determine the left and right most intersections of the		*/
/*	projection of the input rectangle with the scan line.		*/
/*  9)  Collect input from the input image.				*/
/*  10) Emit is to the output image.					*/
/*									*/
/************************************************************************/

int bmTextureMap(	ColorAllocator *		ca,
			int				swapBitmapUnit,
			int				swapBitmapBytes,
			int				swapBitmapBits,
			int				dither,
			unsigned char *			bufferOut,
			const unsigned char *		bufferIn,
			const BitmapDescription *	bdOut,
			const BitmapDescription *	bdIn,
			double				E_x,
			double				E_y,
			double				E_z,
			double				P_x,
			double				P_y,
			double				P_z,
			double				N_x,
			double				N_y,
			double				N_z,
			double				M_x,
			double				M_y,
			double				M_z )
    {
    int			rval= 0;

    double		nmc_x;
    double		nmc_y;
    double		nmc_c;

    double		nmr_x;
    double		nmr_y;
    double		nmr_c;

    double		det_x;
    double		det_y;
    double		det_c;

    double		Q_x[4];
    double		Q_y[4];
    double		L_x[4];
    double		L_y[4];
    double		R_x[4];
    double		R_y[4];

    int			row0;
    int			rowP;
    int			top;
    int			bot;

    int			i;

    PutScreenRow	putRow= (PutScreenRow)0;
    GetSourceRow	getRow= (GetSourceRow)0;
    int			scratchSize= 0;

    FillJob		fj;

    bmInitFillJob( &fj );

    /*  1  */
    if  ( bmGetPutRow( &putRow, &scratchSize, ca,
					    swapBitmapUnit, swapBitmapBytes,
					    swapBitmapBits, bdOut ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( bmGetGetRow( &getRow, bdIn ) )
	{ LDEB(1); return -1; }

    if  ( bmSetFillJob( &fj, ca,
			    bdOut->bdPixelsWide, bdOut->bdPixelsWide,
			    scratchSize, dither ) )
	{ LDEB(scratchSize); rval= -1; goto ready;	}

    /*  2  */
    E_x *= bdOut->bdPixelsWide;
    E_y *= bdOut->bdPixelsWide;
    E_z *= bdOut->bdPixelsWide;
    P_x *= bdOut->bdPixelsWide;
    P_y *= bdOut->bdPixelsWide;
    P_z *= bdOut->bdPixelsWide;
    N_x *= bdOut->bdPixelsWide;
    N_y *= bdOut->bdPixelsWide;
    N_z *= bdOut->bdPixelsWide;
    M_x *= bdOut->bdPixelsWide;
    M_y *= bdOut->bdPixelsWide;
    M_z *= bdOut->bdPixelsWide;

    /*
    FFFDEB(E_x,E_y,E_z);
    FFFDEB(P_x,P_y,P_z);
    FFFDEB(N_x,N_y,N_z);
    FFFDEB(M_x,M_y,M_z);
    */

    /*  4  */
    Q_x[0]= ( P_x- E_x )/ ( ( P_z )/ -E_z+ 1 )+ E_x;
    Q_y[0]= ( P_y- E_y )/ ( ( P_z )/ -E_z+ 1 )+ E_y;

    Q_x[1]= ( P_x- E_x+ N_x )/ ( ( P_z+ N_z )/ -E_z+ 1 )+ E_x;
    Q_y[1]= ( P_y- E_y+ N_y )/ ( ( P_z+ N_z )/ -E_z+ 1 )+ E_y;

    Q_x[2]= ( P_x- E_x+ N_x+ M_x )/ ( ( P_z+ N_z+ M_z )/ -E_z+ 1 )+ E_x;
    Q_y[2]= ( P_y- E_y+ N_y+ M_y )/ ( ( P_z+ N_z+ M_z )/ -E_z+ 1 )+ E_y;

    Q_x[3]= ( P_x- E_x+ M_x )/ ( ( P_z+ M_z )/ -E_z+ 1 )+ E_x;
    Q_y[3]= ( P_y- E_y+ M_y )/ ( ( P_z+ M_z )/ -E_z+ 1 )+ E_y;

#   if SHOW_PROJECTION
    appDebug(
	"FWD P    : (%8.1f,%8.1f,%8.1f) -> (%8.1f,%8.1f) @[%8.1f,%8.1f]\n",
		    P_x, P_y, P_z, Q_x[0], Q_y[0],
		    Q_x[0], bdOut->bdPixelsHigh- Q_y[0] );

    appDebug(
	"FWD P+N  : (%8.1f,%8.1f,%8.1f) -> (%8.1f,%8.1f) @[%8.1f,%8.1f]\n",
		    P_x+ N_x, P_y+ N_y, P_z+ N_z, Q_x[1], Q_y[1],
		    Q_x[1], bdOut->bdPixelsHigh- Q_y[1] );

    appDebug(
	"FWD P  +M: (%8.1f,%8.1f,%8.1f) -> (%8.1f,%8.1f) @[%8.1f,%8.1f]\n",
		    P_x+ M_x, P_y+ M_y, P_z+ M_z, Q_x[3], Q_y[3],
		    Q_x[3], bdOut->bdPixelsHigh- Q_y[3] );

    appDebug(
	"FWD P+N+M: (%8.1f,%8.1f,%8.1f) -> (%8.1f,%8.1f) @[%8.1f,%8.1f]\n",
		    P_x+ N_x+ M_x, P_y+ N_y+ M_y, P_z+ N_z+ M_z,
		    Q_x[2], Q_y[2],
		    Q_x[2], bdOut->bdPixelsHigh- Q_y[2] );
#   endif

    top= bot= 0;
    for ( i= 1; i < 4; i++ )
	{
	if  ( Q_y[i] < Q_y[bot] )
	    { bot= i; 		}
	if  ( Q_y[i] > Q_y[top] )
	    { top= i; 		}
	}

    row0= floor( bdOut->bdPixelsHigh- ( Q_y[top] ) );
    rowP= ceil ( bdOut->bdPixelsHigh- ( Q_y[bot] ) );

    if  ( row0 <  0 )
	{ row0 =  0;	}
    if  ( rowP <  0 )
	{ rowP =  0;	}

    if  ( row0 >= bdOut->bdPixelsHigh )
	{ row0 =  bdOut->bdPixelsHigh;	}
    if  ( rowP >= bdOut->bdPixelsHigh )
	{ rowP =  bdOut->bdPixelsHigh;	}

    /*  5  */
    if  ( bmMapSetBorders( L_x, L_y, R_x, R_y, Q_x, Q_y, top, bot ) )
	{ LLDEB(top,bot); rval= -1; goto ready;	}

#   if SHOW_COLUMNS
    {
    appDebug( "\n" );
    bmShowArray( 'Q', bdOut, Q_x, Q_y );
    appDebug( "\n" );
    bmShowArray( 'L', bdOut, L_x, L_y );
    appDebug( "\n" );
    bmShowArray( 'R', bdOut, R_x, R_y );
    }
#   endif

    /*  6  */
    bmTextureMapBwdCoefficients(	&nmr_x, &nmr_y, &nmr_c,
					&nmc_x, &nmc_y, &nmc_c,
					&det_x, &det_y, &det_c, bdIn,
					P_x, P_y, P_z,
					N_x, N_y, N_z,
					M_x, M_y, M_z,
					E_x, E_y, E_z );

    /*  7,8,9,10  */
    if  ( bmMapPixels( bufferOut, bdOut, bufferIn, bdIn,
	    row0, rowP, top, Q_y, &fj, putRow, getRow, L_x, L_y, R_x, R_y,
	    nmr_x, nmr_y, nmr_c, nmc_x, nmc_y, nmc_c, det_x, det_y, det_c ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    bmCleanFillJob( &fj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Inverse of bmTextureMap. All projection related parameters mean the	*/
/*  same thing as in bmTextureMap().					*/
/*									*/
/************************************************************************/

int bmTextureMapInverse( ColorAllocator *		ca,
			int				swapBitmapUnit,
			int				swapBitmapBytes,
			int				swapBitmapBits,
			int				dither,
			unsigned char *			bufferOut,
			const unsigned char *		bufferIn,
			const BitmapDescription *	bdOut,
			const BitmapDescription *	bdIn,
			double				E_x,
			double				E_y,
			double				E_z,
			double				P_x,
			double				P_y,
			double				P_z,
			double				N_x,
			double				N_y,
			double				N_z,
			double				M_x,
			double				M_y,
			double				M_z )
    {
    return 0;
    }

