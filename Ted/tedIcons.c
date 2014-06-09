/************************************************************************/
/*									*/
/*  Icons and pictures for 'Ted'.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	"appIcons.h"
#   include	"tedApp.h"

#   include	"teddoc.xpm"
#   include	"tedfont.xpm"
#   include	"tedfind.xpm"
#   include	"tedspell.xpm"
#   include	"tedprint.xpm"
#   include	"tedmail.xpm"
#   include	"tedmain.xpm"
#   include	"tedpage.xpm"
#   include	"tedabout.xpm"
#   include	"tedsymbol.xpm"
#   include	"tedtable.xpm"
#   include	"tedprops.xpm"

static NamedPicture	TEDPictures[]=
    {
	{ "teddoc",	teddoc_xpm },
	{ "tedfont",	tedfont_xpm },
	{ "tedfind",	tedfind_xpm },
	{ "tedspell",	tedspell_xpm },
	{ "page",	tedpage_xpm },
	{ "tedmain",	tedmain_xpm },
	{ "print",	tedprint_xpm },
	{ "mail",	tedmail_xpm },
	{ "tedabout",	tedabout_xpm },
	{ "tedsymbol",	tedsymbol_xpm },
	{ "tedtable",	tedtable_xpm },
	{ "props",	tedprops_xpm },
    };

void tedGetNamedPictures(	EditApplication *	ea )
    {
    ea->eaNamedPictures= TEDPictures;
    ea->eaNamedPictureCount= sizeof(TEDPictures)/sizeof(NamedPicture);
    }
