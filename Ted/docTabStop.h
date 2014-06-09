/************************************************************************/
/*									*/
/*  Tabs in a ruler.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TAB_STOP_H
#   define	DOC_TAB_STOP_H

typedef enum TabAlignment
    {
    DOCtaLEFT= 0,

    DOCtaRIGHT,
    DOCtaCENTER,
    DOCtaDECIMAL,

    DOCtaBAR,

    DOCta_COUNT
    } TabAlignment;

# define DOCta_BITS	4

typedef enum TabLeader
    {
    DOCtlNONE= 0,

    DOCtlDOTS,
    DOCtlUNDERLINE,
    DOCtlHYPH,

    DOCtlTHICK,
    DOCtlEQUAL,

    DOCtl_COUNT
    } TabLeader;

# define DOCtl_BITS	4

typedef struct TabStop
    {
    int			tsTwips;
    int			tsPixels;
    unsigned int	tsAlignment:DOCta_BITS;
    unsigned int	tsLeader:DOCtl_BITS;
    unsigned int	tsFromStyleOrList:1;
    } TabStop;

typedef enum TabProperty
    {
    TABpropX= 0,
    TABpropALIGN,
    TABpropLEADER,
    TABpropFROM_STYLE,

    TABprop_COUNT
    } TabProperty;

typedef struct TabStopList
    {
    TabStop *	tslTabStops;
    short int	tslTabStopCount;
    } TabStopList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTabStop(	TabStop *		ts );

void docInitTabStopList(	TabStopList *		tsl );
void docCleanTabStopList(	TabStopList *		tsl );

extern int docAddTabToList(		TabStopList *		tsl,
					const TabStop *		tsNew );

extern int docAddTabToListPixels(	TabStopList *		tsl,
					const TabStop *		tsNew );

extern void docDeleteTabFromList(	TabStopList *		tsl,
					int			n );

extern int docCopyTabStopList(	int *				pChanged,
				TabStopList *			to,
				const TabStopList *		from,
				int				pixels );

extern void docCompareTabStopLists(	int *			pDifferent,
					const TabStopList *	tsl1,
					const TabStopList *	from );

extern int docNextTabStop(	TabStop *			pTs,
				int *				pX,
				int *				pTab,
				const TabStopList *		tsl,
				int				x0Geometry,
				int				x0TextLines,
				int				tabInterval,
				int				xPosition );


extern int docRulerMergeTabs(	TabStopList *		tsl,
				double			xfac,
				int			x0GeometryPixels,
				int			tabCount,
				const TabStop *		tabStops );

#   endif	/*  DOC_TAB_STOP_H	*/
