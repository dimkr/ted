#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appTagval.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Free an array of values, and if necessary its contents.		*/
/*									*/
/************************************************************************/

void appTagvalFreeValueList(	TaggedValueList *	tvl )
    {
    if  ( tvl )
	{
	int		i;

	for ( i= 0; i < tvl->tvlCount; i++ )
	    {
	    if  ( tvl->tvlValues[i].tvNameAllocated	&&
		  tvl->tvlValues[i].tvName		)
		{ free( (char *)tvl->tvlValues[i].tvName );	}
	    if  ( tvl->tvlValues[i].tvValueAllocated	&&
		  tvl->tvlValues[i].tvValue		)
		{ free( (char *)tvl->tvlValues[i].tvValue );	}
	    }

	free( tvl );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Allocate a value list.						*/
/*									*/
/************************************************************************/

TaggedValueList *	appTagvalAllocateList(	int	initialCapacity	)
    {
    TaggedValueList * tvl;
    
    tvl= (TaggedValueList *)malloc(
	    sizeof(TaggedValueList)+ initialCapacity* sizeof(TaggedValue) );

    if  ( ! tvl )
	{ LLDEB(initialCapacity,tvl); return (TaggedValueList *)0;	}

    tvl->tvlCount= 0;

    return tvl;
    }

/************************************************************************/
/*									*/
/*  Add a value to a value list.					*/
/*									*/
/************************************************************************/

TaggedValueList * appTagvalAddValue(	TaggedValueList *	tvl,
					const char *		name,
					const char *		value,
					int			nameAllocated,
					int			valueAllocated )
    {
    TaggedValueList *	rval;
    int			count;

    if  ( tvl )
	{ count= tvl->tvlCount+ 1;	}
    else{ count= 1;			}

    rval= realloc( tvl,
		sizeof(TaggedValueList)+ count* sizeof(TaggedValue) );
    if  ( ! rval )
	{ LLDEB(count,rval); return rval;	}

    rval->tvlCount= count--;

    rval->tvlValues[count].tvName= name;
    rval->tvlValues[count].tvNameAllocated= nameAllocated;
    rval->tvlValues[count].tvValue= value;
    rval->tvlValues[count].tvValueAllocated= valueAllocated;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Retrieve a string,long,double value from a TaggedValueList.		*/
/*									*/
/************************************************************************/

extern int appTagvalGetStringValue(	const char **		pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name )
    {
    int		i;

    if  ( tvl )
	{
	const TaggedValue *	tv= tvl->tvlValues;

	for ( i= 0; i < tvl->tvlCount; tv++, i++ )
	    {
	    if  ( ! strcmp( tv->tvName, name ) )
		{
		*pValue= tv->tvValue;

		if  ( tv->tvValue )
		    { *pNull= 0; return 0; }
		else{ *pNull= 1; return 0; }
		}
	    }
	}

    /* SDEB(name); */ return 1;
    }

extern int appTagvalGetLongValue(	long *			pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name )
    {
    char *		past;
    long		l;

    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tvl, name );

    if  ( res )
	{ SLDEB(name,res); return res;	}

    if  ( n )
	{ *pNull= 1; return 0;	}

    while( *s == ' ' )
	{ s++;	}
    if  ( ! *s )
	{ *pNull= 1; return 0;	}

    l= strtol( s, &past, 10 );

    while( *past == ' ' )
	{ past++;	}

    if  ( *past )
	{ SDEB(s); return -1;	}

    *pNull= 0; *pValue= l; return 0;
    }


extern int appTagvalGetDoubleValue(	double *		pValue,
					int *			pNull,
					const TaggedValueList *	tvl,
					const char *		name )
    {
    char *		past;
    double		d;

    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tvl, name );

    if  ( res )
	{ SLDEB(name,res); return res;	}

    if  ( n )
	{ *pNull= 1; return 0;	}
    while( *s == ' ' )
	{ s++;	}

    if  ( ! *s )
	{ *pNull= 1; return 0;	}

    d= strtod( s, &past );

    while( *past == ' ' )
	{ past++;	}

    if  ( *past )
	{ SDEB(s); return -1;	}

    *pNull= 0; *pValue= d; return 0;
    }
