/************************************************************************/
/*									*/
/*  Ted: Interaction of the application and the current document with	*/
/*  the color chooser etc.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Allocate a color for the current document.				*/
/*									*/
/************************************************************************/

int tedAppAllocateDocumentColor(	EditApplication *	ea,
					const RGB8Color *	rgb8 )
    {
    EditDocument *	ed= ea->eaCurrentDocument;
    int			color;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    color= tedDocAllocateDocumentColor( ed, rgb8 );
    if  ( color < 0 )
	{ LDEB(color); return -1;	}

    return color;
    }

/************************************************************************/
/*									*/
/*  Allocate a color for a document.					*/
/*									*/
/************************************************************************/

int tedDocAllocateDocumentColor(	EditDocument *		ed,
					const RGB8Color *	rgb8 )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    const int			maxColors= 256;
    const int			avoidZero= 1;
    int				color;

    color= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
						avoidZero, maxColors, rgb8 );
    if  ( color < 0 )
	{ LDEB(color); return -1;	}

    return color;
    }

