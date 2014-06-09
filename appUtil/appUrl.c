/************************************************************************/
/*									*/
/*  Retrieve fields from URL's						*/
/*									*/
/*  Implements part of rfc 1738.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	<appUrl.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Extract the scheme part from a URL.					*/
/*									*/
/*  Of failure return -1. Otherwise return the number of characters	*/
/*  consumed including the colon such that url+ rval ponts to the	*/
/*  scheme specific part.						*/
/*									*/
/************************************************************************/

int appUrlGetScheme(		char *		scheme,
				int		schemeLen,
				const char *	url )
    {
    const char *	s= url;
    int			used;
    int			made;

    used= made= 0;
    for (;;)
	{
	if  ( made >= schemeLen )
	    { return -1;	}

	if  ( *s >= 'A' && *s <= 'Z' )
	    { *(scheme++)= *(s++)- 'A'+ 'a'; used++; made++; continue;	}

	if  ( ( *s >= 'a' && *s <= 'z' )	||
	      *s == '+'				||
	      *s == '-'				||
	      *s == '.'				)
	    { *(scheme++)= *(s++); used++; made++; continue;	}

	if  ( *s == ':' )
	    { *(scheme++)= '\0'; used++; break;	}

	return -1;
	}

    return used;
    }

/************************************************************************/
/*									*/
/*  Extract the host part from a URL.					*/
/*									*/
/************************************************************************/

static int appUrlGetHost(	int *		pMade,
				char *		host,
				int		hostLen,
				const char *	s,
				int		xterm )
    {
    int			used;
    int			made;

    used= made= 0;
    for (;;)
	{
	if  ( made >= hostLen )
	    { LLDEB(made,hostLen); return -1;	}

	if  ( *s >= 'A' && *s <= 'Z' )
	    { *(host++)= *(s++)- 'A'+ 'a'; used++; made++; continue;	}

	if  ( ( *s >= 'a' && *s <= 'z' )	||
	      ( *s >= '0' && *s <= '9' )	||
	      *s == '+'				||
	      *s == '-'				||
	      *s == '.'				)
	    { *(host++)= *(s++); used++; made++; continue;	}

	if  ( *s == xterm || *s == '\0' || *s == '/' )
	    { *(host++)= '\0'; break;	}

	SDEB(s); return -1;
	}

    *pMade += made; return used;
    }

/************************************************************************/
/*									*/
/*  Extract the host/port combination from a HTTP URL.			*/
/*									*/
/*  Return the number of characters consumed, including the terminating	*/
/*  slash, such that schemeSpecific+ used points to the end of the	*/
/*  string or to the hpath?search part.					*/
/*									*/
/************************************************************************/

int appUrlGetHttpHostPort(	char *		host,
				int		hostLen,
				char *		port,
				int		portLen,
				const char *	schemeSpecific )
    {
    const char *	s= schemeSpecific;
    int			used;
    int			made;
    int			done;

    if  ( s[0] != '/' || s[1] != '/' )
	{ SDEB(s); return -1;	}

    s += 2; used= 2;

    made= 0;
    done= appUrlGetHost( &made, host, hostLen, s, ':' );

    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    s += done; used += done;

    if  ( *s == ':' )
	{
	s++; used++;

	made= 0;
	for (;;)
	    {
	    if  ( made >= hostLen )
		{ LLDEB(made,hostLen); return -1;	}

	    if  ( *s >= '0' && *s <= '9' )
		{ *(port++)= *(s++); used++; made++; continue;	}

	    if  ( *s == '\0' || *s == '/' )
		{ *(port++)= '\0'; break;	}

	    SDEB(s); return -1;
	    }
	}
    else{ strcpy( port, "80" ); }

    /*
    if  ( *s == '/' )
	{ s++; used++; }
    */

    return used;
    }

/************************************************************************/
/*									*/
/*  Extract the host part from a FILE URL.				*/
/*									*/
/*  Return the number of characters consumed, excluding the terminating	*/
/*  slash, such that schemeSpecific+ used points to the end of the	*/
/*  string or to the path part.						*/
/*									*/
/************************************************************************/

int appUrlGetFileHost(		char *		host,
				int		hostLen,
				const char *	schemeSpecific )
    {
    const char *	s= schemeSpecific;
    int			used;
    int			made;
    int			done;

    if  ( s[0] != '/' || s[1] != '/' )
	{ SDEB(s); return -1;	}

    s += 2; used= 2;

    made= 0;
    done= appUrlGetHost( &made, host, hostLen, s, '/' );

    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    s += done; used += done;

    return used;
    }
