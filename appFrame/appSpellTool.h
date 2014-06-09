/************************************************************************/
/*									*/
/*  Spell tool and spelling checking definitions.			*/
/*									*/
/************************************************************************/

#   include	<ind.h>
#   include	<appFrame.h>
#   include	<appTool.h>

typedef int (*SpellToolFindNext)( void * target, SpellCheckContext * scc );
typedef void (*SpellToolCorrect)( void * target, const unsigned char * guess );

/************************************************************************/
/*  Subroutine declarations.						*/
/************************************************************************/

extern void * appMakeSpellTool(	APP_WIDGET		spellOption,
				EditApplication *	ea,
				const char *		widgetName,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				AppToolDestroy		destroy,
				SpellToolFindNext	findNext,
				SpellToolCorrect	correct,
				void *			target	);

extern void appShowSpellTool(		void *	voidast	);

extern void appSpellMakeGuesses(	void *			voidast,
					const unsigned char *	word	);

extern void appEnableSpellTool(	void *	voidast,
				int	enabled );
