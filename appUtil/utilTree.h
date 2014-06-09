/************************************************************************/
/*									*/
/*  Mapping of string values to (void *)pointers.			*/
/*									*/
/************************************************************************/

#   ifndef		UTIL_TREE_H
#   define		UTIL_TREE_H

typedef int (*UTIL_TREE_CALLBACK)(	const char *		key,
					void *			val,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * utilTreeMakeTree(		int			ownKeys );

/**/
extern int utilTreeStoreValue(		void *			tree,
					void **			pPreviousValue,
					const char **		pStoredKey,
					const char *		key,
					void *			val );

/**/
extern void * utilTreeGetLT(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetLE(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetEQ(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetGE(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetGT(		void *			tree,
					const char **		pKey,
					const char *		key );

/**/
extern void * utilTreeGetFirst(		void *			tree,
					const char **		pKey );

extern void * utilTreeGetPrevious(	void *			tree,
					const char **		pKey );

extern void * utilTreeGetCurrent(	void *			tree,
					const char **		pKey );

extern void * utilTreeGetNext(		void *			tree,
					const char **		pKey );

extern void * utilTreeGetLast(		void *			tree,
					const char **		pKey );

/**/
extern int utilTreeDeleteEQ(		void *			tree,
					const char *		key,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

extern int utilTreeDeleteCurrent(	void *			tree,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeForAll(		void *			tree,
					int			stopOnFailure,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeFreeTree(		void *			tree,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeFreeValue(		const char *		key,
					void *			val,
					void *			through );

#   endif	/*	UTIL_TREE_H	*/
