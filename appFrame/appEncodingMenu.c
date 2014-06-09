/************************************************************************/
/*									*/
/*  Manage a menu to contol the font encoding in a font selection tool.	*/
/*									*/
/************************************************************************/

#   include	<appEncodingMenu.h>
#   include	<appFrame.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adapt to a font, encoding combination.				*/
/*									*/
/************************************************************************/

void appEncodingMenuSetEncoding(
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff,
				int				charsetIdx )
    {
    int				i;

    if  ( charsetIdx < 0 || charsetIdx >= CHARSETidxCOUNT )
	{ appEncodingMenuUnsetEncoding( aem );	}
    else{
	if  ( aem->aemCharsetIndex != charsetIdx )
	    {
	    aem->aemCharsetIndex= charsetIdx;

	    appSetOptionmenu( &(aem->aemEncodingOptionmenu), charsetIdx );
	    }
	}

    for ( i= 0; i < CHARSETidxCOUNT; i++ )
	{
	appGuiEnableWidget( aem->aemFontEncodingOptions[i],
					dff->dffFontForCharsetIndex[i] >= 0 );
	}

    return;
    }

void appEncodingMenuSetDocEncoding(	AppEncodingMenu *	aem,
					const int *		fontForEncoding,
					int			charsetIdx )
    {
    int				i;

    if  ( charsetIdx < 0 || charsetIdx >= CHARSETidxCOUNT )
	{ LLDEB(charsetIdx,CHARSETidxCOUNT);		}
    else{
	if  ( fontForEncoding[charsetIdx] < 0 )
	    { LLDEB(charsetIdx,fontForEncoding[charsetIdx]);	}

	if  ( aem->aemCharsetIndex != charsetIdx )
	    {
	    aem->aemCharsetIndex= charsetIdx;

	    appSetOptionmenu( &(aem->aemEncodingOptionmenu), charsetIdx );
	    }
	}

    for ( i= 0; i < CHARSETidxCOUNT; i++ )
	{
	appGuiEnableWidget( aem->aemFontEncodingOptions[i],
						fontForEncoding[i] >= 0 );
	}

    return;
    }

void appEncodingMenuUnsetEncoding(	AppEncodingMenu *	aem )
    {
    aem->aemCharsetIndex= -1;
    appSetOptionmenu( &(aem->aemEncodingOptionmenu), -1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt to the encodings of a font family.				*/
/*									*/
/************************************************************************/

void appEncodingMenuAdaptToFamilyEncodings(
					AppEncodingMenu *		aem,
					const DocumentFontFamily *	dff )
    {
    int			charsetIdx= -1;

    if  ( aem->aemCharsetIndex >= 0				&&
	  aem->aemCharsetIndex < CHARSETidxCOUNT		&&
	  dff->dffFontForCharsetIndex[aem->aemCharsetIndex] >= 0	)
	{ charsetIdx= aem->aemCharsetIndex; }

    appEncodingMenuSetEncoding( aem, dff, charsetIdx );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the option menu of an encoding menu.				*/
/*									*/
/************************************************************************/

void appEncodingMenuFillOptionmenu(	char * const *		opts,
					APP_OITEM_CALLBACK_T	callBack,
					void *			through,
					AppEncodingMenu *	aem )
    {
    int		i;

    appEmptyOptionmenu( &(aem->aemEncodingOptionmenu) );

    for ( i= 0; i < CHARSETidxCOUNT; i++ )
	{
	const OfficeCharset *	oc;

	oc= utilGetOfficeCharsetByIndex( i );

	aem->aemFontEncodingOptions[i]= appAddItemToOptionmenu(
				    &(aem->aemEncodingOptionmenu),
				    opts[i], callBack, through );

	if  ( ! oc )
	    { XDEB(oc); continue;	}
	if  ( oc->ocOfficeCharsetPreferred >= 0 )
	    {
	    appOptionmenuItemSetVisibility(
				    aem->aemFontEncodingOptions[i], 0 );
	    }
	}

    appSetOptionmenu( &(aem->aemEncodingOptionmenu), 0 );
    aem->aemCharsetIndex= 0;

    appOptionmenuRefreshWidth( &(aem->aemEncodingOptionmenu) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the textual representation of the encoding names.		*/
/*									*/
/************************************************************************/

void appEncodingMenuGetOptionTexts(	char **			opts,
					EditApplication *	ea )
    {
    int				i;
    AppConfigurableResource	acr[CHARSETidxCOUNT];
    AppConfigurableResource *	acri;

    int				ignored= 0;

    acri= acr;
    for ( i= 0; i < CHARSETidxCOUNT; acri++, i++ )
	{
	const OfficeCharset *	oc;

	oc= utilGetOfficeCharsetByIndex( i );
	if  ( ! oc )
	    { break;	}

	APP_SET_RESOURCE( acri, oc->ocId, i* sizeof(char *), oc->ocLabel );
	}

    appGuiGetResourceValues( &ignored, ea, opts, acr, i );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt to a changed family by choosing the first accessible charset.	*/
/*									*/
/************************************************************************/

int appEncodingMenuAdaptToFamily(
				int *				pFontNumber,
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff )
    {
    int				charsetIdx= aem->aemCharsetIndex;

    if  ( charsetIdx < 0				||
	  dff->dffFontForCharsetIndex[charsetIdx] < 0	)
	{
	int	idx;

	charsetIdx= -1;

	for ( idx= 0; idx < CHARSETidxCOUNT; idx++ )
	    {
	    if  ( dff->dffFontForCharsetIndex[idx] >= 0 )
		{ charsetIdx= idx; break;	}
	    }
	}

    if  ( charsetIdx < 0 )
	{ return 1;	}

    if  ( aem->aemCharsetIndex != charsetIdx )
	{ appEncodingMenuSetEncoding( aem, dff, charsetIdx ); }

    *pFontNumber= dff->dffFontForCharsetIndex[charsetIdx];
    return 0;
    }

int appEncodingMenuAdaptToCharsetIndex(
				int *				pFontNumber,
				AppEncodingMenu *		aem,
				const DocumentFontFamily *	dff,
				int				charsetIdx )
    {
    if  ( charsetIdx < 0				||
	  dff->dffFontForCharsetIndex[charsetIdx] < 0	)
	{ return 1;	}

    if  ( aem->aemCharsetIndex != charsetIdx )
	{ appEncodingMenuSetEncoding( aem, dff, charsetIdx ); }

    *pFontNumber= dff->dffFontForCharsetIndex[charsetIdx];
    return 0;
    }
