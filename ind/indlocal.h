#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<ind.h>

#   ifdef	__STDC__
#	include	<stddef.h>
#	include	<stdlib.h>
#	include	<string.h>
#   endif

#   include	<utilEndian.h>

#   define	TNfUSED		1
#   define	TNfACCEPTS	2

#   define	INDMAGIC	0x4d61726b
#   define	INDMAGIC_R	0x6b72614d

#   define	TLsBLOCK	5000
#   define	TNsBLOCK	5000
#   define	ITsBLOCK	5000

#   define	TLgFREE	0xfffffe
#   define	TLgUSED	0xffffff

typedef struct TrieNode
    {
    int			tn_transitions;
    int			tn_items;
    unsigned char	tn_ntrans;
    unsigned char	tn_nitem;
    unsigned char	tn_flags;
    unsigned char	tn_unused;
    } TrieNode;

typedef struct TrieLink
    {
#   ifdef WORDS_BIGENDIAN
    unsigned int		tl_key:8;
    unsigned int		tl_to:24;
#   else
    unsigned int		tl_to:24;
    unsigned int		tl_key:8;
#   endif
    } TrieLink;

/************************************************************************/
/*									*/
/*  An 'IND' struct.							*/
/*									*/
/*  Offsets to the location in the file header are prepended as a	*/
/*  comment.								*/
/*									*/
/************************************************************************/

typedef struct IND
    {
    /*  0  */	int		ind_magic;	/*  To avoid trouble	*/

    /*  -  */	int		ind_fd;		/*  For future use.	*/

    /*  8  */	int		ind_start;
    /*  -  */	int		ind_readonly;

    /*  -  */	TrieNode *	ind_nodes;
    /*  -  */	TrieNode **	indNodePages;
    /* 24  */	int		ind_nnode;
    /* 28  */	int		indAllocatedNodes;

    /*  -  */	TrieLink *	ind_links;
    /*  -  */	TrieLink **	indLinkPages;
    /* 40  */	int		indAllocatedLinks;
    /* 44  */	int		ind_lfree;
    /* 48  */	int		ind_lfull;

    /*  -  */	int *		ind_classes;
    /*  -  */	int **		ind_cpages;
    /* 60  */	int		ind_ccount;
    /* 64  */	int		ind_cfree;
    /* 68  */	int		ind_cfull;
    /* 72  The original size of an ind struct.			*/

    /* 72  */	unsigned char *	indMmappedFile;
    /* 76  */	unsigned long	indMmappedSize;
    } IND;

#   define	NODE(ind,tn) ((ind)->indNodePages[(tn)/TNsBLOCK]+((tn)%TNsBLOCK))
#   define	LINK(ind,tl) ((ind)->indLinkPages[(tl)/TLsBLOCK]+((tl)%TLsBLOCK))
#   define	ITEMS(ind,cl) ((ind)->ind_cpages[(cl)/ITsBLOCK]+((cl)%ITsBLOCK))

extern IND *	indINDmake( int read_only );
extern IND *	indINDread( const char * filename, int read_only );
extern int	indINDput( IND * ind, const unsigned char * );
extern int	indINDforget( IND * ind, unsigned char * );
extern int	indINDget( int *, IND * ind, int,
				const unsigned char * );
extern void	indINDfree( IND * ind );
extern int	indINDwrite( IND * ind, const char * filename );
extern int	indTNmake( IND * ind );
extern void	indTNfree( IND * ind, int tn );
extern int	indTLalloc( IND * ind, int old, int n );
extern void	indTLfree( IND * ind, int tl );
extern IND *	indINDmini( IND * ind );
extern void	indTLprint( IND *, int );
extern void	indTNprint( IND *, int );
extern void	indINDprint( IND * );
extern void	indDump( IND *, int, int );
extern int	indINDstep( int *, IND * ind, int, const unsigned char * );
extern int	indINDguess( IND * ind,
			    const unsigned char *	word,
			    SpellGuessContext *		sgc,
			    int				how,
			    const GuessSubstitution *	typos,
			    int				count,
			    const unsigned char *	charKinds,
			    const unsigned char *	charShifts );

extern int		indITalloc( IND * ind, int old, int n );
extern void		indITfree( IND * ind, int cl );
extern int		indITset( IND * ind , int tn, int item );
extern int		indITget( IND * ind , int tn, int *, int ** );

extern int indWRDget(	IND *			ind,
			int *			pWhatWasShifted,
			const unsigned char *	word,
			int			asPrefix,
			const unsigned char *	charKinds,
			const unsigned char *	charShifts );

extern int	indWRDguess(	IND *				ind,
				const unsigned char *		wrd,
				SpellGuessContext *		sgc,
				const GuessSubstitution *	typos,
				int				count,
				const unsigned char *		charKinds,
				const unsigned char *		charShifts );

extern IND *	indINDrenumber( IND * ind );

extern void indINDcount( IND *	ind );
extern int indITwalk( IND *	ind );
extern int indTLwalk( IND *	ind );
