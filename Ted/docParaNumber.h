/************************************************************************/
/*									*/
/*  Paragraph numbers.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_NUMBER_H
#   define	DOC_PARA_NUMBER_H

#   include	<utilDocFont.h>

/************************************************************************/
/*									*/
/*  Information on Paragraph Numbering.					*/
/*									*/
/************************************************************************/

typedef struct ParagraphNumber
    {
    short		pnLevel;
    short		pnNumberStyle;
    unsigned char	pnJustification;
    unsigned char	pnUsePrevText;
			/************************************************/
			/*  Values match the values for levelnfc in	*/
			/*  list levels.				*/
			/*						*/
			/*  Names come from p51 of the rtf 1.7 document	*/
			/*  Values come from page 23.			*/
			/*						*/
			/************************************************/
#			define	DOCpnDEC	0
#			define	DOCpnUCRM	1
#			define	DOCpnLCRM	2
#			define	DOCpnUCLTR	3
#			define	DOCpnLCLTR	4
#			define	DOCpnORD	5
#			define	DOCpnCARD	6
#			define	DOCpnORDT	7

#			define	DOCpnDBNUM	10
#			define	DOCpnDBNUMD	11
#			define	DOCpnAIU	12
#			define	DOCpnIROHA	13
#			define	DOCpnDBNUML	16
#			define	DOCpnDBNUMT	DOCpnDBNUML	/* ? */
#			define	DOCpnDBNUMK	17
#			define	DOCpnCNUM	18
#			define	DOCpnDECD	19
#			define	DOCpnAIUD	20
#			define	DOCpnIROHAD	21
#			define	DOCpnGANADA	24
#			define	DOCpnGBNUM	26
#			define	DOCpnGBNUMD	27
#			define	DOCpnGBNUML	28
#			define	DOCpnGBNUMK	29
#			define	DOCpnZODIAC	30
#			define	DOCpnZODIACD	31
#			define	DOCpnZODIACL	32
#			define	DOCpnCHOSUNG	42
#			define	DOCpnAIUEO	DOCpnAIU	/* ? */
#			define	DOCpnAIUEOD	DOCpnAIUD	/* ? */
#			define	DOCpnBIDIA	46
#			define	DOCpnBIDIB	48


    short		pnUseHangingIndent;

    int			pnStartAt;
    int			pnIndent;
    int			pnSpace;

    TextAttribute	pnTextAttribute;

    unsigned char	pnTextBefore[32+1];
    unsigned char	pnTextAfter[32+1];

    } ParagraphNumber;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  DOC_PARA_NUMBER_H	*/
