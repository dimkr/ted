#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appSystem.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Actions in the GTK interface that cannot be done without using	*/
/*  plain old X11, or some knowledge of the relationship between GTK	*/
/*  and X11.								*/
/*									*/
/*  The author is aware of the fact that he should not have done this.	*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK


#   include	<gdk/gdkx.h>
#   define	GTK_AND_X11_INCLUDES	1

#   include	<X11/Xresource.h>

#   include	"appFrame.h"
#   include	<appGeoString.h>

void appIconifyShellWidget(		APP_WIDGET		shell )
    {
    XIconifyWindow( GDK_DISPLAY(),
	GDK_WINDOW_XWINDOW( shell->window ), DefaultScreen( GDK_DISPLAY() ) );
    }

void appCopyPixmapValue(	APP_SELECTION_EVENT *	gsd,
				APP_BITMAP_IMAGE	pixmapCopied )
    {
    Pixmap	pixmap= GDK_WINDOW_XWINDOW( pixmapCopied );

    gtk_selection_data_set( gsd, gsd->target, 8* sizeof(Pixmap),
				(unsigned char *)&pixmap, sizeof(Pixmap) );

    return;
    }

void appDrawGtkSetXFillRule(	GdkGC *		gc,
				int		rule )
    {
#   if GTK_MAJOR_VERSION < 2
    GdkGCPrivate *	private= (GdkGCPrivate*)gc;
    GC			xgc= private->xgc;
#   else
    GC			xgc= gdk_x11_gc_get_xgc( gc );
#   endif


    switch( rule )
	{
	case GDK_EVEN_ODD_RULE:	rule= EvenOddRule; break;
	case GDK_WINDING_RULE:	rule= WindingRule; break;
	default:
	    LDEB(rule); rule= EvenOddRule; break;
	}

    XSetFillRule( GDK_DISPLAY(), xgc, rule );
    }

/************************************************************************/
/*									*/
/*  Resolve resource values by going to X11 with the low level XrmXXX	*/
/*  routines.								*/
/*									*/
/*  Implementation follows the schema on p307 of the O'Reilly Xt book.	*/
/*  The numbers in the routine are those in the schema.			*/
/*  !   Command line options are not handeled.				*/
/*  !   $HOME/.Xdefaults-hostname is not handeled.			*/
/*									*/
/*	NYE, Adrian & Tim O'REILLY: "X Toolkit Intrinsics Programming	*/
/*	Manual", ISBN 1-56592-013-9, O'Reilly and Associates,		*/
/*	Sebastopol CA, 1995.						*/
/*									*/
/*									*/
/************************************************************************/

#   define	NAML	256

void appGuiGetResourceValuesGtkX(
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount )
    {
    static int			initialized= 0;
    static XrmDatabase		dbX= (XrmDatabase)0;

    char			name[NAML+1];
    int				appl= strlen( ea->eaApplicationName );
    int				hl;

    static const char		xdefs[]= ".Xdefaults";
    const int			xdefl= sizeof( xdefs )- 1;

    static const char		xlibs[]= "/usr/lib/X11";
    const int			xlibl= sizeof( xlibs )- 1;

    static const char		adefs[]= "app-defaults";
    const int			adefl= sizeof( adefs )- 1;

    if  ( ! initialized )
	{
	char *		rms;
	char *		lang;
	XrmDatabase	db;

	XrmInitialize();

	/*  8: Done by caller  */

	/*  7  */
	db= (XrmDatabase)0;
	lang= getenv( "LANG" );
	if  ( lang && xlibl+ 1+ strlen( lang )+ 1+ adefl+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s/%s",
				xlibs,
				lang,
				adefs,
				ea->eaApplicationName );

	    db= XrmGetFileDatabase( name );
	    }
	if  ( ! db && xlibl+ 1+ adefl+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s",
				xlibs,
				adefs,
				ea->eaApplicationName );
	    db= XrmGetFileDatabase( name );
	    }
	if  ( db )
	    { XrmMergeDatabases( db, &dbX );	}

	/*  6: PATH.. TODO  */
	db= (XrmDatabase)0;

	/*  5: PATH.. TODO  */
	db= (XrmDatabase)0;
	rms= getenv( "XAPPLRESDIR" );
	if  ( rms && strlen( rms )+ 1+ strlen( lang )+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s", rms, lang, ea->eaApplicationName );
	    db= XrmGetFileDatabase( name );
	    }

	if  ( ! db )
	    {
	    hl= appHomeDirectory( name, NAML- appl- 1 );
	    if  ( hl > 0 )
		{
		name[hl]= '/'; hl++;
		strcpy( name+ hl, ea->eaApplicationName );

		db= XrmGetFileDatabase( name );
		}
	    }

	if  ( db )
	    { XrmMergeDatabases( db, &dbX );	}

	/*  4  */
	db= (XrmDatabase)0;
	rms= XResourceManagerString( GDK_DISPLAY() );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }
	else{
	    hl= appHomeDirectory( name, NAML- xdefl- 1 );

	    if  ( hl > 0 )
		{
		name[hl]= '/'; hl++;
		strcpy( name+ hl, xdefs );

		db= XrmGetFileDatabase( name );
		if  ( db )
		    { XrmMergeDatabases( db, &dbX );	}
		}
	    }

	/*  3  */
	db= (XrmDatabase)0;
	rms= XScreenResourceString( DefaultScreenOfDisplay( GDK_DISPLAY() ) );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }

	/*  2: FILE TODO  */
	db= (XrmDatabase)0;
	rms= getenv( "XENVIRONMENT" );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }

	/*  1: ARGS TODO  */
	db= (XrmDatabase)0;

	initialized= 1;
	}

    if  ( dbX )
	{
	int		i;
	char *		values= (char *)pValues;

	for ( i= 0; i < acrCount; acr++, i++ )
	    {
	    int		l;
	    Bool	found;
	    XrmValue	xrmv;
	    char *	type_return= (char *)0;

	    l= strlen( acr->acrResourceName );
	    if  ( appl+ 1+ l > NAML )
		{ SDEB(acr->acrResourceName); continue;	}

	    sprintf( name, "%s.%s",
				ea->eaApplicationName, acr->acrResourceName );

	    found= XrmGetResource( dbX, name, name, &type_return, &xrmv );

	    if  ( found )
		{
		*((const char **)(values+acr->acrStructOffset))=
							(char *)xrmv.addr;
		}
	    }
	}

    return;
    }
				
#   endif
