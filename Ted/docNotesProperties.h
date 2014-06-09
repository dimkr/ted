/************************************************************************/
/*									*/
/*  Note related Properties.						*/
/*									*/
/************************************************************************/

#   include	<utilPropMask.h>

#   ifndef	DOC_NOTE_PROPERTIES_H
#   define	DOC_NOTE_PROPERTIES_H

#   define	DPfetFOOT_ONLY		0
#   define	DPfetEND_ONLY		1
#   define	DPfetFOOT_END_BOTH	2

typedef enum NotesPosition
    {
    FTN_POS_SECT_END= 0,
    FTN_POS_DOC_END,
    FTN_POS_BELOW_TEXT,
    FTN_POS_PAGE_BOTTOM,

    FTN_POS__COUNT
    } NotesPosition;

typedef enum NotesRestart
    {
    FTN_RST_CONTINUOUS= 0,
    FTN_RST_PER_SECTION,
    FTN_RST_PER_PAGE,

    FTN_RST__COUNT
    } NotesRestart;

typedef enum NoteNumberStyle
    {
    FTNstyleNAR= 0,
    FTNstyleNALC,
    FTNstyleNAUC,
    FTNstyleNRLC,
    FTNstyleNRUC,
    FTNstyleNCHI,

    FTNstyle_COUNT
    } NoteNumberStyle;

typedef struct NotesProperties
    {
    int			npStartNumber;
    unsigned int	npPosition:2;
    unsigned int	npRestart:2;
    unsigned int	npNumberStyle:3;
    } NotesProperties;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docUpdNotesProperties(
			PropertyMask *			pChgMask,
			NotesProperties *		np,
			const PropertyMask *		updMask,
			const NotesProperties *		npNew,
			int				propStartNr,
			int				propPosition,
			int				propRestart,
			int				propStyle );

extern void docNotesPropertyDifference(
			PropertyMask *			diffMask,
			const NotesProperties *		np1,
			const PropertyMask *		cmpMask,
			const NotesProperties *		np2,
			int				propStartNr,
			int				propPosition,
			int				propRestart,
			int				propStyle );

#   endif	/*  DOC_NOTES_PROPERTIES_H  */
