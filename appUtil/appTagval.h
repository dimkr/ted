#   ifndef	TAGVAL_H					/*  {{	*/
#   define	TAGVAL_H

/************************************************************************/
/*  A name, value pair:							*/
/************************************************************************/
typedef struct TaggedValue
    {
    const char *	tvName;
    const char *	tvValue;
    unsigned int	tvNameAllocated:1;
    unsigned int	tvValueAllocated:1;
    } TaggedValue;

typedef struct TaggedValueList
    {
    int			tvlCount;
    TaggedValue		tvlValues[1];
    } TaggedValueList;

/************************************************************************/
/*  Procedure declarations.						*/
/************************************************************************/

extern void appTagvalFreeValueList(	TaggedValueList *	tvl );

extern TaggedValueList * appTagvalAllocateList(	int	initialCapacity	);

extern TaggedValueList * appTagvalAddValue(	TaggedValueList *	tvl,
					const char *		name,
					const char *		value,
					int			nameAllocated,
					int			valueAllocated);

extern int appTagvalGetStringValue(	const char **		pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name );

extern int appTagvalGetLongValue(	long *			pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name );

extern int appTagvalGetDoubleValue(	double *		pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name );

#   endif						/*  }}	*/
