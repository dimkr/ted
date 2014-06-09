#   include	<utilCgiEcho.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Echo some information on a cgi request.				*/
/*									*/
/************************************************************************/

void utilCgiEchoTaggedValueList(	TaggedValueList *	tvl,
					const char *		label,
					SimpleOutputStream *	sos )
    {
    int			i;
    const TaggedValue *	tv;

    sioOutPutString( "<TABLE>\r\n", sos );

    if  ( label && label[0] )
	{
	sioOutPutString( "<TR><TD COLSPAN=2><B>", sos );
	sioOutPutString( label, sos );
	sioOutPutString( "</B></TD></TR>\r\n", sos );
	}

    tv= tvl->tvlValues;
    for ( i= 0; i < tvl->tvlCount; tv++, i++ )
	{
	sioOutPutString( "<TR><TD>", sos );

	if  ( tv->tvName )
	    { sioOutPutString( tv->tvName, sos );	}
	else{ sioOutPutString( "&nbsp;", sos );		}

	sioOutPutString( "</TD>\r\n<TD>", sos );

	if  ( tv->tvValue )
	    { sioOutPutString( tv->tvValue, sos );	}
	else{ sioOutPutString( "&nbsp;", sos );		}

	sioOutPutString( "</TD></TR>\r\n", sos );
	}

    sioOutPutString( "</TABLE>\r\n", sos );

    return;
    }

void utilCgiEchoRequest(		const CGIRequest *	cgir,
					SimpleOutputStream *	sos )
    {
    if  ( cgir->cgirHeaderValues )
	{
	utilCgiEchoTaggedValueList( cgir->cgirHeaderValues, "Headers", sos );
	}

    if  ( cgir->cgirEnvironmentValues )
	{
	utilCgiEchoTaggedValueList( cgir->cgirEnvironmentValues,
							"Environment", sos );
	}

    if  ( cgir->cgirQueryValues )
	{
	utilCgiEchoTaggedValueList( cgir->cgirQueryValues, "Query", sos );
	}

    if  ( cgir->cgirCookies )
	{
	utilCgiEchoTaggedValueList( cgir->cgirCookies, "Cookies", sos );
	}

    return;
    }

#   ifdef EXAMPLE_CODE

int main(	int		argc,
		char **		argv )
    {
    int				rval= 0;
    CGIRequest *		cgir= (CGIRequest *)0;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    cgir= appCgiInGetRequest();
    if  ( ! cgir )
	{ XDEB(cgir); rval= -1; goto ready;	}

    sos= sioOutStdoutOpen();
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    sioOutPutString( "Content-Type: text/html\r\n", sos );
    sioOutPutString( "\r\n", sos );

    utilCgiEchoRequest( cgir, sos );

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    if  ( cgir )
	{ appCgiInFreeRequest( cgir );	}

    return rval;
    }

#   endif
