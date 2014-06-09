#   ifndef	UTIL_AFFINE_TRANSFORM_H
#   define	UTIL_AFFINE_TRANSFORM_H

/************************************************************************/
/*									*/
/*  Affine transformation 2 dimensional.				*/
/*									*/
/************************************************************************/

typedef struct AffineTransform2D
    {
    double		at2Axx;
    double		at2Axy;
    double		at2Ayx;
    double		at2Ayy;
    double		at2Tx;
    double		at2Ty;
    } AffineTransform2D;

#   define	AT2_X( x, y, at ) \
			( (at)->at2Axx* (x)+ (at)->at2Ayx* (y)+ (at)->at2Tx )
#   define	AT2_Y( x, y, at ) \
			( (at)->at2Axy* (x)+ (at)->at2Ayy* (y)+ (at)->at2Ty )

/************************************************************************/
/*									*/
/*  Affine transformation 3 dimensional.				*/
/*									*/
/************************************************************************/

typedef struct AffineTransform3D
    {
    double	at3Axx;
    double	at3Axy;
    double	at3Axz;

    double	at3Ayx;
    double	at3Ayy;
    double	at3Ayz;

    double	at3Azx;
    double	at3Azy;
    double	at3Azz;

    double	at3Tx;
    double	at3Ty;
    double	at3Tz;
    } AffineTransform3D;

#   define AT3_X( x, y, z, at ) \
	( (at)->at3Axx* (x)+ (at)->at3Ayx* (y)+ (at)->at3Azx* (z)+ (at)->at3Tx )
#   define AT3_Y( x, y, z, at ) \
	( (at)->at3Axy* (x)+ (at)->at3Ayy* (y)+ (at)->at3Azy* (z)+ (at)->at3Ty )
#   define AT3_Z( x, y, z, at ) \
	( (at)->at3Axz* (x)+ (at)->at3Ayz* (y)+ (at)->at3Azz* (z)+ (at)->at3Tz )

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int utilAffineTransformForTriangles(	AffineTransform2D *	atRes,
						double			x_1,
						double			x_2,
						double			y_1,
						double			y_2,
						double			z_1,
						double			z_2,
						double			p_1,
						double			p_2,
						double			q_1,
						double			q_2,
						double			r_1,
						double			r_2 );

extern void utilInitAffineTransform2D(		AffineTransform2D *	at2 );
extern void utilIdentityAffineTransform2D(	AffineTransform2D *	at2 );
extern void utilRotationAffineTransform2D(	AffineTransform2D *	at2,
						double			a );
extern void utilRotationAffineTransform2DAtan(	AffineTransform2D *	at2,
						double			y,
						double			x );
extern void utilTranslationAffineTransform2D(	AffineTransform2D *	at2,
						double			x,
						double			y );
extern void utilScaleAffineTransform2D(		AffineTransform2D *	at2,
						double			xs,
						double			ys );

extern void utilAffineTransform2DProduct(
					AffineTransform2D *		ba,
					const AffineTransform2D *	b,
					const AffineTransform2D *	a );
extern int utilInvertAffineTransform2D(	AffineTransform2D *		atR,
					const AffineTransform2D *	atF );

extern double utilAffineTransformDeterminant2D(
					const AffineTransform2D *	at2 );

extern void utilInitAffineTransform3D(		AffineTransform3D *	at3 );
extern void utilIdentityAffineTransform3D(	AffineTransform3D *	at3 );
extern void utilXYRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );
extern void utilXZRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );
extern void utilYZRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );

extern void utilAffineTransform3DProduct(
					AffineTransform3D *		ba,
					const AffineTransform3D *	b,
					const AffineTransform3D *	a );

extern double utilAffineTransformDeterminant3D(
					const AffineTransform3D *	at3 );

#   endif	/*  UTIL_AFFINE_TRANSFORM_H	*/
