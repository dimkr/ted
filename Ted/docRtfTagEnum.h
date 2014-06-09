/************************************************************************/
/*									*/
/*  Identifies RTF control words					*/
/*									*/
/************************************************************************/

typedef enum RtfTag
    {
    RTFidRTF,
    RTFidRULER,

    RTFidUPR,
				/****************************************/
				/*  Document group.			*/
				/****************************************/
    RTFidFONTTBL,
    RTFidCOLORTBL,
    RTFidPN,
    RTFidSTYLESHEET,
    RTFidINFO,
				/****************************************/
				/*  Groups inside the document.		*/
				/****************************************/
    RTFidF,
    RTFidREVTBL,
				/****************************************/
				/*  Font number.			*/
				/****************************************/
    RTFidUC,
				/****************************************/
				/*  Character sets.			*/
				/****************************************/
    RTFidRED,
    RTFidGREEN,
    RTFidBLUE,
				/****************************************/
				/*  Color references.			*/
				/****************************************/
    RTFidOGUTTER,
    RTFidRTLGUTTER,

    RTFidDEFLANGFE,
    RTFidVIEWKIND,
    RTFidVIEWSCALE,
				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    RTFidSECTDEFAULTCL,
    RTFidLYTPRTMET,
				/****************************************/
				/*  Document Formatting properties.	*/
				/****************************************/
    RTFidALLPROT,
    RTFidANNOTPROT,
    RTFidBRKFRM,
    RTFidCVMME,
    RTFidDEFFORMAT,
    RTFidDEFLANG,

    RTFidFLDALT,
    RTFidFORMDISP,
    RTFidFORMPROT,
    RTFidFORMSHADE,
    RTFidFRACWIDTH,
    RTFidFTNALT,

    RTFidHYPHAUTO,
    RTFidHYPHCAPS,
    RTFidHYPHCONSEC,
    RTFidHYPHHOTZ,
    RTFidLANDSCAPE,
    RTFidLINESTART,
    RTFidLINKSTYLES,
    RTFidLTRDOC,
    RTFidMAKEBACKUP,

    RTFidMARGMIRROR,
    RTFidNOCOLBAL,
    RTFidNOEXTRASPRL,
    RTFidNOTABIND,
    RTFidOTBLRUL,
    RTFidPRCOLBL,
    RTFidPRINTDATA,
    RTFidPSOVER,
    RTFidPSZ,
    RTFidREVBAR,
    RTFidREVISIONS,
    RTFidREVPROP,
    RTFidREVPROT,
    RTFidRSID,
    RTFidRTLDOC,
    RTFidSPRSLNSP,
    RTFidSPRSSPBF,
    RTFidSPRSTSP,
    RTFidSUBFONTBYSIZE,
    RTFidSWPBDR,
    RTFidTRANSMF,
    RTFidIGHT,
    RTFidWRAPTRSP,
    RTFidVIEWZK,

    RTFidPGBRDROPT,
    RTFidPGBRDRSNAP,

    RTFidNOXLATTOYEN,
    RTFidEXPSHRTN,
    RTFidNOULTRLSPC,
    RTFidDNTBLNSBDB,
    RTFidNOSPACEFORUL,

    RTFidJCOMPRESS,
    RTFidJEXPAND,

    RTFidHORZDOC,
    RTFidDGHSPACE,
    RTFidDGVSPACE,
    RTFidDGHORIGIN,
    RTFidDGVORIGIN,
    RTFidDGHSHOW,
    RTFidDGVSHOW,
    RTFidDGMARGIN,

    RTFidNOLEAD,
    RTFidTRUNCEX,
    RTFidMSMCAP,
    RTFidSPLYTWNINE,
    RTFidFTNLYTWNINE,
    RTFidHTMAUTSP,
    RTFidUSELTBALN,
    RTFidALNTBLIND,
    RTFidLYTCALCTBLWD,
    RTFidLYTTBLRTGR,
    RTFidLNBRKRULE,

    RTFidBDRRLSWSIX,
    RTFidOLDAS,
				/****************************************/
				/*  Section Formatting Properties	*/
				/****************************************/
    RTFidBINFSXN,
    RTFidBINSXN,
    RTFidCOLNO,
    RTFidCOLSR,
    RTFidDS,
    RTFidENDNHERE,
    RTFidLINEBETCOL,
    RTFidLINECONT,
    RTFidLINEMOD,
    RTFidLINEPPAGE,
    RTFidLINERESTART,
    RTFidLINESTARTS,
    RTFidLINEX,
    RTFidLNDSCPSXN,
    RTFidLTRSECT,
    RTFidPGNHN,
    RTFidPGNHNSC,
    RTFidPGNHNSH,
    RTFidPGNHNSM,
    RTFidPGNHNSN,
    RTFidPGNHNSP,
    RTFidPGNX,
    RTFidPGNY,
    RTFidRTLSECT,
    RTFidSECTUNLOCKED,
    RTFidVERTALB,
    RTFidVERTALC,
    RTFidVERTALJ,
    RTFidVERTALT,

    RTFidSECTLINEGRID,
				/****************************************/
				/*  Paragraph Formatting Properties	*/
				/****************************************/
    RTFidLEVEL,
    RTFidLTRPAR,
    RTFidNOLINE,
    RTFidRTLPAR,
    RTFidS,
    RTFidSBYS,
    RTFidSUBDOCUMENT,

    RTFidASPALPHA,
    RTFidASPNUM,

    RTFidSBAUTO,
    RTFidSAAUTO,

    RTFidNOOVERFLOW,

				/****************************************/
				/*  Revision marks for paragraph	*/
				/*  numbers.				*/
				/****************************************/
    RTFidPNRAUTH,
    RTFidPNR_REVPROP,
    RTFidPNRNFC,
    RTFidPNRPNBR,
    RTFidPNRRGB,
    RTFidPNRSTART,
    RTFidPNRSTOP,
    RTFidPNRXST,
				/****************************************/
				/*  Multilevel Lists.			*/
				/****************************************/
    RTFidPNSECLVL,

				/****************************************/
				/*  Tabs.				*/
				/****************************************/
    RTFidTB,
    RTFidTX,

    RTFidADJUSTRIGHT, RTFidADJUSTLEFT,

    RTFidFC,
    RTFidFCS,
				/****************************************/
				/*  Character Formatting Properties	*/
				/****************************************/
    RTFidCB,
    RTFidCCHS,
    RTFidCS,
    RTFidDELETED,
    RTFidDN,
    RTFidEXPND,
    RTFidEXPNDTW,
    /* RTFidF,	*/
    RTFidKERNING,
    RTFidLANG,
    RTFidNOPROOF,
    RTFidLTRCH,
    RTFidOUTL,
    RTFidREVAUTH,
    RTFidREVAUTHDEL,
    RTFidREVDTTM,
    RTFidREVDTTMDEL,
    RTFidREVISED,
    RTFidRTLCH,
    RTFidSHAD,
    RTFidULD,
    RTFidULDB,
    RTFidULNONE,
    RTFidULW,
    RTFidUP,
    RTFidV,
    RTFidWEBHIDDEN,
    RTFidHIGHLIGHT,

    RTFidCGRID,
    RTFidCHARSCALEX,

    RTFidCHBRDR,

    RTFidCHCBPAT,
    RTFidCHCFPAT,
    RTFidCHSHDNG,

    RTFid_FONTALIGN,
				/****************************************/
				/*  Pictures				*/
				/****************************************/
    RTFidPICT,
    RTFidNONSHPPICT,
    RTFidSHPPICT,
    RTFidDO,
				/****************************************/
				/*  Pictures, objects			*/
				/****************************************/
    RTFidOBJECT,
    RTFidOBJCLASS,
    RTFidOBJNAME,
    RTFidOBJDATA,

    RTFidOBJSETSIZE,
    RTFidRSLTMERGE,
				/****************************************/
				/*  Special characters			*/
				/****************************************/
    RTFidBULLET,
    RTFidCHATN,
    RTFidEMDASH,
    RTFidEMSPACE,
    RTFidENDASH,
    RTFidENSPACE,
    RTFidLDBLQUOTE,
    RTFidLQUOTE,
    RTFidLTRMARK,
    RTFidRDBLQUOTE,
    RTFidRQUOTE,
    RTFidRTLMARK,
    RTFidSOFTCOL,
    RTFidSOFTLHEIGHT,
    RTFidSOFTLINE,
    RTFidSOFTPAGE,
    RTFidZWJ,
    RTFidZWNJ,

				/****************************************/
				/*  Hierarchy.				*/
				/****************************************/
    RTFidPAR,
    RTFidCELL,
    RTFidROW,
    RTFidSECT,
    RTFidNESTCELL,

    RTFidNONESTTABLES,
				/****************************************/
				/*  Table definitions			*/
				/****************************************/
    RTFidCELLX,

    RTFidLTRROW,
    RTFidRTLROW,

				/****************************************/
				/*  Positioned objects and frames	*/
				/****************************************/
    RTFidDROPCAPLI,
    RTFidDROPCAPT,
				/****************************************/
				/*  Bullets and Numbering.		*/
				/*  (No destinations)			*/
				/****************************************/
    RTFidPNACROSS,
    RTFidPNHANG,
    RTFidPNLVL,
    RTFidPNLVLBLT,
    RTFidPNLVLBODY,
    RTFidPNLVLCONT,
    RTFidPNNUMONCE,
    RTFidPNRESTART,
    RTFidPNULD,
    RTFidPNULDB,
    RTFidPNULNONE,
    RTFidPNULW,

    RTFidPNRNOT,
				/****************************************/
				/*  Fields, Bookmarks.			*/
				/****************************************/
    RTFidDATAFIELD,
    RTFidFIELD,
    RTFidFLDDIRTY,
    RTFidFLDEDIT,
    RTFidFLDINST,
    RTFidFLDLOCK,
    RTFidFLDPRIV,
    RTFidFLDRSLT,

    RTFidBKMKSTART,
    RTFidBKMKEND,
    RTFidBKMKCOLF,
    RTFidBKMKCOLL,
				/****************************************/
				/*  Index entries.			*/
				/*  Table of contents entries.		*/
				/****************************************/
    RTFidTXE,
    RTFidRXE,
    RTFidIXE,
    RTFidBXE,
    RTFidXEF,

    RTFidTCF,
    RTFidTCL,
				/****************************************/
				/*  Associated Character Properties	*/
				/****************************************/
    RTFidAB,
    RTFidACAPS,
    RTFidACF,
    RTFidADN,
    RTFidAEXPND,
    RTFidAF,
    RTFidAFS,
    RTFidAI,
    RTFidALANG,
    RTFidAOUTL,
    RTFidASCAPS,
    RTFidASHAD,
    RTFidASTRIKE,
    RTFidAUL,
    RTFidAULD,
    RTFidAULDB,
    RTFidAULNONE,
    RTFidAULW,
    RTFidAUP,

    RTFidLOCH,
    RTFidHICH,
    RTFidDBCH,
				/****************************************/
				/*  About bullet numbering.		*/
				/****************************************/
    RTFidPNTXTA,
    RTFidPNTXTB,
    RTFidLISTTEXT,
				/****************************************/
				/*  Groups inside the 'info' group.	*/
				/****************************************/
    RTFidOPERATOR,
    RTFidVERSION,
    RTFidEDMINS,
    RTFidNOFPAGES,
    RTFidNOFWORDS,
    RTFidNOFCHARS,
    RTFidNOFCHARSWS,
    RTFidVERN,
				/****************************************/
				/*  Shapes.				*/
				/****************************************/
    RTFidSHPTXT,
    RTFidSHPRSLT,
    RTFidSHPINST,

    RTFidSP,
    RTFidSV,
				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
    RTFidHR,
    RTFidMIN,
    RTFidSEC,
    RTFidDY,
    RTFidMO,
    RTFidYR,
				/****************************************/
				/*  Unknown				*/
				/****************************************/
    RTFidITAP,
    RTFidLIN,
    RTFidRIN,
    RTFidLANGNP,
    RTFidLANGFE,
    RTFidLANGFENP,
    RTFidNOLNHTADJTBL,

    RTFid__COUNT

    } RtfTag;

