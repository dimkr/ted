#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<sioMemory.h>
#   include	<sioEndian.h>
#   include	<sioHex.h>
#   include	<utilEndian.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilMatchFont.h>
#   include	<utilAffineTransform.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  True Type font.							*/
/*									*/
/*  A considerable part of this code was borrowed from the ttf2pt1	*/
/*  project. Differences in purpose and validations against the		*/
/*  documentation made me change a lot.					*/
/*									*/
/*  SEE: http://developer.apple.com/fonts/TTRefMan/			*/
/*  SEE ALSO: http://www.microsoft.com/OpenType/OTSpec/default.htm	*/
/*									*/
/*  A substantial part of this code is experimental: E.G. it contains	*/
/*  code that can be used to convert from true type fonts to postscript	*/
/*  type1 that was adapted from 'ttf2pt1' but it is never called.	*/
/*									*/
/*  1)  Platform: 0=Unicode, 1= Mac, 3= Windows.			*/
/*  2)  Encoding: 0=Default, 1= Version 1.1, 2) ISO 10646-1993		*/
/*	deprecated, 3) Unicode 2.0 semantics.				*/
/*  3)  Platform,Encoding->bytes					*/
/*	0,*->2								*/
/*	1,0->1								*/
/*									*/
/************************************************************************/

static int utilTtfStringBytes( int p, int e )
    {
    if  ( p == 0 )		{ return 2;	}
    if  ( p == 1 && e == 0 )	{ return 1;	}
    if  ( p == 3 && e == 0 )	{ return 2;	}
    if  ( p == 3 && e == 1 )	{ return 2;	}

    LLDEB(p,e); return 1;
    }

typedef struct TrueTypeTableEntry
    {
    char		ttteTag[4+1];
    unsigned long	ttteChecksum;
    unsigned long	ttteOffset;
    unsigned long	ttteLength;
    unsigned char *	ttteBytes;

    unsigned long	ttteSaveOffset;
    } TrueTypeTableEntry;

typedef struct TrueTypeNameRecord
    {
    unsigned int	ttnrPlatformID;		/*  1  */
    unsigned int	ttnrEncodingID;		/*  2  */
    unsigned int	ttnrLanguageID;
    unsigned int	ttnrNameID;
    unsigned int	ttnrLength;
    unsigned int	ttnrOffset;

    unsigned char *	ttnrSavedBytes;
    } TrueTypeNameRecord;

typedef struct TrueTypeNameTable
    {
    unsigned int		ttntFormat;
    unsigned int		ttntRecordCount;
    unsigned int		ttntOffset;

    TrueTypeNameRecord *	ttntRecords;
    } TrueTypeNameTable;

typedef struct TrueTypeOS_2Table
    {
    unsigned int  	ttotVersion;
    int		 	ttotAvgCharWidth;
    unsigned int 	ttotWeightClass;
    unsigned int 	ttotWidthClass;
    int		 	ttotType;
    int		 	ttotSubscriptXSize;
    int		 	ttotSubscriptYSize;
    int		 	ttotSubscriptXOffset;
    int		 	ttotSubscriptYOffset;
    int		 	ttotSuperscriptXSize;
    int		 	ttotSuperscriptYSize;
    int		 	ttotSuperscriptXOffset;
    int		 	ttotSuperscriptYOffset;
    int		 	ttotStrikeoutSize;
    int		 	ttotStrikeoutPosition;
    int		 	ttotFamilyClass;
    unsigned char 	ttotPanose[10];
    unsigned long 	ttotCharRange[4];
    unsigned char 	ttotVendID[4+1];
    unsigned int 	ttotSelection;
    unsigned int 	ttotFirstCharIndex;
    unsigned int 	ttotLastCharIndex;
    } TrueTypeOS_2Table;

typedef struct TrueTypeHeadTable
    {
    unsigned long	tthtVersion;
    unsigned long	tthtFontRevision;
    unsigned long	tthtChecksumAdjust;
    unsigned long	tthtMagicNo;
    unsigned int	tthtFlags;
    unsigned int	tthtUnitsPerEm;
    unsigned char	tthtCreated[8+1];
    unsigned char	tthtModified[8+1];
    int			tthtXMin;
    int			tthtYMin;
    int			tthtXMax;
    int			tthtYMax;
    unsigned int	tthtMacStyle;
    unsigned int	tthtLowestRecPPem;
    int			tthtFontDirection;
    int			tthtIndexToLocFormat;
    int			tthtGlyphDataFormat;
    } TrueTypeHeadTable;

typedef struct TrueTypePostTable
    {
    int			ttptFormatUpper;
    unsigned int	ttptFormatLower;
    int			ttptItalicAngleUpper;
    unsigned int	ttptItalicAngleLower;
    int			ttptUnderlinePosition;
    int			ttptUnderlineThickness;
    unsigned int	ttptIsFixedPitch;
    /* unsigned int	ttptReserved */
    unsigned long	ttptMinMemType42;
    unsigned long	ttptMaxMemType42;
    unsigned long	ttptMinMemType1;
    unsigned long	ttptMaxMemType1;

    unsigned int	ttptIndexCount;
    unsigned short *	ttptIndices;
    unsigned int	ttptStringCount;
    unsigned char **	ttptStrings;
    } TrueTypePostTable;

typedef struct TrueTypeHheaTable
    {
    unsigned char	hheaVersion[4+1];
    int			hheaAscender;
    int			hheaDescender;
    int			hheaLineGap;
    unsigned int	hheaMaxAdvanceWidth;
    int			hheaMinLsb;
    int			hheaMinRsb;
    int			hheaXMaxExtent;
    int			hheaCaretSlopeRise;
    int			hheaCaretSlopeRun;
    /* int		hheaReserved[5]; */
    int			hheaMetricDataFormat;
    unsigned int	hheaMetricCount;
    } TrueTypeHheaTable;

typedef struct HorizontalMetric
    {
    unsigned int	hmAdvanceWidth;
    int			hmLsb;
    } HorizontalMetric;

typedef struct TrueTypeGlyph
    {
    int				ttgContourCount;	/* signed ! */
    int				ttgXMin;
    int				ttgYMin;
    int				ttgXMax;
    int				ttgYMax;

    const unsigned char *	ttgData;
    } TrueTypeGlyph;

typedef struct TrueTypeCmapRecord
    {
    unsigned int		ttcrPlatformID;		/*  1  */
    unsigned int		ttcrEncodingID;
    unsigned long		ttcrOffset;


    unsigned int		ttcrFormat;
    unsigned int		ttcrLength;
    unsigned int		ttcrLanguage;

				/*  If format == 0  */
    unsigned char		ttcr0GlyphIDArray[256];

				/*  If format == 4  */
    unsigned int		ttcr4SegCountX2;
    unsigned int		ttcr4SearchRange;
    unsigned int		ttcr4EntrySelector;
    unsigned int		ttcr4RangeShift;
    const unsigned char *	ttcr4Data;

				/*  If format == 6  */
    unsigned int		ttcr6FirstCode;
    unsigned int		ttcr6EntryCount;
    unsigned short *		ttcr6GlyphIndices;
    } TrueTypeCmapRecord;

typedef struct TrueTypeCmapTable
    {
    int				ttctVersion;
    int				ttctEncodingRecordCount;
    TrueTypeCmapRecord *	ttctEncodingRecords;
    } TrueTypeCmapTable;

typedef struct TrueTypeKernPair
    {
    unsigned int	ttkpLeft;
    unsigned int	ttkpRight;
    int			ttkpValue;
    } TrueTypeKernPair;

typedef struct TrueTypeKernSub
    {
    unsigned int	ttksVersion;
    unsigned int	ttksLength;
    unsigned int	ttksCoverage;
				/*  Bits in Coverage:			*/
				/*  0x8000: Has vertical kerning values	*/
				/*  0x4000: Has cross stream values	*/
				/*  0x4000: Has variation kerning vals	*/
				/*  0x1f00: Unused: must be 0.		*/
				/*  0x00ff: Contain the format in which	*/
				/*          the pairs are stored.	*/
				/*  So normal horizontal kerning stored	*/
				/*  as pairs has zero upper and lower	*/
				/*  bytes.				*/
    unsigned int	ttksPairCount;
    unsigned int	ttksSearchRange;
    unsigned int	ttksEntrySelector;
    unsigned int	ttksRangeShift;

    TrueTypeKernPair *	ttksPairs;
    } TrueTypeKernSub;

typedef struct TrueTypeKernTable
    {
    int				ttktVersion;
    int				ttktKernSubCount;
    TrueTypeKernSub *		ttktKernSubs;
    } TrueTypeKernTable;

typedef struct TrueTypeFont
    {
    unsigned long		ttfVersion;
    int				ttfTableCount;
    int				ttfSearchRange;
    int				ttfEntrySelector;
    int				ttfRangeShift;

    TrueTypeTableEntry *	ttfTables;

    TrueTypeNameTable		ttfNameTable;
    TrueTypePostTable		ttfPostTable;
    TrueTypeHeadTable		ttfHeadTable;
    TrueTypeHheaTable		ttfHheaTable;
    TrueTypeCmapTable		ttfCmapTable;
    TrueTypeKernTable		ttfKernTable;
    TrueTypeOS_2Table		ttfOS_2Table;

    unsigned long *		ttfLocations;
    int				ttfLocationCount;

    HorizontalMetric *		ttfHorizontalMetrics;
    int				ttfHorizontalMetricCount;

    TrueTypeGlyph *		ttfGlyphs;
    int				ttfGlyphCount;
    } TrueTypeFont;

/************************************************************************/
/*									*/
/*  Elementary bookeeping.						*/
/*									*/
/************************************************************************/

static void utilInitTrueTypeOS_2Table(	TrueTypeOS_2Table *	ttot )
    {
    ttot->ttotVersion= 0;
    ttot->ttotAvgCharWidth= 0;
    ttot->ttotWeightClass= 0;
    ttot->ttotWidthClass= 0;
    ttot->ttotType= 0;
    ttot->ttotSubscriptXSize= 0;
    ttot->ttotSubscriptYSize= 0;
    ttot->ttotSubscriptXOffset= 0;
    ttot->ttotSubscriptYOffset= 0;
    ttot->ttotSuperscriptXSize= 0;
    ttot->ttotSuperscriptYSize= 0;
    ttot->ttotSuperscriptXOffset= 0;
    ttot->ttotSuperscriptYOffset= 0;
    ttot->ttotStrikeoutSize= 0;
    ttot->ttotStrikeoutPosition= 0;
    ttot->ttotFamilyClass= 0;
    memset( ttot->ttotPanose, '\0', 10 );
    ttot->ttotCharRange[0]= 0;
    ttot->ttotCharRange[1]= 0;
    ttot->ttotCharRange[2]= 0;
    ttot->ttotCharRange[3]= 0;
    memset( ttot->ttotVendID, '\0', 4+1 );
    ttot->ttotSelection= 0;
    ttot->ttotFirstCharIndex= 0;
    ttot->ttotLastCharIndex= 0;

    return;
    }

static void utilInitTrueTypePostTable(	TrueTypePostTable *	ttpt )
    {
    ttpt->ttptFormatUpper= 0;
    ttpt->ttptFormatLower= 0;
    ttpt->ttptItalicAngleUpper= 0;
    ttpt->ttptItalicAngleLower= 0;
    ttpt->ttptUnderlinePosition= 0;
    ttpt->ttptUnderlineThickness= 0;
    ttpt->ttptIsFixedPitch= 0;
    ttpt->ttptMinMemType42= 0;
    ttpt->ttptMaxMemType42= 0;
    ttpt->ttptMinMemType1= 0;
    ttpt->ttptMaxMemType1= 0;
    ttpt->ttptIndexCount= 0;
    ttpt->ttptIndices= (unsigned short *)0;
    ttpt->ttptStringCount= 0;
    ttpt->ttptStrings= (unsigned char **)0;
    }

static void utilCleanTrueTypePostTable(	TrueTypePostTable *	ttpt )
    {
    int		name;

    for ( name= 0; name < ttpt->ttptStringCount; name++ )
	{
	if  ( ttpt->ttptStrings[name] )
	    { free( ttpt->ttptStrings[name] );	}
	}

    if  ( ttpt->ttptIndices )
	{ free( ttpt->ttptIndices );	}
    if  ( ttpt->ttptStrings )
	{ free( ttpt->ttptStrings );	}
    }

static void utilInitTrueTypeKernSub(	TrueTypeKernSub *	ttks )
    {
    ttks->ttksVersion= 0;
    ttks->ttksLength= 0;
    ttks->ttksCoverage= 0;
    ttks->ttksPairCount= 0;
    ttks->ttksSearchRange= 0;
    ttks->ttksEntrySelector= 0;
    ttks->ttksRangeShift= 0;

    ttks->ttksPairs= (TrueTypeKernPair *)0;
    }

static void utilCleanTrueTypeKernSub(	TrueTypeKernSub *	ttks )
    {
    if  ( ttks->ttksPairs )
	{ free( ttks->ttksPairs );	}
    }

static void utilInitTrueTypeKernTable(	TrueTypeKernTable *	ttkt )
    {
    ttkt->ttktVersion= 0;
    ttkt->ttktKernSubCount= 0;
    ttkt->ttktKernSubs= (TrueTypeKernSub *)0;
    }

static void utilCleanTrueTypeKernTable(	TrueTypeKernTable *	ttkt )
    {
    int		i;

    for ( i= 0; i < ttkt->ttktKernSubCount; i++ )
	{ utilCleanTrueTypeKernSub( &(ttkt->ttktKernSubs[i]) ); }

    if  ( ttkt->ttktKernSubs )
	{ free( ttkt->ttktKernSubs );	}

    return;
    }

static void utilInitTrueTypeCmapRecord(	TrueTypeCmapRecord * ttcr )
    {
    ttcr->ttcr6GlyphIndices= (unsigned short *)0;
    }

static void utilCleanTrueTypeCmapRecord(	TrueTypeCmapRecord * ttcr )
    {
    if  ( ttcr->ttcr6GlyphIndices )
	{ free( ttcr->ttcr6GlyphIndices );	}
    }

static void utilInitTrueTypeCmapTable(	TrueTypeCmapTable *	ttct )
    {
    ttct->ttctVersion= 0;
    ttct->ttctEncodingRecordCount= 0;
    ttct->ttctEncodingRecords= (TrueTypeCmapRecord *)0;
    }

static void utilCleanTrueTypeCmapTable(	TrueTypeCmapTable *	ttct )
    {
    int		i;

    for ( i= 0; i < ttct->ttctEncodingRecordCount; i++ )
	{
	utilCleanTrueTypeCmapRecord( &(ttct->ttctEncodingRecords[i]) );
	}

    if  ( ttct->ttctEncodingRecords )
	{ free( ttct->ttctEncodingRecords );	}
    }

static void utilInitTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr )
    {
    ttnr->ttnrPlatformID= 0;
    ttnr->ttnrEncodingID= 0;
    ttnr->ttnrLanguageID= 0;
    ttnr->ttnrNameID= 0;
    ttnr->ttnrLength= 0;
    ttnr->ttnrOffset= 0;

    ttnr->ttnrSavedBytes= (unsigned char *)0;

    return;
    }

static void utilCleanTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr )
    {
    if  ( ttnr->ttnrSavedBytes )
	{ free( ttnr->ttnrSavedBytes );	}

    return;
    }

static void utilInitTrueTypeNameTable(	TrueTypeNameTable *	ttnt )
    {
    ttnt->ttntFormat= 0;
    ttnt->ttntRecordCount= 0;
    ttnt->ttntOffset= 0;

    ttnt->ttntRecords= (TrueTypeNameRecord *)0;
    }

static void utilCleanTrueTypeNameTable(	TrueTypeNameTable *	ttnt )
    {
    int		rec;

    for ( rec= 0; rec < ttnt->ttntRecordCount; rec++ )
	{ utilCleanTrueTypeNameRecord( &(ttnt->ttntRecords[rec]) );	}

    if  ( ttnt->ttntRecords )
	{ free( ttnt->ttntRecords );	}
    }

static void utilInitTrueTypeTableEntry(	TrueTypeTableEntry *	ttte )
    {
    ttte->ttteTag[0]= '\0';
    ttte->ttteChecksum= 0;
    ttte->ttteOffset= 0;
    ttte->ttteLength= 0;
    ttte->ttteBytes= (unsigned char *)0;

    ttte->ttteSaveOffset= 0;

    return;
    }

static void utilCleanTrueTypeTableEntry(	TrueTypeTableEntry *	ttte )
    {
    if  ( ttte->ttteBytes )
	{ free( ttte->ttteBytes );	}
    }

static void utilInitTrueTypeFont(	TrueTypeFont *	ttf )
    {
    ttf->ttfVersion= 0;
    ttf->ttfTableCount= 0;
    ttf->ttfSearchRange= 0;
    ttf->ttfEntrySelector= 0;
    ttf->ttfRangeShift= 0;

    ttf->ttfTables= (TrueTypeTableEntry *)0;

    utilInitTrueTypeNameTable( &(ttf->ttfNameTable) );
    /* utilInitTrueTypePostTable( &(ttf->ttfPostTable) ); */
    /* utilInitTrueTypeHeadTable( &(ttf->ttfHeadTable) ); */
    /* utilInitTrueTypeHheaTable( &(ttf->ttfHheaTable) ); */
    utilInitTrueTypeCmapTable( &(ttf->ttfCmapTable) );
    utilInitTrueTypeKernTable( &(ttf->ttfKernTable) );
    utilInitTrueTypePostTable( &(ttf->ttfPostTable) );
    utilInitTrueTypeOS_2Table( &(ttf->ttfOS_2Table) );

    ttf->ttfLocations= (unsigned long *)0;
    ttf->ttfLocationCount= 0;

    ttf->ttfHorizontalMetrics= (HorizontalMetric *)0;
    ttf->ttfHorizontalMetricCount= 0;

    ttf->ttfGlyphs= (TrueTypeGlyph *)0;
    ttf->ttfGlyphCount= 0;

    return;
    }

static void utilCleanTrueTypeFont(	TrueTypeFont *	ttf )
    {
    int		ent;

    for ( ent= 0; ent < ttf->ttfTableCount; ent++ )
	{ utilCleanTrueTypeTableEntry( &(ttf->ttfTables[ent]) );	}

    if  ( ttf->ttfTables )
	{ free( ttf->ttfTables );	}

    utilCleanTrueTypeNameTable( &(ttf->ttfNameTable) );
    /* utilCleanTrueTypePostTable( &(ttf->ttfPostTable) ); */
    /* utilCleanTrueTypeHeadTable( &(ttf->ttfHeadTable) ); */
    /* utilCleanTrueTypeHheaTable( &(ttf->ttfHheaTable) ); */
    utilCleanTrueTypeCmapTable( &(ttf->ttfCmapTable) );
    utilCleanTrueTypeKernTable( &(ttf->ttfKernTable) );
    utilCleanTrueTypePostTable( &(ttf->ttfPostTable) );
    /* utilCleanTrueTypeOS_2Table( &(ttf->ttfOS_2Table) ); */

    if  ( ttf->ttfLocations )
	{ free( ttf->ttfLocations );	}

    if  ( ttf->ttfHorizontalMetrics )
	{ free( ttf->ttfHorizontalMetrics );	}

    if  ( ttf->ttfGlyphs )
	{ free( ttf->ttfGlyphs );	}

    return;
    }

static int utilTrueTypeFontCompareTables(	const void *	vttte1,
						const void *	vttte2 )
    {
    const TrueTypeTableEntry *	ttte1= (const TrueTypeTableEntry *)vttte1;
    const TrueTypeTableEntry *	ttte2= (const TrueTypeTableEntry *)vttte2;

    if  ( ttte1->ttteOffset > ttte2->ttteOffset )
	{ return  1;	}
    if  ( ttte1->ttteOffset < ttte2->ttteOffset )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Open a stream to one of the tables.					*/
/*									*/
/************************************************************************/

static SimpleInputStream * utilTtfOpenTableStream(
					MemoryBuffer *			mb,
					const TrueTypeTableEntry **	pTtte,
					const char *			tag,
					const TrueTypeFont *		ttf )
    {
    int				tab;
    const TrueTypeTableEntry *	ttte;
    SimpleInputStream *		sis;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ! strcmp( ttte->ttteTag, tag ) )
	    { break;	}
	}

    if  ( tab >= ttf->ttfTableCount )
	{ /*SLDEB(tag,ttf->ttfTableCount);*/ return (SimpleInputStream *)0; }

    mb->mbBytes= ttte->ttteBytes;
    mb->mbSize= ttte->ttteLength;

    sis= sioInMemoryOpen( mb );
    if  ( ! sis )
	{ XDEB(sis); return (SimpleInputStream *)0;	}

    *pTtte= ttte;
    return sis;
    }

/************************************************************************/
/*									*/
/*  Extract the name table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractNameTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    int				rec;
    MemoryBuffer		mb;
    SimpleInputStream *		sisName= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeNameTable *		ttnt= &(ttf->ttfNameTable);
    TrueTypeNameRecord *	ttnr;

    int				recordCount;

    utilInitMemoryBuffer( &mb );

    sisName= utilTtfOpenTableStream( &mb, &ttte, "name", ttf );
    if  ( ! sisName )
	{ XDEB(sisName); rval= -1; goto ready;	}

    ttnt->ttntFormat= sioEndianGetBeUint16( sisName );
    recordCount= sioEndianGetBeUint16( sisName );
    ttnt->ttntOffset= sioEndianGetBeUint16( sisName );

    ttnt->ttntRecords= malloc( recordCount* sizeof(TrueTypeNameRecord) );
    if  ( ! ttnt->ttntRecords )
	{ LXDEB(recordCount,ttnt->ttntRecords); rval= -1; goto ready; }

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < recordCount; ttnr++, rec++ )
	{ utilInitTrueTypeNameRecord( ttnr );	}
    ttnt->ttntRecordCount= recordCount;

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < recordCount; ttnr++, rec++ )
	{
	const unsigned char *	from;

	ttnr->ttnrPlatformID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrEncodingID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrLanguageID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrNameID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrLength= sioEndianGetBeUint16( sisName );
	ttnr->ttnrOffset= sioEndianGetBeUint16( sisName );

	from= ttte->ttteBytes+ ttnt->ttntOffset+ ttnr->ttnrOffset;

	ttnr->ttnrSavedBytes= malloc( ttnr->ttnrLength+ 1 );
	if  ( ! ttnr->ttnrSavedBytes )
	    {
	    LXDEB(ttnr->ttnrLength,ttnr->ttnrSavedBytes);
	    rval= -1; goto ready;
	    }

	memcpy( ttnr->ttnrSavedBytes, from, ttnr->ttnrLength );
	ttnr->ttnrSavedBytes[ttnr->ttnrLength]= '\0';

	/* LLSDEB(rec,ttnr->ttnrLength,(char *)ttnr->ttnrSavedBytes); */
	}

  ready:

    if  ( sisName )
	{ sioInClose( sisName );	}

    return rval;
    }

static int utilTtfExtractPostTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisPost= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;
    unsigned int		reserved;

    TrueTypePostTable *		ttpt= &(ttf->ttfPostTable);
    unsigned int		idxCount= 0;
    unsigned int		stringCount= 0;
    int				name;

    utilInitMemoryBuffer( &mb );

    sisPost= utilTtfOpenTableStream( &mb, &ttte, "post", ttf );
    if  ( ! sisPost )
	{ XDEB(sisPost); rval= -1; goto ready;	}

    ttpt->ttptFormatUpper= sioEndianGetBeInt16( sisPost );
    ttpt->ttptFormatLower= sioEndianGetBeUint16( sisPost );
    ttpt->ttptItalicAngleUpper= sioEndianGetBeInt16( sisPost );
    ttpt->ttptItalicAngleLower= sioEndianGetBeUint16( sisPost );
    ttpt->ttptUnderlinePosition= sioEndianGetBeInt16( sisPost );
    ttpt->ttptUnderlineThickness= sioEndianGetBeInt16( sisPost );
    ttpt->ttptIsFixedPitch= sioEndianGetBeUint16( sisPost );
    reserved= sioEndianGetBeUint16( sisPost );
    ttpt->ttptMinMemType42= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMaxMemType42= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMinMemType1= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMaxMemType1= sioEndianGetBeUint32( sisPost );

    switch( ttpt->ttptFormatUpper )
	{
	case 2:
	    if  ( ttpt->ttptFormatLower != 0 )
		{ LDEB(ttpt->ttptFormatLower); goto ready;	}

	    idxCount= sioEndianGetBeUint16( sisPost );
	    ttpt->ttptIndices= malloc( idxCount* sizeof(unsigned short) );
	    if  ( ! ttpt->ttptIndices )
		{ LXDEB( idxCount, ttpt->ttptIndices ); rval= -1; goto ready; }

	    for ( name= 0; name < idxCount; name++ )
		{ ttpt->ttptIndices[name]= 0; }
	    ttpt->ttptIndexCount= idxCount;

	    stringCount= 0;
	    for ( name= 0; name < idxCount; name++ )
		{
		ttpt->ttptIndices[name]= sioEndianGetBeUint16( sisPost );
		if  ( ttpt->ttptIndices[name] >= 258 )
		    { stringCount++;	}
		}

	    if  ( stringCount > 0 )
		{
		ttpt->ttptStrings= malloc( stringCount*
						    sizeof(unsigned char *) );
		if  ( ! ttpt->ttptStrings )
		    {
		    LXDEB(stringCount, ttpt->ttptStrings);
		    rval= -1; goto ready;
		    }

		for ( name= 0; name < stringCount; name++ )
		    { ttpt->ttptStrings[name]= (unsigned char *)0; }

		ttpt->ttptStringCount= stringCount;

		for ( name= 0; name < stringCount; name++ )
		    {
		    int	size= sioInGetCharacter( sisPost );

		    ttpt->ttptStrings[name]= malloc( size+ 1 );
		    if  ( ! ttpt->ttptStrings[name] )
			{
			LXDEB(size,ttpt->ttptStrings[name]);
			rval= -1; goto ready;
			}

		    if  ( sioInReadBytes( sisPost, ttpt->ttptStrings[name],
								size ) != size )
			{ LDEB(size); rval= -1; goto ready;	}
		    ttpt->ttptStrings[name][size]= '\0';
		    }
		}

	    break;

	case 3:
	    /* dummy post table: No glyph names */
	    break;

	default:
	    LLDEB(ttpt->ttptFormatUpper,ttpt->ttptFormatLower);
	    break;
	}

  ready:

    if  ( sisPost )
	{ sioInClose( sisPost );	}

    return rval;
    }

static int utilTtfExtractOS_2Table(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisOS_2= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeOS_2Table *		ttot= &(ttf->ttfOS_2Table);
    int				i;

    utilInitMemoryBuffer( &mb );

    sisOS_2= utilTtfOpenTableStream( &mb, &ttte, "OS/2", ttf );
    if  ( ! sisOS_2 )
	{ XDEB(sisOS_2); goto ready;	}

    ttot->ttotVersion= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotAvgCharWidth= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotWeightClass= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotWidthClass= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotType= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptXSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptYSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptXOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptYOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptXSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptYSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptXOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptYOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotStrikeoutSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotStrikeoutPosition= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotFamilyClass= sioEndianGetBeInt16( sisOS_2 );

    for ( i= 0; i < 10; i++ )
	{ ttot->ttotPanose[i]= sioInGetCharacter( sisOS_2 );	}

    ttot->ttotCharRange[0]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[1]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[2]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[3]= sioEndianGetBeUint32( sisOS_2 );

    for ( i= 0; i < 4; i++ )
	{ ttot->ttotVendID[i]= sioInGetCharacter( sisOS_2 );	}
    ttot->ttotVendID[i]= '\0';
    i= 3;
    while( i > 0 && ttot->ttotVendID[i] == ' ' )
	{ ttot->ttotVendID[i]= '\0'; i--; }

    ttot->ttotSelection= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotFirstCharIndex= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotLastCharIndex= sioEndianGetBeUint16( sisOS_2 );

  ready:

    if  ( sisOS_2 )
	{ sioInClose( sisOS_2 );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the Hhea table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractHheaTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisHhea= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    int				i;
    int				reserved;

    TrueTypeHheaTable *		hhea= &(ttf->ttfHheaTable);

    utilInitMemoryBuffer( &mb );

    sisHhea= utilTtfOpenTableStream( &mb, &ttte, "hhea", ttf );
    if  ( ! sisHhea )
	{ XDEB(sisHhea); rval= -1; goto ready;	}

    for ( i= 0; i < 4; i++ )
	{ hhea->hheaVersion[i]= sioInGetCharacter( sisHhea ); }
    hhea->hheaVersion[4]= '\0';

    hhea->hheaAscender= sioEndianGetBeInt16( sisHhea );
    hhea->hheaDescender= sioEndianGetBeInt16( sisHhea );
    hhea->hheaLineGap= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMaxAdvanceWidth= sioEndianGetBeUint16( sisHhea );
    hhea->hheaMinLsb= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMinRsb= sioEndianGetBeInt16( sisHhea );
    hhea->hheaXMaxExtent= sioEndianGetBeInt16( sisHhea );
    hhea->hheaCaretSlopeRise= sioEndianGetBeInt16( sisHhea );
    hhea->hheaCaretSlopeRun= sioEndianGetBeInt16( sisHhea );

    for ( i= 0; i < 5; i++ )
	{ reserved= sioEndianGetBeInt16( sisHhea );	}

    hhea->hheaMetricDataFormat= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMetricCount= sioEndianGetBeUint16( sisHhea );

  ready:

    if  ( sisHhea )
	{ sioInClose( sisHhea );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the head table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractHeadTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisHead= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeHeadTable *		ttht= &(ttf->ttfHeadTable);

    int				i;

    utilInitMemoryBuffer( &mb );

    sisHead= utilTtfOpenTableStream( &mb, &ttte, "head", ttf );
    if  ( ! sisHead )
	{ XDEB(sisHead); rval= -1; goto ready;	}

    ttht->tthtVersion= sioEndianGetBeUint32( sisHead );
    ttht->tthtFontRevision= sioEndianGetBeUint32( sisHead );
    ttht->tthtChecksumAdjust= sioEndianGetBeUint32( sisHead );
    ttht->tthtMagicNo= sioEndianGetBeUint32( sisHead );
    ttht->tthtFlags= sioEndianGetBeUint16( sisHead );
    ttht->tthtUnitsPerEm= sioEndianGetBeUint16( sisHead );

    for ( i= 0; i < 8; i++ )
	{ ttht->tthtCreated[i]= sioInGetCharacter( sisHead ); }
    ttht->tthtCreated[8]= '\0';

    for ( i= 0; i < 8; i++ )
	{ ttht->tthtModified[i]= sioInGetCharacter( sisHead ); }
    ttht->tthtModified[8]= '\0';

    ttht->tthtXMin= sioEndianGetBeInt16( sisHead );
    ttht->tthtYMin= sioEndianGetBeInt16( sisHead );
    ttht->tthtXMax= sioEndianGetBeInt16( sisHead );
    ttht->tthtYMax= sioEndianGetBeInt16( sisHead );

    ttht->tthtMacStyle= sioEndianGetBeUint16( sisHead );
    ttht->tthtLowestRecPPem= sioEndianGetBeUint16( sisHead );
    ttht->tthtFontDirection= sioEndianGetBeInt16( sisHead );
    ttht->tthtIndexToLocFormat= sioEndianGetBeInt16( sisHead );
    ttht->tthtGlyphDataFormat= sioEndianGetBeInt16( sisHead );

  ready:

    if  ( sisHead )
	{ sioInClose( sisHead );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the location table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractLocaTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisLoca= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeHeadTable *		ttht= &(ttf->ttfHeadTable);

    int				i;

    utilInitMemoryBuffer( &mb );

    sisLoca= utilTtfOpenTableStream( &mb, &ttte, "loca", ttf );
    if  ( ! sisLoca )
	{ XDEB(sisLoca); rval= -1; goto ready;	}

    switch( ttht->tthtIndexToLocFormat )
	{
	case 0:
	    if  ( ttte->ttteLength % 2 )
		{ LLDEB(ttte->ttteLength,2); rval= -1; goto ready; }
	    ttf->ttfLocationCount= ttte->ttteLength/ 2;
	    break;

	case 1:
	    if  ( ttte->ttteLength % 4 )
		{ LLDEB(ttte->ttteLength,4); rval= -1; goto ready; }
	    ttf->ttfLocationCount= ttte->ttteLength/ 4;
	    break;
	default:
	    LDEB(ttht->tthtIndexToLocFormat); rval= -1; goto ready;
	}

    ttf->ttfLocations= malloc( ttf->ttfLocationCount* sizeof(unsigned long) );
    if  ( ! ttf->ttfLocations )
	{
	LXDEB(ttf->ttfLocationCount,ttf->ttfLocations);
	rval= -1; goto ready;
	}

    switch( ttht->tthtIndexToLocFormat )
	{
	case 0:
	    for ( i= 0; i < ttf->ttfLocationCount; i++ )
		{ ttf->ttfLocations[i]= 2* sioEndianGetBeUint16( sisLoca ); }
	    break;

	case 1:
	    for ( i= 0; i < ttf->ttfLocationCount; i++ )
		{ ttf->ttfLocations[i]= sioEndianGetBeUint32( sisLoca ); }
	    break;

	default:
	    LDEB(ttht->tthtIndexToLocFormat); rval= -1; goto ready;
	}

  ready:

    if  ( sisLoca )
	{ sioInClose( sisLoca );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the horizontal metrics table from a TrueType file.		*/
/*									*/
/************************************************************************/

static int utilTtfExtractHmtxTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisHmtx= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    const TrueTypeHheaTable *	hhea= &(ttf->ttfHheaTable);

    int				i;
    int				partialCount;
    int				advanceWidth= 0;

    utilInitMemoryBuffer( &mb );

    sisHmtx= utilTtfOpenTableStream( &mb, &ttte, "hmtx", ttf );
    if  ( ! sisHmtx )
	{ XDEB(sisHmtx); rval= -1; goto ready;	}

    if  ( ttte->ttteLength % 2 )
	{
	LLDEB(ttte->ttteLength,2);
	LLDEB(hhea->hheaMetricDataFormat,hhea->hheaMetricCount);
	/*rval= -1; goto ready;*/
	}
    partialCount= ( ttte->ttteLength- 4 * hhea->hheaMetricCount )/ 2;

    ttf->ttfHorizontalMetricCount= hhea->hheaMetricCount+ partialCount;

    ttf->ttfHorizontalMetrics= malloc( ttf->ttfHorizontalMetricCount*
						    sizeof(HorizontalMetric) );
    if  ( ! ttf->ttfHorizontalMetrics )
	{
	LXDEB(ttf->ttfHorizontalMetricCount,ttf->ttfHorizontalMetrics);
	rval= -1; goto ready;
	}

    for ( i= 0; i < hhea->hheaMetricCount; i++ )
	{
	ttf->ttfHorizontalMetrics[i].hmAdvanceWidth= advanceWidth=
					    sioEndianGetBeUint16( sisHmtx );
	ttf->ttfHorizontalMetrics[i].hmLsb= sioEndianGetBeInt16( sisHmtx );
	}

    for ( i= hhea->hheaMetricCount; i < ttf->ttfHorizontalMetricCount; i++ )
	{
	ttf->ttfHorizontalMetrics[i].hmAdvanceWidth= advanceWidth;
	ttf->ttfHorizontalMetrics[i].hmLsb= sioEndianGetBeInt16( sisHmtx );
	}

  ready:

    if  ( sisHmtx )
	{ sioInClose( sisHmtx );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the glyph table from a TrueType file.			*/
/*									*/
/************************************************************************/

# define GLYFSZ				2000

# define ONOROFF			0x01
# define XSHORT				0x02
# define YSHORT				0x04
# define REPEAT				0x08
# define XSAME				0x10
# define YSAME				0x20

# define ARG_1_AND_2_ARE_WORDS		0x0001
# define ARGS_ARE_XY_VALUES 		0x0002
# define XY_BOUND_TO_GRID		0x0004
# define WE_HAVE_A_SCALE		0x0008
# define MORE_COMPONENTS		0x0020
# define WE_HAVE_AN_X_AND_Y_SCALE	0x0040
# define WE_HAVE_A_TWO_BY_TWO		0x0080
# define WE_HAVE_INSTRUCTIONS		0x0100
# define USE_MY_METRICS			0x0200

# define fg_rmoveto(a,x,y) appDebug( "fg_rmoveto( %g, %g )\n", x, y )
# define fg_rlineto(a,x,y) appDebug( "fg_rlineto( %g, %g )\n", x, y )
# define g_closepath(a) appDebug( "g_closepath()\n" )
# define fg_rrcurveto(a,x1,y1,x2,y2,x3,y3) appDebug( "fg_rrcurveto(%g,%g,%g,%g,%g,%g)\n", x1,y1,x2,y2,x3,y3 )

static int utilTtfTranslateSimpleGlyphData(
				    const TrueTypeFont *	ttf,
				    const AffineTransform2D *	at,
				    const TrueTypeGlyph *	ttg )
    {
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);
    double			fac= 1000.0/ ttht->tthtUnitsPerEm;
    const unsigned char *	data= ttg->ttgData;

    int				cc= ttg->ttgContourCount;

    unsigned char		flag[GLYFSZ];
    int				xrel[GLYFSZ];
    int				yrel[GLYFSZ];
    double			xc[GLYFSZ];
    double			yc[GLYFSZ];

    const unsigned char *	cep;
    const unsigned char *	p;
    int				lastP;
    int				nP;

    int				k;
    int				j;
    int				i;

    double			x;
    double			y;

    cep= data;
    lastP= utilEndianExtractBeInt16( cep+ 2* cc- 2  );
    nP= utilEndianExtractBeInt16( cep+ 2* cc  );
    p= cep+ 2* cc+ nP+ 2;

    j= k= 0;
    while( k <= lastP )
	{
	flag[k]= p[j];
	if  ( p[j] & REPEAT )
	    {
	    int	n= p[j+1];

	    while( n > 0 )
		{ flag[++k]= p[j]; n--; }
	    j++;
	    }
	j++; k++;
	}

    for ( k= 0; k <= lastP; k++ )
	{
	if  ( flag[k] & XSHORT )
	    {
	    if  ( flag[k] & XSAME )
		{ xrel[k] =  p[j++];	}
	    else{ xrel[k] = -p[j++];	}
	    }
	else{
	    if  ( flag[k] & XSAME )
		{ xrel[k]= 0;				}
	    else{ xrel[k]= utilEndianExtractBeInt16( p+ j ); j += 2;	}
	    }
	}

    for ( k= 0; k <= lastP; k++ )
	{
	if  ( flag[k] & YSHORT )
	    {
	    if  ( flag[k] & YSAME )
		{ yrel[k] =  p[j++];	}
	    else{ yrel[k] = -p[j++];	}
	    }
	else{
	    if  ( flag[k] & YSAME )
		{ yrel[k]= 0;				}
	    else{ yrel[k]= utilEndianExtractBeInt16( p+ j ); j += 2;	}
	    }
	}

    x= xrel[0];
    y= yrel[0];
    if  ( at )
	{
	xc[0]= fac* AT2_X( x, y, at );
	yc[0]= fac* AT2_Y( x, y, at );

	for ( k= 1; k <= lastP; k++ )
	    {
	    x += xrel[k];
	    y += yrel[k];

	    xc[k]= fac* AT2_X( x, y, at );
	    yc[k]= fac* AT2_Y( x, y, at );
	    }
	}
    else{
	xc[0]= fac* x;
	yc[0]= fac* y;

	for ( k= 1; k <= lastP; k++ )
	    {
	    x += xrel[k];
	    y += yrel[k];

	    xc[k]= fac* x;
	    yc[k]= fac* y;
	    }
	}

    i= j= 0;
    x= xc[0]; y= yc[0];
    while( i <= lastP )
	{
	int	contEnd= utilEndianExtractBeUint16( cep+ 2* j  );
	int	contStart= i;

	if  ( i <= contEnd )
	    {
	    x= xc[i]; y= yc[i];
	    fg_rmoveto( g, x, y );
	    i++;
	    }

	while( i <= contEnd )
	    {
	    int		cs;
	    int		ce;
	    int		nguide;

	    if  ( flag[i] & ONOROFF )
		{
		x= xc[i]; y= yc[i];
		fg_rlineto( g, x, y );
		i++; continue;
		}

	    cs= i- 1; nguide= 0;
	    for (;;)
		{
		if  ( i > contEnd )
		    { ce= contStart; break;	}
		if  ( flag[i] & ONOROFF )
		    { ce= i; break; }
		i++; nguide++;
		}

	    switch( nguide )
		{
		int		k1;

		case 0:
		    fg_rlineto(g, xc[ce], yc[ce]);

		    x = xc[ce]; y = yc[ce]; break;

		case 1:
		    fg_rrcurveto(g,
			      (xc[cs] + 2.0 * xc[cs + 1]) / 3.0,
			      (yc[cs] + 2.0 * yc[cs + 1]) / 3.0,
			      (2.0 * xc[cs + 1] + xc[ce]) / 3.0,
			      (2.0 * yc[cs + 1] + yc[ce]) / 3.0,
				    xc[ce], yc[ce] );

		    x = xc[ce]; y = yc[ce]; break;

		case 2:
		    fg_rrcurveto(g,
			     (-xc[cs] + 4.0 * xc[cs + 1]) / 3.0,
			     (-yc[cs] + 4.0 * yc[cs + 1]) / 3.0,
			      (4.0 * xc[cs + 2] - xc[ce]) / 3.0,
			      (4.0 * yc[cs + 2] - yc[ce]) / 3.0,
				    xc[ce], yc[ce] );

		    x = xc[ce]; y = yc[ce]; break;

		case 3:
		    fg_rrcurveto(g,
			      (xc[cs] + 2.0 * xc[cs + 1]) / 3.0,
			      (yc[cs] + 2.0 * yc[cs + 1]) / 3.0,
			  (5.0 * xc[cs + 1] + xc[cs + 2]) / 6.0,
			  (5.0 * yc[cs + 1] + yc[cs + 2]) / 6.0,
			      (xc[cs + 1] + xc[cs + 2]) / 2.0,
			      (yc[cs + 1] + yc[cs + 2]) / 2.0 );

		    fg_rrcurveto(g,
			  (xc[cs + 1] + 5.0 * xc[cs + 2]) / 6.0,
			  (yc[cs + 1] + 5.0 * yc[cs + 2]) / 6.0,
			  (5.0 * xc[cs + 2] + xc[cs + 3]) / 6.0,
			  (5.0 * yc[cs + 2] + yc[cs + 3]) / 6.0,
			      (xc[cs + 3] + xc[cs + 2]) / 2.0,
			      (yc[cs + 3] + yc[cs + 2]) / 2.0 );

		    fg_rrcurveto(g,
			  (xc[cs + 2] + 5.0 * xc[cs + 3]) / 6.0,
			  (yc[cs + 2] + 5.0 * yc[cs + 3]) / 6.0,
			      (2.0 * xc[cs + 3] + xc[ce]) / 3.0,
			      (2.0 * yc[cs + 3] + yc[ce]) / 3.0,
				    xc[ce], yc[ce] );

		    y = yc[ce]; x = xc[ce]; break;

		default:
		    k1 = cs + nguide;
		    fg_rrcurveto(g,
			      (xc[cs] + 2.0 * xc[cs + 1]) / 3.0,
			      (yc[cs] + 2.0 * yc[cs + 1]) / 3.0,
			  (5.0 * xc[cs + 1] + xc[cs + 2]) / 6.0,
			  (5.0 * yc[cs + 1] + yc[cs + 2]) / 6.0,
			      (xc[cs + 1] + xc[cs + 2]) / 2.0,
			      (yc[cs + 1] + yc[cs + 2]) / 2.0 );

		    for (k = cs + 2; k <= k1 - 1; k++)
			{
			fg_rrcurveto(g,
				(xc[k - 1] + 5.0 * xc[k]) / 6.0,
				(yc[k - 1] + 5.0 * yc[k]) / 6.0,
				(5.0 * xc[k] + xc[k + 1]) / 6.0,
				(5.0 * yc[k] + yc[k + 1]) / 6.0,
				    (xc[k] + xc[k + 1]) / 2.0,
				    (yc[k] + yc[k + 1]) / 2.0 );

			}

		    fg_rrcurveto(g,
			      (xc[k1 - 1] + 5.0 * xc[k1]) / 6.0,
			      (yc[k1 - 1] + 5.0 * yc[k1]) / 6.0,
				  (2.0 * xc[k1] + xc[ce]) / 3.0,
				  (2.0 * yc[k1] + yc[ce]) / 3.0,
				    xc[ce], yc[ce] );

		    x = xc[ce]; y = yc[ce]; 
		    break;
		}
	    }

	g_closepath( g );
	i++; j++;
	}

    return 0;
    }

static double utilTtfMatrixElement(	const unsigned char *	data )
    {
    int		val= utilEndianExtractBeInt16( data );

    return ( val >> 14 )+ (( val & 0x3fff )/ 16384.0 );
    }

static int utilTtfTranslateGlyphData(
				const TrueTypeFont *		ttf,
				const AffineTransform2D *	at,
				const TrueTypeGlyph *		ttg )
    {
    const unsigned char *	data= ttg->ttgData;
    unsigned int		flags;

    if  ( ttg->ttgContourCount >= 0 )
	{
	utilTtfTranslateSimpleGlyphData( ttf, at, ttg );
	return 0;
	}

    do  {
	unsigned int		glyphno;
	int			arg1;
	int			arg2;

	AffineTransform2D	atHere;

	utilIdentityAffineTransform2D( &atHere );

	flags= utilEndianExtractBeUint16( data ); data += 2;
	glyphno= utilEndianExtractBeUint16( data ); data += 2;

	if  ( flags & ARG_1_AND_2_ARE_WORDS )
	    {
	    arg1= utilEndianExtractBeInt16( data ); data += 2;
	    arg2= utilEndianExtractBeInt16( data ); data += 2;
	    }
	else{
	    arg1= *((signed char *)data); data++;
	    arg2= *((signed char *)data); data++;
	    }

	if  ( flags & WE_HAVE_A_SCALE )
	    {
	    atHere.at2Axx= utilTtfMatrixElement( data ); data += 2;
	    atHere.at2Ayy= atHere.at2Axx;
	    }
	else{
	    if  ( flags & WE_HAVE_AN_X_AND_Y_SCALE )
		{
		atHere.at2Axx= utilTtfMatrixElement( data ); data += 2;
		atHere.at2Ayy= utilTtfMatrixElement( data ); data += 2;
		}
	    else{
		if  ( flags & WE_HAVE_A_TWO_BY_TWO )
		    {
		    atHere.at2Axx= utilTtfMatrixElement( data ); data += 2;
		    atHere.at2Ayx= utilTtfMatrixElement( data ); data += 2;
		    atHere.at2Axy= utilTtfMatrixElement( data ); data += 2;
		    atHere.at2Ayy= utilTtfMatrixElement( data ); data += 2;
		    }
		}
	    }

	if  ( glyphno >= ttf->ttfGlyphCount )
	    { LLDEB(glyphno,ttf->ttfGlyphCount); return -1; }

	if  ( fabs( atHere.at2Axx ) > fabs( atHere.at2Axy ) )
	    { atHere.at2Tx= fabs( atHere.at2Axx );	}
	else{ atHere.at2Tx= fabs( atHere.at2Axy );	}
	if  ( fabs( fabs( atHere.at2Axx )- fabs( atHere.at2Ayx ) )
							<= 33.0/ 65536.0 )
	    { atHere.at2Tx *= 2.0;	}

	if  ( fabs( atHere.at2Ayx ) > fabs( atHere.at2Ayy ) )
	    { atHere.at2Ty= fabs( atHere.at2Ayx );	}
	else{ atHere.at2Ty= fabs( atHere.at2Ayy );	}
	if  ( fabs( fabs( atHere.at2Ayx )- fabs( atHere.at2Ayy ) )
							<= 33.0/ 65536.0 )
	    { atHere.at2Ty *= 2.0;	}

	if  ( flags & ARGS_ARE_XY_VALUES )
	    {
	    atHere.at2Tx *= arg1;
	    atHere.at2Ty *= arg2;
	    }
	else{
	    XXDEB(flags,ARGS_ARE_XY_VALUES);
	    }

	if  ( at )
	    { utilAffineTransform2DProduct( &atHere, at, &atHere );	}

	utilTtfTranslateGlyphData( ttf, &atHere, ttf->ttfGlyphs+ glyphno );

	} while ( flags & MORE_COMPONENTS );

    return 0;
    }

static int utilTtfTranslateGlyphs(	const TrueTypeFont *	ttf )
    {
    int				glyphno;
    TrueTypeGlyph *		ttg;

    ttg= ttf->ttfGlyphs;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount; ttg++, glyphno++ )
	{
	utilTtfTranslateGlyphData( ttf, (const AffineTransform2D *)0, ttg );
	}

    return 0;
    }

static int utilTtfExtractGlyfTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    int				tab;
    const TrueTypeTableEntry *	ttte;
    TrueTypeGlyph *		ttg;

    int				glyphno;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ! strcmp( ttte->ttteTag, "glyf" ) )
	    { break;	}
	}

    if  ( tab >= ttf->ttfTableCount )
	{ LLDEB(tab,ttf->ttfTableCount); rval= -1; goto ready;	}

    ttf->ttfGlyphCount= ttf->ttfLocationCount- 1;

    ttf->ttfGlyphs= malloc( ttf->ttfGlyphCount* sizeof(TrueTypeGlyph) );
    if  ( ! ttf->ttfGlyphs )
	{ LXDEB(ttf->ttfGlyphCount,ttf->ttfGlyphs); rval= -1; goto ready; }

    ttg= ttf->ttfGlyphs;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount; ttg++, glyphno++ )
	{
	const unsigned char *	data;

	data= ttte->ttteBytes+ ttf->ttfLocations[glyphno];

	/* signed ! */
	ttg->ttgContourCount= utilEndianExtractBeInt16( data ); data += 2;

	ttg->ttgXMin= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgYMin= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgXMax= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgYMax= utilEndianExtractBeInt16( data ); data += 2;

	ttg->ttgData= data;
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the encoding table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractCmapTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisCmap= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;
    TrueTypeCmapTable *		ttct= &(ttf->ttfCmapTable);
    TrueTypeCmapRecord *	ttcr;

    int				i;

    utilInitMemoryBuffer( &mb );

    sisCmap= utilTtfOpenTableStream( &mb, &ttte, "cmap", ttf );
    if  ( ! sisCmap )
	{ XDEB(sisCmap); rval= -1; goto ready;	}

    ttct->ttctVersion= sioEndianGetBeUint16( sisCmap );
    ttct->ttctEncodingRecordCount= sioEndianGetBeUint16( sisCmap );

    ttct->ttctEncodingRecords= malloc( ttct->ttctEncodingRecordCount*
						sizeof(TrueTypeCmapRecord) );
    if  ( ! ttct->ttctEncodingRecords )
	{
	LXDEB(ttct->ttctEncodingRecordCount,ttct->ttctEncodingRecords);
	rval= -1; goto ready;
	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{ utilInitTrueTypeCmapRecord( ttcr );	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	const unsigned char *	p;
	int			ent;

	ttcr->ttcrPlatformID= sioEndianGetBeUint16( sisCmap );
	ttcr->ttcrEncodingID= sioEndianGetBeUint16( sisCmap );
	ttcr->ttcrOffset= sioEndianGetBeUint32( sisCmap );

	p= ttte->ttteBytes+ ttcr->ttcrOffset;
	ttcr->ttcrFormat= utilEndianExtractBeUint16( p ); p += 2;
	ttcr->ttcrLength= utilEndianExtractBeUint16( p ); p += 2;
	ttcr->ttcrLanguage= utilEndianExtractBeUint16( p ); p += 2;

	switch( ttcr->ttcrFormat )
	    {
	    case 0:
		memcpy( ttcr->ttcr0GlyphIDArray, p, ttcr->ttcrLength- 6 );
		break;

	    case 4:
		ttcr->ttcr4SegCountX2= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4SearchRange= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4EntrySelector= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4RangeShift= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4Data= p;
		break;

	    case 6:
		ttcr->ttcr6FirstCode= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr6EntryCount= utilEndianExtractBeUint16( p ); p += 2;

		ttcr->ttcr6GlyphIndices=
			malloc( ttcr->ttcr6EntryCount* sizeof(unsigned int) );
		if  ( ! ttcr->ttcr6GlyphIndices )
		    {
		    LXDEB(ttcr->ttcr6EntryCount,ttcr->ttcr6GlyphIndices);
		    rval= -1; goto ready;
		    }
		for ( ent= 0; ent < ttcr->ttcr6EntryCount; ent++ )
		    {
		    ttcr->ttcr6GlyphIndices[ent]=
					utilEndianExtractBeUint16( p ); p += 2;
		    }
		break;

	    default:
		LDEB(ttcr->ttcrFormat);
		break;
	    }
	}

  ready:

    if  ( sisCmap )
	{ sioInClose( sisCmap );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the kerning table from a TrueType file.			*/
/*									*/
/*  1)  The kerning table is optional, so it is not an error if it	*/
/*	cannot be opened.						*/
/*									*/
/************************************************************************/

static int ttfExtractFormat0KernPairs(	const TrueTypeKernSub *	ttks,
					const unsigned char *	p )
    {
    int				pair;
    TrueTypeKernPair *		ttkp;

    ttkp= ttks->ttksPairs;
    for ( pair= 0; pair < ttks->ttksPairCount; ttkp++, pair++ )
	{
	ttkp->ttkpLeft= utilEndianExtractBeUint16( p ); p += 2;
	ttkp->ttkpRight= utilEndianExtractBeUint16( p ); p += 2;
	ttkp->ttkpValue= utilEndianExtractBeInt16( p ); p += 2;
	}

    return 0;
    }

static int utilTtfExtractKernTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    MemoryBuffer		mb;
    SimpleInputStream *		sisKern= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;
    TrueTypeKernTable *		ttkt= &(ttf->ttfKernTable);
    TrueTypeKernSub *		ttks;

    int				sub;
    int				subCount;
    const unsigned char *	data;

    utilInitMemoryBuffer( &mb );

    /*  1  */
    sisKern= utilTtfOpenTableStream( &mb, &ttte, "kern", ttf );
    if  ( ! sisKern )
	{ rval= 0; goto ready;	}

    ttkt->ttktVersion= sioEndianGetBeUint16( sisKern );
    subCount= sioEndianGetBeUint16( sisKern );

    ttkt->ttktKernSubs= malloc( subCount* sizeof(TrueTypeKernSub) );
    if  ( ! ttkt->ttktKernSubs )
	{
	LXDEB(subCount,ttkt->ttktKernSubs);
	rval= -1; goto ready;
	}

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < subCount; ttks++, sub++ )
	{ utilInitTrueTypeKernSub( ttks );	}

    ttkt->ttktKernSubCount= subCount;

    data= ttte->ttteBytes+ 4;

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < subCount; ttks++, sub++ )
	{
	const unsigned char *	p= data;
	unsigned int		format;

	ttks->ttksVersion= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksLength= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksCoverage= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksPairCount= utilEndianExtractBeUint16( p ); p += 2;

	if  ( ttks->ttksPairCount > 0 )
	    {
	    ttks->ttksPairs= malloc( ttks->ttksPairCount*
						sizeof(TrueTypeKernPair) );
	    if  ( ! ttks->ttksPairs )
		{
		LXDEB(ttks->ttksPairCount,ttks->ttksPairs);
		ttks->ttksPairCount= 0;
		rval= -1; goto ready;
		}

	    format= ttks->ttksCoverage & 0x00ff;
	    switch( format )
		{
		case 0:
		case 1: /* against doc, as in FreeType library */
		    ttks->ttksSearchRange=
				utilEndianExtractBeUint16( p ); p += 2;
		    ttks->ttksEntrySelector=
				utilEndianExtractBeUint16( p ); p += 2;
		    ttks->ttksRangeShift=
				utilEndianExtractBeUint16( p ); p += 2;

		    if  ( ttfExtractFormat0KernPairs( ttks, p ) )
			{
			ttks->ttksPairCount= 0;
			LDEB(format); rval= -1; goto ready;
			}
		    break;

#		if 0
		case 1:
		LLDEB(sub,subCount);
		    {
		    const unsigned char *	stateTable= p;
		    const unsigned char *	classTable;

		    int stateClassCount;
		    int stateClassOffset;
		    int stateArrayOffset;
		    int entryTableOffset;
		    int valueOffset;

		    stateClassCount=
				utilEndianExtractBeUint16( p ); p += 2;
		    stateClassOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    stateArrayOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    entryTableOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    valueOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    }
		    break;
#		endif

		default:
		    XLDEB(ttks->ttksCoverage,ttks->ttksPairCount);
		    ttks->ttksPairCount= 0;
		    break;
		}
	    }

	data += ttks->ttksLength;
	}

  ready:

    if  ( sisKern )
	{ sioInClose( sisKern );	}

    return rval;
    }



/************************************************************************/
/*									*/
/*  Try to find names for a format4 unicode cmap.			*/
/*									*/
/************************************************************************/

static int utilTtfSetUnicodeCmapFormat4Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    const unsigned char *	p= ttcr->ttcr4Data;

    int				n= ttcr->ttcr4SegCountX2;
    const unsigned char *	e= p+ 0;
    const unsigned char *	s= p+ 1* n+ 2;
    const unsigned char *	d= p+ 2* n+ 2;
    const unsigned char *	r= p+ 3* n+ 2;

    int				seg;
    int				segCount= n/ 2- 1;

    if  ( ! p )
	{ XDEB(p); return -1; }

    for ( seg= 0; seg < segCount; seg++ )
	{
	int	ss;
	int	ee;
	int	dd;
	int	rr;

	int	v;

	ss= utilEndianExtractBeUint16( s ); s += 2;
	ee= utilEndianExtractBeUint16( e ); e += 2;
	dd= utilEndianExtractBeInt16( d ); d += 2;
	rr= utilEndianExtractBeUint16( r ); r += 2;

	for ( v= ss; v <= ee; v++ )
	    {
	    int			g;
	    const char *	name;
	    char *		saved;

	    if  ( rr > 0 )
		{
		int o= rr/ 2+ ( v- ss );
		g= utilEndianExtractBeUint16( r+ 2* o- 2 );
		}
	    else{
		g= ( v+ dd ) % 65536;
		}

	    if  ( g < 0 || g >= ttf->ttfGlyphCount )
		{ LLDEB(g,ttf->ttfGlyphCount); return -1; }

	    if  ( afi->afiMetrics[g].acmN )
		{ /*SDEB(afi->afiMetrics[g].acmN);*/ continue;	}

	    name= psUnicodeToGlyphName( v );
	    if  ( ! name )
		{ XDEB(name); return -1;	}
	    saved= strdup( name );
	    if  ( ! saved )
		{ XDEB(saved); return -1;	}
	    afi->afiMetrics[g].acmN= saved;
	    }
	}

    return 0;
    }

static int utilTtfSetWindowsCmapFormat4Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    const unsigned char *	p= ttcr->ttcr4Data;

    int				n= ttcr->ttcr4SegCountX2;
    const unsigned char *	e= p+ 0;
    const unsigned char *	s= p+ 1* n+ 2;
    const unsigned char *	d= p+ 2* n+ 2;
    const unsigned char *	r= p+ 3* n+ 2;

    int				seg;
    int				segCount= n/ 2- 1;

    if  ( ! p )
	{ XDEB(p); return -1; }

    for ( seg= 0; seg < segCount; seg++ )
	{
	int	ss;
	int	ee;
	int	dd;
	int	rr;

	int	v;

	ss= utilEndianExtractBeUint16( s ); s += 2;
	ee= utilEndianExtractBeUint16( e ); e += 2;
	dd= utilEndianExtractBeInt16( d ); d += 2;
	rr= utilEndianExtractBeUint16( r ); r += 2;

	for ( v= ss; v <= ee; v++ )
	    {
	    int			c;
	    int			g;
	    char *		saved;
	    char		name[25];

	    if  ( rr > 0 )
		{
		int o= rr/ 2+ ( v- ss );
		g= utilEndianExtractBeUint16( r+ 2* o- 2 );
		}
	    else{
		g= ( v+ dd ) % 65536;
		}

	    c= v % 256;

	    if  ( g < 0 || g >= ttf->ttfGlyphCount )
		{ LLDEB(g,ttf->ttfGlyphCount); return -1; }

	    if  ( afi->afiMetrics[g].acmN )
		{
		/* NO!
		if  ( strcmp( afi->afiMetrics[g].acmN, ".null" )	&&
		      strcmp( afi->afiMetrics[g].acmN, ".notdef" )	)
		    { LLSDEB(c,g,afi->afiMetrics[g].acmN); continue;	}
		*/
		continue;
		}

	    sprintf( name, "win%04x", v );
	    saved= strdup( name );
	    if  ( ! saved )
		{ XDEB(saved); return -1;	}
	    afi->afiMetrics[g].acmN= saved;
	    afi->afiMetrics[g].acmC= c;
	    afi->afiDefaultCodeToGlyph[c]= g;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to set glyph names from the names found in the post table.	*/
/*									*/
/************************************************************************/

static int utilTtfSetPostGlyphNames(
				const TrueTypeFont *		ttf,
				AfmCharMetric *			acm )
    {
    const TrueTypePostTable *	ttpt= &(ttf->ttfPostTable);

    if  ( ttpt->ttptFormatUpper == 2 && ttpt->ttptFormatLower == 0 )
	{
	int			glyphno;
	TrueTypeGlyph *		ttg;

	ttg= ttf->ttfGlyphs;
	for ( glyphno= 0; glyphno < ttf->ttfGlyphCount;
						acm++, ttg++, glyphno++ )
	    {
	    int		idx;

	    if  ( acm->acmN )
		{ /* SDEB(acm->acmN); */ continue; }

	    if  ( glyphno >= ttpt->ttptIndexCount )
		{ LLDEB(glyphno,ttpt->ttptIndexCount); break; }

	    idx= ttpt->ttptIndices[glyphno];
	    if  ( idx < 258 )
		{
		acm->acmN= strdup( psAppleTtfGlyphNames[idx] );
		if  ( ! acm->acmN )
		    { XDEB(acm->acmN); return -1; }
		continue;
		}

	    idx -= 258;
	    if  ( idx >= ttpt->ttptStringCount )
		{ LLDEB(idx,ttpt->ttptStringCount); return -1; }

	    acm->acmN= strdup( (char *)ttpt->ttptStrings[idx] );
	    if  ( ! acm->acmN )
		{ XDEB(acm->acmN); return -1; }
	    }

	return 0;
	}

    /*  post table has no glyph names */
    if  ( ttpt->ttptFormatUpper == 3 && ttpt->ttptFormatLower == 0 )
	{ return 0;	}

    LLDEB(ttpt->ttptFormatUpper,ttpt->ttptFormatLower);
    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to find names for the glyphs in the font.			*/
/*									*/
/*  1)  Look for unicode encoding.					*/
/*  2)  Requirement per Appendix B of the ttf specification. (1)	*/
/*  3)  Requirement per Appendix B of the ttf specification. (2)	*/
/*									*/
/************************************************************************/

static int utilTtfSetCharNames(	const TrueTypeFont *	ttf,
				AfmFontInfo *		afi )
    {
    const TrueTypeCmapTable *	ttct= &(ttf->ttfCmapTable);
    const TrueTypeCmapRecord *	ttcr;

    int				i;

    /*  2  */
    afi->afiMetrics[0].acmN= strdup( ".notdef" );
    if  ( ! afi->afiMetrics[0].acmN )
	{ XDEB(afi->afiMetrics[0].acmN); return -1;	}
    /*  3  */
    afi->afiMetrics[1].acmN= strdup( ".null" );
    if  ( ! afi->afiMetrics[1].acmN )
	{ XDEB(afi->afiMetrics[1].acmN); return -1;	}

    /*  1  */
    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	if  ( ttcr->ttcrPlatformID == 0		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	if  ( ttcr->ttcrPlatformID == 3		&&
	      ttcr->ttcrEncodingID == 1		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	}

    if  ( i < ttct->ttctEncodingRecordCount )
	{
	if  ( utilTtfSetUnicodeCmapFormat4Names( ttf, ttcr, afi ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	if  ( ttcr->ttcrPlatformID == 3		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	}

    if  ( i < ttct->ttctEncodingRecordCount )
	{
	if  ( utilTtfSetWindowsCmapFormat4Names( ttf, ttcr, afi ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{ LLLDEB(ttcr->ttcrPlatformID,ttcr->ttcrEncodingID,ttcr->ttcrFormat); }

    LDEB(ttct->ttctEncodingRecordCount); return -1;
    }

/************************************************************************/
/*									*/
/*  Write an afm metric file.						*/
/*									*/
/************************************************************************/

static int utilTtfGetName(	const TrueTypeNameTable * 	ttnt,
				int				nameID )
    {
    int				found= -1;
    int				rec;
    const TrueTypeNameRecord *	ttnr;
    const TrueTypeNameRecord *	ttnrFound= (const TrueTypeNameRecord *)0;

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < ttnt->ttntRecordCount; ttnr++, rec++ )
	{
	if  ( ttnr->ttnrNameID != nameID )
	    { continue; }

	if  ( found < 0 )
	    { found= rec; ttnrFound= ttnr; break;	}

	/*
	LDEB(ttnr->ttnrPlatformID);
	LDEB(ttnr->ttnrEncodingID);
	LDEB(ttnr->ttnrLanguageID);
	SDEB((char *)ttnr->ttnrSavedBytes);
	*/
	}

    return found;
    }

static int utilTtfScaledInteger(	const TrueTypeFont *	ttf,
					int			val )
    {
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);
    double			fac= 1000.0/ ttht->tthtUnitsPerEm;

    if  ( val > 0 )
	{ return fac* val+ 0.5;	}
    else{ return fac* val- 0.5;	}
    }

static void utilTtfSetCharMetrics(	const TrueTypeFont *	ttf,
					AfmCharMetric *		acm )
    {
    const TrueTypeGlyph *	ttg;
    const HorizontalMetric *	hm;

    int				glyphno;

    ttg= ttf->ttfGlyphs;
    hm= ttf->ttfHorizontalMetrics;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount;
					    acm++, ttg++, hm++, glyphno++ )
	{
	if  ( glyphno >= ttf->ttfHorizontalMetricCount )
	    {
	    LLDEB(glyphno,ttf->ttfHorizontalMetricCount);
	    acm->acmWX= utilTtfScaledInteger( ttf, ttg->ttgXMax );
	    }
	else{
	    acm->acmWX= utilTtfScaledInteger( ttf, hm->hmAdvanceWidth );
	    }

	acm->acmBBox.abbLeft= utilTtfScaledInteger( ttf, ttg->ttgXMin );
	acm->acmBBox.abbBottom= utilTtfScaledInteger( ttf, ttg->ttgYMin );
	acm->acmBBox.abbRight= utilTtfScaledInteger( ttf, ttg->ttgXMax );
	acm->acmBBox.abbTop= utilTtfScaledInteger( ttf, ttg->ttgYMax );
	}

    return;
    }

static int utilTtfSaveName(	char **				pName,
				const TrueTypeNameTable *	ttnt,
				int				which )
    {
    int				recFound;
    char *			name;
    const TrueTypeNameRecord *	ttnr;

    recFound= utilTtfGetName( ttnt, which );
    if  ( recFound < 0 )
	{ LLDEB(which,recFound); return -1;	}

    ttnr= ttnt->ttntRecords+ recFound;

    name= malloc( ttnr->ttnrLength+ 1 );
    if  ( ! name )
	{ LXDEB(ttnr->ttnrLength,name);	}

    if  ( utilTtfStringBytes( ttnr->ttnrPlatformID, ttnr->ttnrEncodingID )
									== 2 )
	{
	int		i;

	for ( i= 0; i < ttnr->ttnrLength/ 2; i++ )
	    { name[i]= ttnr->ttnrSavedBytes[2*i+ 1]; }
	name[i]= '\0';
	}
    else{
	memcpy( name, ttnr->ttnrSavedBytes, ttnr->ttnrLength );
	name[ttnr->ttnrLength]= '\0';
	}

    if  ( *pName )
	{ free( *pName );	}

    *pName= name;

    while( *name )
	{
	if  ( *name == '\n' )
	    { *name= ' ';	}

	name++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract kerning pairs from the true type font.			*/
/*									*/
/************************************************************************/

static int utilTtfGetKernPairs(		AfmFontInfo *		afi,
					const TrueTypeFont *	ttf )
    {
    AfmCharMetric *		acm;

    int				k0;
    int				glyphno;

    const TrueTypeKernTable *	ttkt= &(ttf->ttfKernTable);

    int				sub;
    const TrueTypeKernSub *	ttks;

    const TrueTypeKernPair *	ttkp;

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < ttkt->ttktKernSubCount; ttks++, sub++ )
	{
	if  ( ttks->ttksPairCount > 0 )
	    { break;	}
	}

    if  ( sub >= ttkt->ttktKernSubCount )
	{ return 0;	}

    k0= 0; ttkp= ttks->ttksPairs;
    acm= afi->afiMetrics;
    for ( glyphno= 0; glyphno < afi->afiMetricCount; acm++, glyphno++ )
	{
	int		n;
	int		i;

	while( k0 < ttks->ttksPairCount		&&
	       ttkp->ttkpLeft < glyphno		)
	    { ttkp++; k0++;	}

	if  ( k0 >= ttks->ttksPairCount )
	    { break;	}
	if  ( ttkp->ttkpLeft > glyphno )
	    { continue;	}

	n= 1;
	while( k0+ n < ttks->ttksPairCount	&&
	       ttkp[n].ttkpLeft == glyphno	)
	    { n++;	}

	acm->acmKernPairs= malloc( n* sizeof( AfmKerningPair ) );
	if  ( ! acm->acmKernPairs )
	    { LXDEB(n,acm->acmKernPairs); return -1;	}

	for ( i= 0; i < n; i++ )
	    {
	    acm->acmKernPairs[i].akpPosition= ttkp[i].ttkpRight;
	    acm->acmKernPairs[i].akpXVec=
			    utilTtfScaledInteger( ttf, ttkp[i].ttkpValue );
	    }

	acm->acmKernPairCount= n;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract character metrics from the true type font.			*/
/*									*/
/************************************************************************/

static int utilTtfGetCharMetricsAndEncoding(
					AfmFontInfo *		afi,
					const TrueTypeFont *	ttf )
    {
    AfmCharMetric *		acm;
    int				enc;

    int				glyphno;

    const int			complain= 0;

    afi->afiMetrics= malloc( ttf->ttfGlyphCount* sizeof(AfmCharMetric) );
    if  ( ! afi->afiMetrics )
	{ LXDEB(ttf->ttfGlyphCount,afi->afiMetrics); return -1; }
    acm= afi->afiMetrics;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount; acm++, glyphno++ )
	{ psInitAfmCharMetric( acm ); }
    afi->afiMetricCount= ttf->ttfGlyphCount;

    utilTtfSetCharMetrics( ttf, afi->afiMetrics );
    utilTtfSetCharNames( ttf, afi );
    utilTtfSetPostGlyphNames( ttf, afi->afiMetrics );

    if  ( psGetFontEncodings( afi, complain ) < 0 )
	{ LDEB(1); return -1;	}

    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	{
	if  ( afi->afiSupportedCharsets[enc].scSupported )
	    { break;	}
	}

    if  ( enc >= ENCODINGps_COUNT )
	{
	int		encodedGlyphs= 0;

	acm= afi->afiMetrics;
	for ( glyphno= 0; glyphno < afi->afiMetricCount; acm++, glyphno++ )
	    {
	    int		code= afi->afiMetrics[glyphno].acmC;

	    if  ( code >= 0 && code < 256 )
		{
		encodedGlyphs++;
		afi->afiDefaultCodeToGlyph[code]= glyphno;
		}
	    }

	if  ( encodedGlyphs < 3 )
	    {
	    acm= afi->afiMetrics;
	    for ( glyphno= 0; glyphno < afi->afiMetricCount; acm++, glyphno++ )
		{
		if  ( glyphno >= 256 )
		    { break;	}

		afi->afiMetrics[glyphno].acmC= glyphno;
		}
	    }
	}
    else{
	int	code;

	for ( code= 0; code < 256; code++ )
	    {
	    glyphno= afi->afiSupportedCharsets[enc].scCodeToGlyphMapping[code];

	    afi->afiDefaultCodeToGlyph[code]= glyphno;

	    if  ( glyphno >= 0 )
		{
		acm= afi->afiMetrics+ glyphno;
		if  ( acm->acmC < 0 )
		    { acm->acmC= code;	}
		}
	    }
	}

    if  ( enc == ENCODINGpsISO_8859_1 )
	{ afi->afiEncodingScheme= strdup( "ISOLatin1Encoding" );	}
    else{ afi->afiEncodingScheme= strdup( "FontSpecific" );	}

    afi->afiXHeight= 0;
    afi->afiCapHeight= 0;
    acm= afi->afiMetrics;
    for ( glyphno= 0; glyphno < afi->afiMetricCount; acm++, glyphno++ )
	{
	if  ( ! acm->acmN )
	    { continue;	}
	if  ( ! strcmp( acm->acmN, "x" ) )
	    { afi->afiXHeight= acm->acmBBox.abbTop;	}
	if  ( ! strcmp( acm->acmN, "H" ) )
	    { afi->afiCapHeight= acm->acmBBox.abbTop;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to derive complete font information from a true type font.	*/
/*									*/
/************************************************************************/

static int utilTtfFontInfo(	AfmFontInfo *		afi,
				const TrueTypeFont *	ttf )
    {
    const TrueTypeNameTable *	ttnt= &(ttf->ttfNameTable);
    const TrueTypePostTable *	ttpt= &(ttf->ttfPostTable);
    const TrueTypeHheaTable *	hhea= &(ttf->ttfHheaTable);
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);

    int				recFound;

    char *			str;
    int				start;
    int				length;
    int				weight;
    int				width;

    if  ( utilTtfGetCharMetricsAndEncoding( afi, ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfGetKernPairs( afi, ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfSaveName( &(afi->afiFontName), ttnt, 6 ) )
	{ LDEB(6); return -1;	}

    if  ( utilTtfSaveName( &(afi->afiFullName), ttnt, 4 ) )
	{ LDEB(4); return -1;	}

    if  ( utilTtfSaveName( &(afi->afiNotice), ttnt, 0 ) )
	{ LDEB(0); return -1;	}

    if  ( utilTtfSaveName( &(afi->afiFamilyName), ttnt, 1 ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfSaveName( &(afi->afiVersion), ttnt, 5 ) )
	{ LDEB(5); return -1;	}

    recFound= utilTtfGetName( ttnt, 2 );
    if  ( recFound < 0 )
	{ LLDEB(2,recFound); return -1; }
    str= (char *)ttnt->ttntRecords[recFound].ttnrSavedBytes;
    if  ( utilFontWeightFromString( &weight, &start, &length, str ) )
	{
	afi->afiWeight= malloc( length+ 1 );
	if  ( ! afi->afiWeight )
	    { XDEB(afi->afiWeight); return -1;	}
	strncpy( afi->afiWeight, str+ start, length )[length]= '\0';
	}
    if  ( utilFontWidthFromString( &width, &start, &length, str ) )
	{
	afi->afiWidth= malloc( length+ 1 );
	if  ( ! afi->afiWidth )
	    { XDEB(afi->afiWidth); return -1;	}
	strncpy( afi->afiWidth, str+ start, length )[length]= '\0';
	}

    if  ( ! afi->afiWeight )
	{
	str= afi->afiFullName;
	if  ( utilFontWeightFromString( &weight, &start, &length, str ) )
	    {
	    afi->afiWeight= malloc( length+ 1 );
	    if  ( ! afi->afiWeight )
		{ XDEB(afi->afiWeight); return -1;	}
	    strncpy( afi->afiWeight, str+ start, length )[length]= '\0';
	    }
	}

    if  ( ! afi->afiWeight )
	{
	str= afi->afiFullName;
	if  ( utilFontWidthFromString( &width, &start, &length, str ) )
	    {
	    afi->afiWidth= malloc( length+ 1 );
	    if  ( ! afi->afiWidth )
		{ XDEB(afi->afiWidth); return -1;	}
	    strncpy( afi->afiWidth, str+ start, length )[length]= '\0';
	    }
	}

    if  ( ttpt->ttptItalicAngleUpper >= 0 )
	{
	afi->afiItalicAngle= 
		ttpt->ttptItalicAngleUpper+
		ttpt->ttptItalicAngleLower/ 65536.0;
	}
    else{
	afi->afiItalicAngle= 
		ttpt->ttptItalicAngleUpper-
		ttpt->ttptItalicAngleLower/ 65536.0;
	}

    if  ( afi->afiItalicAngle > -0.05 && afi->afiItalicAngle < 0.05 )
	{ afi->afiItalicAngle= 0.0;	}
    afi->afiTanItalicAngle= tan( afi->afiItalicAngle );

    afi->afiIsFixedPitch= ttpt->ttptIsFixedPitch != 0;

    afi->afiFontBBox.abbLeft= utilTtfScaledInteger( ttf, ttht->tthtXMin );
    afi->afiFontBBox.abbBottom= utilTtfScaledInteger( ttf, ttht->tthtYMin );
    afi->afiFontBBox.abbRight= utilTtfScaledInteger( ttf, ttht->tthtXMax );
    afi->afiFontBBox.abbTop= utilTtfScaledInteger( ttf, ttht->tthtYMax );

    afi->afiUnderlinePosition=
		    utilTtfScaledInteger( ttf, ttpt->ttptUnderlinePosition );
    afi->afiUnderlineThickness=
		    utilTtfScaledInteger( ttf, ttpt->ttptUnderlineThickness );
    afi->afiAscender=
		    utilTtfScaledInteger( ttf, hhea->hheaAscender );
    afi->afiDescender=
		    utilTtfScaledInteger( ttf, hhea->hheaDescender );

    if  ( ttf->ttfOS_2Table.ttotVendID[0] )
	{
	afi->afiVendor= strdup( (char *)ttf->ttfOS_2Table.ttotVendID );
	if  ( ! afi->afiVendor )
	    { XDEB(afi->afiVendor); return -1;	}
	}

    return 0;
    }

static void utilTtfWriteCharStrings(	SimpleOutputStream *	sos,
					int			nchars,
					const AfmFontInfo *	afi )
    {
    int				i;
    const AfmCharMetric *	acm= afi->afiMetrics;

    sioOutPrintf( sos, "/CharStrings %d dict dup begin\n", nchars );

    acm= afi->afiMetrics;
    for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	{
	if  ( ! acm->acmN )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	sioOutPrintf( sos, "  /%s %d def\n", acm->acmN, i );
	}

    sioOutPrintf( sos, "end readonly def\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a selection of the true type file to the sfnts dictionary	*/
/*									*/
/*  1)  List of tables that might be used by the PostScript engine.	*/
/*	See paragraph 4.7 on p 12 of the T42 spec. [Techmote #5012].	*/
/*  2)  Determine new file offsets for the tables.			*/
/*									*/
/************************************************************************/

static long utilTtfWriteTableByteArray(	SimpleOutputStream *	sos,
					long			filePos,
					const char *		tag,
					const unsigned char *	bytes,
					unsigned long		count,
					int			pad )
    {
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;

    const int			wide= 72;
    const int			lastNL= 0;

    sioOutPrintf( sos, "%% %s %ld bytes @%lu\n", tag, count, filePos );

    sioOutPrintf( sos, "<\n" );
    sosHex= sioOutHexOpenFolded( sos, wide, lastNL );
    if  ( ! sosHex )
	{ XDEB(sosHex); filePos= -1; goto ready;	}

    if  ( sioOutWriteBytes( sosHex, bytes, count ) != count )
	{ LDEB(count); filePos= -1; goto ready;	}

    if  ( pad > 1 )
	{
	while( count % pad )
	    { sioOutPutCharacter( '\0', sosHex ); count++; }
	}

    filePos += count;

    sioOutPutCharacter( '\0', sosHex );
    sioOutClose( sosHex ); sosHex= (SimpleOutputStream *)0;
    sioOutPrintf( sos, ">\n" );

  ready:

    if  ( sosHex )
	{ sioOutClose( sosHex );	}

    return filePos;
    }

static int utilTtfWriteT42Sfnts(	SimpleOutputStream *	sos,
					const TrueTypeFont *	ttf )
    {
    int				rval= 0;
    int				tab;
    TrueTypeTableEntry *	ttte= ttf->ttfTables;
    unsigned long		offset;
    unsigned long		filePos;
    int				tableCount;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;

    const int			wide= 72;
    const int			lastNL= 0;

    /*  1  */
    static const char * const	sfntfTags[]=
	{
	"head", "hhea", "hmtx", "loca",
	"prep", "fpgm", "vhea", "glyf",
	"cvt ", "maxp", "vmtx",
	};

    offset= 12;
    tableCount= 0;

    /*  2  */
    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	int	i;

	for ( i= 0; i < sizeof(sfntfTags)/sizeof(char *); i++ )
	    {
	    if  ( ! strcmp( ttte->ttteTag, sfntfTags[i] ) )
		{ break;	}
	    }

	if  ( i >= sizeof(sfntfTags)/sizeof(char *) )
	    { ttte->ttteSaveOffset= 0;	}
	else{
	    ttte->ttteSaveOffset= offset;
	    tableCount++;
	    offset += ttte->ttteLength;
	    while( offset % 4 )
		{ offset++;	}
	    }
	}

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ttte->ttteSaveOffset > 0 )
	    { ttte->ttteSaveOffset += 16* tableCount;
	    }
	}

    filePos= 0;

    sioOutPrintf( sos, "/sfnts[\n" );

    sioOutPrintf( sos, "<\n" );
    sosHex= sioOutHexOpenFolded( sos, wide, lastNL );
    if  ( ! sosHex )
	{ XDEB(sosHex); rval= -1; goto ready;	}

    sioEndianPutBeUint32( ttf->ttfVersion, sosHex ); filePos += 4;
    sioEndianPutBeUint16( tableCount, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfSearchRange, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfEntrySelector, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfRangeShift, sosHex ); filePos += 2;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	int		b;

	if  ( ttte->ttteSaveOffset == 0 )
	    { continue;	}

	for ( b= 0; b < 4; b++ )
	    { sioOutPutCharacter( ttte->ttteTag[b], sosHex );	}
	filePos += 4;

	sioEndianPutBeUint32( ttte->ttteChecksum, sosHex ); filePos += 4;
	sioEndianPutBeUint32( ttte->ttteSaveOffset, sosHex ); filePos += 4;
	sioEndianPutBeUint32( ttte->ttteLength, sosHex ); filePos += 4;
	}

    sioOutPutCharacter( '\0', sosHex );
    sioOutClose( sosHex ); sosHex= (SimpleOutputStream *)0;
    sioOutPrintf( sos, ">\n" );

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ttte->ttteSaveOffset == 0 )
	    { continue;	}

	if  ( filePos != ttte->ttteSaveOffset )
	    { LLDEB(filePos,ttte->ttteSaveOffset); rval= -1; goto ready; }

	if  ( ttte->ttteLength > 65528 )
	    {
	    long	pos= 0;
	    int		locPos= 0;
	    long	upto= 65534;

	    if  ( strcmp( ttte->ttteTag, "glyf" ) )
		{
		SLDEB(ttte->ttteTag,ttte->ttteLength);
		rval= -1; goto ready;
		}

	    while( pos < ttte->ttteLength )
		{
		int		l, r, m;
		int		pad= 2;

		l= locPos; r= ttf->ttfLocationCount; m= ( l+ r )/ 2;

		while ( l < m )
		    {
		    if  ( ttf->ttfLocations[m] > upto )
			{ r= m;	}
		    else{ l= m; }

		    m= ( l+ r )/ 2;
		    }

		upto= ttf->ttfLocations[m];

		if  ( upto == ttte->ttteLength )
		    { pad= 4;	}

		filePos= utilTtfWriteTableByteArray( sos, filePos,
			ttte->ttteTag, ttte->ttteBytes+ pos, upto- pos, pad );

		if  ( filePos < 0 )
		    {
		    SLLDEB(ttte->ttteTag,ttte->ttteLength,filePos);
		    rval= -1; goto ready;
		    }

		pos= upto;
		upto += 65528;
		locPos= m;
		}
	    }
	else{
	    filePos= utilTtfWriteTableByteArray( sos, filePos, ttte->ttteTag,
				    ttte->ttteBytes, ttte->ttteLength, 4 );
	    if  ( filePos < 0 )
		{
		SLLDEB(ttte->ttteTag,ttte->ttteLength,filePos);
		rval= -1; goto ready;
		}
	    }
	}

    sioOutPrintf( sos, "]def\n" );

  ready:

    if  ( sosHex )
	{ sioOutClose( sosHex );	}

    return rval;
    }

static int utilTtfWriteT42(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi,
				const TrueTypeFont *	ttf )
    {
    int				i;
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);
    const AfmCharMetric *	acm;
    int				map[256];
    int				nchars;

    for ( i= 0; i < 256; i++ )
	{ map[i]= -1;	}

    nchars= 0;
    acm= afi->afiMetrics;
    for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	{
	if  ( ! acm->acmN )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	nchars++;

	if  ( acm->acmC < 0 || acm->acmC >= 256 )
	    { /*LDEB(acm->acmC);*/ continue;	}

	map[acm->acmC]= i;
	}

    sioOutPrintf( sos, "%%!PS-TrueTypeFont-%lu-%lu\n",
				    ttht->tthtVersion,
				    ttht->tthtFontRevision );
    sioOutPrintf( sos, "11 dict begin\n" );
    sioOutPrintf( sos, "  /FontName /%s def\n", afi->afiFontName );
    sioOutPrintf( sos, "  /Encoding 256 array\n" );
    sioOutPrintf( sos, "   0 1 255 { 1 index exch /.notdef put } for\n" );

    for ( i= 0; i < 256; i++ )
	{
	if  ( map[i] < 0 )
	    { continue;	}

	acm= afi->afiMetrics+ map[i];
	sioOutPrintf( sos, "   dup %d /%s put\n", acm->acmC, acm->acmN );
	}

    sioOutPrintf( sos, "  readonly def\n" );
    sioOutPrintf( sos, "  /PaintType 0 def\n" );
    sioOutPrintf( sos, "  /FontMatrix [1 0 0 1 0 0] def\n" );
    sioOutPrintf( sos, "  /FontBBox [%d %d %d %d] def\n",
					afi->afiFontBBox.abbLeft,
					afi->afiFontBBox.abbBottom,
					afi->afiFontBBox.abbRight,
					afi->afiFontBBox.abbTop );
    sioOutPrintf( sos, "  /FontType 42 def\n" );

    psWriteFontInfoDict( sos, afi );

    utilTtfWriteT42Sfnts( sos, ttf );

    utilTtfWriteCharStrings( sos, nchars, afi );

    sioOutPrintf( sos, "FontName currentdict end definefont pop\n" );

    return 0;
    }

static int utilTtfWriteT42File(	SimpleOutputStream *	sosPf42,
				const TrueTypeFont *	ttf )
    {
    int				rval= 0;

    AfmFontInfo			afi;

    psInitAfmFontInfo( &afi );

    if  ( utilTtfFontInfo( &afi, ttf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilTtfWriteT42( sosPf42, &afi, ttf );

  ready:

    psCleanAfmFontInfo( &afi );

    return rval;
    }

static int utilTtfWriteT1(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi,
				const TrueTypeFont *	ttf )
    {
    int				i;
    const AfmCharMetric *	acm;
    int				map[256];
    int				nchars;

    for ( i= 0; i < 256; i++ )
	{ map[i]= -1;	}

    nchars= 0;
    acm= afi->afiMetrics;
    for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	{
	if  ( ! acm->acmN )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	nchars++;

	if  ( acm->acmC < 0 || acm->acmC >= 256 )
	    { /*LDEB(acm->acmC);*/ continue;	}

	map[acm->acmC]= i;
	}

    sioOutPrintf( sos, "%%!PS-AdobeFont-1.0: %s %s\n",
				    afi->afiFontName,
				    afi->afiNotice );
    sioOutPrintf( sos, "12 dict begin\n" );
    sioOutPrintf( sos, "  /FontName /%s def\n", afi->afiFontName );
    sioOutPrintf( sos, "  /Encoding 256 array\n" );
    sioOutPrintf( sos, "   0 1 255 { 1 index exch /.notdef put } for\n" );

    for ( i= 0; i < 256; i++ )
	{
	if  ( map[i] < 0 )
	    { continue;	}

	acm= afi->afiMetrics+ map[i];
	sioOutPrintf( sos, "   dup %d /%s put\n", acm->acmC, acm->acmN );
	}

    sioOutPrintf( sos, "  readonly def\n" );
    sioOutPrintf( sos, "  /PaintType 0 def\n" );
    sioOutPrintf( sos, "  /StrokeWidth 0 def\n" );
    sioOutPrintf( sos, "  /FontMatrix [0.001 0 0 0.001 0 0] def\n" );
    sioOutPrintf( sos, "  /FontBBox [%d %d %d %d] def\n",
					afi->afiFontBBox.abbLeft,
					afi->afiFontBBox.abbBottom,
					afi->afiFontBBox.abbRight,
					afi->afiFontBBox.abbTop );
    sioOutPrintf( sos, "  /FontType 1 def\n" );

    psWriteFontInfoDict( sos, afi );

    sioOutPrintf( sos, "currentdict end\n" );
    sioOutPrintf( sos, "currentfile eexec\n" );
    sioOutPrintf( sos, "dup /Private 16 dict dup begin\n" );
    sioOutPrintf( sos, "  /RD{string currentfile exch readstring pop}executeonly def\n" );
    sioOutPrintf( sos, "  /ND{noaccess def}executeonly def\n" );
    sioOutPrintf( sos, "  /NP{noaccess put}executeonly def\n" );

    return 0;
    }


static int utilTtfWriteT1File(	SimpleOutputStream *	sosPf1,
				const TrueTypeFont *	ttf )
    {
    int				rval= 0;

    AfmFontInfo			afi;

    psInitAfmFontInfo( &afi );

    if  ( utilTtfFontInfo( &afi, ttf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilTtfWriteT1( sosPf1, &afi, ttf );

  ready:

    psCleanAfmFontInfo( &afi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a true type font from file.					*/
/*									*/
/************************************************************************/

static int utilTtfLoadFile(	TrueTypeFont *		ttf,
				SimpleInputStream *	sisTtf )
    {
    long			filePos= 0;

    int				tab;
    int				tableCount;
    TrueTypeTableEntry *	ttte;

    ttf->ttfVersion= sioEndianGetBeUint32( sisTtf ); filePos += 4;
    tableCount= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfSearchRange= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfEntrySelector= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfRangeShift= sioEndianGetBeUint16( sisTtf ); filePos += 2;

    ttf->ttfTables= (TrueTypeTableEntry *)malloc( tableCount*
					    sizeof(TrueTypeTableEntry) );
    if  ( ! ttf->ttfTables )
	{ LXDEB(tableCount,ttf->ttfTables); return -1;	}

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{ utilInitTrueTypeTableEntry( ttte );	}
    ttf->ttfTableCount= tableCount;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{
	int		b;

	for ( b= 0; b < 4; b++ )
	    { ttte->ttteTag[b]= sioInGetCharacter( sisTtf );	}
	ttte->ttteTag[b]= '\0';
	filePos += 4;

	ttte->ttteChecksum= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	ttte->ttteOffset= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	ttte->ttteLength= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	}

    qsort( ttf->ttfTables, ttf->ttfTableCount, sizeof(TrueTypeTableEntry),
					    utilTrueTypeFontCompareTables );

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{
	/*
	appDebug( "%3d: \"%.4s\" @%-7lu: %6lu bytes (%lu)\n",
					    tab, ttte->ttteTag,
					    ttte->ttteOffset, ttte->ttteLength,
					    ttte->ttteChecksum );
	*/

	if  ( filePos > ttte->ttteOffset )
	    { LLDEB(filePos,ttte->ttteOffset); return -1;	}

	while( filePos < ttte->ttteOffset )
	    {
	    int	c= sioInGetCharacter( sisTtf );
	    if  ( c == EOF )
		{ LLLDEB(filePos,ttte->ttteOffset,c); return -1; }
	    filePos++;
	    }

	ttte->ttteBytes= malloc( ttte->ttteLength );
	if  ( ! ttte->ttteBytes )
	    { LXDEB(ttte->ttteLength,ttte->ttteBytes); return -1; }
	if  ( sioInReadBytes( sisTtf, ttte->ttteBytes,
				    ttte->ttteLength ) != ttte->ttteLength )
	    { LDEB(ttte->ttteLength); return -1;	}

	filePos += ttte->ttteLength;
	}

    if  ( utilTtfExtractHeadTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractNameTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractPostTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractHheaTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractLocaTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractHmtxTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractGlyfTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractCmapTable( ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfExtractKernTable( ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfExtractOS_2Table( ttf ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate Truetype font to PostScript type 1.			*/
/*									*/
/*  Experimental code for future development and experimentation.	*/
/*									*/
/************************************************************************/

int psTtfToPt1(		SimpleOutputStream *	sosPt1,
			const char *		fontFileName,
			SimpleInputStream *	sisTtf )
    {
    int				rval= 0;
    TrueTypeFont		ttf;

    utilInitTrueTypeFont( &ttf );

    if  ( utilTtfLoadFile( &ttf, sisTtf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilTtfTranslateGlyphs( &ttf );

    utilTtfWriteT1File( sosPt1, &ttf );

  ready:

    utilCleanTrueTypeFont( &ttf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Translate Truetype font to PostScript type 42.			*/
/*									*/
/************************************************************************/

int psTtfToPf42(		SimpleOutputStream *	sosPf42,
				const char *		fontFileName,
				SimpleInputStream *	sisTtf )
    {
    int				rval= 0;
    TrueTypeFont		ttf;

    utilInitTrueTypeFont( &ttf );

    if  ( utilTtfLoadFile( &ttf, sisTtf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilTtfWriteT42File( sosPf42, &ttf );

  ready:

    utilCleanTrueTypeFont( &ttf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Translate Truetype font to PostScript type 42.			*/
/*									*/
/************************************************************************/

int psTtfToAfm(		SimpleOutputStream *	sosAfm,
			const char *		fontFileName,
			SimpleInputStream *	sisTtf )
    {
    int				rval= 0;

    AfmFontInfo			afi;
    TrueTypeFont		ttf;

    utilInitTrueTypeFont( &ttf );
    psInitAfmFontInfo( &afi );

    if  ( fontFileName )
	{ afi.afiFontFileName= strdup( fontFileName );	}

    if  ( utilTtfLoadFile( &ttf, sisTtf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilTtfFontInfo( &afi, &ttf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psWriteAfmFile( sosAfm, &afi );

  ready:

    psCleanAfmFontInfo( &afi );
    utilCleanTrueTypeFont( &ttf );

    return rval;
    }

