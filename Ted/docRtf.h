/************************************************************************/
/*									*/
/*  Read/Write RTF files.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_H
#   define	DOC_RTF_H

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<time.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	<sioGeneral.h>

#   include	"docRtfTagHash.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/
/*									*/
/*  Context for Reading RTF.						*/
/*									*/
/*  To ensure proper field/bookmark nesting.. A stack op active		*/
/*  is kept to be flushed at the appropriate transitions. (Field end,	*/
/*  field start, paragraph end.)					*/
/*									*/
/************************************************************************/

typedef struct RtfBookmarkLevel
    {
    int				rblFieldNumber;
    struct RtfBookmarkLevel *	rblPrevious;
    } RtfBookmarkLevel;

typedef struct RtfReadingState
    {
    int				rrsBytesPerUnicode;
    int				rrsUnicodeBytesToSkip;

    TextAttribute		rrsTextAttribute;
    ParagraphProperties		rrsParagraphProperties;
    SectionProperties		rrsSectionProperties;

    MemoryBuffer		rrsSavedText;

    struct RtfReadingState *	rrsPrev;
    } RtfReadingState;

typedef struct RtfReadingContext
    {
    RtfReadingState *		rrcState;

    int				rrcLevel;
    int				rrcSplitLevel;
    int				rrcInIgnoredGroup;
				/****************************************/
				/*  The depth in the document		*/
				/*  == Type of BufferItem.		*/
				/****************************************/
    int				rrcComplainUnknown;
				/****************************************/
				/*  Complain about unknown control	*/
				/****************************************/
    int				rrcCharacterAhead;
    int				rrcAfterNoteref;
				/****************************************/
				/*  Character that could not be unread	*/
				/*  using ungetc().			*/
				/****************************************/
    int				rrcCurrentLine;
    BufferDocument *		rrcBd;
    BufferItem *		rrcBi;
				/****************************************/
				/*  The current position in the input.	*/
				/****************************************/
    int				rrcInDeletedText;
    BorderProperties		rrcTextBorderProperties;
    int				rrcTextShadingPattern;
    int				rrcTextShadingLevel;
    int				rrcTextForeground;
    int				rrcTextBackground;
				/****************************************/
				/*  Text attributes of the current pos.	*/
				/****************************************/
    FramePosition		rrcParaFramePosition;
    FramePosition		rrcRowFramePosition;
    CellProperties		rrcCellProperties;
    RowProperties		rrcRowProperties;
    BorderProperties		rrcBorderProperties;
    DrawingShape *		rrcDrawingShape;
    int				rrcNextObjectVertex;
    DocumentStyle		rrcDocumentStyle;
    TabStop			rrcTabStop;
    RGB8Color			rrcColor;
    int				rrcGotComponent;
    struct tm			rrcTm;
    unsigned char *		rrcInfoText;
    int				rrcExternalItemKind;

    ParagraphNumber		rrcParagraphNumber;
    int				rrcWhichPnText;
    ParagraphNumber *		rrcParagraphNumbers;
    int				rrcParagraphNumberCount;

    DocumentList		rrcDocumentList;
    DocumentListLevel		rrcDocumentListLevel;

    ListOverride		rrcListOverride;
    ListOverrideLevel		rrcListOverrideLevel;

				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    DocumentFont		rrcCurrentFont;

				/****************************************/
				/*  For reading the font table.		*/
				/*  Style sheet.			*/
				/****************************************/
    InsertedObject *		rrcInsertedObject;
    PictureProperties		rrcPictureProperties;
    int				rrcFieldNumber;
    unsigned char		rrcBookmarkName[DOCmaxBOOKMARK+1];
    unsigned char		rrcBookmarkSize;
    RtfBookmarkLevel *		rrcTopBookmark;
				/****************************************/
				/*  For reading 'objects' and pictures.	*/
				/*  For reading 'fields'.		*/
				/****************************************/
    int				rrcJustAfterPntext;
    int				rrcGotDocGeometry;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/
    unsigned char		rrcDefaultInputMapping[256];
    const unsigned char *	rrcTextInputMapping;
    const PostScriptFontList *	rrcPostScriptFontList;
    } RtfReadingContext;


/************************************************************************/
/*									*/
/*  What you can find in an RTF file.					*/
/*									*/
/************************************************************************/

#   define	RTFfiCTRLGROUP	1
#   define	RTFfiWORD	2
#   define	RTFfiCHAR	3
#   define	RTFfiTAB	4
#   define	RTFfiCLOSE	5
#   define	RTFfiSTARGROUP	6
#   define	RTFfiTEXTGROUP	7

/************************************************************************/
/*									*/
/*  Writing context.							*/
/*									*/
/************************************************************************/

typedef struct PushedAttribute
    {
    struct PushedAttribute *	paNext;
    TextAttribute		paTextAttribute;
    } PushedAttribute;

typedef struct RtfWritingContext
    {
    TextAttribute		rwcTextAttribute;
    ParagraphProperties		rwcParagraphProperties;
    RowProperties		rwcRowProperties;

    TextAttribute		rwcOutsideTableTextAttribute;
    ParagraphProperties		rwcOutsideTableParagraphProperties;

    PushedAttribute *		rwcOutsideFldrsltAttribute;
    int				rwcInFldrslt;
    int				rwcInTable;

    int				rwcHasPrecedingTags;
    int				rwcSaveBookmarks;

    int				rwcSaveAsLink;
    int				rwcSaveAsLinkAsRef;
    int				rwcSaveAsLinkAsPageref;
    DocumentField		rwcSaveAsHyperlinkField;
    DocumentField		rwcSaveAsRefField;
    DocumentField		rwcSaveAsPagerefField;

    unsigned char		rwcDefaultOutputMapping[256];
    const unsigned char *	rwcTextOutputMapping;
    } RtfWritingContext;

/************************************************************************/
/*									*/
/*  Map control words to functions that handle them.			*/
/*									*/
/*  This structure will be gradually extended to also cover the		*/
/*  Microsoft WordProcessingML markup language.				*/
/*									*/
/************************************************************************/

typedef struct RtfControlWord
    {
    const char *	rcwWord;
    int			rcwID;
    ItemLevel		rcwLevel;
    int			(*rcwApply)( SimpleInputStream *		sis,
				    const struct RtfControlWord *	rcw,
				    int					arg,
				    RtfReadingContext *			rrc );
    int			rcwEnumValue;
    int			(*rcwPrepare)( SimpleInputStream *		sis,
				    const struct RtfControlWord *	rcw,
				    int					arg,
				    RtfReadingContext *			rrc );
    struct RtfControlWord *	rcwDetailWords;

    const char *	rcwXmlName;
    } RtfControlWord;

#   define	TEDszRTFCONTROL		32

typedef int (*RtfAddTextParticule)(	RtfReadingContext *		rrc,
					const unsigned char *		text,
					int				len );

typedef int (*RtfCommitGroup)(		RtfReadingContext *		rrc );

/************************************************************************/
/*  Common data.							*/
/************************************************************************/

extern RtfControlWord	docRtfEmptyTable[];
extern RtfControlWord	docRtfDocumentWords[];
extern RtfControlWord	docRtfDocumentGroups[];
extern RtfControlWord	docRtfListLevelGroups[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfInitReadingContext(	RtfReadingContext *		rrc );
extern void docRtfCleanReadingContext(	RtfReadingContext *		rrc );
extern int docRtfCopyReadingContext(	RtfReadingContext *		to,
					const RtfReadingContext *	from );
extern void docRtfCopyReadingContextBack( RtfReadingContext *		to,
					RtfReadingContext *	from );

extern const RtfControlWord * docRtfFindWord(	const char *	controlWord,
					const RtfControlWord *	contolWords,
					int			listOnly );

extern int docRtfApplyControlWord(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			gotArg,
					int			arg,
					RtfReadingContext *	rrc	);

extern int docRtfReadGroup(	SimpleInputStream *	sis,
				int			textLevel,

				const RtfControlWord *	firstApply,
				int			gotArg,
				int			arg,

				RtfReadingContext *	rrc,

				const RtfControlWord *	contolWords,
				const RtfControlWord *	groupWords,

				RtfAddTextParticule	addParticule,
				RtfCommitGroup		commitGroup );

extern int docRtfFindControl(	SimpleInputStream *	sis,
				RtfReadingContext *	rrc,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg		);

extern int docRtfIgnoreWord(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	);

extern int docRtfIgnoreText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len );

extern int docRtfRefuseText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len );

extern int docRtfSaveText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len );

extern int docRtfRememberText(	char **			pTarget,
				int *			pSize,
				RtfReadingContext *	rrc,
				int			removeSemicolon );

extern int docRtfRememberUText(	unsigned char **	pTarget,
				int *			pSize,
				RtfReadingContext *	rrc,
				int			removeSemicolon );

extern int docRtfSkipGroup(	SimpleInputStream *	sis,
				const RtfControlWord *	groupRcw,
				int			textLevel,
				RtfReadingContext *	rrc );

extern int docRtfConsumeGroup(	SimpleInputStream *	sis,
				int			textLevel,
				RtfReadingContext *	rrc,
				const RtfControlWord *	contolWords,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule );

extern int docRtfReadPict(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadNonshppict(SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadShppict(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadObject(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadDrawingObject(
				SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfAdjustLevel(	RtfReadingContext *	rrc,
				int			toLevel,
				int			textLevel );

extern int docRtfReadShape(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfTextParticule(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len );

extern int docRtfReadField(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadTextField(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfLookupEntry(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadExtIt(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfReadFootnote(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfBkmkStart(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfBkmkEnd(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfTextSpecialToField(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfFlushBookmarks(	int			fieldNumber,
					BufferItem *		bi,
					RtfReadingContext *	rrc );

extern int docRtfFontTable(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfColorTable(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRevisionTable(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfStylesheet(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfListTable(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfReadInfo(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberDocProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfWriteArgTag(		const char *		tag,
					int *			pCol,
					int			arg,
					SimpleOutputStream *	sos );

extern void docRtfWriteTag(		const char *		tag,
					int *			pCol,
					SimpleOutputStream *	sos );

extern void docRtfEscapeString(		const unsigned char *	s,
					const unsigned char *	outputMapping,
					int *			pCol,
					int			n,
					SimpleOutputStream *	sos );

extern void docRtfWriteStringDestination(
					const char *		tag,
					int *			pCol,
					const unsigned char *	s,
					int			n,
					int			addSemicolon,
					SimpleOutputStream *	sos );

extern void docRtfWriteNextLine(	int *			pCol,
					SimpleOutputStream *	sos );

extern void docRtfSaveBorder(		const char *			tag,
					int *				pCol,
					const BorderProperties *	bp,
					SimpleOutputStream *		sos );

extern void docRtfSaveTextAttribute(	SimpleOutputStream *	sos,
					int *			pCol,
					const PropertyMask *	updMask,
					const TextAttribute *	ta );

extern void docRtfSaveParagraphProperties( SimpleOutputStream *	sos,
				int *				pCol,
				const PropertyMask *		updMask,
				const ParagraphProperties *	pp );

extern void docRtfWriteDestinationBegin( const char *		tag,
					int *			pCol,
					SimpleOutputStream *	sos );

extern void docRtfWriteDestinationEnd(	int *			pCol,
					SimpleOutputStream *	sos );

extern int docRtfSavePictureTags(	const PictureProperties *	pip,
					int *				pCol,
					SimpleOutputStream *		sos );

extern int docRtfWriteMemoryBuffer(	const MemoryBuffer *		mb,
					int *				pCol,
					SimpleOutputStream *		sos );

extern int docRtfReadUnknownGroup(	SimpleInputStream *	sis,
					RtfReadingContext *	rrc );

extern int docRtfReadWordGroup(	SimpleInputStream *	sis,
				int			textLevel,
				RtfReadingContext *	rrc,
				int			gotArg,
				int			arg,
				const char *		controlWord,
				const RtfControlWord *	contolWords,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule );

extern void docRtfSaveSectionProperties( SimpleOutputStream *	sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const PropertyMask *		updMask,
				const SectionProperties *	sp );

extern void docRtfWriteFontTable( SimpleOutputStream *		sos,
				int *				pCol,
				const DocumentFontList *	dfl );

extern void docRtfWriteColorTable( SimpleOutputStream *		sos,
				int *				pCol,
				const DocumentProperties *	dp );

extern void docRtfWriteRevisorTable( SimpleOutputStream *	sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentProperties *	dp );

extern void docRtfWriteStyleSheet( SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentStyleSheet *	dss );

extern int docRtfRememberParagraphProperty( SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfBeginBorder(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfBrdrProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

extern int docRtfRememberSectionProperty( SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberTextProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberPntextProperty(SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberTextShadingPattern(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfWriteArgDestinationBegin( const char *	tag,
					int *			pCol,
					int			arg,
					SimpleOutputStream *	sos );

extern int docRtfSaveDocumentProperties( SimpleOutputStream *	sos,
					int *			pCol,
					RtfWritingContext *	rwc,
					const BufferDocument *	bd );

extern void docRtfReadSetAnsicpg(	RtfReadingContext *	rrc,
					int			arg );

extern void docRtfSaveParaFrameProperties(
				SimpleOutputStream *		sos,
				int *				pCol,
				const PropertyMask *		updMask,
				const FramePosition *		fp );

extern int docRtfRememberParaFrameProperty(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberRowFrameProperty(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfSaveSectHeadersFooters(SimpleOutputStream *	sos,
					int *			pCol,
					const BufferItem *	bi,
					const BufferDocument *	bd,
					RtfWritingContext *	rwc );

extern int docRtfSaveExternalItem(	SimpleOutputStream *	sos,
					int *			pCol,
					const BufferItem *	bi,
					const BufferDocument *	bd,
					const char *		tag,
					RtfWritingContext *	rwc );

extern int docRtfSaveDocNotesSeparators(SimpleOutputStream *	sos,
					int *			pCol,
					const BufferDocument *	bd,
					RtfWritingContext *	rwc );

extern void docRtfSaveCellProperties(	const CellProperties *	cp,
					int *			pCol,
					int			shiftLeft,
					SimpleOutputStream *	sos );

extern int docRtfRememberCellProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfSaveRowProperties(	const RowProperties *	rp,
					int *			pCol,
					int			col0,
					int			col1,
					SimpleOutputStream *	sos );

extern int docRtfRememberRowProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfSaveAutoSpace(	int *			pCol,
					SimpleOutputStream *	sos,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number );

extern int docRtfRememberStyleProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberListLevelProperty(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberListProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfWriteListTable(
				SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentListTable *	dlt );

extern void docRtfPushReadingState(	RtfReadingContext *	rrc,
					RtfReadingState *	rrs );

extern void docRtfPopReadingState(	RtfReadingContext *	rrc );

extern int docRtfRememberListOverrideLevelProperty(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfRememberListOverrideProperty(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfListOverrideTable(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern void docRtfWriteListOverrideTable(
				SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const ListOverrideTable *	lot );

extern void docRtfWriteListLevel(
				SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentListLevel *	dll );

extern int docRtfRememberTabStopProperty(
				SimpleInputStream *		sis,
				const RtfControlWord *		rcw,
				int				arg,
				RtfReadingContext *		rrc );

extern void docRtfSaveTabStopList(
				SimpleOutputStream *		sos,
				int *				pCol,
				const TabStopList *		tsl );

extern int docRtfFontProperty(	SimpleInputStream *		sis,
				const RtfControlWord *		rcw,
				int				arg,
				RtfReadingContext *		rrc );

extern const RtfControlWord * docRtfFindPropertyWord(
					const char *		controlWord );

extern const RtfControlWord * docRtfFindShapePropertyWord(
					const char *		controlWord );

extern int docRtfObjectProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfPictureProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfCommitListLevelStyle(	RtfReadingContext *	rrc );

extern int docRtfTextSpecialChar(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfTextSpecialParticule(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfDrawingObjectProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfDrawingObjectCoordinate(
					SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfDocInfoGroup(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfPnProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpArray(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpString(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpPicture(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpNumber(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpPositionNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpTypeNumber(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpLockNumber(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpFillNumber(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpLineNumber(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpGroupedNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpConnectNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpTxboxNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpShadowNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpColor(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfReadExternalItem(	BufferItem **		pBi,
					int *			pExtItKind,
					SimpleInputStream *	sis,
					RtfReadingContext *	rrc,
					const SelectionScope *	ss );

extern int docRtfShapeProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

extern int docRtfShpGeometryNumber(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc );

#    endif	/*  DOC_RTF_H	*/
