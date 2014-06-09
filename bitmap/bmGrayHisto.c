#   include	"bitmapConfig.h"

#   include	<math.h>

#   include	<bmGrayHisto.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Various gray scale histogram base Thresholding algorithms.		*/
/*									*/
/*  Most of the approaches and some of the implementation is derived	*/
/*  from the following book:						*/
/*									*/
/*  PARKER, J.R.: Algorithms for Image Processing and Computer Vision,	*/
/*	Wiley Computer Publishing, New York, 1996, ISBN 0-471-14056-2	*/
/*									*/
/************************************************************************/

void bmInitThresholderHistogram(	ThresholderHistogram *	th )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{ th->thHistogram[i]= 0; }

    th->thHistogramSize= 0;
    th->thPixelCount= 0;
    th->thThreshold= -1;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Mean pixel value.			*/
/*									*/
/************************************************************************/

void bmThresholdMean(	ThresholderHistogram *		th )
    {
    int		i;

    double	sr;

    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{ sr += th->thHistogram[i]* i;	}

    th->thThreshold= sr/ th->thPixelCount;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using a fraction.				*/
/*									*/
/************************************************************************/

void bmThresholdQuantile(	ThresholderHistogram *		th,
				double				frac )
    {
    int		i;

    double	sr;

    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	if  ( ( sr + th->thHistogram[i] )/ th->thPixelCount > frac )
	    { break;	}

	sr += th->thHistogram[i];
	}

    th->thThreshold= i;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the two peak method.			*/
/*									*/
/*  1)  Determine the primary peak.					*/
/*  2)  Determine the second peak.					*/
/*  3)  Assume lowest value is background.				*/
/*  4)  Guess the threshold value is exactly between the peaks.		*/
/*  5)  But look for a valley between the two peaks.			*/
/*									*/
/************************************************************************/

void bmThreshold2Peaks(	ThresholderHistogram *		th )
    {
    int		i0;
    int		i1;

    int		p1= 0;
    int		p2= 1;

    long	v;
    long	v1= 0;
    long	v2= 0;

    int		i;
    int		p;

    /*  1  */
    i0= i1= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	v= th->thHistogram[i];

	if  ( v1 <  v )
	    { v1=   v; i0= i1= i; }
	if  ( v1 == v )
	    { i1= i; }
	}

    p1= ( i0+ i1 )/ 2;

    /*  2  */
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	v= ( i- p1 );
	v= v* v* th->thHistogram[i];

	if  ( v2 <  v )
	    { v2=   v; i0= i1= i; }
	if  ( v2 == v )
	    { i1= i; }
	}

    p2= ( i0+ i1 )/ 2;

    /*  3  */
    if  ( p1 > p2 )
	{ i= p1; p1= p2; p2= i; }

    /*  4  */
    th->thThreshold= ( p1+ p2 )/ 2;

    /*  5  */
    i0= i1= th->thThreshold;
    v1= th->thHistogram[th->thThreshold];

    i0= i1= p1;
    v1= th->thHistogram[p1];
    p= ( p1+ p2 )/ 2;

    for ( i= p1; i <= p2; i++ )
	{
	v= ( i- p1 )* ( p2- i );
	v= v* ( p- th->thHistogram[i] );

	if  ( v1 <  v )
	    { v1=   v; i0= i1= i; }
	if  ( v1 == v )
	    { i1= i; }
	}

    th->thThreshold= ( i0+ i1 )/ 2;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Ridler Iterative Selection	*/
/*  method.								*/
/*									*/
/*  1)  First guess: the average pixel value;				*/
/*  2)  Determine the mean gray level of pixels not over the threshold	*/
/*  3)  Determine the mean gray level of pixels over the threshold	*/
/*  4)  A new estimate of the threshold is the average of the means	*/
/*	calculated above.						*/
/*  5)  Iteration stops if the new threshold is equal to the previous	*/
/*	one.								*/
/*									*/
/************************************************************************/

void bmThresholdRidler(	ThresholderHistogram *		th )
    {
    int		i;

    double	sr;
    int		tr;

    /*  1  */
    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{ sr += th->thHistogram[i]* i;	}
    tr= sr/ th->thPixelCount;

    for (;;)
	{
	int	no= 0;
	int	nb= 0;
	double	so= 0;
	double	sb= 0;

	int	tn;

	/*  2  */
	for ( i= 0; i <= tr; i++ )
	    {
	    nb += th->thHistogram[i];
	    sb += th->thHistogram[i]* i;
	    }

	/*  3  */
	for ( i= tr+ 1; i < th->thHistogramSize; i++ )
	    {
	    no += th->thHistogram[i];
	    so += th->thHistogram[i]* i;
	    }

	if  ( no == 0 )
	    { no=   1;	}
	if  ( nb == 0 )
	    { nb=   1;	}

	/*  4  */
	tn= ( so/no+ sb/nb )/ 2.0;

	/*  5  */
	if  ( tn == tr )
	    {
	    th->thThreshold= tr;
	    return;
	    }
	
	tr= tn;
	}
    }

/************************************************************************/
/*									*/
/*  Determine the threshold minimizing the ration of between- class and	*/
/*  overall variance.							*/
/*									*/
/*  1)  Overall mean and variance.					*/
/*  2)  Below threshold.						*/
/*  3)  Above threshold.						*/
/*									*/
/************************************************************************/

void bmThresholdVariance(	ThresholderHistogram *		th )
    {
    int		i;

    double	sum_b;
    double	ssq_b;
    double	ub;
    double	vb;
    int		nb;

    double	sum_o;
    double	ssq_o;
    double	uo;
    double	vo;
    int		no;

    int		tr;
    double	vmin;

    int		tr0;
    int		tr1;

#   if 0

    double	sum_t;
    double	ssq_t;
    double	ut;
    double	vt;

    /*  1  */
    sum_t= 0; ssq_t= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	sum_t += (double)th->thHistogram[i]* i;
	ssq_t += (double)th->thHistogram[i]* i* i;
	}
    ut= sum_t/ th->thPixelCount;
    vt= ssq_t/ th->thPixelCount- ut* ut;

#   endif

    tr= 0;

    /*  2  */
    vb= 0; ub= 0;
    sum_b= 0; ssq_b= 0; nb= 0;
    for ( i= 0; i <= tr; i++ )
	{
	nb    += th->thHistogram[i];
	sum_b += (double)th->thHistogram[i]* i;
	ssq_b += (double)th->thHistogram[i]* i* i;
	}
    if  ( nb > 0 )
	{
	ub= sum_b/ nb;
	vb= ssq_b/ nb- ub* ub;
	}

    /*  3  */
    vo= 0; uo= 0;
    sum_o= 0; ssq_o= 0; no= 0;
    for ( i= tr+ 1; i < th->thHistogramSize; i++ )
	{
	no    += th->thHistogram[i];
	sum_o += (double)th->thHistogram[i]* i;
	ssq_o += (double)th->thHistogram[i]* i* i;
	}
    if  ( no > 0 )
	{
	uo= sum_o/ no;
	vo= ssq_o/ no- uo* uo;
	}

    vmin= vo+ vb;
    tr0= tr1= tr;

    for ( tr= 1; tr < th->thHistogramSize; tr++ )
	{
	/*  2  */
	vb= 0; ub= 0;
	nb    += th->thHistogram[tr];
	sum_b += (double)th->thHistogram[tr]* tr;
	ssq_b += (double)th->thHistogram[tr]* tr* tr;

	if  ( nb > 0 )
	    {
	    ub= sum_b/ nb;
	    vb= ssq_b/ nb- ub* ub;
	    }

	/*  3  */
	vo= 0; uo= 0;
	no    -= th->thHistogram[tr];
	sum_o -= (double)th->thHistogram[tr]* tr;
	ssq_o -= (double)th->thHistogram[tr]* tr* tr;

	if  ( no > 0 )
	    {
	    uo= sum_o/ no;
	    vo= ssq_o/ no- uo* uo;
	    }

	if  ( vb+ vo <  vmin )
	    { tr0= tr1= tr; vmin= vb+ vo;	}
	if  ( vb+ vo == vmin )
	    { tr1= tr;				}
	}

    th->thThreshold= ( tr0+ tr1 )/ 2;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold minimizing the error function.		*/
/*									*/
/*  1)  Overall mean and variance.					*/
/*  2)  Below threshold.						*/
/*  3)  Above threshold.						*/
/*									*/
/************************************************************************/

void bmThresholdMinimumError(	ThresholderHistogram *		th )
    {
    int		i;

    double	sum_b;
    double	ssq_b;
    double	ub;
    double	vb;
    double	sb;
    int		nb;

    double	sum_o;
    double	ssq_o;
    double	uo;
    double	vo;
    double	so;
    int		no;

    int		tr;
    double	j;
    double	jmin= 1e+20;

    int		tr0;
    int		tr1;

    int		s;
    int		p;

    s= 0; while( th->thHistogram[s] == 0 )
	{ s++; }
    p= th->thHistogramSize; while( th->thHistogram[p-1] == 0 )
	{ p--; }

#   if 0

    double	sum_t;
    double	ssq_t;
    double	ut;
    double	vt;

    /*  1  */
    sum_t= 0; ssq_t= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	sum_t += (double)th->thHistogram[i]* i;
	ssq_t += (double)th->thHistogram[i]* i* i;
	}
    ut= sum_t/ th->thPixelCount;
    vt= ssq_t/ th->thPixelCount- ut* ut;

#   endif

    tr= s+ 1;

    /*  2  */
    sb= 0.0; vb= 0; ub= 0;
    sum_b= 0; ssq_b= 0; nb= 0;
    for ( i= s; i <= tr; i++ )
	{
	nb    += th->thHistogram[i];
	sum_b += (double)th->thHistogram[i]* i;
	ssq_b += (double)th->thHistogram[i]* i* i;
	}

    /*  3  */
    so= 0.0; vo= 0; uo= 0;
    sum_o= 0; ssq_o= 0; no= 0;
    for ( i= tr+ 1; i < p; i++ )
	{
	no    += th->thHistogram[i];
	sum_o += (double)th->thHistogram[i]* i;
	ssq_o += (double)th->thHistogram[i]* i* i;
	}

    jmin= 1.0e+20;
    tr0= tr1= tr;

    for ( tr= tr+ 1; tr < p- 1; tr++ )
	{
	/*  2  */
	sb= 0.0; vb= 0; ub= 0;
	nb    += th->thHistogram[tr];
	sum_b += (double)th->thHistogram[tr]* tr;
	ssq_b += (double)th->thHistogram[tr]* tr* tr;

	if  ( nb > 0 )
	    {
	    ub= sum_b/ nb;
	    vb= ssq_b/ nb- ub* ub;
	    sb= sqrt( vb );
	    }

	/*  3  */
	so= 0.0; vo= 0; uo= 0;
	no    -= th->thHistogram[tr];
	sum_o -= (double)th->thHistogram[tr]* tr;
	ssq_o -= (double)th->thHistogram[tr]* tr* tr;

	if  ( no > 0 )
	    {
	    uo= sum_o/ no;
	    vo= ssq_o/ no- uo* uo;
	    so= sqrt( vo );
	    }

	if  ( no == 0 || nb == 0 || vb == 0 || vo == 0 )
	    { continue; }

	j= 1.0+
	    2* ( nb* log( sb     )+ no* log( so     ) )-
	    2* ( nb* log( 1.0*nb )+ no* log( 1.0*no ) );

	if  ( j <  jmin )
	    { tr0= tr1= tr; jmin= j;	}
	if  ( j == jmin )
	    { tr1= tr;				}
	}

    th->thThreshold= ( tr0+ tr1 )/ 2;

    return;
    }


/************************************************************************/
/*									*/
/*  Determine the threshold using the Kapur maximal entropy method.	*/
/*									*/
/************************************************************************/

void bmThresholdKapur(	ThresholderHistogram *		th )
    {
    int		i;

    double	p;
    double	Pt[256];

    double	Ht;

    int		tr;
    int		tr0;
    int		tr1;
    double	N= th->thPixelCount;

    double	Hmax;

    double	Hb;
    double	Ho;

    p= 0; Ht= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	p += (double)th->thHistogram[i];
	Pt[i]= p/ N;

#   	if 0
	if  ( th->thHistogram[i] > 0 && Pt[i] > 0 )
	    {
	    double	r= th->thHistogram[i]/ ( N* Pt[i] );

	    Ht -= r* log( r );
	    }
#	endif
	}

    tr= 0;

    Hb= 0;
    for ( i= 0; i <= tr; i++ )
	{
	if  ( th->thHistogram[i] > 0 && Pt[i] > 0 )
	    {
	    double	r= th->thHistogram[i]/ ( N* Pt[i] );

	    Hb -= r* log( r );
	    }
	}

    Ho= 0;
    for ( i= tr+ 1; i < th->thHistogramSize; i++ )
	{
	if  ( th->thHistogram[i] > 0 && Pt[i] < 1 )
	    {
	    double	r= th->thHistogram[i]/ ( N* ( 1- Pt[i] ) );

	    Ho -= r* log( r );
	    }
	}

    Hmax= Hb+ Ho;
    tr0= tr1= tr;

    for ( tr= 1; tr < th->thHistogramSize; tr++ )
	{
	if  ( th->thHistogram[tr] > 0 && Pt[tr] > 0 )
	    {
	    double	r= th->thHistogram[tr]/ ( N* Pt[tr] );

	    Hb -= r* log( r );
	    }

	if  ( th->thHistogram[tr] > 0 && Pt[tr] < 1 )
	    {
	    double	r= th->thHistogram[tr]/ ( N* ( 1- Pt[tr] ) );

	    Ho += r* log( r );
	    }

	if  ( Hb+ Ho >  Hmax )
	    { tr0= tr1= tr; Hmax= Hb+ Ho;	}
	if  ( Hb+ Ho == Hmax )
	    { tr1= tr;				}
	}

    th->thThreshold= ( tr0+ tr1 )/ 2;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Johannsen minimal entropy method.	*/
/*									*/
/************************************************************************/

void bmThresholdJohannsen(	ThresholderHistogram *		th )
    {
    double	Pt[256];
    double	Pq[256];

    int		tr;
    double	N= th->thPixelCount;

    int		e;
    int		s;

    double	Smin= N;

    {
    int		i;
    double	p;

    p= 0;
    Pt[0]= (double)th->thHistogram[0]/N;
    Pq[0]= 1- Pt[0];
    for ( i= 1; i < th->thHistogramSize; i++ )
	{
	p += (double)th->thHistogram[i];
	Pt[i]= p/ N;
	Pq[i]= 1.0- Pt[i-1];
	}
    }

    s= 0; e= 255;
    while( th->thHistogram[s] == 0 )
	{ s++; }
    while( th->thHistogram[e] == 0 )
	{ e--; }

    s++; e--;

    for ( tr= s; tr <= e; tr++ )
	{
	double		Sb;
	double		So;
	double		p;
	double		Ep;

	double		Epr= 0;
	double		Enx= 0;

	if  ( th->thHistogram[tr] == 0 )
	    { continue;	}

	p= (double)th->thHistogram[tr]/ N;
	Ep= -p* log( p );

	if  ( Pt[tr-1] > 0.0 )
	    { Epr= -Pt[tr-1]* log( Pt[tr-1] ); }
	Sb= log( Pt[tr] )+ ( 1.0/ Pt[tr] )* ( Ep+  Epr );

	if  ( Pq[tr+1] > 0.0 )
	    { Enx= -Pq[tr+1]* log( Pq[tr+1] ); }
	So= log( Pq[tr] )+ ( 1.0/ Pq[tr] )* ( Ep+  Enx );

	if  ( Sb+ So < Smin )
	    {
	    th->thThreshold= tr;
	    Smin= Sb+ So;
	    }
	}

    return;
    }

