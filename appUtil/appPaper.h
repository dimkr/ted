
extern int appPaperGetInformation(	int		n,
					int *		pWidth,
					int *		pHeight,
					const char **	pLabel );

extern int appPaperGetBySize(		int		width,
					int		height );

extern int appPaperGetByMnemonic(	const char *	mnemonic );

extern int appPaperFormatFromString(	int *		pFormat,
					int *		pWidth,
					int *		pHeight,
					int		unitType,
					const char *	paperString );
