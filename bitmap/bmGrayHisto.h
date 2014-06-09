#   ifndef		BM_GRAY_HISTO_H
#   define		BM_GRAY_HISTO_H

/************************************************************************/
/*									*/
/*  Thresholding implementations.					*/
/*									*/
/************************************************************************/

typedef struct ThresholderHistogram
    {
    long	thHistogram[256];
    int		thHistogramSize;
    int		thPixelCount;
    int		thThreshold;
    } ThresholderHistogram;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void bmInitThresholderHistogram(	ThresholderHistogram *		th );

extern void bmThresholdMean(		ThresholderHistogram *		th );

extern void bmThresholdQuantile(	ThresholderHistogram *		th,
					double				frac );

extern void bmThreshold2Peaks(		ThresholderHistogram *		th );

extern void bmThresholdRidler(		ThresholderHistogram *		th );

extern void bmThresholdVariance(	ThresholderHistogram *		th );

extern void bmThresholdMinimumError(	ThresholderHistogram *		th );

extern void bmThresholdKapur(		ThresholderHistogram *		th );

extern void bmThresholdJohannsen(	ThresholderHistogram *		th );


#   endif	/*	BM_GRAY_HISTO_H	*/

