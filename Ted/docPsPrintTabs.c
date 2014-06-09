/************************************************************************/
/*									*/
/*  Save PostScript procedures for the implementation of tab stops.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<sioGeneral.h>
#   include	"docDraw.h"
#   include	"docPsPrint.h"

#   include	<appDebugon.h>

static int psPrintDrawTab(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const TextAttribute *	ta,
				int			x0,
				int			x1,
				int			baseLine,
				int			step,
				const char *		tabProc )
    {
    x0= step* ( ( x0+ step- 1 )/ step );
    if  ( x1 <= x0 )
	{ return 0;	}

    docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

    sioOutPrintf( ps->psSos, "%d %d %d %s\n",
		    x1- x0+ xShift, x0+ xShift, baseLine+ yShift, tabProc );

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

int docPsPrintTab(	PrintingState *			ps,
			DrawingContext *		dc,
			int				xShift,
			int				yShift,
			const BufferItem *		bi,
			const TextParticule *		tp,
			ParticuleData *			pd,
			int				baseLine,
			int				lineHeight )
    {
    BufferDocument *	bd= dc->dcDocument;
    const TabStopList *	tsl= &(bi->biParaTabStopList);
    const TabStop *	ts= tsl->tslTabStops+ pd->pdTabNumber;

    int			x0= pd->pdX0+ lineHeight/ 4;
    int			x1= pd->pdX0+ pd->pdWidth- lineHeight/2;

    TextAttribute	ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    switch( ts->tsLeader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							60, "dot-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							60, "dot-tab" );
		}

	    break;

	case DOCtlUNDERLINE:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							20, "ul-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							20, "ul-tab" );
		}

	    break;

	case DOCtlHYPH:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							140, "dash-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							140, "dash-tab" );
		}

	    break;

	case DOCtlTHICK:
	    LDEB(ts->tsLeader);
	    break;

	case DOCtlEQUAL:
	    LDEB(ts->tsLeader);
	    break;

	default:
	    LDEB(ts->tsLeader);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save procedures to use implement tab leades in PostScript.		*/
/*									*/
/************************************************************************/

static const char *	DOC_PS_dot_tab[]=
    {
    "/dot-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dot_tab_bold[]=
    {
    "/dot-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab[]=
    {
    "/dash-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab_bold[]=
    {
    "/dash-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab[]=
    {
    "/ul-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab_bold[]=
    {
    "/ul-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

void docPsSaveTabLeaderProcedures(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, DOC_PS_dot_tab,
				sizeof(DOC_PS_dot_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dot_tab_bold,
				sizeof(DOC_PS_dot_tab_bold)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dash_tab,
				sizeof(DOC_PS_dash_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dash_tab_bold,
				sizeof(DOC_PS_dash_tab_bold)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_ul_tab,
				sizeof(DOC_PS_ul_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_ul_tab_bold,
				sizeof(DOC_PS_ul_tab_bold)/sizeof(char *) );

    return;
    }

