/************************************************************************/
/*									*/
/*  Notes tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_NOTES_TOOL_H
#   define	TED_NOTES_TOOL_H

/************************************************************************/
/*									*/
/*  A notes tool, i.e. the 'Notes' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct NotesPageResources
    {
    char *		nprCurrentNoteText;
    char *		nprFootnoteText;
    char *		nprEndnoteText;
    char *		nprFixedTextText;
    char *		nprToNoteRefText;
    char *		nprToNoteText;
    char *		nprRevertNoteText;
    char *		nprChangeNoteText;

    char *		nprFootnotesText;
    char *		nprEndnotesText;

    char *		nprFirstNumberText;
    char *		nprPositionText;
    char *		nprRestartText;
    char *		nprStyleText;

    char *		nprPositionMenuTexts[FTN_POS__COUNT];
    char *		nprRestartMenuTexts[FTN_RST__COUNT];
    char *		nprNumberStyleMenuTexts[FTNstyle_COUNT];
    } NotesPageResources;

typedef struct NotePropertiesTool
    {
    APP_WIDGET			nptFrame;
    APP_WIDGET			nptPaned;
    APP_WIDGET			nptStartNumberText;

				/*  Not all positions used!	*/
    AppOptionmenu		nptPositionOptionmenu;
    APP_WIDGET			nptPositionOptions[FTN_POS__COUNT];

				/*  Not all positions used!	*/
    AppOptionmenu		nptRestartOptionmenu;
    APP_WIDGET			nptRestartOptions[FTN_RST__COUNT];

    AppOptionmenu		nptStyleOptionmenu;
    APP_WIDGET			nptStyleOptions[FTNstyle_COUNT];
    } NotePropertiesTool;

typedef struct NotesTool
    {
    EditApplication *		ntApplication;
    AppInspector *		ntInspector;
    const NotesPageResources *	ntPageResources;

    int				ntNoteNumber;
    int				ntNoteKindChosen;
    int				ntNoteKindSet;
    int				ntFixedTextChosen;
    int				ntFixedTextSet;
    int				ntInsideNote;

    unsigned char		ntNoteTextSet[20+1];
    unsigned char		ntNoteTextChosen[20+1];

    DocumentProperties		ntPropertiesChosen;
    DocumentProperties		ntPropertiesSet;

    /****/
    APP_WIDGET			ntCurrentNoteFrame;
    APP_WIDGET			ntCurrentNotePaned;
    APP_WIDGET			ntFootnoteToggle;
    APP_WIDGET			ntEndnoteToggle;
    APP_WIDGET			ntFixedTextToggle;
    APP_WIDGET			ntFixedTextText;

    APP_WIDGET			ntToNoteRefButton;
    APP_WIDGET			ntToNoteButton;

    APP_WIDGET			ntRevertNoteButton;
    APP_WIDGET			ntChangeNoteButton;

    /****/
    NotePropertiesTool		ntFootnotePropertiesTool;
    NotePropertiesTool		ntEndnotePropertiesTool;
    } NotesTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetNotesResourceTable( EditApplication *	ea,
					NotesPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshNotesTool(
				NotesTool *			nt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				BufferDocument *		bd );

extern void tedFormatCleanNotesTool(	NotesTool *		nt );

#   endif	/*  TED_NOTES_TOOL_H */
