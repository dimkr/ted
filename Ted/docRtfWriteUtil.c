/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<utilFontEncoding.h>
#   include	<appSystem.h>
#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void docRtfWriteNextLine(	int *			pCol,
				SimpleOutputStream *	sos )
    {
    int		col= *pCol;

    if  ( col > 0 )
	{
	sioOutPutString( "\r\n", sos );
	col= 0;
	}

    *pCol= col; return;
    }

void docRtfWriteTag(	const char *		tag,
			int *			pCol,
			SimpleOutputStream *	sos )
    {
    int		col= *pCol;
    int		len= strlen( tag );

    col += len;

    if  ( col > 72 )
	{
	docRtfWriteNextLine( &col, sos );
	col += len;
	}

    sioOutPutString( tag, sos );

    *pCol= col; return;
    }

void docRtfWriteDestinationBegin(	const char *		tag,
					int *			pCol,
					SimpleOutputStream *	sos )
    {
    int		col= *pCol;
    int		len= strlen( tag );

    col += 1+ len;

    if  ( col > 72 )
	{
	docRtfWriteNextLine( &col, sos );
	col += 1+ len;
	}

    sioOutPutCharacter( '{', sos );
    sioOutPutString( tag, sos );

    *pCol= col; return;
    }

void docRtfWriteArgDestinationBegin(	const char *		tag,
					int *			pCol,
					int			arg,
					SimpleOutputStream *	sos )
    {
    int		col= *pCol;
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    col += 1+ len;

    if  ( col > 72 )
	{
	docRtfWriteNextLine( &col, sos );
	col += 1+ len;
	}

    sioOutPutCharacter( '{', sos );
    sioOutPutString( tag, sos );
    sioOutPutString( scratch, sos );

    *pCol= col; return;
    }

void docRtfWriteDestinationEnd(		int *			pCol,
					SimpleOutputStream *	sos )
    {
    sioOutPutCharacter( '}', sos ); *pCol += 1;
    }

void docRtfWriteArgTag(	const char *		tag,
			int *			pCol,
			int			arg,
			SimpleOutputStream *	sos )
    {
    int		col= *pCol;
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    col += len;

    if  ( col > 72 )
	{
	docRtfWriteNextLine( &col, sos );
	col += len;
	}

    sioOutPutString( tag, sos );
    sioOutPutString( scratch, sos );

    *pCol= col; return;
    }

void docRtfEscapeString(	const unsigned char *	s,
				const unsigned char *	outputMapping,
				int *			pCol,
				int			n,
				SimpleOutputStream *	sos )
    {
    while( n > 0 )
	{
	int		c= *s;

	if  ( outputMapping )
	    { c= outputMapping[c];	}

	switch( c )
	    {
	    case '{': case '\\': case '}':
		sioOutPutCharacter( '\\', sos );
		sioOutPutCharacter( c, sos );
		*pCol += 2;
		break;
	    default:
		if  ( c < 32 || c > 127 )
		    {
		    static char	hexdigits[]= "0123456789abcdef";

		    sioOutPutCharacter( '\\', sos );
		    sioOutPutCharacter( '\'', sos );
		    sioOutPutCharacter( hexdigits[ ( c >> 4 ) & 0x0f ], sos );
		    sioOutPutCharacter( hexdigits[ ( c >> 0 ) & 0x0f ], sos );
		    *pCol += 4;
		    }
		else{
		    sioOutPutCharacter( c, sos );
		    *pCol += 1;
		    }
		break;
	    }

	n--; s++;
	}

    return;
    }

void docRtfWriteStringDestination(	const char *		tag,
					int *			pCol,
					const unsigned char *	s,
					int			n,
					int			addSemicolon,
					SimpleOutputStream *	sos )
    {
    const unsigned char * const outputMapping= (const unsigned char *)0;

    docRtfWriteDestinationBegin( tag, pCol, sos );

    if  ( n > 0 )
	{ sioOutPutCharacter( ' ', sos ); *pCol += 1;	}

    docRtfEscapeString( s, outputMapping, pCol, n, sos );

    if  ( addSemicolon )
	{ sioOutPutCharacter( ';', sos ); *pCol += 1;	}

    docRtfWriteDestinationEnd( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Write the tags that describe the geometry of a picture object.	*/
/*									*/
/************************************************************************/

int docRtfSavePictureTags(		const PictureProperties *	pip,
					int *				pCol,
					SimpleOutputStream *		sos )
    {
    int		xExt= pip->pip_xWinExt;
    int		yExt= pip->pip_yWinExt;

    if  ( xExt == 0 )
	{ xExt= (int)( 100000.0* pip->pipTwipsWide )/ ( 20* POINTS_PER_M ); }
    if  ( yExt == 0 )
	{ yExt= (int)( 100000.0* pip->pipTwipsHigh )/ ( 20* POINTS_PER_M ); }

    docRtfWriteArgTag( "\\picw", pCol, xExt, sos );
    docRtfWriteArgTag( "\\pich", pCol, yExt, sos );

    if  ( pip->pipScaleXSet != 100 )
	{ docRtfWriteArgTag( "\\picscalex", pCol, pip->pipScaleXSet, sos ); }
    if  ( pip->pipScaleYSet != 100 )
	{ docRtfWriteArgTag( "\\picscaley", pCol, pip->pipScaleYSet, sos ); }

    docRtfWriteArgTag( "\\picwgoal", pCol, pip->pipTwipsWide, sos );
    docRtfWriteArgTag( "\\pichgoal", pCol, pip->pipTwipsHigh, sos );

    if  ( pip->pipTopCropTwips != 0 )
	{ docRtfWriteArgTag( "\\piccropt", pCol, pip->pipTopCropTwips, sos ); }
    if  ( pip->pipBottomCropTwips != 0 )
	{ docRtfWriteArgTag( "\\piccropb", pCol, pip->pipBottomCropTwips, sos ); }
    if  ( pip->pipLeftCropTwips != 0 )
	{ docRtfWriteArgTag( "\\piccropl", pCol, pip->pipLeftCropTwips, sos ); }
    if  ( pip->pipRightCropTwips != 0 )
	{ docRtfWriteArgTag( "\\piccropr", pCol, pip->pipRightCropTwips, sos ); }

    /*  Not Necessary..
    if  ( pip->pipBliptag == 0 )
	{ pip->pipBliptag= appGetTimestamp(); 	}
    */

    if  ( pip->pipBliptag != 0 )
	{ docRtfWriteArgTag( "\\bliptag", pCol, pip->pipBliptag, sos ); }

    if  ( pip->pipBmUnitsPerInch > 0 )
	{ docRtfWriteArgTag( "\\blipupi", pCol, pip->pipBmUnitsPerInch, sos ); }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write object data.							*/
/*									*/
/************************************************************************/

int docRtfWriteMemoryBuffer(	const MemoryBuffer *		mb,
				int *				pCol,
				SimpleOutputStream *		sos )
    {
    const unsigned char *	s;
    int				i;

    int				col= *pCol;

    if  ( col+ mb->mbSize > 78 )
	{ docRtfWriteNextLine( &col, sos ); }

    s= mb->mbBytes;
    for ( i= 0; i < mb->mbSize; s++, i++ )
	{
	if  ( ! ( i % 78 ) )
	    { sioOutPutString( "\r\n", sos ); col= 0; }

	sioOutPutCharacter( *s, sos ); col++;
	}

    sioOutPutString( "\r\n}", sos ); col= 1;

    *pCol= col; return 0;
    }

/************************************************************************/
/*									*/
/*  Write a color table.						*/
/*									*/
/************************************************************************/

void docRtfWriteColorTable(	SimpleOutputStream *		sos,
				int *				pCol,
				const DocumentProperties *	dp )
    {
    int				i;
    const RGB8Color *		rgb8= dp->dpColors;

    docRtfWriteDestinationBegin( "\\colortbl", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < dp->dpColorCount; rgb8++, i++ )
	{
	if  ( i != dp->dpDefaultColor )
	    {
	    docRtfWriteArgTag( "\\red", pCol, rgb8->rgb8Red, sos );
	    docRtfWriteArgTag( "\\green", pCol, rgb8->rgb8Green, sos );
	    docRtfWriteArgTag( "\\blue", pCol, rgb8->rgb8Blue, sos );
	    }
	sioOutPutCharacter( ';', sos ); *pCol += 1;
	docRtfWriteNextLine( pCol, sos );
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a revision table.						*/
/*									*/
/************************************************************************/

void docRtfWriteRevisorTable(	SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentProperties *	dp )
    {
    int				i;

    docRtfWriteDestinationBegin( "\\*\\revtbl", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < dp->dpAuthorCount; i++ )
	{
	int		col= 0;

	docRtfWriteDestinationBegin( "", pCol, sos );

	docRtfEscapeString( dp->dpAuthors[i], outputMapping, &col,
				strlen( (char *)dp->dpAuthors[i] ), sos );

	docRtfWriteDestinationEnd( pCol, sos );
	docRtfWriteNextLine( pCol, sos );
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a word 2000 style table or cell formatting property.		*/
/*									*/
/************************************************************************/

void docRtfSaveAutoSpace(		int *			pCol,
					SimpleOutputStream *	sos,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number )
    {
    if  ( number != 0 || unit != TRautoNONE )
	{ docRtfWriteArgTag( unitTag, pCol, unit, sos );	}

    if  ( number != 0 )
	{ docRtfWriteArgTag( numberTag, pCol, number, sos );	}

    return;
    }

