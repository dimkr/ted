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

#   include	<bitmap.h>

#   include	<appSystem.h>
#   include	<appWinMeta.h>
#   include	<appImage.h>

#   include	<sioGeneral.h>
#   include	<sioStdio.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<sioBase64.h>

#   include	"docBuf.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

#   include	<charnames.h>

#   define	TWIPS_TO_SIZE(x)	(((x)+9)/18)

#   define	IDlenTAIL		200

#   define	USE_PNG			0
#   define	USE_GIF			1

/************************************************************************/
/*									*/
/*  Information used when writing HTML.					*/
/*									*/
/************************************************************************/

typedef struct HtmlWritingContext
    {
    SimpleOutputStream *	hwcSos;
    BufferDocument *		hwcBd;

    int				hwcAsMimeAggregate;

    const char *		hwcFilename;
    const char *		hwcMimeBoundary;

    int				hwcBaselength;
    int				hwcRelativeOffset;
    int				hwcInHyperlink;
    int				hwcInBookmark;
    int				hwcInPageref;
    int				hwcBytesInLink;
    TextAttribute		hwcDefaultAttribute;
    ParagraphProperties		hwcParagraphProperties;

    int				hwcColumn;

    int				hwcImageCount;
    int				hwcNoteRefCount;
    int				hwcNoteDefCount;

    char *			hwcNameScratch;
    int				hwcWriteImageFromObjectData;
    bmWriteBitmap		hwcWriteThisBitmap;

    char			hwcImageMimeType[40+1];
    char			hwcContentIdTail[IDlenTAIL+1];

    int				hwcCurrentAttributeNumber;
    } HtmlWritingContext;

static void docInitHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    hwc->hwcSos= (SimpleOutputStream *)0;
    hwc->hwcBd= (BufferDocument *)0;

    hwc->hwcAsMimeAggregate= 0;

    hwc->hwcFilename= (const char *)0;
    hwc->hwcMimeBoundary= (const char *)0;

    hwc->hwcBaselength= 0;
    hwc->hwcRelativeOffset= 0;
    hwc->hwcInHyperlink= 0;
    hwc->hwcInBookmark= 0;
    hwc->hwcInPageref= 0;
    hwc->hwcBytesInLink= 0;

    utilInitTextAttribute( &(hwc->hwcDefaultAttribute) );
    hwc->hwcDefaultAttribute.taFontSizeHalfPoints= 24;
    hwc->hwcDefaultAttribute.taFontNumber= -1;

    docInitParagraphProperties( &(hwc->hwcParagraphProperties) );

    hwc->hwcColumn= 0;

    hwc->hwcImageCount= 0;
    hwc->hwcNoteRefCount= 0;
    hwc->hwcNoteDefCount= 0;

    hwc->hwcNameScratch= (char *)0;
    hwc->hwcWriteImageFromObjectData= 0;
    hwc->hwcWriteThisBitmap= (bmWriteBitmap)0;

    hwc->hwcImageMimeType[0]= '\0';
    hwc->hwcContentIdTail[0]= '\0';

    hwc->hwcCurrentAttributeNumber= -1;
    return;
    }

static void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    docCleanParagraphProperties( &(hwc->hwcParagraphProperties) );

    if  ( hwc->hwcNameScratch )
	{ free( hwc->hwcNameScratch );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

static void docHtmlSetImageNameAndType(	HtmlWritingContext *		hwc,
					const PictureProperties *	pip,
					const char *			ext,
					const char *			mime )
    {
    if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	{ sprintf( hwc->hwcNameScratch, "%08lx.%s", pip->pipBliptag, ext ); }
    else{
	sprintf( hwc->hwcNameScratch, "%.*s.img/%08lx.%s",
		hwc->hwcBaselength, hwc->hwcFilename, pip->pipBliptag, ext );
	}

    strcpy( hwc->hwcImageMimeType, mime );

    return;
    }

static int docHtmlMakeNameForImage(	HtmlWritingContext *	hwc,
					InsertedObject *	io )
    {
    char *		fresh;
    AppBitmapImage *	abi;
    int			siz;

    PictureProperties *	pip= &(io->ioPictureProperties);

    siz= hwc->hwcBaselength+ 50;
    fresh= (char *)realloc( hwc->hwcNameScratch, siz+ 1 );
    if  ( ! fresh )
	{ LXDEB(siz,fresh); return -1;  }
    hwc->hwcNameScratch= fresh;

    if  ( pip->pipBliptag == 0 )
	{ pip->pipBliptag= appGetTimestamp();	}

    abi= (AppBitmapImage *)io->ioPrivate;
    if  ( ! abi )
	{ return 1;	}

    if  ( io->ioKind == DOCokPICTJPEGBLIP )
	{
	docHtmlSetImageNameAndType( hwc, pip, "jpg", "image/jpeg" );
	hwc->hwcWriteImageFromObjectData= 1;
	hwc->hwcWriteThisBitmap= bmJpegWriteJfif; /* ignored */
	return 0;
	}

    if  ( io->ioKind == DOCokPICTPNGBLIP )
	{
	docHtmlSetImageNameAndType( hwc, pip, "png", "image/png" );
	hwc->hwcWriteImageFromObjectData= 1;
	hwc->hwcWriteThisBitmap= bmPngWritePng; /* ignored */
	return 0;
	}

    if  ( bmCanWriteGifFile( &(abi->abiBitmap), 89, 10.0 )	&&
	  bmCanWriteJpegFile( &(abi->abiBitmap), 0, 10.0 )	)
	{ return 1;	}

#   if  USE_PNG
    if  ( abi->abiBitmap.bdColorEncoding == BMcoRGB8PALETTE )
	{
	docHtmlSetImageNameAndType( hwc, pip, "png", "image/png" );
	hwc->hwcWriteImageFromObjectData= 0;
	hwc->hwcWriteThisBitmap= bmPngWritePng;
	return 0;
	}
#   endif

#   if  USE_GIF
    if  ( ! bmCanWriteGifFile( &(abi->abiBitmap), 89, 10.0 ) )
	{
	docHtmlSetImageNameAndType( hwc, pip, "gif", "image/gif" );
	hwc->hwcWriteImageFromObjectData= 0;
	hwc->hwcWriteThisBitmap= bmGifWriteGif;
	return 0;
	}
#   endif

    if  ( ! bmCanWriteJpegFile( &(abi->abiBitmap), 0, 10.0 ) )
	{
	docHtmlSetImageNameAndType( hwc, pip, "jpg", "image/jpeg" );
	hwc->hwcWriteImageFromObjectData= 0;
	hwc->hwcWriteThisBitmap= bmJpegWriteJfif;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

static void docHtmlPutString(		const char *		s,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;
    int				col= hwc->hwcColumn;

    /*  mime  */
    if  ( col == 0 && s[0] == '-' )
	{ sioOutPutCharacter( ' ', sos ); col += 1; }

    sioOutPutString( s, sos ); col += strlen( s );

    hwc->hwcColumn= col; return;
    }

static void docHtmlNewLine(	HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    if  ( hwc->hwcAsMimeAggregate )
	{ sioOutPutCharacter( '\r', sos );	}

    sioOutPutCharacter( '\n', sos );
    hwc->hwcColumn= 0;
    }

static void docHtmlWriteTagStringArg(	const char *		tag,
					const char *		arg,
					HtmlWritingContext *	hwc )
    {
    if  ( hwc->hwcColumn > 1				&&
	  hwc->hwcColumn+ 1+ strlen( tag )+ 1+ 3 > 76	)
	{ docHtmlNewLine( hwc );		}
    else{ docHtmlPutString( " ", hwc );	}

    docHtmlPutString( tag, hwc );
    docHtmlPutString( "=", hwc );
    docHtmlPutString( arg, hwc );

    return;
    }

static void docHtmlWriteTagIntArg(	const char *		tag,
					int			arg,
					HtmlWritingContext *	hwc )
    {
    char	scratch[20];

    sprintf( scratch, "%d", arg );

    docHtmlWriteTagStringArg( tag, scratch, hwc );

    return;
    }

static void docHtmlEscapeCharacters(	const unsigned char *	s,
					int			len,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    while( len > 0 )
	{
	switch( *s )
	    {
	    case '"':
		docHtmlPutString( "&quot;", hwc );
		break;

	    case '&':
		docHtmlPutString( "&amp;", hwc );
		break;

	    case '>':
		docHtmlPutString( "&gt;", hwc );
		break;

	    case '<':
		docHtmlPutString( "&lt;", hwc );
		break;

	    case '-':
		/*  mime  */
		if  ( hwc->hwcColumn == 0 )
		    { sioOutPutCharacter( ' ', sos ); hwc->hwcColumn += 1; }

		sioOutPutCharacter( *s, sos ); hwc->hwcColumn += 1;
		break;

	    default:
		sioOutPutCharacter( *s, sos ); hwc->hwcColumn += 1;
		break;
	    }

	s++; len--;
	}

    return;
    }

static void docHtmlEscapeString(	const unsigned char *	s,
					HtmlWritingContext *	hwc )
    {
    docHtmlEscapeCharacters( s, strlen( (const char *)s ), hwc );

    return;
    }

# if 0
static int docHtmlFontSize(	int	halfPoints )
    {
    if  ( halfPoints/2 < 6 )	{ return 1;	}
    if  ( halfPoints/2 < 9 )	{ return 2;	}

    if  ( halfPoints/2 > 24 )	{ return 7;	}
    if  ( halfPoints/2 > 19 )	{ return 6;	}
    if  ( halfPoints/2 > 15 )	{ return 5;	}
    if  ( halfPoints/2 > 11 )	{ return 4;	}

    return 3;
    }
# endif

/************************************************************************/
/*									*/
/*  Translate an RTF font designation to an HTML/CSS1 style one.	*/
/*									*/
/************************************************************************/

static int docHtmlFontFamilyIndicator(	char *			target,
					int			maxSize,
					const DocumentFont *	df )
    {
    const char *	genericFamily= (const char *)0;
    const char *	hintFamily= (const char *)0;

    int			genericSize= 0;
    int			hintSize= 0;
    int			nameSize= 0;
    int			size= 0;

    int			commas= 0;

    if  ( ! strcmp( df->dfFamilyStyle, "froman" ) )
	{
	genericFamily= "serif";
	hintFamily= "Times\",\"Times New Roman";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fswiss" ) )
	{
	genericFamily= "sans-serif";
	hintFamily= "Helvetica\",\"Arial";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fscript" ) )
	{
	genericFamily= "cursive";
	hintFamily= "Zapf-Chancery";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fdecor" ) )
	{
	genericFamily= "cursive";
	hintFamily= "Zapf-Chancery";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fmodern" ) )
	{
	genericFamily= "monospace";
	hintFamily= "Courier";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "ftech" ) )
	{
	hintFamily= "Symbol";
	}

    if  ( genericFamily )
	{
	commas++;
	genericSize= strlen( genericFamily ); /* keyword -> no quotes */
	}
    if  ( hintFamily )
	{
	commas++;
	hintSize= 1+ strlen( hintFamily )+ 1;
	}
    nameSize= 1+ strlen( df->dfName )+ 1;

    size= nameSize+ genericSize+ hintSize+ commas;
    if  ( size > maxSize )
	{ LLLLDEB(nameSize,genericSize,hintSize,maxSize); return -1; }

    *(target++)= '"';
    strcpy( target, df->dfName ); target += nameSize- 2;
    *(target++)= '"';

    if  ( hintFamily )
	{
	*(target++)= ',';
	*(target++)= '"';
	strcpy( target, hintFamily ); target += hintSize- 2;
	*(target++)= '"';
	}

    if  ( genericFamily )
	{
	*(target++)= ',';
	strcpy( target, genericFamily ); target += genericSize;
	}

    *(target  )= '\0';
    return size;
    }

/************************************************************************/
/*									*/
/*  Change attributes.							*/
/*									*/
/************************************************************************/


static void docHtmlChangeAttributes(	HtmlWritingContext *		hwc,
					int				taNr )
    {
    if  ( taNr == hwc->hwcCurrentAttributeNumber )
	{ return;	}

    if  ( hwc->hwcCurrentAttributeNumber >= 0 )
	{
	docHtmlPutString( "</SPAN>", hwc );
	}

    if  ( taNr >= 0 )
	{
	char	scratch[20];

	sprintf( scratch, "t%d", taNr );

	docHtmlPutString( "<SPAN", hwc );
	docHtmlWriteTagStringArg( "CLASS", scratch, hwc );
	docHtmlPutString( ">", hwc );
	}

    hwc->hwcCurrentAttributeNumber= taNr;
    }

static int docHtmlSaveImgTag(	int				w,
				int				h,
				HtmlWritingContext *		hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    docHtmlPutString( "<IMG", hwc );

    if  ( hwc->hwcColumn > 10					&&
	  hwc->hwcColumn+ 6+ strlen( hwc->hwcNameScratch ) > 76	)
	{ docHtmlNewLine( hwc );		}
    else{ docHtmlPutString( " ", hwc );	}

    docHtmlPutString( "SRC=\"", hwc );

    if  ( hwc->hwcAsMimeAggregate )
	{
	docHtmlPutString( "cid:", hwc );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '.', sos );
	sioOutPutString( hwc->hwcContentIdTail, sos );
	}
    else{
	docHtmlPutString( hwc->hwcNameScratch+ hwc->hwcRelativeOffset,
								hwc );
	}

    docHtmlPutString( "\"", hwc );

    docHtmlWriteTagIntArg( "WIDTH", w, hwc );
    docHtmlWriteTagIntArg( "HEIGHT", h, hwc );

    docHtmlPutString( " ALT=\"&lt;IMG&gt;\">", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlSavePicture(	InsertedObject *	io,
				int *			pDone,
				HtmlWritingContext *	hwc )
    {
    int				res;

    const AppBitmapImage *	abi;
    const BitmapDescription *	bd;

    int				w;
    int				h;
    int				d;

    if  ( ! io->ioPrivate )
	{
	if  ( docGetBitmapForObject( io ) )
	    { LDEB(1); return 1;	}
	}

    abi= (AppBitmapImage *)io->ioPrivate;
    if  ( ! abi )
	{ return 0;	}
    bd= &(abi->abiBitmap);

    res= docHtmlMakeNameForImage( hwc, io );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0;	}

    w= TWIPS_TO_SIZE( ( io->ioScaleXSet* io->ioTwipsWide )/100 );
    h= TWIPS_TO_SIZE( ( io->ioScaleYSet* io->ioTwipsHigh )/100 );

    d= ( 100* bd->bdPixelsWide- 100* w )/ bd->bdPixelsWide;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ w= bd->bdPixelsWide;	}

    d= ( 100* bd->bdPixelsHigh- 100* h )/ bd->bdPixelsHigh;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ h= bd->bdPixelsHigh;	}

    if  ( docHtmlSaveImgTag( w, h, hwc ) )
	{ SDEB(hwc->hwcNameScratch); return -1;	}

    hwc->hwcImageCount++;

    *pDone= 1; return 0;
    }

static int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
				const char *			fileName,
				int				fileSize,
				const char *			markName,
				int				markSize,
				const char *			refName,
				int				refSize )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    int				afterSpace;
    int				needed= 0;

    docHtmlPutString( "<A", hwc );
    afterSpace= 0;

    if  ( fileSize > 0 || markSize > 0 )
	{
	needed += 1+ 6+ fileSize+ markSize+ 1;
	
	if  ( markSize > 0 )
	    { needed += 1;	}
	}

    if  ( refSize > 0 )
	{ needed += 1+ 6+ refSize+ 1;	}

    if  ( hwc->hwcColumn > 5		&&
	  hwc->hwcColumn+ needed > 76	)
	{ docHtmlNewLine( hwc ); afterSpace= 1;		}

    if  ( fileSize > 0 || markSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "HREF=\"", hwc );

	if  ( fileSize > 0 )
	    {
	    while( fileSize > 0 )
		{
		sioOutPutCharacter( *fileName, sos );
		fileName++; fileSize--; hwc->hwcColumn++;
		}
	    }

	if  ( markName && markSize > 0 )
	    {
	    sioOutPutCharacter( '#', sos );

	    while( markSize > 0 )
		{
		sioOutPutCharacter( *markName, sos );
		markName++; markSize--; hwc->hwcColumn++;
		}
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( refSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "NAME=\"", hwc );

	while( refSize > 0 )
	    {
	    sioOutPutCharacter( *refName, sos );
	    refName++; refSize--; hwc->hwcColumn++;
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    docHtmlPutString( ">", hwc );

    return 0;
    }

static int docHtmlStartField(	const DocumentField *		df,
				HtmlWritingContext *		hwc,
				const BufferItem *		bi,
				int				attNr )
    {
    const char *	fileName= (const char *)0;
    int			fileSize= 0;
    const char *	markName= (const char *)0;
    int			markSize= 0;
    const char *	refName= (const char *)0;
    int			refSize= 0;

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		char		ref[25+1];
		char		def[25+1];

		if  ( bi->biInExternalItem == DOCinBODY )
		    {
		    sprintf( ref, "_NREF_%d", hwc->hwcNoteRefCount+ 1 );
		    sprintf( def, "_NDEF_%d", hwc->hwcNoteRefCount+ 1 );

		    markName= def;
		    refName=  ref;
		    }
		else{
		    sprintf( ref, "_NREF_%d", hwc->hwcNoteDefCount+ 1 );
		    sprintf( def, "_NDEF_%d", hwc->hwcNoteDefCount+ 1 );

		    markName= ref;
		    refName=  def;
		    }

		markSize= strlen( markName );
		refSize= strlen( refName );

		docHtmlChangeAttributes( hwc, -1 );

		docHtmlStartAnchor( hwc,
					fileName, fileSize,
					markName, markSize,
					refName, refSize );

		docHtmlChangeAttributes( hwc, attNr );

		hwc->hwcBytesInLink= 0;
		}

	    break;

	case DOCfkHYPERLINK:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		if  ( ! docFieldGetHyperlink( df,
				&fileName, &fileSize, &markName, &markSize ) )
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc,
					    fileName, fileSize,
					    markName, markSize,
					    refName, refSize );

		    docHtmlChangeAttributes( hwc, attNr );

		    hwc->hwcBytesInLink= 0;
		    }
		}
	    break;

	case DOCfkBOOKMARK:
	    hwc->hwcInBookmark++;
	    if  ( ! hwc->hwcInHyperlink && hwc->hwcInBookmark == 1 )
		{
		if  ( ! docFieldGetBookmark( df, &refName, &refSize ) )
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc,
					    fileName, fileSize,
					    markName, markSize,
					    refName, refSize );

		    docHtmlChangeAttributes( hwc, attNr );
		    }
		}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref++;
	    break;

	default:
	    break;
	}

    return 0;
    }

static int docHtmlFinishField(	const DocumentField *		df,
				HtmlWritingContext *		hwc )
    {
    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	case DOCfkHYPERLINK:
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</A>", hwc );
		}
	    hwc->hwcInHyperlink--;
	    if  ( hwc->hwcInHyperlink < 0 )
		{ hwc->hwcInHyperlink= 0;	}
	    break;

	case DOCfkBOOKMARK:
	    if  ( ! hwc->hwcInHyperlink && hwc->hwcInBookmark == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</A>", hwc );
		}
	    hwc->hwcInBookmark--;
	    if  ( hwc->hwcInBookmark < 0 )
		{ hwc->hwcInBookmark= 0;	}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref--;
	    break;

	default:
	    break;
	}
    
    return 0;
    }

static int docHtmlSaveParticules( const BufferItem *		bi,
				int				part,
				int				partUpto,
				HtmlWritingContext *		hwc )
    {
    int					done= 0;

    int					afterSpace= 0;

    TextParticule *			tp= bi->biParaParticules+ part;
    int					stroff= tp->tpStroff;
    unsigned char *			s= bi->biParaString+ stroff;

    int					pictureDone;
    InsertedObject *			io;

    TextAttribute			ta;

    const DocumentField *		df;
    const FieldKindInformation *	fki;

    int					len;

    const BufferDocument *		bd= hwc->hwcBd;

    while( part < partUpto )
	{
	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	switch( tp->tpKind )
	    {
	    case DOCkindTAB:
		docHtmlPutString( " ", hwc );
		afterSpace= 1;
		s++; stroff++;
		break;

	    case DOCkindTEXT:
		docHtmlChangeAttributes( hwc, tp->tpTextAttributeNumber );

		if  ( afterSpace && hwc->hwcColumn+ tp->tpStrlen > 76 )
		    { docHtmlNewLine( hwc );	}

		len= tp->tpStroff+ tp->tpStrlen- stroff;

		if  ( ! hwc->hwcInHyperlink	||
		      ! hwc->hwcInPageref	||
		      hwc->hwcBytesInLink == 0	)
		    {
		    docHtmlEscapeCharacters( s, len, hwc );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += len;	}
		    }

		s += len;
		stroff += len;

		afterSpace= 0;
		if  ( tp->tpStrlen > 0 && s[-1] == ' ' )
		    { afterSpace= 1;	}
		break;

	    case DOCkindOBJECT:
		pictureDone= 0;
		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  (   io->ioKind == DOCokPICTWMETAFILE		||
			io->ioKind == DOCokPICTPNGBLIP			||
			io->ioKind == DOCokPICTJPEGBLIP			||
			io->ioKind == DOCokMACPICT			||
		      ( io->ioKind == DOCokOLEOBJECT 		&&
		        io->ioResultKind == DOCokPICTWMETAFILE	)	)
		    {
		    if  ( docHtmlSavePicture( io, &pictureDone, hwc ) )
			{ XDEB(io);	}
		    }

		if  ( ! pictureDone )
		    { docHtmlPutString( " ", hwc );	}

		afterSpace= 0; s++; stroff++;
		break;

	    case DOCkindFIELDSTART:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    {
		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );
		    }

		if  ( df->dfKind == DOCfkBOOKMARK )
		    {
		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;
		    char	scratch[20+1];

		    count= docCountParticulesInField( bi, part, partUpto );

		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );

		    if  ( bi->biInExternalItem == DOCinBODY )
			{
			sprintf( scratch, "%d", hwc->hwcNoteRefCount+ 1 );
			hwc->hwcNoteRefCount++;
			}
		    else{
			sprintf( scratch, "%d", hwc->hwcNoteDefCount+ 1 );
			hwc->hwcNoteDefCount++;
			}

		    docHtmlPutString( scratch, hwc );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += strlen( scratch );	}

		    done= count+ 1;
		    stroff= tp[done].tpStroff; s= bi->biParaString+ stroff;
		    break;
		    }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindFIELDEND:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( df->dfKind == DOCfkCHFTN )
		    { docHtmlFinishField( df, hwc );	}

		if  ( df->dfKind == DOCfkBOOKMARK )
		    { docHtmlFinishField( df, hwc ); }

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    { docHtmlFinishField( df, hwc ); }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		docHtmlPutString( "<BR>", hwc );
		docHtmlNewLine( hwc );
		afterSpace= 0;
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		break;

	    case DOCkindXE:
	    case DOCkindTC:
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		break;

	    case DOCkindNOTE:
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;

	    default:
		LDEB(tp->tpKind);
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;
	    }

	done++; part++; tp++;
	}

    docHtmlChangeAttributes( hwc, -1 );

    return done;
    }

/************************************************************************/
/*									*/
/*  The body of a paragraph is written as a <DIV> or <TD> division.	*/
/*									*/
/************************************************************************/

static void docHtmlEmitBorderStyle(
			    const char *		whatBorder,
			    const BorderProperties *	bp,
			    HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    docHtmlPutString( whatBorder, hwc );
    sioOutPutCharacter( ':', sos ); hwc->hwcColumn++;

    /* width */
    sioOutPrintf( sos, " %dpt", ( bp->bpPenWideTwips+ 10 )/ 20 );
    hwc->hwcColumn += 5;

    /* style */
    docHtmlPutString( " solid", hwc );

    /* color */
    if  ( bp->bpColor == 0 )
	{
	docHtmlPutString( " inherit", hwc );
	}
    else{
	const BufferDocument *		bd= hwc->hwcBd;
	const DocumentProperties *	dp= &(bd->bdProperties);

	const RGB8Color *		rgb8= dp->dpColors+ bp->bpColor;

	char				scratch[30];

	sprintf( scratch, " #%02x%02x%02x",
					rgb8->rgb8Red,
					rgb8->rgb8Green,
					rgb8->rgb8Blue );

	docHtmlPutString( scratch, hwc );
	}

    sioOutPutCharacter( ';', sos ); hwc->hwcColumn++;

    return;
    }

static void docHtmlEmitBackgroundProperty(
				const ItemShading *		is,
				HtmlWritingContext *		hwc )
    {
    const BufferDocument *	bd= hwc->hwcBd;

    int				isSolid= 0;
    int				r= 0;
    int				g= 0;
    int				b= 0;

    char			scratch[39];

    if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
	{ LDEB(1);	}

    if  ( isSolid )
	{
	sprintf( scratch, "\"#%02x%02x%02x\"", r, g, b );

	docHtmlWriteTagStringArg( "BGCOLOR", scratch, hwc );
	}

    return;
    }

static int docHtmlUseBackgroundStyle( 
				const ItemShading *		is,
				HtmlWritingContext *		hwc )
    {
    const BufferDocument *	bd= hwc->hwcBd;

    int				isSolid= 0;
    int				r= 0;
    int				g= 0;
    int				b= 0;

    if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
	{ LDEB(1);	}

    if  ( isSolid )
	{ return 1;	}

    return 0;
    }

static void docHtmlEmitBackgroundStyle(
				const ItemShading *		is,
				HtmlWritingContext *		hwc )
    {
    const BufferDocument *	bd= hwc->hwcBd;

    int				isSolid= 0;
    int				r= 0;
    int				g= 0;
    int				b= 0;

    char			scratch[50];

    if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
	{ LDEB(1);	}

    if  ( isSolid )
	{
	sprintf( scratch, "background-color: #%02x%02x%02x;", r, g, b );

	docHtmlPutString( scratch, hwc );
	}

    return;
    }


static void docHtmlStartParagraphBody(	const BufferItem *	paraBi,
					const char *		tag,
					int			fontHeight,
					int			isBulleted,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;
    int				useStyle= 0;

    if  ( paraBi->biParaLeftIndentTwips <= -100	||
	  paraBi->biParaLeftIndentTwips >=  100	)
	{ useStyle++;	}

    if  ( ! isBulleted						&&
	  ( paraBi->biParaFirstIndentTwips <= -100	||
	    paraBi->biParaFirstIndentTwips >=  100	)	)
	{ useStyle++;	}

    if  ( DOCisBORDER( &(paraBi->biParaTopBorder) ) )
	{ useStyle++;	}
    if  ( DOCisBORDER( &(paraBi->biParaLeftBorder) ) )
	{ useStyle++;	}
    if  ( DOCisBORDER( &(paraBi->biParaRightBorder) ) )
	{ useStyle++;	}
    if  ( DOCisBORDER( &(paraBi->biParaBottomBorder) ) )
	{ useStyle++;	}

    if  ( docHtmlUseBackgroundStyle( &(paraBi->biParaShading), hwc ) )
	{ useStyle++;	}

    sioOutPutCharacter( '<', sos ); hwc->hwcColumn++;
    docHtmlPutString( tag, hwc );

    switch( paraBi->biParaAlignment )
	{
	case DOCiaLEFT:
	    break;
	case DOCiaRIGHT:
	    docHtmlPutString( " ALIGN=\"RIGHT\"", hwc );
	    break;

	case DOCiaCENTERED:
	    docHtmlPutString( " ALIGN=\"CENTER\"", hwc );
	    break;

	case DOCiaJUSTIFIED:
	    docHtmlPutString( " ALIGN=\"JUSTIFY\"", hwc );
	    break;

	default:
	    LDEB(paraBi->biParaAlignment);
	    break;
	}

    /* No!
    if  ( ! useStyle && paraBi->biParaShading.isPattern == DOCspSOLID )
	{ docHtmlEmitBackgroundProperty( &(paraBi->biParaShading), hwc ); }
    */

    if  ( useStyle > 0 )
	{
	docHtmlPutString( " STYLE=\"", hwc );

	if  ( paraBi->biParaLeftIndentTwips <= -100	||
	      paraBi->biParaLeftIndentTwips >=  100	)
	    {
	    docHtmlPutString( "padding-left:", hwc );
	    sioOutPrintf( sos, "%dpt;", paraBi->biParaLeftIndentTwips/ 20 );
	    hwc->hwcColumn += 6;
	    }

	if  ( ! isBulleted					&&
	      ( paraBi->biParaFirstIndentTwips <= -100	||
		paraBi->biParaFirstIndentTwips >=  100	)	)
	    {
	    docHtmlPutString( "text-indent:", hwc );
	    sioOutPrintf( sos, "%dpt;", paraBi->biParaFirstIndentTwips/ 20 );
	    hwc->hwcColumn += 6;
	    }

	if  ( paraBi->biParaShading.isPattern == DOCspSOLID )
	    { docHtmlEmitBackgroundStyle( &(paraBi->biParaShading), hwc ); }

	if  ( DOCisBORDER( &(paraBi->biParaTopBorder) ) )
	    {
	    docHtmlEmitBorderStyle( "border-top",
					&(paraBi->biParaTopBorder), hwc );
	    }
	if  ( DOCisBORDER( &(paraBi->biParaLeftBorder) ) )
	    {
	    docHtmlEmitBorderStyle( "border-left",
					&(paraBi->biParaLeftBorder), hwc );
	    }
	if  ( DOCisBORDER( &(paraBi->biParaRightBorder) ) )
	    {
	    docHtmlEmitBorderStyle( "border-right",
					&(paraBi->biParaRightBorder), hwc );
	    }
	if  ( DOCisBORDER( &(paraBi->biParaBottomBorder) ) )
	    {
	    docHtmlEmitBorderStyle( "border-bottom",
					&(paraBi->biParaBottomBorder), hwc );
	    }

	docHtmlPutString( "\"", hwc );
	}

    docHtmlPutString( ">", hwc );

    if  ( paraBi->biParaSpaceBeforeTwips > fontHeight/ 2 )
	{
	docHtmlPutString( "&nbsp;<BR>", hwc );
	docHtmlNewLine( hwc );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Decide whether the beginning of the paragraph is a bullet.		*/
/*									*/
/************************************************************************/

static int docHtmlParagraphIsBulleted(	int *			pTabParticule,
					int *			pBulletAttr,
					const BufferDocument *	bd,
					const BufferItem *	bi )
    {
    int			stroff;
    int			part;
    TextParticule *	tp;
    int			contentCount= 0;
    int			bulletAttribute= -1;

    if  ( bi->biParaAlignment != DOCiaLEFT	&&
	  bi->biParaAlignment != DOCiaJUSTIFIED	)
	{ return 0;	}

    if  ( bi->biInExternalItem == DOCinFOOTNOTE	||
	  bi->biInExternalItem == DOCinENDNOTE	)
	{ return 0;	}

    if  ( bi->biParaLeftIndentTwips <= 0 )
	{ return 0;	}
    if  ( bi->biParaFirstIndentTwips >= 0 )
	{ return 0;	}
    if  ( bi->biParaFirstIndentTwips < -bi->biParaLeftIndentTwips )
	{ return 0;	}

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	switch( tp->tpKind )
	    {
	    const DocumentField *		df;
	    const FieldKindInformation *	fki;

	    case DOCkindTAB:
		if  ( bulletAttribute < 0 )
		    { bulletAttribute= tp->tpTextAttributeNumber;	}
		*pTabParticule= part;
		*pBulletAttr= bulletAttribute;
		return 1;

	    case DOCkindOBJECT:
	    case DOCkindTEXT:
		if  ( contentCount > 0 )
		    { return 0;	}
		bulletAttribute= tp->tpTextAttributeNumber;
		contentCount++;
		break;

	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    { return 0;	}

		if  ( df->dfKind == DOCfkBOOKMARK )
		    { return 0;	}

		break;

	    default:
		break;
	    }

	stroff= tp->tpStroff+ tp->tpStrlen;
	if  ( stroff > 10 )
	    { return 0;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a 'Paragraph'							*/
/*									*/
/*  But not as a <P>...</P>.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveParaItem(	const BufferItem *		paraBi,
				HtmlWritingContext *		hwc )
    {
    TextParticule *		tp;
    unsigned char *		s;

    int				isBulleted= 0;
    int				tabParticule= 0;

    int				part= 0;
    int				stroff= 0;

    TextAttribute		ta;
    int				fontHeight;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    int				bulletAttribute= -1;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    const BufferDocument *	bd= hwc->hwcBd;

    if  ( paraBi->biParaParticuleCount == 0		||
	  paraBi->biParaStrlen == 0			)
	{
	docHtmlPutString( "<DIV>&nbsp;</DIV>", hwc );
	docHtmlNewLine( hwc );
	return 0;
	}

    isBulleted= docHtmlParagraphIsBulleted( &tabParticule, &bulletAttribute,
								bd, paraBi );

    part= 0;
    stroff= 0;
    tp= paraBi->biParaParticules+ part;
    s= paraBi->biParaString+ stroff;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    fontHeight= 10* ta.taFontSizeHalfPoints;

    if  ( isBulleted )
	{
	SimpleOutputStream *	sos= hwc->hwcSos;

	int			left= paraBi->biParaLeftIndentTwips;
	int			first= left+ paraBi->biParaFirstIndentTwips;

	if  ( first > 100 )
	    {
	    docHtmlPutString( "<SPAN STYLE=\"float:left; width: ", hwc );
	    sioOutPrintf( sos, "%dpt;\"", first/ 20 );
	    hwc->hwcColumn += 6;

	    if  ( bulletAttribute >= 0 )
		{
		char	scratch[20];

		sprintf( scratch, "t%d", bulletAttribute );
		docHtmlWriteTagStringArg( "CLASS", scratch, hwc );
		hwc->hwcCurrentAttributeNumber= bulletAttribute;
		}

	    docHtmlPutString( ">&nbsp;</SPAN>", hwc );
	    docHtmlNewLine( hwc );

	    left -= first;
	    }

	docHtmlPutString( "<SPAN STYLE=\"float:left; width: ", hwc );
	sioOutPrintf( sos, "%dpt;\"", left/ 20 );
	hwc->hwcColumn += 6;

	if  ( bulletAttribute >= 0 )
	    {
	    char	scratch[20];

	    sprintf( scratch, "t%d", bulletAttribute );
	    docHtmlWriteTagStringArg( "CLASS", scratch, hwc );
	    hwc->hwcCurrentAttributeNumber= bulletAttribute;
	    }

	docHtmlPutString( ">", hwc );

	if  ( docHtmlSaveParticules( paraBi, part, tabParticule, hwc ) < 0 )
	    { LDEB(part); return -1; }

	if  ( bulletAttribute < 0 )
	    { docHtmlPutString( "</SPAN>", hwc );	}

	docHtmlNewLine( hwc );

	part= tabParticule+ 1;
	}

    docHtmlStartParagraphBody( paraBi, "DIV", fontHeight, isBulleted, hwc );

    if  ( docHtmlSaveParticules( paraBi, part,
				paraBi->biParaParticuleCount, hwc ) < 0 )
	{ LDEB(part); return -1;	}

    if  ( paraBi->biParaSpaceAfterTwips > fontHeight/ 2 )
	{ docHtmlPutString( "<BR>&nbsp;</DIV>", hwc );	}
    else{
	/* BUG in IE6 ! */
	if  ( isBulleted && bulletAttribute >= 0  )
	    {
	    sioOutPrintf( hwc->hwcSos,
			    "<!-- IE --><SPAN CLASS=t%d>&nbsp;</SPAN>",
			    bulletAttribute );
	    }

	docHtmlPutString( "</DIV>", hwc );		
	}
    docHtmlNewLine( hwc );

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask, &(hwc->hwcParagraphProperties),
			&ppUpdMask, &(paraBi->biParaProperties),
			colorMap, listStyleMap ) )
	{ LDEB(1);	}

    return 0;
    }

static int docHtmlSaveRowItem(	const BufferItem *		rowBi,
				HtmlWritingContext *		hwc )
    {
    int				i;
    const CellProperties *	cp;

    if  ( ! hwc->hwcParagraphProperties.ppInTable )
	{
	int			cellPadding;
	int			useBorder= 0;

	cp= rowBi->biRowCells;
	for ( i= 0; i < rowBi->biChildCount; cp++, i++ )
	    {
	    if  ( DOCisBORDER( &(cp->cpTopBorder) )	||
		  DOCisBORDER( &(cp->cpLeftBorder) )	||
		  DOCisBORDER( &(cp->cpRightBorder) )	||
		  DOCisBORDER( &(cp->cpBottomBorder) )	)
		{ useBorder= 1; break;	}
	    }

	docHtmlPutString( "<TABLE CELLSPACING=0", hwc );

	if  ( useBorder )
	    { docHtmlPutString( " BORDER=\"1\"", hwc ); }

	cellPadding= TWIPS_TO_SIZE( rowBi->biRowHalfGapWidthTwips )- 4;
	if  ( cellPadding < 1 )
	    { docHtmlWriteTagIntArg( "CELLPADDING", 0, hwc ); }
	if  ( cellPadding > 1 )
	    { docHtmlWriteTagIntArg( "CELLPADDING", cellPadding, hwc ); }

	docHtmlPutString( "><TR VALIGN=\"TOP\">", hwc );
	hwc->hwcParagraphProperties.ppInTable= 1;
	}
    else{
	docHtmlPutString( "<TR VALIGN=\"TOP\">", hwc );
	}

    cp= rowBi->biRowCells;
    for ( i= 0; i < rowBi->biChildCount; cp++, i++ )
	{
	int				j;
	BufferItem *			cellBi= rowBi->biChildren[i];
	int				wide;

	int				bottomTwips= -1;
	int				stripHigh= -1;
	const int			page= 0;
	const int			column= 0;

	const BufferDocument *		bd= hwc->hwcBd;

	BlockFrame			bf;
	ParagraphFrame			pf;

	int				useStyle= 0;

	int				rowspan= 1;
	int				colspan= 1;

	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue;	}

	docTableDetermineCellspans( &rowspan, &colspan, rowBi, i );

	docBlockFrameTwips( &bf, cellBi->biChildren[0], bd, page, column );

	docParagraphFrameTwips( &pf, &bf, bottomTwips, stripHigh,
						    cellBi->biChildren[0] );
	wide= TWIPS_TO_SIZE( pf.pfX1GeometryTwips- pf.pfX0GeometryTwips );

	if  ( DOCisBORDER( &(cp->cpTopBorder) ) )
	    { useStyle++;	}
	if  ( DOCisBORDER( &(cp->cpLeftBorder) ) )
	    { useStyle++;	}
	if  ( DOCisBORDER( &(cp->cpRightBorder) ) )
	    { useStyle++;	}
	if  ( DOCisBORDER( &(cp->cpBottomBorder) ) )
	    { useStyle++;	}

	docHtmlPutString( "<TD", hwc );
	docHtmlWriteTagIntArg( "WIDTH", wide, hwc );

	if  ( colspan > 1 )
	    { docHtmlWriteTagIntArg( "COLSPAN", colspan, hwc );	}
	if  ( rowspan > 1 )
	    { docHtmlWriteTagIntArg( "ROWSPAN", rowspan, hwc );	}

	if  ( ! useStyle && cp->cpShading.isPattern == DOCspSOLID )
	    { docHtmlEmitBackgroundProperty( &(cp->cpShading), hwc );	}

	if  ( useStyle )
	    {
	    docHtmlPutString( " STYLE=\"", hwc );

	    if  ( cp->cpShading.isPattern == DOCspSOLID )
		{ docHtmlEmitBackgroundStyle( &(cp->cpShading), hwc );	}

	    if  ( DOCisBORDER( &(cp->cpTopBorder) ) )
		{
		docHtmlEmitBorderStyle( "border-top",
					    &(cp->cpTopBorder), hwc );
		}
	    if  ( DOCisBORDER( &(cp->cpLeftBorder) ) )
		{
		docHtmlEmitBorderStyle( "border-left",
					    &(cp->cpLeftBorder), hwc );
		}
	    if  ( DOCisBORDER( &(cp->cpRightBorder) ) )
		{
		docHtmlEmitBorderStyle( "border-right",
					    &(cp->cpRightBorder), hwc );
		}
	    if  ( DOCisBORDER( &(cp->cpBottomBorder) ) )
		{
		docHtmlEmitBorderStyle( "border-bottom",
					    &(cp->cpBottomBorder), hwc );
		}

	    docHtmlPutString( "\"", hwc );
	    }

	docHtmlPutString( ">", hwc );
	docHtmlNewLine( hwc );

	for ( j= 0; j < cellBi->biChildCount; j++ )
	    {
	    BufferItem *	para= cellBi->biChildren[j];

	    if  ( docHtmlSaveParaItem( para, hwc ) )
		{ LDEB(1); return -1;	}
	    }

	docHtmlPutString( "</TD>", hwc );
	if  ( i < rowBi->biChildCount- 1 )
	    { docHtmlNewLine( hwc );	}
	}

    docHtmlPutString( "</TR>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlSaveItem(	const BufferItem *		bi,
				HtmlWritingContext *		hwc )
    {
    int			i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	rowAsGroup:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docHtmlSaveItem( bi->biChildren[i], hwc ) )
		    { LDEB(i); return -1;	}
		}

	    if  ( bi->biLevel == DOClevSECT		&&
		  hwc->hwcParagraphProperties.ppInTable	)
		{
		docHtmlPutString( "</TABLE>", hwc );
		docHtmlNewLine( hwc );
		hwc->hwcParagraphProperties.ppInTable= 0;
		}
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{
		if  ( hwc->hwcParagraphProperties.ppInTable )
		    {
		    docHtmlPutString( "</TABLE>", hwc );
		    docHtmlNewLine( hwc );
		    hwc->hwcParagraphProperties.ppInTable= 0;
		    }

		goto rowAsGroup;
		}

	    if  ( docHtmlSaveRowItem( bi, hwc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docHtmlSaveParaItem( bi, hwc ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start to output an image						*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docHtmlStartImage(	HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;
    SimpleOutputStream *	sosImage;

    if  ( hwc->hwcAsMimeAggregate )
	{
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc->hwcMimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Type: ", sos );
	sioOutPutString( hwc->hwcImageMimeType, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Id: <", sos );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '.', sos );
	sioOutPutString( hwc->hwcContentIdTail, sos );
	sioOutPutCharacter( '>', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Transfer-Encoding: ", sos );
	sioOutPutString( "base64", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Disposition: inline;", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( " filename=\"", sos );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '"', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sosImage= sioOutBase64Open( sos );
	if  ( ! sosImage )
	    { SXDEB(hwc->hwcNameScratch,sosImage); return sosImage; }
	}
    else{
	sosImage= sioOutStdioOpen( hwc->hwcNameScratch );
	if  ( ! sosImage )
	    { SXDEB(hwc->hwcNameScratch,sosImage); return sosImage; }
	}

    return sosImage;
    }

/************************************************************************/
/*									*/
/*  Save the images in the document.					*/
/*									*/
/************************************************************************/

static int docHtmlSaveImageBytes(	HtmlWritingContext *	hwc,
					InsertedObject *	io )
    {
    int				rval= 0;

    int				res;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisHex= (SimpleInputStream *)0;
    AppBitmapImage *		abi;

    res= docHtmlMakeNameForImage( hwc, io );
    if  ( res < 0 )
	{ LDEB(res); rval= -1; goto ready;	}
    if  ( res > 0 )
	{ rval= 0; goto ready;	}

    abi= (AppBitmapImage *)io->ioPrivate;

    sosImage= docHtmlStartImage( hwc );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( hwc->hwcWriteImageFromObjectData )
	{
	int		done;
	unsigned char	buf[1024];

	sisMem= sioInMemoryOpen( &(io->ioObjectData) );
	if  ( ! sisMem )
	    { XDEB(sisMem); rval= -1; goto ready;	}

	sisMem= sioInMemoryOpen( &(io->ioObjectData) );
	if  ( ! sisMem )
	    { XDEB(sisMem); rval= -1; goto ready;	}
	sisHex= sioInHexOpen( sisMem );
	if  ( ! sisHex )
	    { XDEB(sisHex); rval= -1; goto ready;	}

	while( ( done= sioInReadBytes( sisHex, buf, sizeof(buf) ) ) > 0 )
	    {
	    if  ( sioOutWriteBytes( sosImage, buf, done ) != done )
		{ LDEB(done);	}
	    }
	}
    else{
	if  ( (*hwc->hwcWriteThisBitmap)( &(abi->abiBitmap), abi->abiBuffer,
								sosImage ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    if  ( sisHex )
	{ sioInClose( sisHex );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

static int docHtmlSaveImages(	const BufferItem *		bi,
				HtmlWritingContext *		hwc )
    {
    int			i;
    TextParticule *	tp;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docHtmlSaveImages( bi->biChildren[i], hwc ) )
		    { LDEB(i); return -1;	}
		}

	    return 0;

	case DOClevPARA:
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    tp= bi->biParaParticules;
    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= bi->biParaObjects+ tp->tpObjectNumber;

	if  (   io->ioKind == DOCokPICTWMETAFILE		||
		io->ioKind == DOCokPICTPNGBLIP			||
		io->ioKind == DOCokPICTJPEGBLIP			||
		io->ioKind == DOCokMACPICT			||
	      ( io->ioKind == DOCokOLEOBJECT 		&&
		io->ioResultKind == DOCokPICTWMETAFILE	)	)
	    {
	    if  ( docHtmlSaveImageBytes( hwc, io ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save styles corresponding to the different text attributes in the	*/
/*  document.								*/
/*									*/
/*  1)  Silly netscape really makes fonts too small. Add a little.	*/
/*									*/
/************************************************************************/

static void docHtmlSaveTextAttributeStyle(
				    const TextAttribute *	ta,
				    int				n,
				    void *			through )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)through;
    char			scratch[150+ 1];
    int				size;
    int				fontSize;

    const BufferDocument *		bd= hwc->hwcBd;
    const DocumentProperties *		dp= &(bd->bdProperties);
    const DocumentFontList *		dfl= &(dp->dpFontList);
    const DocumentFont *		df;

    /*  1  */
    fontSize= ta->taFontSizeHalfPoints;
    fontSize= ( 6* ta->taFontSizeHalfPoints )/ 5;

    df= dfl->dflFonts+ ta->taFontNumber;

    sprintf( scratch, "%d", n );

    docHtmlPutString( "SPAN.t", hwc );
    docHtmlPutString( scratch, hwc );
    docHtmlNewLine( hwc );
    docHtmlPutString( "  {", hwc );
    docHtmlNewLine( hwc );

    if  ( ta->taTextColorNumber > 0 )
	{
	const RGB8Color *	rgb8= dp->dpColors+ ta->taTextColorNumber;

	sprintf( scratch, "#%02x%02x%02x",
					rgb8->rgb8Red,
					rgb8->rgb8Green,
					rgb8->rgb8Blue );

	docHtmlPutString( "  color: ", hwc );
	docHtmlPutString( scratch, hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    size= docHtmlFontFamilyIndicator( scratch, sizeof(scratch)- 1, df );
    if  ( size < 0 )
	{ LDEB(size);	}
    if  ( size > 0 )
	{
	docHtmlPutString( "  font-family: ", hwc );
	docHtmlPutString( scratch, hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taFontIsBold )
	{
	docHtmlPutString( "  font-weight: bold", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taFontIsSlanted )
	{
	docHtmlPutString( "  font-style: italic", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taTextIsUnderlined )
	{
	docHtmlPutString( "  text-decoration: underline", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taHasStrikethrough )
	{
	docHtmlPutString( "  text-decoration: line-through", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taSmallCaps )
	{
	docHtmlPutString( "  font-variant: small-caps", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT )
	{
	docHtmlPutString( "  vertical-align: super", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    if  ( ta->taSuperSub == DOCfontSUBSCRIPT )
	{
	docHtmlPutString( "  vertical-align: sub", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    sprintf( scratch, "%d%spt", fontSize/ 2, fontSize % 2?".5":"" );
    docHtmlPutString( "  font-size: ", hwc );
    docHtmlPutString( scratch, hwc );
    docHtmlPutString( ";", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "  }", hwc );
    docHtmlNewLine( hwc );

    return;
    }

static void docHtmlSaveTextAttributeStyles(	HtmlWritingContext *	hwc )
    {
    const BufferDocument *		bd= hwc->hwcBd;

    docHtmlPutString( "<STYLE type=\"text/css\">", hwc );
    docHtmlNewLine( hwc );
    /*
    docHtmlPutString( "<!--", hwc );
    docHtmlNewLine( hwc );
    */

    utilForAllTextAttributes( &(bd->bdTextAttributeList),
			    docHtmlSaveTextAttributeStyle, (void *)hwc );

    /*
    docHtmlPutString( "-->", hwc );
    docHtmlNewLine( hwc );
    */
    docHtmlPutString( "</STYLE>", hwc );
    docHtmlNewLine( hwc );

    return;
    }

/************************************************************************/
/*									*/
/*  Save a document to HTML, or to a html mail mime aggregate.		*/
/*									*/
/************************************************************************/

int docHtmlSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				int			asMimeAggr,
				const char *		mimeBoundary,
				const char *		filename )
    {
    HtmlWritingContext		hwc;

    const BufferItem *		bi= &bd->bdItem;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    docInitHtmlWritingContext( &hwc );

    hwc.hwcSos= sos;
    hwc.hwcBd= bd;
    hwc.hwcAsMimeAggregate= asMimeAggr;
    hwc.hwcMimeBoundary= mimeBoundary;

    if  ( filename )
	{
	const char *	s;

	hwc.hwcFilename= filename;
	s= strrchr( filename, '.' );
	if  ( s )
	    { hwc.hwcBaselength= s- filename;		}
	else{ hwc.hwcBaselength= strlen( filename );	}

	s= strrchr( filename, '/' );
	if  ( s )
	    { hwc.hwcRelativeOffset= s- filename+ 1;	}
	else{ hwc.hwcRelativeOffset= 0;			}
	}

    if  ( hwc.hwcAsMimeAggregate )
	{
	if  ( appMakeUniqueString( hwc.hwcContentIdTail, IDlenTAIL ) )
	    { LDEB(IDlenTAIL); return -1;	}

	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc.hwcMimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Type: text/html", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Transfer-Encoding: 8bit", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	}

    docHtmlPutString( "<HTML>", &hwc );
    docHtmlNewLine( &hwc );

    docHtmlPutString( "<HEAD>", &hwc );
    docHtmlNewLine( &hwc );

    if  ( dp->dpTitle )
	{
	docHtmlPutString( "<TITLE>", &hwc );
	docHtmlEscapeString( dp->dpTitle, &hwc );
	docHtmlPutString( "</TITLE>", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpSubject )
	{
	docHtmlPutString(
		    "<META NAME=\"description\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpSubject, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpKeywords )
	{
	docHtmlPutString(
		    "<META NAME=\"keywords\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpKeywords, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpDoccomm )
	{
	docHtmlPutString(
		    "<META NAME=\"comment\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpDoccomm, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpAuthor )
	{
	docHtmlPutString(
		    "<META NAME=\"author\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpAuthor, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpHlinkbase )
	{
	docHtmlPutString( "<BASE HREF=\"", &hwc );
	docHtmlEscapeString( dp->dpHlinkbase, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    docHtmlSaveTextAttributeStyles( &hwc );

    docHtmlPutString( "</HEAD>", &hwc );
    docHtmlNewLine( &hwc );

    docHtmlPutString( "<BODY BGCOLOR=\"#ffffff\" TEXT=\"#000000\"", &hwc );

    if  ( dg->dgLeftMarginTwips > 300		||
	  dg->dgRightMarginTwips > 300		||
	  dg->dgTopMarginTwips > 300		||
	  dg->dgBottomMarginTwips > 300		)
	{
	char	scratch[40];

	docHtmlPutString( " STYLE=\"", &hwc );

	if  ( dg->dgTopMarginTwips > 300 )
	    {
	    sprintf( scratch, "margin-top: %dpt;",
						dg->dgTopMarginTwips/ 30 );
	    docHtmlPutString( scratch, &hwc );
	    }

	if  ( dg->dgLeftMarginTwips > 300 )
	    {
	    sprintf( scratch, "margin-left: %dpt;",
						dg->dgLeftMarginTwips/ 30 );
	    docHtmlPutString( scratch, &hwc );
	    }

	if  ( dg->dgRightMarginTwips > 300 )
	    {
	    sprintf( scratch, "margin-right: %dpt;",
						dg->dgRightMarginTwips/ 30 );
	    docHtmlPutString( scratch, &hwc );
	    }

	if  ( dg->dgBottomMarginTwips > 300 )
	    {
	    sprintf( scratch, "margin-bottom: %dpt;",
						dg->dgBottomMarginTwips/ 30 );
	    docHtmlPutString( scratch, &hwc );
	    }

	docHtmlPutString( "\"", &hwc );
	}

    docHtmlPutString( ">", &hwc );
    docHtmlNewLine( &hwc );

    if  ( docHtmlSaveItem( bi, &hwc ) )
	{ LDEB(bi->biLevel); return -1; }

    if  ( hwc.hwcNoteRefCount > 0 )
	{
	int		i;
	DocumentNote *	dn;

	docHtmlPutString( "<HR>", &hwc );

	dn= bd->bdNotes;
	for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	    {
	    ExternalItem *	ei;

	    if  ( dn->dnParaNr < 0 )
		{ continue;	}

	    ei= &(dn->dnExternalItem);
	    if  ( ! ei->eiItem )
		{ XDEB(ei->eiItem); continue;	}

	    if  ( docHtmlSaveItem( ei->eiItem, &hwc ) )
		{ LDEB(bi->biLevel); return -1; }
	    }

	if  ( hwc.hwcNoteDefCount != hwc.hwcNoteRefCount )
	    { LLDEB(hwc.hwcNoteDefCount,hwc.hwcNoteRefCount);	}
	}

    docHtmlPutString( "</BODY></HTML>", &hwc );
    docHtmlNewLine( &hwc );

    if  ( ! hwc.hwcAsMimeAggregate	&&
	    hwc.hwcImageCount > 0	)
	{
	strncpy( hwc.hwcNameScratch, hwc.hwcFilename, hwc.hwcBaselength );
	strcpy( hwc.hwcNameScratch+ hwc.hwcBaselength, ".img" );

	if  ( appTestDirectory( hwc.hwcNameScratch )	&&
	      appMakeDirectory( hwc.hwcNameScratch )	)
	    { SDEB(hwc.hwcNameScratch); return -1;	}
	}

    if  ( hwc.hwcImageCount > 0			&&
	  docHtmlSaveImages( bi, &hwc )		)
	{ LDEB(hwc.hwcImageCount); return -1;	}

    if  ( hwc.hwcAsMimeAggregate )
	{
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc.hwcMimeBoundary, sos );
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	}

    docCleanHtmlWritingContext( &hwc );

    return 0;
    }
