/************************************************************************/
/*									*/
/*  Find a bookmark in a document.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docFind.h"

typedef struct FindBookmark
    {
    const char *	fbMarkName;
    int			fbMarkSize;
    int			fbObjectNumber;
    } FindBookmark;

/************************************************************************/
/*									*/
/*  Find the occurrence of a bookmark in a paragraph.			*/
/*									*/
/************************************************************************/

static int docBookmarkParaFindNext(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    FindBookmark *	fb= (FindBookmark *)through;

    int			partFrom;

    int			part;
    TextParticule *	tp;

    const int		lastOne= 1;

    partFrom= 0;
    if  ( dpFrom->dpStroff != 0						&&
	  docFindParticule( &partFrom, bi, dpFrom->dpStroff, lastOne )	)
	{ LDEB(dpFrom->dpStroff); return -1;	}

    if  ( partFrom < 0 || partFrom >= bi->biParaParticuleCount )
	{ LLDEB(partFrom,bi->biParaParticuleCount); return -1;	}

    tp= bi->biParaParticules+ partFrom;
    for ( part= partFrom; part < bi->biParaParticuleCount; tp++, part++ )
	{
	const DocumentField *	df;
	const char *		foundName;
	int			foundSize;

	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( df->dfKind == DOCfkBOOKMARK				&&
		  ! docFieldGetBookmark( df, &foundName, &foundSize )	&&
		  foundSize == fb->fbMarkSize				&&
		  ! memcmp( foundName, fb->fbMarkName, fb->fbMarkSize )	)
		{
		if  ( fb->fbObjectNumber >= 0 )
		    { LDEB(fb->fbObjectNumber);	}

		fb->fbObjectNumber= tp->tpObjectNumber;

		ds->dsBegin.dpBi= bi;
		ds->dsBegin.dpStroff= tp->tpStroff;
		}
	    }

	if  ( tp->tpKind == DOCkindFIELDEND		&&
	      tp->tpObjectNumber >= 0			&&
	      tp->tpObjectNumber == fb->fbObjectNumber	)
	    {
	    ds->dsEnd.dpBi= bi;
	    ds->dsEnd.dpStroff= tp->tpStroff;

	    return 0;
	    }
	}

    fb->fbObjectNumber= -1;
    return 1;
    }

int docFindBookmarkInDocument(	DocumentSelection *	ds,
				BufferDocument *	bd,
				const char *		markName,
				int			markSize )
    {
    int				ret;

    DocumentPosition		dpFrom;

    DocumentSelection		dsNew;

    FindBookmark		fb;

    docInitDocumentPosition( &dpFrom );
    docInitDocumentSelection( &dsNew );

    fb.fbMarkName= markName;
    fb.fbMarkSize= markSize;
    fb.fbObjectNumber= -1;

    ret= docFindFindNextInDocument( &dsNew, &dpFrom, bd,
				    docBookmarkParaFindNext, (void *)&fb );

    if  ( ret < 0 )
	{ LDEB(ret); return ret;	}

    if  ( ret == 0 )
	{
	const int	direction= 1;
	const int	col0= -1;
	const int	col1= -1;

	if  ( dsNew.dsBegin.dpBi != dsNew.dsEnd.dpBi )
	    { XXDEB(dsNew.dsBegin.dpBi,dsNew.dsEnd.dpBi); return 1; }

	docSetRangeSelection( ds, &(dsNew.dsBegin), &(dsNew.dsEnd),
							direction, col0, col1 );

	return 0;
	}

    return 1;
    }

