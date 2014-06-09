/************************************************************************/
/*  A printer as found on the system.					*/
/************************************************************************/

#   ifndef	UTIL_PRINTER_H
#   define	UTIL_PRINTER_H

#   define	APPprinterPIPE		1
#   define	APPprinterTMPFILE	2
#   define	APPprinterTOFILE	3

typedef struct PrintDestination
    {
    char *	pdCommand;
    char *	pdPrinterName;
    int		pdPrintKind;
    int		pdPercentCount;
    int		pdCommandLength;
    } PrintDestination;

/************************************************************************/
/*  Routine declarations.						*/
/************************************************************************/

extern int utilPrinterGetPrinters(	int *			pPrinterCount,
					int *			pDefaultPrinter,
					PrintDestination **	pDestinations,
					const char *		customCommand,
					const char *		customName,
					const char *		customCommand2,
					const char *		customName2 );

#   endif
