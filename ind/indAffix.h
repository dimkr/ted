/************************************************************************/
/*									*/
/*  Procedures to read an ispell affix file and dictionary files and	*/
/*  to build a finite automaton that recognises the same words.		*/
/*									*/
/************************************************************************/

typedef int (*PUT_FUN)	( void * voidind, const unsigned char * word );

extern void * indAffixReadFile(	const char *		filename,
				const unsigned char *	charKinds,
				const unsigned char *	charShifts );

extern int indAffixApplyRules(	const void *		voidars,
				const unsigned char *	word,
				int			len,
				const unsigned char *	rules,
				void *			voidind,
				PUT_FUN			fun );

extern int indAffixChar(	const void *		voidars,
				const unsigned char *	buf,
				int *			pPos );

extern void indAffixListRules(	const void *		voidars );

extern void indAffixFreeRules(	void *			voidars );
