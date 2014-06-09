/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilTextAttributeAdmin.h"

/************************************************************************/
/*									*/
/*  1)  Number of text attributes per page in the list.			*/
/*									*/
/************************************************************************/

#   define	TALsizePAGE	256

/************************************************************************/
/*									*/
/*  Initialize/Clean value nodes.					*/
/*									*/
/************************************************************************/

void utilInitIntegerValueNode(	IntegerValueNode *	ivn )
    {
    ivn->ivnIsLeaf= 0;
    ivn->ivnChildCount= 0;
    ivn->ivnChildren= (IntegerValueNode *)0;

    return;
    }

void utilCleanIntegerValueNode(	IntegerValueNode *	ivn )
    {
    int		i;

    if  ( ivn->ivnIsLeaf )
	{ return;	}

    for ( i= 0; i < ivn->ivnChildCount; i++ )
	{
	utilCleanIntegerValueNode( &(ivn->ivnChildren[i]) );
	}

    if  ( ivn->ivnChildren )
	{ free( ivn->ivnChildren );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void utilInitTextAttributeList(	TextAttributeList *	tal )
    {
    tal->talPages= (TextAttribute **)0;
    tal->talCount= 0;

    utilInitIntegerValueNode( &(tal->talValueNodes) );

    return;
    }

void utilCleanTextAttributeList(	TextAttributeList *	tal )
    {
    int		page;
    int		pageCount= ( tal->talCount+ TALsizePAGE- 1 )/TALsizePAGE;

    for ( page= 0; page < pageCount; page++ )
	{
	if  ( tal->talPages[page] )
	    { free( tal->talPages[page] );	}
	}

    if  ( tal->talPages )
	{ free( tal->talPages );	}

    utilCleanIntegerValueNode( &(tal->talValueNodes) );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute number to a struct value.		*/
/*									*/
/************************************************************************/

void utilGetTextAttributeByNumber(	TextAttribute *			ta,
					const TextAttributeList *	tal,
					int				n )
    {
    int		page= n/ TALsizePAGE;

    if  ( n < 0 || n >= tal->talCount )
	{ LLDEB(n,tal->talCount); utilInitTextAttribute( ta ); return; }

    if  ( ! tal->talPages[page] )
	{ XDEB(tal->talPages[page]); utilInitTextAttribute( ta ); return; }

    *ta= tal->talPages[page][n % TALsizePAGE];
    return;
    }

/************************************************************************/
/*									*/
/*  Call a function for all TextAttributes in the list.			*/
/*									*/
/************************************************************************/

void utilForAllTextAttributes(	const TextAttributeList *	tal,
				TextAttributeFunction		f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < tal->talCount; n++ )
	{
	int			page= n/ TALsizePAGE;
	const TextAttribute *	ta= &(tal->talPages[page][n % TALsizePAGE]);

	(*f)( ta, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int utilTextAttributeNumber(	TextAttributeList *		tal,
				const TextAttribute *		ta )
    {
    int			prop;
    IntegerValueNode *	ivn= &(tal->talValueNodes);

    int			page;
    int			pageCount;

    for ( prop= 0; prop < TAprop_COUNT; prop++ )
	{
	int	propval;

	switch( prop )
	    {
	    case TApropDOC_FONT_NUMBER:
		propval= ta->taFontNumber;
		break;
	    case TApropFONTSIZE:
		propval= ta->taFontSizeHalfPoints;
		break;
	    case TApropFONTBOLD:
		propval= ta->taFontIsBold;
		break;
	    case TApropFONTSLANTED:
		propval= ta->taFontIsSlanted;
		break;
	    case TApropTEXTUNDERLINED:
		propval= ta->taTextIsUnderlined;
		break;
	    case TApropSUPERSUB:
		propval= ta->taSuperSub;
		break;
	    case TApropSMALLCAPS:
		propval= ta->taSmallCaps;
		break;
	    case TApropCAPITALS:
		propval= ta->taCapitals;
		break;
	    case TApropSTRIKETHROUGH:
		propval= ta->taHasStrikethrough;
		break;
	    case TApropTEXT_COLOR:
		propval= ta->taTextColorNumber;
		break;
	    case TApropTEXT_STYLE:
		propval= ta->taTextStyleNumber;
		break;

	    default:
		LDEB(prop); return -1;
	    }

	if  ( propval < 0 )
	    { LLDEB(prop,propval); propval= 0;	}

	if  ( propval >= ivn->ivnChildCount )
	    {
	    IntegerValueNode *	fresh;

	    fresh= realloc( ivn->ivnChildren,
				( propval+ 1 )* sizeof( IntegerValueNode ) );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}
	    ivn->ivnChildren= fresh;

	    fresh += ivn->ivnChildCount;
	    while( ivn->ivnChildCount < propval+ 1 )
		{
		utilInitIntegerValueNode( fresh );
		fresh++; ivn->ivnChildCount++;
		}
	    }

	ivn= &(ivn->ivnChildren[propval]);
	}

    if  ( ivn->ivnIsLeaf )
	{ return ivn->ivnReference;	}

    pageCount= ( tal->talCount+ TALsizePAGE- 1 )/TALsizePAGE;
    page= tal->talCount/ TALsizePAGE;

    if  ( page >= pageCount )
	{
	TextAttribute **	fresh;

	fresh= realloc( tal->talPages,
				( pageCount+ 1 )* sizeof(TextAttribute *) );
	if  ( ! fresh )
	    { LXDEB(tal->talCount,fresh); return -1;	}
	tal->talPages= fresh;

	tal->talPages[page]= malloc( TALsizePAGE* sizeof(TextAttribute) );
	if  ( ! tal->talPages[page] )
	    { LXDEB(page,tal->talPages[page]); return -1;	}
	}

    tal->talPages[page][tal->talCount % TALsizePAGE]= *ta;
    ivn->ivnIsLeaf= 1;
    ivn->ivnReference= tal->talCount++;

    return ivn->ivnReference;
    }
