/************************************************************************/
/*									*/
/*  Table of rtf tags that are not treated as an exception.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<utilFontEncoding.h>
#   include	<utilJenkinsPerfectHash.h>

#   include	"docRtf.h"

#   include	<appDebugon.h>

#   define	BUILD_NO_HASH				/*  for sed */

#   ifdef	BUILD_HASH

#	define		docRtfBrdrProperty			0
#	define		docRtfBeginBorder			0
#	define		docRtfRememberProperty			0
#	define		docRtfRememberDocProperty		0
#	define		docRtfRememberStyleProperty		0
#	define		docRtfRememberSectionProperty		0
#	define		docRtfRememberRowProperty		0
#	define		docRtfRememberCellProperty		0
#	define		docRtfRememberParagraphProperty		0
#	define		docRtfRememberTextProperty		0
#	define		docRtfRememberListLevelProperty		0
#	define		docRtfRememberListProperty		0
#	define		docRtfRememberListOverrideProperty	0
#	define		docRtfRememberListOverrideLevelProperty	0
#	define		docRtfRememberTabStopProperty		0
#	define		docRtfRememberParaFrameProperty		0
#	define		docRtfRememberRowFrameProperty		0
#	define		docRtfFontProperty			0
#	define		docRtfRememberPntextProperty		0
#	define		docRtfObjectProperty			0
#	define		docRtfPictureProperty			0
#	define		docRtfTextSpecialChar			0
#	define		docRtfTextSpecialParticule		0
#	define		docRtfTextSpecialToField		0
#	define		docRtfRememberTextShadingPattern	0
#	define		docRtfDrawingObjectProperty		0
#	define		docRtfDrawingObjectCoordinate		0
#	define		docRtfShpProperty			0
#	define		docRtfPnProperty			0

#   endif

/************************************************************************/
/*									*/
/*  Control words to remember border properties.			*/
/*									*/
/************************************************************************/

# define RTF_BORDER_PROP( s, id ) \
		    { s, id, DOClevANY, docRtfBrdrProperty, }
# define RTF_BORDER_STYLE(s,v) \
		    { s, BRDRpropSTYLE, DOClevANY, docRtfBrdrProperty, v, }

static RtfControlWord	docRtfBorderWords[]=
    {
	RTF_BORDER_PROP( "brdrw",		BRDRpropPEN_WIDE ),
	RTF_BORDER_PROP( "brdrcf",		BRDRpropCOLOR ),
	RTF_BORDER_PROP( "brsp",		BRDRpropSPACING ),
	RTF_BORDER_PROP( "brdrart",		BRDRpropART ),

	/**/

	RTF_BORDER_STYLE( "brdrnone",		DOCbsNONE ),
	RTF_BORDER_STYLE( "brdrs",		DOCbsS ),
	RTF_BORDER_STYLE( "brdrth",		DOCbsTH ),
	RTF_BORDER_STYLE( "brdrsh",		DOCbsSH ),

	RTF_BORDER_STYLE( "brdrdb",		DOCbsDB ),
	RTF_BORDER_STYLE( "brdrhair",		DOCbsHAIR ),
	RTF_BORDER_STYLE( "brdrdot",		DOCbsDOT ),
	RTF_BORDER_STYLE( "brdrdash",		DOCbsDASH  ),
	RTF_BORDER_STYLE( "brdrdashd",		DOCbsDASHD ),
	RTF_BORDER_STYLE( "brdrdashdd",		DOCbsDASHDD ),
	RTF_BORDER_STYLE( "brdrtriple",		DOCbsTRIPLE ),

	RTF_BORDER_STYLE( "brdrtnthsg",		DOCbsTNTHSG ),
	RTF_BORDER_STYLE( "brdrthtnsg",		DOCbsTHTNSG ),
	RTF_BORDER_STYLE( "brdrtnthtnsg",	DOCbsTNTHTNSG ),
	RTF_BORDER_STYLE( "brdrtnthmg",		DOCbsTNTHMG ),
	RTF_BORDER_STYLE( "brdrthtnmg",		DOCbsTHTNMG ),
	RTF_BORDER_STYLE( "brdrtnthtnmg",	DOCbsTNTHTNMG ),
	RTF_BORDER_STYLE( "brdrtnthlg",		DOCbsTNTHLG ),
	RTF_BORDER_STYLE( "brdrthtnlg",		DOCbsTHTNLG ),
	RTF_BORDER_STYLE( "brdrtnthtnlg",	DOCbsTNTHTNLG ),

	RTF_BORDER_STYLE( "brdrwavy",		DOCbsWAVY ),
	RTF_BORDER_STYLE( "brdrwavydb",		DOCbsWAVYDB ),
	RTF_BORDER_STYLE( "brdrdashsm",		DOCbsDASHSM ),
	RTF_BORDER_STYLE( "brdrdashdotstr",	DOCbsDASHDOTSTR ),
	RTF_BORDER_STYLE( "brdremboss",		DOCbsEMBOSS ),
	RTF_BORDER_STYLE( "brdrengrave",	DOCbsENGRAVE ),
	RTF_BORDER_STYLE( "brdroutset",		DOCbsOUTSET ),

	RTF_BORDER_STYLE( "brdrtbl",		DOCbsTBL ),
	RTF_BORDER_STYLE( "brdrnil",		DOCbsNIL ),

	/**/

	{ 0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Table of common rtf tags.						*/
/*									*/
/************************************************************************/

# define	RTF_REMEMBER(s,id) \
    { s,      id,         DOClevANY,  docRtfRememberProperty, }


# define	RTF_BORDER(s,id) \
    { s,	id,	DOClevANY,  docRtfRememberProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }


# define	RTF_STYLE_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, }
# define	RTF_STYLE_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, }
# define	RTF_STYLE_VAL_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, v }

# define	RTF_TEXT_DEFAULT(s) \
		{ s, TAprop_NONE, DOClevANY, docRtfRememberTextProperty, }
# define	RTF_TEXT_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, }
# define	RTF_TEXT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, v, }
# define	RTF_TEXT_FLAG(s,xmlElem,id) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, }

# define	RTF_PNTEXT_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, }
# define	RTF_PNTEXT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, v, }
# define	RTF_PNTEXT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, }


# define	RTF_SECT_DEFAULT(s) \
		{ s, SPprop_NONE, DOClevANY, docRtfRememberSectionProperty, }
# define	RTF_SECT_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, }
# define	RTF_SECT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, v, }
# define	RTF_SECT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, }

# define	RTF_PARA_FRAME_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberParaFrameProperty, }
# define	RTF_PARA_FRAME_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberParaFrameProperty, v, }
# define	RTF_PARA_FRAME_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberParaFrameProperty, }

# define	RTF_ROW_FRAME_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowFrameProperty, }
# define	RTF_ROW_FRAME_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberRowFrameProperty, v, }
# define	RTF_ROW_FRAME_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowFrameProperty, }

# define	RTF_CELL_DEFAULT(s) \
		{ s, CLprop_NONE, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, v, }
# define	RTF_CELL_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

# define	RTF_ROW_DEFAULT(s) \
		{ s, RPprop_NONE, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, v, }
# define	RTF_ROW_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

# define	RTF_LISTLEV_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberListLevelProperty, }

# define	RTF_LIST_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberListProperty, }
# define	RTF_LIST_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberListProperty, v, }
# define	RTF_LIST_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberListProperty, }

# define	RTF_LISTOVERR_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberListOverrideProperty, }

# define	RTF_LISTOVERRLEV_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberListOverrideLevelProperty, }

# define	RTF_TAB_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTabStopProperty, v }
# define	RTF_TAB_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberTabStopProperty }

# define	RTF_FONT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfFontProperty, v }
# define	RTF_FONT_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfFontProperty, }

# define	RTF_SPECIAL_CHAR(s,id) \
		{ s, id, DOClevTEXT, docRtfTextSpecialChar, }
# define	RTF_SPECIAL_PART(s,id) \
		{ s, id, DOClevTEXT, docRtfTextSpecialParticule, }
# define	RTF_SPECIAL_FIELD(s,id) \
		{ s, id, DOClevTEXT, docRtfTextSpecialToField, }

# define	RTF_TEXT_SHADING_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTextShadingPattern, v }

RtfControlWord	docRtfPropertyWords[]=
{
				/****************************************/
				/*  Text properties.			*/
				/*  Character Formatting Properties	*/
				/****************************************/
    RTF_TEXT_DEFAULT( "plain" ),

    RTF_TEXT_NUMBER( "f",		TApropDOC_FONT_NUMBER ),
    RTF_TEXT_NUMBER( "fs",		TApropFONTSIZE ),
    RTF_TEXT_NUMBER( "cf",		TApropTEXT_COLOR ),
    RTF_TEXT_NUMBER( "cs",		TApropTEXT_STYLE ),

    RTF_TEXT_FLAG( "b", "w:b",		TApropFONTBOLD ),
    RTF_TEXT_FLAG( "i", "w:i",		TApropFONTSLANTED ),
    RTF_TEXT_FLAG( "ul", "w:u",		TApropTEXTUNDERLINED ),

    RTF_TEXT_ENUM( "sub",		TApropSUPERSUB, DOCfontSUBSCRIPT ),
    RTF_TEXT_ENUM( "super",		TApropSUPERSUB, DOCfontSUPERSCRIPT ),
    RTF_TEXT_ENUM( "nosupersub",	TApropSUPERSUB, DOCfontREGULAR ),

    RTF_TEXT_FLAG( "caps", "w:caps",	TApropCAPITALS ),
    RTF_TEXT_FLAG( "scaps", "w:smallcaps",
					TApropSMALLCAPS ),
    RTF_TEXT_FLAG( "strike", "w:strike",
					TApropSTRIKETHROUGH ),

    /**/

    RTF_PNTEXT_NUMBER( "pnf",		TApropDOC_FONT_NUMBER ),
    RTF_PNTEXT_NUMBER( "pnfs",		TApropFONTSIZE ),
    RTF_PNTEXT_NUMBER( "pncf",		TApropTEXT_COLOR ),

    RTF_PNTEXT_FLAG( "pnb",		TApropFONTBOLD ),
    RTF_PNTEXT_FLAG( "pni",		TApropFONTSLANTED ),
    RTF_PNTEXT_FLAG( "pnul",		TApropTEXTUNDERLINED ),

    RTF_PNTEXT_FLAG( "pncaps",		TApropCAPITALS ),
    RTF_PNTEXT_FLAG( "pnscaps",		TApropSMALLCAPS ),
    RTF_PNTEXT_FLAG( "pnstrike",	TApropSTRIKETHROUGH ),

    /*
    RTF_REMEMBER( "pnuld",		RTFidPNULD ),
    RTF_REMEMBER( "pnuldb",		RTFidPNULDB ),
    RTF_REMEMBER( "pnulnone",		RTFidPNULNONE ),
    */

# define	RTF_PN_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfPnProperty }
# define	RTF_PN_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfPnProperty }
# define	RTF_PN_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfPnProperty, v }

    RTF_PN_NUMBER( "pnstart",	LLpropSTARTAT ),

    RTF_PN_ENUM( "pndec",	LLpropSTYLE,	DOCpnDEC ),
    RTF_PN_ENUM( "pnucrm",	LLpropSTYLE,	DOCpnUCRM ),
    RTF_PN_ENUM( "pnlcrm",	LLpropSTYLE,	DOCpnLCRM ),
    RTF_PN_ENUM( "pnucltr",	LLpropSTYLE,	DOCpnUCLTR ),
    RTF_PN_ENUM( "pnlcltr",	LLpropSTYLE,	DOCpnLCLTR ),
    RTF_PN_ENUM( "pnord",	LLpropSTYLE,	DOCpnORD ),
    RTF_PN_ENUM( "pncard",	LLpropSTYLE,	DOCpnCARD ),
    RTF_PN_ENUM( "pnordt",	LLpropSTYLE,	DOCpnORDT ),

    RTF_PN_ENUM( "pndbnum",	LLpropSTYLE,	DOCpnDBNUM ),
    RTF_PN_ENUM( "pndbnumd",	LLpropSTYLE,	DOCpnDBNUMD ),
    RTF_PN_ENUM( "pnaiu",	LLpropSTYLE,	DOCpnAIU ),
    RTF_PN_ENUM( "pniroha",	LLpropSTYLE,	DOCpnIROHA ),
    RTF_PN_ENUM( "pndbnuml",	LLpropSTYLE,	DOCpnDBNUML ),
    RTF_PN_ENUM( "pndbnumt",	LLpropSTYLE,	DOCpnDBNUMT ),
    RTF_PN_ENUM( "pndbnumk",	LLpropSTYLE,	DOCpnDBNUMK ),
    RTF_PN_ENUM( "pncnum",	LLpropSTYLE,	DOCpnCNUM ),
    RTF_PN_ENUM( "pndecd",	LLpropSTYLE,	DOCpnDECD ),
    RTF_PN_ENUM( "pnaiud",	LLpropSTYLE,	DOCpnAIUD ),
    RTF_PN_ENUM( "pnirohad",	LLpropSTYLE,	DOCpnIROHAD ),
    RTF_PN_ENUM( "pnganada",	LLpropSTYLE,	DOCpnGANADA ),
    RTF_PN_ENUM( "pngbnum",	LLpropSTYLE,	DOCpnGBNUM ),
    RTF_PN_ENUM( "pngbnumd",	LLpropSTYLE,	DOCpnGBNUMD ),
    RTF_PN_ENUM( "pngbnuml",	LLpropSTYLE,	DOCpnGBNUML ),
    RTF_PN_ENUM( "pngbnumk",	LLpropSTYLE,	DOCpnGBNUMK ),
    RTF_PN_ENUM( "pnzodiac",	LLpropSTYLE,	DOCpnZODIAC ),
    RTF_PN_ENUM( "pnzodiacd",	LLpropSTYLE,	DOCpnZODIACD ),
    RTF_PN_ENUM( "pnzodiacl",	LLpropSTYLE,	DOCpnZODIACL ),
    RTF_PN_ENUM( "pnchosung",	LLpropSTYLE,	DOCpnCHOSUNG ),
    RTF_PN_ENUM( "pnaiueo",	LLpropSTYLE,	DOCpnAIUEO ),
    RTF_PN_ENUM( "pnaiueod",	LLpropSTYLE,	DOCpnAIUEOD ),
    RTF_PN_ENUM( "pnbidia",	LLpropSTYLE,	DOCpnBIDIA ),
    RTF_PN_ENUM( "pnbidib",	LLpropSTYLE,	DOCpnBIDIB ),

    RTF_PN_ENUM( "pnql",	LLpropJUSTIFY,	0 ),
    RTF_PN_ENUM( "pnqc",	LLpropJUSTIFY,	1 ),
    RTF_PN_ENUM( "pnqr",	LLpropJUSTIFY,	2 ),

    RTF_PN_NUMBER( "pnindent",	LLpropINDENT ),
    RTF_PN_NUMBER( "pnsp",	LLpropSPACE ),
    RTF_PN_FLAG( "pnprev",	LLpropPREV ),

    /**/

    RTF_REMEMBER( "lang",		RTFidLANG ),
    RTF_REMEMBER( "noproof",		RTFidNOPROOF ),
    RTF_REMEMBER( "ulnone",		RTFidULNONE ),
    RTF_REMEMBER( "up",			RTFidUP ),
    RTF_REMEMBER( "cgrid",		RTFidCGRID ),

    RTF_REMEMBER( "cb",			RTFidCB ),
    RTF_REMEMBER( "cchs",		RTFidCCHS ),
    RTF_REMEMBER( "deleted",		RTFidDELETED ),
    RTF_REMEMBER( "dn",			RTFidDN ),
    RTF_REMEMBER( "outl",		RTFidOUTL ),
    RTF_REMEMBER( "revauth",		RTFidREVAUTH ),
    RTF_REMEMBER( "crauth",		RTFidREVAUTH ),
    RTF_REMEMBER( "crdate",		RTFidREVAUTH ),
    RTF_REMEMBER( "revauthdel",		RTFidREVAUTHDEL ),
    RTF_REMEMBER( "revdttm",		RTFidREVDTTM ),
    RTF_REMEMBER( "revdttmdel",		RTFidREVDTTMDEL ),
    RTF_REMEMBER( "revised",		RTFidREVISED ),
    RTF_REMEMBER( "rtlch",		RTFidRTLCH ),
    RTF_REMEMBER( "ltrch",		RTFidLTRCH ),
    RTF_REMEMBER( "shad",		RTFidSHAD ),

    RTF_REMEMBER( "fcs",		RTFidFCS ),

    RTF_REMEMBER( "uld",		RTFidULD ),
    RTF_REMEMBER( "uldb",		RTFidULDB ),
    RTF_REMEMBER( "ulw",		RTFidULW ),
    RTF_REMEMBER( "v",			RTFidV ),
    RTF_REMEMBER( "webhidden",		RTFidWEBHIDDEN ),
    RTF_REMEMBER( "kerning",		RTFidKERNING ),
    RTF_REMEMBER( "expnd",		RTFidEXPND ),
    RTF_REMEMBER( "expndtw",		RTFidEXPNDTW ),
    RTF_REMEMBER( "charscalex",		RTFidCHARSCALEX ),
    RTF_REMEMBER( "highlight",		RTFidHIGHLIGHT ),

    RTF_TEXT_SHADING_ENUM( "chbghoriz",		0, DOCspBGHORIZ ),
    RTF_TEXT_SHADING_ENUM( "chbgvert",		0, DOCspBGVERT ),
    RTF_TEXT_SHADING_ENUM( "chbgfdiag",		0, DOCspBGFDIAG ),
    RTF_TEXT_SHADING_ENUM( "chbgbdiag",		0, DOCspBGBDIAG ),
    RTF_TEXT_SHADING_ENUM( "chbgcross",		0, DOCspBGCROSS ),
    RTF_TEXT_SHADING_ENUM( "chbgdcross",	0, DOCspBGDCROSS ),
    RTF_TEXT_SHADING_ENUM( "chbgdkhoriz",	0, DOCspBGDKHORIZ ),
    RTF_TEXT_SHADING_ENUM( "chbgdkvert",	0, DOCspBGDKVERT ),
    RTF_TEXT_SHADING_ENUM( "chbgdkfdiag",	0, DOCspBGDKFDIAG ),
    RTF_TEXT_SHADING_ENUM( "chbgdkbdiag",	0, DOCspBGDKBDIAG ),
    RTF_TEXT_SHADING_ENUM( "chbgdkcross",	0, DOCspBGDKCROSS ),
    RTF_TEXT_SHADING_ENUM( "chbgdkdcross",	0, DOCspBGDKDCROSS ),

    RTF_BORDER(   "chbrdr",		RTFidCHBRDR ),

    RTF_REMEMBER( "chcbpat",		RTFidCHCBPAT ),
    RTF_REMEMBER( "chcfpat",		RTFidCHCFPAT ),
    RTF_REMEMBER( "chshdng",		RTFidCHSHDNG ),

    RTF_REMEMBER( "faauto",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "fahang",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "facenter",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "faroman",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "favar",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "fafixed",		RTFid_FONTALIGN ),

				/****************************************/
				/*  Paragraph properties.		*/
				/****************************************/

# define	RTF_PARA_DEFAULT(s) \
		{ s, PPprop_NONE, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_ENUM(s,xmlExpr,id,v) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, v, }
# define	RTF_PARA_FLAG(s,xmlExpr,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

# define	RTF_PARA_FLAGX(s,xmlElem,id) \
		    RTF_PARA_FLAG(s,"//w:pPr/" xmlElem "/@w:val", id )
# define	RTF_PARA_ENUMX(s,xmlElem,xmlAttr,xmlVal,id, v) \
		    RTF_PARA_ENUM(s,"//w:pPr/" xmlElem \
		    "[@" xmlAttr "='" xmlVal "']/@" xmlAttr, id, v )

# define	RTF_PARA_ENUMR(s,id,v) \
		    RTF_PARA_ENUM(s,(char *)0,id,v)

    RTF_PARA_DEFAULT( "pard" ),

    RTF_PARA_NUMBER( "s",		PPpropSTYLE ),
    RTF_PARA_NUMBER( "ls",		PPpropLISTOVERRIDE ),

    RTF_PARA_NUMBER( "li",		PPpropLEFT_INDENT ),
    RTF_PARA_NUMBER( "fi",		PPpropFIRST_INDENT ),
    RTF_PARA_NUMBER( "ri",		PPpropRIGHT_INDENT ),

    RTF_PARA_ENUMX( "ql", "w:jc", "w:val", "left",
					PPpropALIGNMENT, DOCiaLEFT ),
    RTF_PARA_ENUMX( "qr", "w:jc", "w:val", "right",
					PPpropALIGNMENT, DOCiaRIGHT ),
    RTF_PARA_ENUMX( "qj", "w:jc", "w:val", "both",
					PPpropALIGNMENT, DOCiaJUSTIFIED ),
    RTF_PARA_ENUMX( "qc", "w:jc", "w:val", "center",
					PPpropALIGNMENT, DOCiaCENTERED ),

    RTF_PARA_FLAGX( "pagebb", "w:pageBreakBefore", PPpropNEWPAGE ),

    RTF_PARA_ENUMR( "nowidctlpar", 	PPpropWIDCTLPAR, 0 ),
    RTF_PARA_ENUMR( "widctlpar",	PPpropWIDCTLPAR, 1 ),

    RTF_PARA_FLAGX( "keep", "w:keepLines",	PPpropKEEP ),
    RTF_PARA_FLAGX( "keepn", "w:keepNext",	PPpropKEEPN ),

    RTF_PARA_NUMBER( "sb",		PPpropSPACE_BEFORE ),
    RTF_PARA_NUMBER( "sa",		PPpropSPACE_AFTER ),
    RTF_PARA_NUMBER( "sl",		PPpropLINE_SPACING_DIST ),

    RTF_PARA_FLAG( "slmult", (char *)0,	PPpropLINE_SPACING_MULT ),

    RTF_PARA_NUMBER( "outlinelevel",	PPpropOUTLINELEVEL ),
    RTF_PARA_NUMBER( "ilvl",		PPpropLISTLEVEL ),

    RTF_PARA_FLAGX( "hyphpar", (char *)0, PPpropHYPHPAR ),
    RTF_PARA_FLAGX( "intbl", (char *)0,	PPpropIN_TABLE ),

    RTF_PARA_NUMBER( "shading",		PPpropSHADE_LEVEL ),

    RTF_PARA_ENUMR( "bgbdiag",		PPpropSHADE_PATTERN, DOCspBGBDIAG ),
    RTF_PARA_ENUMR( "bgcross",		PPpropSHADE_PATTERN, DOCspBGCROSS ),
    RTF_PARA_ENUMR( "bgdcross",		PPpropSHADE_PATTERN, DOCspBGDCROSS ),
    RTF_PARA_ENUMR( "bgdkbdiag",	PPpropSHADE_PATTERN, DOCspBGDKBDIAG ),
    RTF_PARA_ENUMR( "bgdkcross",	PPpropSHADE_PATTERN, DOCspBGDKCROSS ),
    RTF_PARA_ENUMR( "bgdkdcross",	PPpropSHADE_PATTERN, DOCspBGDKDCROSS),
    RTF_PARA_ENUMR( "bgdkfdiag",	PPpropSHADE_PATTERN, DOCspBGDKFDIAG ),
    RTF_PARA_ENUMR( "bgdkhoriz",	PPpropSHADE_PATTERN, DOCspBGDKHORIZ ),
    RTF_PARA_ENUMR( "bgdkvert",		PPpropSHADE_PATTERN, DOCspBGDKVERT ),
    RTF_PARA_ENUMR( "bgfdiag",		PPpropSHADE_PATTERN, DOCspBGFDIAG ),
    RTF_PARA_ENUMR( "bghoriz",		PPpropSHADE_PATTERN, DOCspBGHORIZ ),
    RTF_PARA_ENUMR( "bgvert",		PPpropSHADE_PATTERN, DOCspBGVERT ),

    RTF_PARA_NUMBER( "cfpat",		PPpropSHADE_FORE_COLOR ),
    RTF_PARA_NUMBER( "cbpat",		PPpropSHADE_BACK_COLOR ),

    RTF_PARA_BORDER( "brdrt",		PPpropTOP_BORDER ),
    RTF_PARA_BORDER( "brdrb",		PPpropBOTTOM_BORDER ),
    RTF_PARA_BORDER( "brdrl",		PPpropLEFT_BORDER ),
    RTF_PARA_BORDER( "brdrr",		PPpropRIGHT_BORDER ),
    RTF_PARA_BORDER( "box",		PPpropBOX_BORDER ),
    RTF_PARA_BORDER( "brdrbtw",		PPpropBETWEEN_BORDER ),
    RTF_PARA_BORDER( "brdrbar",		PPpropBAR_BORDER ),

					/**/

    RTF_REMEMBER( "adjustright",	RTFidADJUSTRIGHT ),
    RTF_REMEMBER( "adjustleft",		RTFidADJUSTLEFT ),

    RTF_REMEMBER( "sbauto",		RTFidSBAUTO ),
    RTF_REMEMBER( "saauto",		RTFidSAAUTO ),

    RTF_REMEMBER( "nooverflow",		RTFidNOOVERFLOW ),

    RTF_REMEMBER( "aspalpha",		RTFidASPALPHA ),
    RTF_REMEMBER( "aspnum",		RTFidASPNUM ),

				/****************************************/
				/*  Positioned Objects and Frames:	*/
				/*  (Row properties.)			*/
				/****************************************/
    /* no TFPpropABSW */
    /* no TFPpropABSH */

    /* no TFPpropLOCKED_TO_PARA */

    /* no TFPpropWRAP_STYLE, FWSwrapNOWRAP */
    /* no TFPpropWRAP_STYLE, FWSwrapOVERLAY */

    RTF_ROW_FRAME_FLAG( "tabsnoovrlp",	TFPpropNO_OVERLAP ),

    RTF_ROW_FRAME_ENUM( "tphmrg",	TFPpropHORIZONTAL_REF, HFRphMRG ),
    RTF_ROW_FRAME_ENUM( "tphpg",	TFPpropHORIZONTAL_REF, HFRphPG ),
    RTF_ROW_FRAME_ENUM( "tphcol",	TFPpropHORIZONTAL_REF, HFRphCOL ),

    RTF_ROW_FRAME_NUMBER( "tposx",	TFPpropHORIZONTAL_POS ),
    RTF_ROW_FRAME_NUMBER( "tposnegx",	TFPpropHORIZONTAL_POS ),

    RTF_ROW_FRAME_ENUM( "tposxl",	TFPpropHORIZONTAL_ALIGN, HFAposXL ),
    RTF_ROW_FRAME_ENUM( "tposxr",	TFPpropHORIZONTAL_ALIGN, HFAposXR ),
    RTF_ROW_FRAME_ENUM( "tposxc",	TFPpropHORIZONTAL_ALIGN, HFAposXC ),
    RTF_ROW_FRAME_ENUM( "tposxi",	TFPpropHORIZONTAL_ALIGN, HFAposXI ),
    RTF_ROW_FRAME_ENUM( "tposxo",	TFPpropHORIZONTAL_ALIGN, HFAposXO ),

    RTF_ROW_FRAME_ENUM( "tpvmrg",	TFPpropVERTICAL_REF, VFRpvMRG ),
    RTF_ROW_FRAME_ENUM( "tpvpara",	TFPpropVERTICAL_REF, VFRpvPARA ),
    RTF_ROW_FRAME_ENUM( "tpvpg",	TFPpropVERTICAL_REF, VFRpvPG ),

    RTF_ROW_FRAME_NUMBER( "tposy",	TFPpropVERTICAL_POS ),
    RTF_ROW_FRAME_NUMBER( "tposnegy",	TFPpropVERTICAL_POS ),

    RTF_ROW_FRAME_ENUM( "tposyb",	TFPpropVERTICAL_ALIGN, VFAposYB ),
    RTF_ROW_FRAME_ENUM( "tposyc",	TFPpropVERTICAL_ALIGN, VFAposYC ),
    RTF_ROW_FRAME_ENUM( "tposyil",	TFPpropVERTICAL_ALIGN, VFAposYIL ),
    RTF_ROW_FRAME_ENUM( "tposyin",	TFPpropVERTICAL_ALIGN, VFAposYIN ),
    RTF_ROW_FRAME_ENUM( "tposyout",	TFPpropVERTICAL_ALIGN, VFAposYOUT ),
    RTF_ROW_FRAME_ENUM( "tposyt",	TFPpropVERTICAL_ALIGN, VFAposYT ),

    /*  no TFPpropDFRMTXT */
    /*  no TFPpropDFRMTXTX */
    /*  no TFPpropDFRMTXTY */

    RTF_ROW_FRAME_NUMBER( "tdfrmtxtLeft",	TFPpropDFRMTXTL ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtRight",	TFPpropDFRMTXTR ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtTop",		TFPpropDFRMTXTT ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtBottom",	TFPpropDFRMTXTB ),

				/****************************************/
				/*  Positioned Objects and Frames:	*/
				/*  (Paragraph properties.)		*/
				/****************************************/

    RTF_PARA_FRAME_NUMBER( "absw",	TFPpropABSW ),
    RTF_PARA_FRAME_NUMBER( "absh",	TFPpropABSH ),

    RTF_PARA_FRAME_FLAG( "abslock",	TFPpropLOCKED_TO_PARA ),

    RTF_PARA_FRAME_ENUM( "nowrap",	TFPpropWRAP_STYLE, FWSwrapNOWRAP ),
    RTF_PARA_FRAME_ENUM( "overlay",	TFPpropWRAP_STYLE, FWSwrapOVERLAY ),

    RTF_PARA_FRAME_FLAG( "absnoovrlp",	TFPpropNO_OVERLAP ),

    RTF_PARA_FRAME_ENUM( "phmrg",	TFPpropHORIZONTAL_REF, HFRphMRG ),
    RTF_PARA_FRAME_ENUM( "phpg",	TFPpropHORIZONTAL_REF, HFRphPG ),
    RTF_PARA_FRAME_ENUM( "phcol",	TFPpropHORIZONTAL_REF, HFRphCOL ),

    RTF_PARA_FRAME_NUMBER( "posx",	TFPpropHORIZONTAL_POS ),
    RTF_PARA_FRAME_NUMBER( "posnegx",	TFPpropHORIZONTAL_POS ),

    RTF_PARA_FRAME_ENUM( "posxl",	TFPpropHORIZONTAL_ALIGN, HFAposXL ),
    RTF_PARA_FRAME_ENUM( "posxr",	TFPpropHORIZONTAL_ALIGN, HFAposXR ),
    RTF_PARA_FRAME_ENUM( "posxc",	TFPpropHORIZONTAL_ALIGN, HFAposXC ),
    RTF_PARA_FRAME_ENUM( "posxi",	TFPpropHORIZONTAL_ALIGN, HFAposXI ),
    RTF_PARA_FRAME_ENUM( "posxo",	TFPpropHORIZONTAL_ALIGN, HFAposXO ),

    RTF_PARA_FRAME_ENUM( "pvmrg",	TFPpropVERTICAL_REF, VFRpvMRG ),
    RTF_PARA_FRAME_ENUM( "pvpara",	TFPpropVERTICAL_REF, VFRpvPARA ),
    RTF_PARA_FRAME_ENUM( "pvpg",	TFPpropVERTICAL_REF, VFRpvPG ),

    RTF_PARA_FRAME_NUMBER( "posy",	TFPpropVERTICAL_POS ),
    RTF_PARA_FRAME_NUMBER( "posnegy",	TFPpropVERTICAL_POS ),

    RTF_PARA_FRAME_ENUM( "posyb",	TFPpropVERTICAL_ALIGN, VFAposYB ),
    RTF_PARA_FRAME_ENUM( "posyc",	TFPpropVERTICAL_ALIGN, VFAposYC ),
    RTF_PARA_FRAME_ENUM( "posyil",	TFPpropVERTICAL_ALIGN, VFAposYIL ),
    RTF_PARA_FRAME_ENUM( "posyin",	TFPpropVERTICAL_ALIGN, VFAposYIN ),
    RTF_PARA_FRAME_ENUM( "posyout",	TFPpropVERTICAL_ALIGN, VFAposYOUT ),
    RTF_PARA_FRAME_ENUM( "posyt",	TFPpropVERTICAL_ALIGN, VFAposYT ),

    RTF_PARA_FRAME_NUMBER( "dxfrtext",	TFPpropDFRMTXT ),
    RTF_PARA_FRAME_NUMBER( "dfrmtxtx",	TFPpropDFRMTXTX ),
    RTF_PARA_FRAME_NUMBER( "dfrmtxty",	TFPpropDFRMTXTY ),

    /*  no TFPpropDFRMTXTL */
    /*  no TFPpropDFRMTXTR */
    /*  no TFPpropDFRMTXTT */
    /*  no TFPpropDFRMTXTB */

    /**/
    RTF_REMEMBER( "dropcapli",		RTFidDROPCAPLI ),
    RTF_REMEMBER( "dropcapt",		RTFidDROPCAPT ),
				/****************************************/
				/*  Revision marks for paragraph	*/
				/*  numbers.				*/
				/****************************************/
    RTF_REMEMBER( "pnrauth",		RTFidPNRAUTH ),
    RTF_REMEMBER( "pnrdate",		RTFidPNR_REVPROP ),
    RTF_REMEMBER( "pnrnot",		RTFidPNR_REVPROP ),
    RTF_REMEMBER( "pnrnfc",		RTFidPNRNFC ),
    RTF_REMEMBER( "pnrpnbr",		RTFidPNRPNBR ),
    RTF_REMEMBER( "pnrrgb",		RTFidPNRRGB ),
    RTF_REMEMBER( "pnrstart",		RTFidPNRSTART ),
    RTF_REMEMBER( "pnrstop",		RTFidPNRSTOP ),
    RTF_REMEMBER( "pnrxst",		RTFidPNRXST ),

				/****************************************/
				/*  Tabs.				*/
				/****************************************/
    RTF_REMEMBER( "tx",			RTFidTX ),
    RTF_REMEMBER( "tb",			RTFidTB ),

				/****************************************/
				/*  Associated Character Properties	*/
				/****************************************/
    RTF_REMEMBER( "ab",			RTFidAB ),
    RTF_REMEMBER( "acaps",		RTFidACAPS ),
    RTF_REMEMBER( "acf",		RTFidACF ),
    RTF_REMEMBER( "adn",		RTFidADN ),
    RTF_REMEMBER( "aexpnd",		RTFidAEXPND ),
    RTF_REMEMBER( "af",			RTFidAF ),
    RTF_REMEMBER( "afs",		RTFidAFS ),
    RTF_REMEMBER( "ai",			RTFidAI ),
    RTF_REMEMBER( "alang",		RTFidALANG ),
    RTF_REMEMBER( "aoutl",		RTFidAOUTL ),
    RTF_REMEMBER( "ascaps",		RTFidASCAPS ),
    RTF_REMEMBER( "ashad",		RTFidASHAD ),
    RTF_REMEMBER( "astrike",		RTFidASTRIKE ),
    RTF_REMEMBER( "aul",		RTFidAUL ),
    RTF_REMEMBER( "auld",		RTFidAULD ),
    RTF_REMEMBER( "auldb",		RTFidAULDB ),
    RTF_REMEMBER( "aulnone",		RTFidAULNONE ),
    RTF_REMEMBER( "aulw",		RTFidAULW ),
    RTF_REMEMBER( "aup",		RTFidAUP ),
    RTF_REMEMBER( "loch",		RTFidLOCH ),
    RTF_REMEMBER( "hich",		RTFidHICH ),
    RTF_REMEMBER( "dbch",		RTFidDBCH ),

				/****************************************/
				/*  Row, Cell/Column properties.	*/
				/****************************************/

    RTF_REMEMBER( "ltrrow",		RTFidLTRROW ),
    RTF_REMEMBER( "rtlrow",		RTFidRTLROW ),

    RTF_REMEMBER( "cellx",		RTFidCELLX ),

    RTF_CELL_DEFAULT( "celld" ),

    RTF_CELL_BORDER( "clbrdrt",		CLpropTOP_BORDER ),
    RTF_CELL_BORDER( "clbrdrb",		CLpropBOTTOM_BORDER ),
    RTF_CELL_BORDER( "clbrdrl",		CLpropLEFT_BORDER ),
    RTF_CELL_BORDER( "clbrdrr",		CLpropRIGHT_BORDER ),

    RTF_CELL_ENUM( "clbgbdiag",		CLpropSHADE_PATTERN, DOCspBGBDIAG ),
    RTF_CELL_ENUM( "clbgcross",		CLpropSHADE_PATTERN, DOCspBGCROSS ),
    RTF_CELL_ENUM( "clbgdcross",	CLpropSHADE_PATTERN, DOCspBGDCROSS ),
    RTF_CELL_ENUM( "clbgdkbdiag",	CLpropSHADE_PATTERN, DOCspBGDKBDIAG ),
    RTF_CELL_ENUM( "clbgdkcross",	CLpropSHADE_PATTERN, DOCspBGDKCROSS ),
    RTF_CELL_ENUM( "clbgdkdcross",	CLpropSHADE_PATTERN, DOCspBGDKDCROSS ),
    RTF_CELL_ENUM( "clbgdkfdiag",	CLpropSHADE_PATTERN, DOCspBGDKFDIAG ),
    RTF_CELL_ENUM( "clbgdkhor",		CLpropSHADE_PATTERN, DOCspBGDKHORIZ ),
    RTF_CELL_ENUM( "clbgdkvert",	CLpropSHADE_PATTERN, DOCspBGDKVERT ),
    RTF_CELL_ENUM( "clbgfdiag",		CLpropSHADE_PATTERN, DOCspBGFDIAG ),
    RTF_CELL_ENUM( "clbghoriz",		CLpropSHADE_PATTERN, DOCspBGHORIZ ),
    RTF_CELL_ENUM( "clbgvert",		CLpropSHADE_PATTERN, DOCspBGVERT ),

    RTF_CELL_NUMBER( "clcfpat",		CLpropSHADE_FORE_COLOR ),
    RTF_CELL_NUMBER( "clcbpat",		CLpropSHADE_BACK_COLOR ),
    RTF_CELL_NUMBER( "clshdng",		CLpropSHADE_LEVEL ),

    RTF_CELL_NUMBER( "clcfpatraw",	CLpropSHADE_FORE_COLOR ), /*?*/
    RTF_CELL_NUMBER( "clcbpatraw",	CLpropSHADE_BACK_COLOR ), /*?*/
    RTF_CELL_NUMBER( "clshdngraw",	CLpropSHADE_LEVEL ), /*?*/

    RTF_CELL_ENUM( "clvertalt",		CLpropTEXT_VERT_ALIGN,
							DOCtvaTOP ),
    RTF_CELL_ENUM( "clvertalc",		CLpropTEXT_VERT_ALIGN,
							DOCtvaCENTERED ),
    RTF_CELL_ENUM( "clvertalb",		CLpropTEXT_VERT_ALIGN,
							DOCtvaBOTTOM ),

    RTF_CELL_ENUM( "cltxlrtb",		CLpropTEXT_FLOW, CLflowTXLRTB ),
    RTF_CELL_ENUM( "cltxtbrl",		CLpropTEXT_FLOW, CLflowTXTBRL ),
    RTF_CELL_ENUM( "cltxbtlr",		CLpropTEXT_FLOW, CLflowTXBTLR ),
    RTF_CELL_ENUM( "cltxlrtbv",		CLpropTEXT_FLOW, CLflowTXLRTBV ),
    RTF_CELL_ENUM( "cltxtbrlv",		CLpropTEXT_FLOW, CLflowTXTBRLV ),

    RTF_CELL_FLAG( "clmgf",		CLpropLEFT_IN_MERGED ),
    RTF_CELL_FLAG( "clmrg",		CLpropMERGED_WITH_LEFT ),
    RTF_CELL_FLAG( "clvmgf",		CLpropTOP_IN_MERGED ),
    RTF_CELL_FLAG( "clvmrg",		CLpropMERGED_WITH_ABOVE ),
    RTF_CELL_FLAG( "clNoWrap",		CLpropNO_WRAP ),

    RTF_CELL_FLAG( "clshdrawnil",	CLpropNO_SHADING ),

    RTF_CELL_NUMBER( "clwWidth",		CLpropCLW_WIDTH ),
    RTF_CELL_NUMBER( "clftsWidth",	CLpropCLFTS_WIDTH ),

					/********************************/
					/* NOTE: Word 2003 swaps left	*/
					/*       and top.		*/
					/********************************/
    RTF_CELL_NUMBER( "clpadt",		CLpropLEFT_PADDING ), /* !!! */
    RTF_CELL_NUMBER( "clpadb",		CLpropBOTTOM_PADDING ),
    RTF_CELL_NUMBER( "clpadl",		CLpropTOP_PADDING ), /* !!! */
    RTF_CELL_NUMBER( "clpadr",		CLpropRIGHT_PADDING ),

    RTF_CELL_NUMBER( "clpadft",		CLpropLEFT_PADDING_UNIT ), /* !!! */
    RTF_CELL_NUMBER( "clpadfb",		CLpropBOTTOM_PADDING_UNIT ),
    RTF_CELL_NUMBER( "clpadfl",		CLpropTOP_PADDING_UNIT ), /* !!! */
    RTF_CELL_NUMBER( "clpadfr",		CLpropRIGHT_PADDING_UNIT ),

				/****************************************/
				/*  Row properties.			*/
				/****************************************/

    RTF_ROW_DEFAULT( "trowd" ),

    RTF_ROW_NUMBER( "trgaph",		RPpropGAP_WIDTH ),
    RTF_ROW_NUMBER( "trleft",		RPpropLEFT_INDENT ),
    RTF_ROW_NUMBER( "trrh",		RPpropHEIGHT ),

    RTF_ROW_FLAG( "trhdr",		RPpropIS_TABLE_HEADER ),
    RTF_ROW_FLAG( "trkeep",		RPpropKEEP_ON_ONE_PAGE ),
    RTF_ROW_FLAG( "trkeepfollow",	RPpropKEEP_WITH_NEXT ),
    RTF_ROW_FLAG( "trautofit",		RPpropAUTOFIT ),

    RTF_ROW_ENUM( "trql",		RPpropALIGNMENT,	DOCiaLEFT ),
    RTF_ROW_ENUM( "trqr",		RPpropALIGNMENT,	DOCiaRIGHT ),
    RTF_ROW_ENUM( "trqc",		RPpropALIGNMENT,	DOCiaCENTERED ),

    /**/
    RTF_ROW_BORDER( "trbrdrt",		RPpropTOP_BORDER ),
    RTF_ROW_BORDER( "trbrdrb",		RPpropBOTTOM_BORDER ),
    RTF_ROW_BORDER( "trbrdrl",		RPpropLEFT_BORDER ),
    RTF_ROW_BORDER( "trbrdrr",		RPpropRIGHT_BORDER ),
    RTF_ROW_BORDER( "trbrdrh",		RPpropHORIZ_BORDER ),
    RTF_ROW_BORDER( "trbrdrv",		RPpropVERT_BORDER ),

    /**/
    RTF_ROW_NUMBER( "trwWidth",		RPpropTRW_WIDTH ),
    RTF_ROW_NUMBER( "trftsWidth",	RPpropTRFTS_WIDTH ),

    /**/
    RTF_ROW_NUMBER( "trspdl",		RPpropTRSPDL ),
    RTF_ROW_NUMBER( "trspdr",		RPpropTRSPDR ),
    RTF_ROW_NUMBER( "trspdt",		RPpropTRSPDT ),
    RTF_ROW_NUMBER( "trspdb",		RPpropTRSPDB ),

    RTF_ROW_NUMBER( "trspdfl",		RPpropTRSPDFL ),
    RTF_ROW_NUMBER( "trspdfr",		RPpropTRSPDFR ),
    RTF_ROW_NUMBER( "trspdft",		RPpropTRSPDFT ),
    RTF_ROW_NUMBER( "trspdfb",		RPpropTRSPDFB ),

    /**/
    RTF_ROW_NUMBER( "trpaddl",		RPpropTRPADDL ),
    RTF_ROW_NUMBER( "trpaddr",		RPpropTRPADDR ),
    RTF_ROW_NUMBER( "trpaddt",		RPpropTRPADDT ),
    RTF_ROW_NUMBER( "trpaddb",		RPpropTRPADDB ),

    RTF_ROW_NUMBER( "trpaddfl",		RPpropTRPADDFL ),
    RTF_ROW_NUMBER( "trpaddfr",		RPpropTRPADDFR ),
    RTF_ROW_NUMBER( "trpaddft",		RPpropTRPADDFT ),
    RTF_ROW_NUMBER( "trpaddfb",		RPpropTRPADDFB ),

    /**/
    RTF_ROW_NUMBER( "trwWidthB",		RPpropTRW_WIDTHB ),
    RTF_ROW_NUMBER( "trwWidthA",		RPpropTRW_WIDTHA ),

    RTF_ROW_NUMBER( "trftsWidthB",	RPpropTRFTS_WIDTHB ),
    RTF_ROW_NUMBER( "trftsWidthA",	RPpropTRFTS_WIDTHA ),

    /**/
    RTF_ROW_ENUM( "trbgbdiag",		RPpropSHADE_PATTERN, DOCspBGBDIAG ),
    RTF_ROW_ENUM( "trbgcross",		RPpropSHADE_PATTERN, DOCspBGCROSS ),
    RTF_ROW_ENUM( "trbgdcross",		RPpropSHADE_PATTERN, DOCspBGDCROSS ),
    RTF_ROW_ENUM( "trbgdkbdiag",	RPpropSHADE_PATTERN, DOCspBGDKBDIAG ),
    RTF_ROW_ENUM( "trbgdkcross",	RPpropSHADE_PATTERN, DOCspBGDKCROSS ),
    RTF_ROW_ENUM( "trbgdkdcross",	RPpropSHADE_PATTERN, DOCspBGDKDCROSS ),
    RTF_ROW_ENUM( "trbgdkfdiag",	RPpropSHADE_PATTERN, DOCspBGDKFDIAG ),
    RTF_ROW_ENUM( "trbgdkhor",		RPpropSHADE_PATTERN, DOCspBGDKHORIZ ),
    RTF_ROW_ENUM( "trbgdkvert",		RPpropSHADE_PATTERN, DOCspBGDKVERT ),
    RTF_ROW_ENUM( "trbgfdiag",		RPpropSHADE_PATTERN, DOCspBGFDIAG ),
    RTF_ROW_ENUM( "trbghoriz",		RPpropSHADE_PATTERN, DOCspBGHORIZ ),
    RTF_ROW_ENUM( "trbgvert",		RPpropSHADE_PATTERN, DOCspBGVERT ),

    RTF_ROW_NUMBER( "trcfpat",		RPpropSHADE_FORE_COLOR ),
    RTF_ROW_NUMBER( "trcbpat",		RPpropSHADE_BACK_COLOR ),
    RTF_ROW_NUMBER( "trshdng",		RPpropSHADE_LEVEL ),

    RTF_ROW_FLAG( "tbllkborder",	RPpropAUTOFORMAT_BORDERS ),
    RTF_ROW_FLAG( "tbllkshading",	RPpropAUTOFORMAT_SHADING ),
    RTF_ROW_FLAG( "tbllkfont",		RPpropAUTOFORMAT_FONT ),
    RTF_ROW_FLAG( "tbllkcolor",		RPpropAUTOFORMAT_COLOR ),
    RTF_ROW_FLAG( "tbllkbestfit",	RPpropAUTOFORMAT_APPLY_BEST_FIT ),
    RTF_ROW_FLAG( "tbllkhdrrows",	RPpropAUTOFORMAT_FIRST_ROW ),
    RTF_ROW_FLAG( "tbllklastrow",	RPpropAUTOFORMAT_LAST_ROW ),
    RTF_ROW_FLAG( "tbllkhdrcols",	RPpropAUTOFORMAT_FIRST_COLUMN ),
    RTF_ROW_FLAG( "tbllklastcol",	RPpropAUTOFORMAT_LAST_COLUMN ),

    RTF_ROW_NUMBER( "irow",		RPpropROW_NUMBER ),
    RTF_ROW_NUMBER( "irowband",		RPpropROW_BAND_NUMBER ),
    RTF_ROW_NUMBER( "ts",		RPpropROW_STYLE ),

    RTF_ROW_FLAG( "lastrow",		RPpropIS_LAST_ROW ),
    RTF_ROW_FLAG( "trauth",		RPpropTRAUTH ),

				/****************************************/
				/*  Document formatting.		*/
				/****************************************/

# define	RTF_DOC_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, }
# define	RTF_DOC_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, }
# define	RTF_DOC_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, v }
# define	RTF_DOC_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

    RTF_DOC_NUMBER( "margl",		DGpropLEFT_MARGIN ),
    RTF_DOC_NUMBER( "margr",		DGpropRIGHT_MARGIN ),
    RTF_DOC_NUMBER( "margt",		DGpropTOP_MARGIN ),
    RTF_DOC_NUMBER( "margb",		DGpropBOTTOM_MARGIN ),
    RTF_DOC_NUMBER( "paperh",		DGpropPAGE_HEIGHT ),
    RTF_DOC_NUMBER( "paperw",		DGpropPAGE_WIDTH ),
    RTF_DOC_NUMBER( "gutter",		DGpropGUTTER ),

    RTF_DOC_FLAG( "facingp",		DPpropFACING_PAGES ),
    RTF_DOC_FLAG( "widowctrl",		DPpropWIDOWCTRL ),
    RTF_DOC_FLAG( "twoonone",		DPpropTWO_ON_ONE ),
    RTF_DOC_FLAG( "doctemp",		DPpropDOCTEMP ),

    RTF_REMEMBER( "psz",		RTFidPSZ ),

    RTF_REMEMBER( "jcompress",		RTFidJCOMPRESS),
    RTF_REMEMBER( "jexpand",		RTFidJEXPAND),

    RTF_REMEMBER( "horzdoc",		RTFidHORZDOC),
    RTF_REMEMBER( "dghspace",		RTFidDGHSPACE),
    RTF_REMEMBER( "dgvspace",		RTFidDGVSPACE),
    RTF_REMEMBER( "dghorigin",		RTFidDGHORIGIN),
    RTF_REMEMBER( "dgvorigin",		RTFidDGVORIGIN),
    RTF_REMEMBER( "dghshow",		RTFidDGHSHOW),
    RTF_REMEMBER( "dgvshow",		RTFidDGVSHOW),
    RTF_REMEMBER( "dgmargin",		RTFidDGMARGIN),

    RTF_DOC_NUMBER( "pgnstart",		DPpropSTART_PAGE ),
    RTF_DOC_NUMBER( "deftab",		DPpropDEFTAB ),

    RTF_DOC_NUMBER( "deff",		DPpropDEFF ),
    RTF_DOC_NUMBER( "stshfdbch",		DPpropSTSHFDBCH ),
    RTF_DOC_NUMBER( "stshfloch",		DPpropSTSHFLOCH ),
    RTF_DOC_NUMBER( "stshfhich",		DPpropSTSHFHICH ),
    RTF_DOC_NUMBER( "stshfbi",		DPpropSTSHFBI ),

    RTF_DOC_BORDER( "pgbrdrt",		DPpropTOP_BORDER ),
    RTF_DOC_BORDER( "pgbrdrb",		DPpropBOTTOM_BORDER ),
    RTF_DOC_BORDER( "pgbrdrl",		DPpropLEFT_BORDER ),
    RTF_DOC_BORDER( "pgbrdrr",		DPpropRIGHT_BORDER ),
    RTF_DOC_BORDER( "pgbrdrhead",	DPpropHEAD_BORDER ),
    RTF_DOC_BORDER( "pgbrdrfoot",	DPpropFOOT_BORDER ),

    RTF_REMEMBER( "truncex",		RTFidTRUNCEX ),
    RTF_REMEMBER( "nolead",		RTFidNOLEAD ),
    RTF_REMEMBER( "msmcap",		RTFidMSMCAP ),
    RTF_REMEMBER( "splytwnine",		RTFidSPLYTWNINE ),
    RTF_REMEMBER( "ftnlytwnine",	RTFidFTNLYTWNINE ),
    RTF_REMEMBER( "htmautsp",		RTFidHTMAUTSP ),
    RTF_REMEMBER( "useltbaln",		RTFidUSELTBALN ),
    RTF_REMEMBER( "alntblind",		RTFidALNTBLIND ),
    RTF_REMEMBER( "lytcalctblwd",	RTFidLYTCALCTBLWD ),
    RTF_REMEMBER( "lyttblrtgr",		RTFidLYTTBLRTGR ),
    RTF_REMEMBER( "lnbrkrule",		RTFidLNBRKRULE ),

    RTF_REMEMBER( "bdrrlswsix",		RTFidBDRRLSWSIX ),
    RTF_REMEMBER( "oldas",		RTFidOLDAS ),

    RTF_DOC_FLAG( "sprstsm",		DPpropIGNORED ),
    RTF_DOC_FLAG( "pgbrdropt",		DPpropIGNORED ),
    RTF_DOC_FLAG( "pgbrdrsnap",		DPpropIGNORED ),
    RTF_DOC_FLAG( "allowfieldendsel",	DPpropIGNORED ),
    RTF_DOC_FLAG( "nobrkwrptbl",	DPpropIGNORED ),
    RTF_DOC_FLAG( "snaptogridincell",	DPpropIGNORED ),
    RTF_DOC_FLAG( "wrppunct",		DPpropIGNORED ),
    RTF_DOC_FLAG( "asianbrkrule",	DPpropIGNORED ),
    RTF_DOC_FLAG( "newtblstyruls",	DPpropIGNORED ),
    RTF_DOC_FLAG( "nogrowautofit",	DPpropIGNORED ),
    RTF_DOC_FLAG( "rempersonalinfo",	DPpropIGNORED ),
    RTF_DOC_FLAG( "nojkernpunct",	DPpropIGNORED ),
    RTF_DOC_FLAG( "doctype",		DPpropIGNORED ),


				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    RTF_REMEMBER( "noxlattoyen",	RTFidNOXLATTOYEN ),
    RTF_REMEMBER( "expshrtn",		RTFidEXPSHRTN ),
    RTF_REMEMBER( "noultrlspc",		RTFidNOULTRLSPC ),
    RTF_REMEMBER( "dntblnsbdb",		RTFidDNTBLNSBDB ),
    RTF_REMEMBER( "nospaceforul",	RTFidNOSPACEFORUL ),

				/****************************************/
				/*  Paragraph Numbering.		*/
				/****************************************/
    RTF_REMEMBER( "pnhang",		RTFidPNHANG ),

    RTF_REMEMBER( "pnuld",		RTFidPNULD ),
    RTF_REMEMBER( "pnuldb",		RTFidPNULDB ),
    RTF_REMEMBER( "pnulnone",		RTFidPNULNONE ),

				/****************************************/
				/*  Section properties.			*/
				/****************************************/
    RTF_SECT_DEFAULT( "sectd" ),

    RTF_SECT_NUMBER( "headery",		DGpropHEADER_POSITION ),
    RTF_SECT_NUMBER( "footery",		DGpropFOOTER_POSITION ),
    RTF_SECT_NUMBER( "pgwsxn",		DGpropPAGE_WIDTH ),
    RTF_SECT_NUMBER( "pghsxn",		DGpropPAGE_HEIGHT ),
    RTF_SECT_NUMBER( "marglsxn",		DGpropLEFT_MARGIN ),
    RTF_SECT_NUMBER( "margtsxn",		DGpropTOP_MARGIN ),
    RTF_SECT_NUMBER( "margrsxn",		DGpropRIGHT_MARGIN ),
    RTF_SECT_NUMBER( "margbsxn",		DGpropBOTTOM_MARGIN ),
    RTF_SECT_NUMBER( "guttersxn",	DGpropGUTTER ),

    RTF_SECT_NUMBER( "ds",		SPpropSTYLE ),

    RTF_SECT_FLAG( "titlepg",		SPpropTITLEPG ),

    RTF_SECT_ENUM( "sbknone",		SPpropBREAK_KIND, DOCsbkNONE ),
    RTF_SECT_ENUM( "sbkcol",		SPpropBREAK_KIND, DOCsbkCOL ),
    RTF_SECT_ENUM( "sbkpage",		SPpropBREAK_KIND, DOCsbkPAGE ),
    RTF_SECT_ENUM( "sbkeven",		SPpropBREAK_KIND, DOCsbkEVEN ),
    RTF_SECT_ENUM( "sbkodd",		SPpropBREAK_KIND, DOCsbkODD ),

    RTF_SECT_ENUM( "pgndec",		SPpropNUMBER_STYLE, DOCpgnDEC ),
    RTF_SECT_ENUM( "pgnucrm",		SPpropNUMBER_STYLE, DOCpgnUCRM ),
    RTF_SECT_ENUM( "pgnlcrm",		SPpropNUMBER_STYLE, DOCpgnLCRM ),
    RTF_SECT_ENUM( "pgnucltr",		SPpropNUMBER_STYLE, DOCpgnUCLTR ),
    RTF_SECT_ENUM( "pgnlcltr",		SPpropNUMBER_STYLE, DOCpgnLCLTR ),

    RTF_SECT_ENUM( "pgncont",		SPpropPAGE_RESTART, 0 ),
    RTF_SECT_ENUM( "pgnrestart",	SPpropPAGE_RESTART, 1 ),

    RTF_SECT_NUMBER( "pgnstarts",	SPpropSTART_PAGE ),

    RTF_SECT_NUMBER( "cols",		SPpropCOLUMN_COUNT ),
    RTF_SECT_NUMBER( "colsx",		SPpropCOLUMN_SPACING ),
    RTF_SECT_NUMBER( "colw",		SPpropCOLUMN_WIDTH ),

    /**/

    RTF_REMEMBER( "sectdefaultcl",	RTFidSECTDEFAULTCL ),

    RTF_REMEMBER( "pgnhn",		RTFidPGNHN ),
    RTF_REMEMBER( "pgnhnsh",		RTFidPGNHNSH ),
    RTF_REMEMBER( "pgnhnsp",		RTFidPGNHNSP ),
    RTF_REMEMBER( "pgnhnsc",		RTFidPGNHNSC ),
    RTF_REMEMBER( "pgnhnsm",		RTFidPGNHNSM ),
    RTF_REMEMBER( "pgnhnsn",		RTFidPGNHNSN ),

    RTF_REMEMBER( "pgnx",		RTFidPGNX ),
    RTF_REMEMBER( "pgny",		RTFidPGNY ),

    RTF_REMEMBER( "linex",		RTFidLINEX ),

    RTF_REMEMBER( "colno",		RTFidCOLNO ),
    RTF_REMEMBER( "colsr",		RTFidCOLSR ),

    RTF_REMEMBER( "vertalb",		RTFidVERTALB ),
    RTF_REMEMBER( "vertalc",		RTFidVERTALC ),
    RTF_REMEMBER( "vertalj",		RTFidVERTALJ ),
    RTF_REMEMBER( "vertalt",		RTFidVERTALT ),

    RTF_REMEMBER( "sectlinegrid",	RTFidSECTLINEGRID ),

    RTF_REMEMBER( "endnhere",		RTFidENDNHERE ),
    RTF_REMEMBER( "binfsxn",		RTFidBINFSXN ),
    RTF_REMEMBER( "binsxn",		RTFidBINSXN ),
    RTF_REMEMBER( "linebetcol",		RTFidLINEBETCOL ),
    RTF_REMEMBER( "linecont",		RTFidLINECONT ),
    RTF_REMEMBER( "linemod",		RTFidLINEMOD ),
    RTF_REMEMBER( "lineppage",		RTFidLINEPPAGE ),
    RTF_REMEMBER( "linerestart",	RTFidLINERESTART ),
    RTF_REMEMBER( "linestarts",		RTFidLINESTARTS ),
    RTF_REMEMBER( "lndscpsxn",		RTFidLNDSCPSXN ),
    RTF_REMEMBER( "ltrsect",		RTFidLTRSECT ),
    RTF_REMEMBER( "rtlsect",		RTFidRTLSECT ),
    RTF_REMEMBER( "sectunlocked",	RTFidSECTUNLOCKED ),

				/****************************************/
				/*  Section properties: (Notes)		*/
				/****************************************/

    RTF_SECT_ENUM( "sftntj",		SPpropFOOTNOTE_POSITION,
						    FTN_POS_BELOW_TEXT ),
    RTF_SECT_ENUM( "sftnbj",		SPpropFOOTNOTE_POSITION,
						    FTN_POS_PAGE_BOTTOM ),

    RTF_SECT_NUMBER( "sftnstart",		SPpropFOOTNOTE_STARTNR ),
    RTF_SECT_NUMBER( "saftnstart",	SPpropENDNOTE_STARTNR ),

    RTF_SECT_ENUM( "sftnrstpg",		SPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_PAGE ),
    RTF_SECT_ENUM( "sftnrestart",	SPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_SECTION ),
    RTF_SECT_ENUM( "sftnrstcont",	SPpropFOOTNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),

    RTF_SECT_ENUM( "saftnrstcont",	SPpropENDNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),
    RTF_SECT_ENUM( "saftnrestart",	SPpropENDNOTE_RESTART,
						    FTN_RST_PER_SECTION ),

    RTF_SECT_ENUM( "sftnnar",		SPpropFOOTNOTE_STYLE, FTNstyleNAR ),
    RTF_SECT_ENUM( "sftnnalc",		SPpropFOOTNOTE_STYLE, FTNstyleNALC ),
    RTF_SECT_ENUM( "sftnnauc",		SPpropFOOTNOTE_STYLE, FTNstyleNAUC ),
    RTF_SECT_ENUM( "sftnnrlc",		SPpropFOOTNOTE_STYLE, FTNstyleNRLC ),
    RTF_SECT_ENUM( "sftnnruc",		SPpropFOOTNOTE_STYLE, FTNstyleNRUC ),
    RTF_SECT_ENUM( "sftnnchi",		SPpropFOOTNOTE_STYLE, FTNstyleNCHI ),

    RTF_SECT_ENUM( "saftnnar",		SPpropENDNOTE_STYLE, FTNstyleNAR ),
    RTF_SECT_ENUM( "saftnnalc",		SPpropENDNOTE_STYLE, FTNstyleNALC ),
    RTF_SECT_ENUM( "saftnnauc",		SPpropENDNOTE_STYLE, FTNstyleNAUC ),
    RTF_SECT_ENUM( "saftnnrlc",		SPpropENDNOTE_STYLE, FTNstyleNRLC ),
    RTF_SECT_ENUM( "saftnnruc",		SPpropENDNOTE_STYLE, FTNstyleNRUC ),
    RTF_SECT_ENUM( "saftnnchi",		SPpropENDNOTE_STYLE, FTNstyleNCHI ),

				/****************************************/
				/*  Document properties: (Notes)	*/
				/****************************************/

    RTF_DOC_NUMBER( "fet",		DPpropNOTETYPES ),

    RTF_DOC_ENUM( "endnotes",		DPpropFOOTNOTE_POSITION,
						    FTN_POS_SECT_END ),
    RTF_DOC_ENUM( "enddoc",		DPpropFOOTNOTE_POSITION,
						    FTN_POS_DOC_END ),
    RTF_DOC_ENUM( "ftntj",		DPpropFOOTNOTE_POSITION,
						    FTN_POS_BELOW_TEXT ),
    RTF_DOC_ENUM( "ftnbj",		DPpropFOOTNOTE_POSITION,
						    FTN_POS_PAGE_BOTTOM ),

    RTF_DOC_ENUM( "aendnotes",		DPpropENDNOTE_POSITION,
						    FTN_POS_SECT_END ),
    RTF_DOC_ENUM( "aenddoc",		DPpropENDNOTE_POSITION,
						    FTN_POS_DOC_END ),
    RTF_DOC_ENUM( "aftntj",		DPpropENDNOTE_POSITION,
						    FTN_POS_BELOW_TEXT ),
    RTF_DOC_ENUM( "aftnbj",		DPpropENDNOTE_POSITION,
						    FTN_POS_PAGE_BOTTOM ),

    RTF_DOC_NUMBER( "ftnstart",		DPpropFOOTNOTE_STARTNR ),
    RTF_DOC_NUMBER( "aftnstart",		DPpropENDNOTE_STARTNR ),

    RTF_DOC_ENUM( "ftnrstpg",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_PAGE ),
    RTF_DOC_ENUM( "ftnrestart",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_SECTION ),
    RTF_DOC_ENUM( "ftnrstcont",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),

    RTF_DOC_ENUM( "aftnrstcont",	DPpropENDNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),
    RTF_DOC_ENUM( "aftnrestart",	DPpropENDNOTE_RESTART,
						    FTN_RST_PER_SECTION ),

    RTF_DOC_ENUM( "ftnnar",		DPpropFOOTNOTE_STYLE, FTNstyleNAR ),
    RTF_DOC_ENUM( "ftnnalc",		DPpropFOOTNOTE_STYLE, FTNstyleNALC ),
    RTF_DOC_ENUM( "ftnnauc",		DPpropFOOTNOTE_STYLE, FTNstyleNAUC ),
    RTF_DOC_ENUM( "ftnnrlc",		DPpropFOOTNOTE_STYLE, FTNstyleNRLC ),
    RTF_DOC_ENUM( "ftnnruc",		DPpropFOOTNOTE_STYLE, FTNstyleNRUC ),
    RTF_DOC_ENUM( "ftnnchi",		DPpropFOOTNOTE_STYLE, FTNstyleNCHI ),

    RTF_DOC_ENUM( "aftnnar",		DPpropENDNOTE_STYLE, FTNstyleNAR ),
    RTF_DOC_ENUM( "aftnnalc",		DPpropENDNOTE_STYLE, FTNstyleNALC ),
    RTF_DOC_ENUM( "aftnnauc",		DPpropENDNOTE_STYLE, FTNstyleNAUC ),
    RTF_DOC_ENUM( "aftnnrlc",		DPpropENDNOTE_STYLE, FTNstyleNRLC ),
    RTF_DOC_ENUM( "aftnnruc",		DPpropENDNOTE_STYLE, FTNstyleNRUC ),
    RTF_DOC_ENUM( "aftnnchi",		DPpropENDNOTE_STYLE, FTNstyleNCHI ),

    RTF_REMEMBER( "ftnalt",		RTFidFTNALT ),

				/****************************************/
				/*  StyleSheet.				*/
				/****************************************/
    RTF_STYLE_FLAG( "additive",		DSpropADDITIVE ),
    RTF_STYLE_NUMBER( "sbasedon",	DSpropBASED_ON ),
    RTF_STYLE_NUMBER( "snext",		DSpropNEXT ),
    RTF_STYLE_NUMBER( "slink",		DSpropLINK ),

    /*

    Handeled for the style where the reference to the style is handeled.

    RTF_STYLE_VAL_ENUM( "ds",		DSpropSTYLE_NUMBER, DOClevSECT ),
    RTF_STYLE_VAL_ENUM( "s",		DSpropSTYLE_NUMBER, DOClevPARA ),
    RTF_STYLE_VAL_ENUM( "cs",		DSpropSTYLE_NUMBER, DOClevTEXT ),
    */

    RTF_STYLE_FLAG( "sautoupd",		DSpropAUTO_UPD ),
    RTF_STYLE_FLAG( "shidden",		DSpropHIDDEN ),
    RTF_STYLE_FLAG( "slocked",		DSpropLOCKED ),
    RTF_STYLE_FLAG( "ssemihidden",	DSpropSEMI_HIDDEN ),
    RTF_STYLE_FLAG( "spersonal",	DSpropPERSONAL ),

				/****************************************/
				/*  Encoding.				*/
				/****************************************/
    RTF_DOC_ENUM( "ansi",		DPpropDOC_CHARSET, DOCcharsetANSI ),
    RTF_DOC_ENUM( "mac",		DPpropDOC_CHARSET, DOCcharsetMAC ),
    RTF_DOC_ENUM( "pc",			DPpropDOC_CHARSET, DOCcharsetPC ),
    RTF_DOC_ENUM( "pca",		DPpropDOC_CHARSET, DOCcharsetPCA ),
    RTF_DOC_NUMBER( "ansicpg",		DPpropANSICPG ),

				/****************************************/
				/*  Unknown				*/
				/****************************************/
    RTF_REMEMBER( "itap",		RTFidITAP ),
    RTF_REMEMBER( "lin",		RTFidLIN ),
    RTF_REMEMBER( "rin",		RTFidRIN ),
    RTF_REMEMBER( "langnp",		RTFidLANGNP ),
    RTF_REMEMBER( "langfe",		RTFidLANGFE ),
    RTF_REMEMBER( "langfenp",		RTFidLANGFENP ),
    RTF_REMEMBER( "nolnhtadjtbl",	RTFidNOLNHTADJTBL ),

				/****************************************/
				/*  Document Formatting properties.	*/
				/****************************************/
    RTF_REMEMBER( "allprot",		RTFidALLPROT ),
    RTF_REMEMBER( "annotprot",		RTFidANNOTPROT ),
    RTF_REMEMBER( "brkfrm",		RTFidBRKFRM ),
    RTF_REMEMBER( "cvmme",		RTFidCVMME ),
    RTF_REMEMBER( "defformat",		RTFidDEFFORMAT ),
    RTF_REMEMBER( "deflang",		RTFidDEFLANG ),
    RTF_REMEMBER( "fldalt",		RTFidFLDALT ),
    RTF_REMEMBER( "formdisp",		RTFidFORMDISP ),
    RTF_REMEMBER( "formprot",		RTFidFORMPROT ),
    RTF_REMEMBER( "formshade",		RTFidFORMSHADE ),
    RTF_REMEMBER( "fracwidth",		RTFidFRACWIDTH ),
    RTF_REMEMBER( "hyphauto",		RTFidHYPHAUTO ),
    RTF_REMEMBER( "hyphcaps",		RTFidHYPHCAPS ),
    RTF_REMEMBER( "hyphconsec",		RTFidHYPHCONSEC ),
    RTF_REMEMBER( "hyphhotz",		RTFidHYPHHOTZ ),
    RTF_REMEMBER( "landscape",		RTFidLANDSCAPE ),
    RTF_REMEMBER( "linestart",		RTFidLINESTART ),
    RTF_REMEMBER( "linkstyles",		RTFidLINKSTYLES ),
    RTF_REMEMBER( "ltrdoc",		RTFidLTRDOC ),
    RTF_REMEMBER( "makebackup",		RTFidMAKEBACKUP ),
    RTF_REMEMBER( "margmirror",		RTFidMARGMIRROR ),
    RTF_REMEMBER( "nocolbal",		RTFidNOCOLBAL ),
    RTF_REMEMBER( "noextrasprl",	RTFidNOEXTRASPRL ),
    RTF_REMEMBER( "notabind",		RTFidNOTABIND ),
    RTF_REMEMBER( "otblrul",		RTFidOTBLRUL ),
    RTF_REMEMBER( "prcolbl",		RTFidPRCOLBL ),
    RTF_REMEMBER( "printdata",		RTFidPRINTDATA ),
    RTF_REMEMBER( "psover",		RTFidPSOVER ),
    RTF_REMEMBER( "revbar",		RTFidREVBAR ),
    RTF_REMEMBER( "revisions",		RTFidREVISIONS ),
    RTF_REMEMBER( "revprop",		RTFidREVPROP ),
    RTF_REMEMBER( "revprot",		RTFidREVPROT ),
    RTF_REMEMBER( "rtldoc",		RTFidRTLDOC ),
    RTF_REMEMBER( "sprslnsp",		RTFidSPRSLNSP ),
    RTF_REMEMBER( "sprsspbf",		RTFidSPRSSPBF ),
    RTF_REMEMBER( "sprstsp",		RTFidSPRSTSP ),
    RTF_REMEMBER( "subfontbysize",	RTFidSUBFONTBYSIZE ),
    RTF_REMEMBER( "swpbdr",		RTFidSWPBDR ),
    RTF_REMEMBER( "transmf",		RTFidTRANSMF ),
    RTF_REMEMBER( "ight",		RTFidIGHT ),
    RTF_REMEMBER( "wraptrsp",		RTFidWRAPTRSP ),
    RTF_REMEMBER( "viewzk",		RTFidVIEWZK ),
    RTF_REMEMBER( "viewkind",		RTFidVIEWKIND ),
    RTF_REMEMBER( "viewscale",		RTFidVIEWSCALE ),
    RTF_REMEMBER( "lytprtmet",		RTFidLYTPRTMET ),
    RTF_REMEMBER( "ogutter",		RTFidOGUTTER ),
    RTF_REMEMBER( "rtlgutter",		RTFidRTLGUTTER ),
    RTF_REMEMBER( "deflangfe",		RTFidDEFLANGFE ),
    RTF_REMEMBER( "uc",			RTFidUC ),
    RTF_REMEMBER( "level",		RTFidLEVEL ),
    RTF_REMEMBER( "ltrpar",		RTFidLTRPAR ),
    RTF_REMEMBER( "noline",		RTFidNOLINE ),
    RTF_REMEMBER( "rtlpar",		RTFidRTLPAR ),
    RTF_REMEMBER( "sbys",		RTFidSBYS ),
    RTF_REMEMBER( "subdocument",	RTFidSUBDOCUMENT ),

				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
    RTF_REMEMBER( "hr",			RTFidHR ),
    RTF_REMEMBER( "min",		RTFidMIN ),
    RTF_REMEMBER( "sec",		RTFidSEC ),
    RTF_REMEMBER( "dy",			RTFidDY ),
    RTF_REMEMBER( "mo",			RTFidMO ),
    RTF_REMEMBER( "yr",			RTFidYR ),

				/****************************************/
				/*  Unsupported W2002 revision		*/
				/*  control.				*/
				/****************************************/
    RTF_REMEMBER( "insrsid",		RTFidRSID ),
    RTF_REMEMBER( "rsidroot",		RTFidRSID ),
    RTF_REMEMBER( "delrsid",		RTFidRSID ),
    RTF_REMEMBER( "charrsid",		RTFidRSID ),
    RTF_REMEMBER( "sectrsid",		RTFidRSID ),
    RTF_REMEMBER( "pararsid",		RTFidRSID ),
    RTF_REMEMBER( "tblrsid",		RTFidRSID ),
    RTF_REMEMBER( "styrsid",		RTFidRSID ),

				/****************************************/
				/*  List table &c.			*/
				/****************************************/
    RTF_LISTLEV_NUMBER( "levelstartat",		LLpropSTARTAT ),
    RTF_LISTLEV_NUMBER( "levelnfcn",		LLpropSTYLE ),
    RTF_LISTLEV_NUMBER( "levelnfc",		LLpropSTYLE ),		/**/
    RTF_LISTLEV_NUMBER( "leveljcn",		LLpropJUSTIFY ),
    RTF_LISTLEV_NUMBER( "leveljc",		LLpropJUSTIFY ),	/**/
    RTF_LISTLEV_NUMBER( "levelfollow",		LLpropFOLLOW ),
    RTF_LISTLEV_NUMBER( "levellegal",		LLpropLEGAL ),
    RTF_LISTLEV_NUMBER( "levelnorestart",	LLpropNORESTART ),
    RTF_LISTLEV_NUMBER( "levelpicture",		LLpropPICTURE ),
    RTF_LISTLEV_NUMBER( "fbias",			LLpropFBIAS ),

    RTF_LISTLEV_NUMBER( "leveltemplateid",	LLpropTEMPLATEID ),

    RTF_LISTLEV_NUMBER( "levelold",		LLpropOLD ),		/**/
    RTF_LISTLEV_NUMBER( "levelprev",		LLpropPREV ),		/**/
    RTF_LISTLEV_NUMBER( "levelprevspace",	LLpropPREVSPACE ),	/**/
    RTF_LISTLEV_NUMBER( "levelindent",		LLpropINDENT ),		/**/
    RTF_LISTLEV_NUMBER( "levelspace",		LLpropSPACE ),		/**/

    RTF_LIST_NUMBER( "listid",			DLpropLISTID ),
    RTF_LIST_NUMBER( "listtemplateid",		DLpropTEMPLATEID ),
    RTF_LIST_ENUM( "listsimple",		DLpropHYBRID, 0 ),
    RTF_LIST_ENUM( "listhybrid",		DLpropHYBRID, 1 ),
    RTF_LIST_FLAG( "listrestarthdn",		DLpropRESTARTHDN ),
    RTF_LIST_NUMBER( "liststyleid",		DLpropSTYLEID ),

    RTF_LISTOVERR_NUMBER( "listoverridecount",	LOpropOVERRIDECOUNT ),
    /* "listid" ->	LOpropLISTID used via list */
    /* "ls" ->		LOpropINDEX  used via paragraph properties */

    RTF_LISTOVERRLEV_FLAG( "listoverridestartat",	LOLpropSTARTAT ),
    RTF_LISTOVERRLEV_FLAG( "listoverrideformat",	LOLpropFORMAT ),

				/****************************************/
				/*  Tab stops.				*/
				/*					*/
				/*  "tql" is not part of the rtf spec	*/
				/*  but it can be found in some		*/
				/*  documents.				*/
				/*					*/
				/*  "tx" is handeled differently.	*/
				/*					*/
				/****************************************/

    RTF_TAB_ENUM( "tql",	TABpropALIGN,	DOCtaLEFT ),
    RTF_TAB_ENUM( "tqr",	TABpropALIGN,	DOCtaRIGHT ),
    RTF_TAB_ENUM( "tqc",	TABpropALIGN,	DOCtaCENTER ),
    RTF_TAB_ENUM( "tqdec",	TABpropALIGN,	DOCtaDECIMAL ),

    RTF_TAB_ENUM( "tldot",	TABpropLEADER,	DOCtlDOTS ),
    RTF_TAB_ENUM( "tlul",	TABpropLEADER,	DOCtlUNDERLINE ),
    RTF_TAB_ENUM( "tlhyph",	TABpropLEADER,	DOCtlHYPH ),
    RTF_TAB_ENUM( "tlth",	TABpropLEADER,	DOCtlTHICK ),
    RTF_TAB_ENUM( "tleq",	TABpropLEADER,	DOCtlEQUAL ),

    RTF_TAB_FLAG( "jclisttab",	TABpropFROM_STYLE ),

				/****************************************/
				/*  Font table.				*/
				/****************************************/
    RTF_FONT_ENUM( "fnil",	DFpropFAMILY_STYLE,	DFstyleFNIL ),
    RTF_FONT_ENUM( "froman",	DFpropFAMILY_STYLE,	DFstyleFROMAN ),
    RTF_FONT_ENUM( "fswiss",	DFpropFAMILY_STYLE,	DFstyleFSWISS ),
    RTF_FONT_ENUM( "fmodern",	DFpropFAMILY_STYLE,	DFstyleFMODERN ),
    RTF_FONT_ENUM( "fscript",	DFpropFAMILY_STYLE,	DFstyleFSCRIPT ),
    RTF_FONT_ENUM( "fdecor",	DFpropFAMILY_STYLE,	DFstyleFDECOR ),
    RTF_FONT_ENUM( "ftech",	DFpropFAMILY_STYLE,	DFstyleFTECH ),

    RTF_FONT_NUMBER( "fcharset",	DFpropCHARSET ),
    RTF_FONT_NUMBER( "cpg",	DFpropCODEPAGE ),
    RTF_FONT_NUMBER( "fprq",	DFpropPITCH ),

				/****************************************/
				/*  Pictures/Objects			*/
				/****************************************/

# define	RTF_PICT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfPictureProperty, v }
# define	RTF_PICT_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfPictureProperty, }
# define	RTF_PICT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfPictureProperty, }

    RTF_PICT_ENUM( "macpict",	PIPpropTYPE,	DOCokMACPICT ),
    RTF_PICT_ENUM( "wmetafile",	PIPpropTYPE,	DOCokPICTWMETAFILE ),
    RTF_PICT_ENUM( "pngblip",	PIPpropTYPE,	DOCokPICTPNGBLIP ),
    RTF_PICT_ENUM( "jpegblip",	PIPpropTYPE,	DOCokPICTJPEGBLIP ),
    RTF_PICT_ENUM( "emfblip",	PIPpropTYPE,	DOCokPICTEMFBLIP ),
    RTF_PICT_ENUM( "dibitmap",	PIPpropTYPE,	DOCokDIBITMAP ),
    RTF_PICT_ENUM( "wbitmap",	PIPpropTYPE,	DOCokWBITMAP ),
    RTF_PICT_ENUM( "pmmetafile",PIPpropTYPE,	DOCokPMMETAFILE ),

    RTF_PICT_NUMBER( "wbmbitspixel",	PIPpropWBMBITSPIXEL ),
    RTF_PICT_NUMBER( "wbmplanes",	PIPpropWBMPLANES ),
    RTF_PICT_NUMBER( "wbmwidthbytes",	PIPpropWBMWIDTHBYTES ),

    RTF_PICT_NUMBER( "picw",		PIPpropPICX_WIN_EXT ),
    RTF_PICT_NUMBER( "pich",		PIPpropPICY_WIN_EXT ),

    RTF_PICT_NUMBER( "picwgoal",		PIPpropPICTWIPS_WIDE ),
    RTF_PICT_NUMBER( "pichgoal",		PIPpropPICTWIPS_HIGH ),

    RTF_PICT_NUMBER( "picscalex",	PIPpropPICSCALE_X ),
    RTF_PICT_NUMBER( "picscaley",	PIPpropPICSCALE_Y ),

    RTF_PICT_NUMBER( "piccropt",		PIPpropPICCROP_TOP ),
    RTF_PICT_NUMBER( "piccropb",		PIPpropPICCROP_BOTTOM ),
    RTF_PICT_NUMBER( "piccropl",		PIPpropPICCROP_LEFT ),
    RTF_PICT_NUMBER( "piccropr",		PIPpropPICCROP_RIGHT ),

    RTF_PICT_FLAG( "defshp",		PIPpropDEFSHP ),

    RTF_PICT_FLAG( "picscaled",		PIPpropPICSCALED ),

    RTF_PICT_NUMBER( "bliptag",		PIPpropBLIPTAG ),

    RTF_PICT_FLAG( "picbmp",		PIPpropPICBMP ),
    RTF_PICT_NUMBER( "picbpp",		PIPpropPICBPP ),
    RTF_PICT_NUMBER( "blipupi",		PIPpropBLIPUPI ),

# define	RTF_OBJ_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfObjectProperty, v }
# define	RTF_OBJ_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfObjectProperty, }
# define	RTF_OBJ_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfObjectProperty, }

    RTF_OBJ_ENUM( "rsltrtf",	IOpropRESULT_KIND,	RESULTkindRTF ),
    RTF_OBJ_ENUM( "rslttxt",	IOpropRESULT_KIND,	RESULTkindTXT ),
    RTF_OBJ_ENUM( "rsltpict",	IOpropRESULT_KIND,	RESULTkindPICT ),
    RTF_OBJ_ENUM( "rsltbmp",	IOpropRESULT_KIND,	RESULTkindBMP ),
    RTF_OBJ_ENUM( "rslthtml",	IOpropRESULT_KIND,	RESULTkindHTML ),

    RTF_OBJ_ENUM( "objemb",	IOpropEMBED_KIND, EMBEDkindOBJEMB ),
    RTF_OBJ_ENUM( "objlink",	IOpropEMBED_KIND, EMBEDkindOBJLINK ),
    RTF_OBJ_ENUM( "objautlink",	IOpropEMBED_KIND, EMBEDkindOBJAUTLINK ),
    RTF_OBJ_ENUM( "objsub",	IOpropEMBED_KIND, EMBEDkindOBJSUB ),
    RTF_OBJ_ENUM( "objpub",	IOpropEMBED_KIND, EMBEDkindOBJPUB ),
    RTF_OBJ_ENUM( "objicemb",	IOpropEMBED_KIND, EMBEDkindOBJICEMB ),
    RTF_OBJ_ENUM( "objhtml",	IOpropEMBED_KIND, EMBEDkindOBJHTML ),
    RTF_OBJ_ENUM( "objocx",	IOpropEMBED_KIND, EMBEDkindOBJOCX ),

    RTF_OBJ_NUMBER( "objw",		IOpropOBJTWIPS_WIDE ),
    RTF_OBJ_NUMBER( "objh",		IOpropOBJTWIPS_HIGH ),

    RTF_OBJ_NUMBER( "objscalex",		IOpropOBJSCALE_X ),
    RTF_OBJ_NUMBER( "objscaley",		IOpropOBJSCALE_Y ),

    RTF_OBJ_NUMBER( "objcropt",		IOpropOBJCROP_TOP ),
    RTF_OBJ_NUMBER( "objcropb",		IOpropOBJCROP_BOTTOM ),
    RTF_OBJ_NUMBER( "objcropl",		IOpropOBJCROP_LEFT ),
    RTF_OBJ_NUMBER( "objcropr",		IOpropOBJCROP_RIGHT ),

    RTF_OBJ_NUMBER( "objsetsize",	IOprop_UNSUPPORTED ),
    RTF_OBJ_NUMBER( "rsltmerge",		IOprop_UNSUPPORTED ),

				/****************************************/
				/*  Special characters (mostly ignored)	*/
				/****************************************/
    RTF_SPECIAL_CHAR( "bullet",		RTFidBULLET ),
    RTF_SPECIAL_CHAR( "chatn",		RTFidCHATN ),
    RTF_SPECIAL_CHAR( "emdash",		RTFidEMDASH ),
    RTF_SPECIAL_CHAR( "emspace",	RTFidEMSPACE ),
    RTF_SPECIAL_CHAR( "endash",		RTFidENDASH ),
    RTF_SPECIAL_CHAR( "enspace",	RTFidENSPACE ),
    RTF_SPECIAL_CHAR( "ldblquote",	RTFidLDBLQUOTE ),
    RTF_SPECIAL_CHAR( "lquote",		RTFidLQUOTE ),
    RTF_SPECIAL_CHAR( "ltrmark",	RTFidLTRMARK ),
    RTF_SPECIAL_CHAR( "rdblquote",	RTFidRDBLQUOTE ),
    RTF_SPECIAL_CHAR( "rquote",		RTFidRQUOTE ),
    RTF_SPECIAL_CHAR( "rtlmark",	RTFidRTLMARK ),
    RTF_SPECIAL_CHAR( "softcol",	RTFidSOFTCOL ),
    RTF_SPECIAL_CHAR( "softlheight",	RTFidSOFTLHEIGHT ),
    RTF_SPECIAL_CHAR( "softline",	RTFidSOFTLINE ),
    RTF_SPECIAL_CHAR( "softpage",	RTFidSOFTPAGE ),
    RTF_SPECIAL_CHAR( "zwj",		RTFidZWJ ),
    RTF_SPECIAL_CHAR( "zwnj",		RTFidZWNJ ),

    RTF_SPECIAL_PART( "tab",		DOCkindTAB ),
    RTF_SPECIAL_PART( "line",		DOCkindLINEBREAK ),
    RTF_SPECIAL_PART( "lbr",		DOCkindLINEBREAK ),
    RTF_SPECIAL_PART( "page",		DOCkindPAGEBREAK ),
    RTF_SPECIAL_PART( "column",		DOCkindCOLUMNBREAK ),
    RTF_SPECIAL_PART( "chftnsep",	DOCkindCHFTNSEP ),
    RTF_SPECIAL_PART( "chftnsepc",	DOCkindCHFTNSEPC ),

    RTF_SPECIAL_FIELD( "chdate",	DOCfkDATE ),
    RTF_SPECIAL_FIELD( "chpgn",		DOCfkPAGE ),
    RTF_SPECIAL_FIELD( "chtime",	DOCfkTIME ),
    RTF_SPECIAL_FIELD( "chftn",		DOCfkCHFTN ),
    RTF_SPECIAL_FIELD( "sectnum",	DOCfkSECTION ),

    RTF_SPECIAL_FIELD( "chdpa",		DOCfkDATE ),
    RTF_SPECIAL_FIELD( "chdpl",		DOCfkDATE ),

				/****************************************/
				/*  Word 95 drawing objects.		*/
				/****************************************/
# define	RTF_DO_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfDrawingObjectProperty }
# define	RTF_DO_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfDrawingObjectProperty, v }
# define	RTF_DO_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfDrawingObjectProperty }
# define	RTF_DO_COORD(s,id) \
		{ s, id, DOClevANY, docRtfDrawingObjectCoordinate }

    RTF_DO_FLAG( "dolock",	DOpropANCHOR_LOCKED ),

    RTF_DO_ENUM( "dobxpage",	DOpropX_ATTACH,		SHPbxPAGE ),
    RTF_DO_ENUM( "dobxcolumn",	DOpropX_ATTACH,		SHPbxCOLUMN ),
    RTF_DO_ENUM( "dobxmargin",	DOpropX_ATTACH,		SHPbxMARGIN ),

    RTF_DO_ENUM( "dobypage",	DOpropY_ATTACH,		SHPbyPAGE ),
    RTF_DO_ENUM( "dobypara",	DOpropY_ATTACH,		SHPbyPARA ),
    RTF_DO_ENUM( "dobymargin",	DOpropY_ATTACH,		SHPbyMARGIN ),

    RTF_DO_ENUM( "dparc",	DOpropKIND, SHPtyARC ),
    RTF_DO_ENUM( "dpcallout",	DOpropKIND, SHPtyCALLOUT_1 ),
    RTF_DO_ENUM( "dpellipse",	DOpropKIND, SHPtyELLIPSE ),
    RTF_DO_ENUM( "dpline",	DOpropKIND, SHPtyLINE ),
    RTF_DO_ENUM( "dppolygon",	DOpropKIND, SHPtyFREEFORM_OR_NON_AUTOSHAPE ),
    RTF_DO_ENUM( "dppolyline",	DOpropKIND, SHPtyFREEFORM_OR_NON_AUTOSHAPE ),
    RTF_DO_ENUM( "dprect",	DOpropKIND, SHPtyRECTANGLE ),
    RTF_DO_ENUM( "dptxbx",	DOpropKIND, SHPtyTEXT_BOX ),

    RTF_DO_ENUM( "dplinesolid",	DOpropLINE_STYLE,	DSdashSOLID ),
    RTF_DO_ENUM( "dplinehollow",DOpropLINE_STYLE,	DSdashHOLLOW ),
    RTF_DO_ENUM( "dplinedash",	DOpropLINE_STYLE,	DSdashDASHED ),
    RTF_DO_ENUM( "dplinedot",	DOpropLINE_STYLE,	DSdashDOT ),
    RTF_DO_ENUM( "dplinedado",	DOpropLINE_STYLE,	DSdashDASHDOT ),
    RTF_DO_ENUM( "dplinedadodo",DOpropLINE_STYLE,	DSdashDASHDOTDOT ),

    RTF_DO_ENUM( "dpaendhol",	DOpropEND_ARROW_HEAD,	DSarrowOPEN_ARROW ),
    RTF_DO_ENUM( "dpaendsol",	DOpropEND_ARROW_HEAD,	DSarrowARROW ),
    RTF_DO_ENUM( "dpastarthol",	DOpropSTART_ARROW_HEAD,	DSarrowOPEN_ARROW ),
    RTF_DO_ENUM( "dpastartsol",	DOpropSTART_ARROW_HEAD,	DSarrowARROW ),

    RTF_DO_NUMBER( "dpaendl",	DOpropEND_ARROW_LENGTH ),
    RTF_DO_NUMBER( "dpastartl",	DOpropSTART_ARROW_LENGTH ),
    RTF_DO_NUMBER( "dpaendw",	DOpropEND_ARROW_WIDTH ),
    RTF_DO_NUMBER( "dpastartw",	DOpropSTART_ARROW_WIDTH ),

    RTF_DO_NUMBER( "dpfillpat",	DOpropFILL_PATTERN ),

    RTF_DO_FLAG( "dparcflipx",	DOpropARC_FLIP_X ),
    RTF_DO_FLAG( "dparcflipy",	DOpropARC_FLIP_Y ),

    RTF_DO_NUMBER( "dpx",	DOpropX ),
    RTF_DO_NUMBER( "dpy",	DOpropY ),
    RTF_DO_NUMBER( "dodhgt",	DOpropZ ),
    RTF_DO_NUMBER( "dpxsize",	DOpropWIDE ),
    RTF_DO_NUMBER( "dpysize",	DOpropHIGH ),

    RTF_DO_NUMBER( "dptxbxmar",	DOpropTEXT_BOX_MARGIN ),

    RTF_DO_NUMBER( "dplinew",	DOpropLINE_WIDTH ),
    RTF_DO_NUMBER( "dppolycount",DOpropPOINT_COUNT ),

    RTF_DO_NUMBER( "dplinecor",	DOpropLINE_RED ),
    RTF_DO_NUMBER( "dplinecog",	DOpropLINE_GREEN ),
    RTF_DO_NUMBER( "dplinecob",	DOpropLINE_BLUE ),
    RTF_DO_NUMBER( "dplinegray",	DOpropLINE_GRAY ),

    RTF_DO_NUMBER( "dpfillfgcr",	DOpropFILL_FORE_RED ),
    RTF_DO_NUMBER( "dpfillfgcg",	DOpropFILL_FORE_GREEN ),
    RTF_DO_NUMBER( "dpfillfgcb",	DOpropFILL_FORE_BLUE ),
    RTF_DO_NUMBER( "dpfillfggray", DOpropFILL_FORE_GRAY ),

    RTF_DO_NUMBER( "dpfillbgcr",	DOpropFILL_BACK_RED ),
    RTF_DO_NUMBER( "dpfillbgcg",	DOpropFILL_BACK_GREEN ),
    RTF_DO_NUMBER( "dpfillbgcb",	DOpropFILL_BACK_BLUE ),
    RTF_DO_NUMBER( "dpfillbggray", DOpropFILL_BACK_GRAY ),

    RTF_DO_COORD( "dpptx",	DOpropX ),
    RTF_DO_COORD( "dppty",	DOpropY ),

				/****************************************/
				/*  Shapes.				*/
				/****************************************/

# define	RTF_SHAPE_NUMBER(s,id) \
		{ s, id, DOClevANY, docRtfShpProperty, }
# define	RTF_SHAPE_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfShpProperty, }
# define	RTF_SHAPE_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfShpProperty, v, }

    RTF_SHAPE_NUMBER( "shpleft",		SHPpropLEFT ),
    RTF_SHAPE_NUMBER( "shptop",		SHPpropTOP ),
    RTF_SHAPE_NUMBER( "shpbottom",	SHPpropBOTTOM ),
    RTF_SHAPE_NUMBER( "shpright",	SHPpropRIGHT ),

    RTF_SHAPE_NUMBER( "shplid",		SHPpropLID ),
    RTF_SHAPE_NUMBER( "shpz",		SHPpropZ ),

    RTF_SHAPE_FLAG( "shpfhdr",		SHPpropFHDR ),

    RTF_SHAPE_ENUM( "shpbxmargin",	SHPpropBX,	SHPbxMARGIN ),
    RTF_SHAPE_ENUM( "shpbxpage",	SHPpropBX,	SHPbxPAGE ),
    RTF_SHAPE_ENUM( "shpbxcolumn",	SHPpropBX,	SHPbxCOLUMN ),
    /*RTF_SHAPE_ENUM( "shpbx???",	SHPpropBX,	SHPbxCHARACTER ),*/
    RTF_SHAPE_ENUM( "shpbxignore",	SHPpropBX,	SHPbxIGNORE ),

    RTF_SHAPE_ENUM( "shpbymargin",	SHPpropBY,	SHPbyMARGIN ),
    RTF_SHAPE_ENUM( "shpbypage",	SHPpropBY,	SHPbyPAGE ),
    RTF_SHAPE_ENUM( "shpbypara",	SHPpropBY,	SHPbyPARA ),
    /*RTF_SHAPE_ENUM( "shpby???",	SHPpropBY,	SHPbyLINE ),*/
    RTF_SHAPE_ENUM( "shpbyignore",	SHPpropBY,	SHPbyIGNORE ),

    RTF_SHAPE_NUMBER( "shpwr",		SHPpropWR ),
    RTF_SHAPE_NUMBER( "shpwrk",		SHPpropWRK ),
    RTF_SHAPE_NUMBER( "shpfblwtxt",	SHPpropFBLWTXT ),

    RTF_SHAPE_FLAG( "shplockanchor",	SHPpropLOCKANCHOR ),

				/****************************************/
				/*  End.				*/
				/****************************************/
    { 0, 0, 0 }
};

static int docRtfMakeTagHash( void )
    {
    int			rval= 0;
    int			i;

    const unsigned char **	keyStrings= (const unsigned char **)0;
    int				keyStringCount;

    keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

    keyStrings= malloc( keyStringCount* sizeof(const unsigned char *) );
    if  ( ! keyStrings )
	{ LXDEB(keyStringCount,keyStrings); rval= -1; goto ready;	}

    for ( i= 0; i < keyStringCount; i++ )
	{
	keyStrings[i]= (const unsigned char *)docRtfPropertyWords[i].rcwWord;
	}

    if  ( utilJenkinsPerfectHash( keyStrings, keyStringCount,
						    "docRtfTagHash.c",
						    "\"docRtfTagHash.h\"",
						    "docRtfTagHash",
						    "docRtfTagIndex" ) )
	{ LDEB(keyStringCount); rval= 0;	}

  ready:

    if  ( keyStrings )
	{ free( keyStrings );	}

    return rval;
    }

#   ifndef	BUILD_HASH

static int docRtfCheckTagHash( void )
    {
    int			rval= 0;
    int			i;

    int			keyStringCount;

    keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

    for ( i= 0; i < keyStringCount; i++ )
	{
	const unsigned char *	key;
	int			len;
	int			idx;

	key= (const unsigned char *)docRtfPropertyWords[i].rcwWord;
	len= strlen( docRtfPropertyWords[i].rcwWord );

	idx= docRtfTagIndex( key, len );

	if  ( idx != i )
	    {
	    long hash= docRtfTagHash( key, len );

	    LSLLDEB(i,docRtfPropertyWords[i].rcwWord,idx,hash); rval= -1;
	    }
	}

    return rval;
    }

const RtfControlWord * docRtfFindPropertyWord(
					const char *	controlWord )
    {
    const RtfControlWord *	contolWords;

    static int			checked= 1;

    if  ( ! checked )
	{
	checked= 1;

	if  ( docRtfCheckTagHash() )
	    {
	    checked= -1;

	    LDEB(1);
	    if  ( docRtfMakeTagHash() )
		{ LDEB(1);	}
	    }
	}

    if  ( checked > 0 )
	{
	int			idx;
	int			keyStringCount;

	keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

	idx= docRtfTagIndex( (const unsigned char *)controlWord,
							strlen( controlWord ) );

	if  ( idx >= 0 && idx < keyStringCount )
	    {
	    contolWords= docRtfPropertyWords+ idx;

	    if  ( ! strcmp( controlWord, contolWords->rcwWord ) )
		{ return contolWords;	}
	    }

	return (const RtfControlWord *)0;
	}
    else{
	contolWords= docRtfPropertyWords;

	while( contolWords->rcwWord )
	    {
	    if  ( ! strcmp( contolWords->rcwWord, controlWord ) )
		{ return contolWords;	}

	    contolWords++;
	    }

	return (const RtfControlWord *)0;
	}
    }

#   endif

#   ifdef	BUILD_HASH

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeTagHash() )
	    { LDEB(1); return 1;	}

	return 0;
	}

#   endif

