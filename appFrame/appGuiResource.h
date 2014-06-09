/************************************************************************/
/*									*/
/*  Translation of GUI Resource primitives to the actual implementing	*/
/*  GUI system. As we do not have the pretentence to introduce our own	*/
/*  GUI layer, as much as possible is done through macros. (#defines)	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_RESOURCE_H
#   define	APP_GUI_RESOURCE_H

#   ifdef USE_GTK
#   include	<gtk/gtk.h>
#   endif

#   ifdef USE_MOTIF
#   include	<X11/Xlib.h>
#   include	<X11/Intrinsic.h>
#   endif

/************************************************************************/
/*									*/
/*  Resource table entries. In order no to complicate things, all	*/
/*  values are strings. No attempt to make a fully generalised		*/
/*  abstracted universal solution for calling strtol() or strtod()	*/
/*  occasionally is made.						*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

typedef struct AppConfigurableResource
    {
    const char *	acrResourceName;
    unsigned int	acrStructOffset;
    const char *	acrDefaultValue;
    } AppConfigurableResource;

#   define	APP_RESOURCE( x, o, d )					\
				{ (x), (o), (d) }

#   define	APP_SET_RESOURCE( acr, x, o, d )			\
				{					\
				(acr)->acrResourceName= (x);		\
				(acr)->acrStructOffset= (o);		\
				(acr)->acrDefaultValue= (d);		\
				}

#   endif

#   ifdef USE_MOTIF

    typedef XtResource AppConfigurableResource;

#   define	acrResourceName	resource_name
#   define	acrStructOffset	resource_offset
#   define	acrDefaultValue	default_addr

#   define	APP_RESOURCE( x, o, d )					\
				{					\
				(x),(x),				\
				XtRString, sizeof(char *),		\
				(o),					\
				XtRString, (d),				\
				}

#   define	APP_SET_RESOURCE( acr, x, o, d )			\
				{					\
				(acr)->resource_name= (x);		\
				(acr)->resource_class= (x);		\
				(acr)->resource_type= XtRString;	\
				(acr)->resource_size= sizeof(char *);	\
				(acr)->resource_offset= (o);		\
				(acr)->default_type= XtRString;		\
				(acr)->default_addr= (d);		\
				}

#   endif

#   endif	/* APP_GUI_RESOURCE_H */
