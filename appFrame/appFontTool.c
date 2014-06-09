/************************************************************************/
/*									*/
/*  Font Chooser.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<appGeoString.h>
#   include	<utilPropMask.h>
#   include	<utilTree.h>

#   include	<appFrame.h>
#   include	<appFontTool.h>
#   include	"appDraw.h"

/************************************************************************/
/*									*/
/*  Value lists that can be used when no particular font is selected.	*/
/*									*/
/************************************************************************/

static int EditFontToolSizesHalfPoints[]=
			    {
			    2* 8,
			    2* 9,
			    2* 10,
			    2* 11,
			    2* 12,
			    2* 14,
			    2* 16,
			    2* 18,
			    2* 24,
			    2* 36,
			    2* 48,
			    2* 64
			    };

static const int EditFontToolSizeCount=
		sizeof(EditFontToolSizesHalfPoints)/sizeof(int);

/************************************************************************/
/*									*/
/*  Format the size of the font in the Text box below the list.		*/
/*									*/
/************************************************************************/

static void appFontToolFormatSize(	char *		scratch,
					int		sizeHalfPoints )
    {
    if  ( sizeHalfPoints % 2 )
	{ sprintf( scratch, "%d.5", sizeHalfPoints/ 2 );	}
    else{ sprintf( scratch, "%d", sizeHalfPoints/ 2 );		}
    }

/************************************************************************/
/*									*/
/*  Redraw the sample of the font chooser.				*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appFontRedraw, w, voidafc, exposeEvent )
    {
    AppFontChooser *			afc= (AppFontChooser *)voidafc;
    AppDrawingData *			add= &(afc->afcDrawingData);

    DocumentRectangle			drClip;

    int					x;
    int					y;
    int					wide;
    int					high;

    int					textWide;
    int					fontAscent;
    int					fontDescent;

    const int				ox= 0;
    const int				oy= 0;

    int					l= strlen( afc->afcChoiceText );

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundWhite( add );
    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    if  ( ! afc->afcFont )
	{ /*SXDEB(afc->afcChoiceText,afc->afcFont);*/ return;	}

    appDrawSetForegroundBlack( add );

    appDrawGetSizeOfWidget( &wide, &high, w );

    appDrawTextExtents( &textWide, &fontAscent, &fontDescent,
				add, afc->afcFont, afc->afcChoiceText, l );

    y= ( high- fontAscent- fontDescent+ 1 )/ 2;
    y= y+ fontAscent;
    x= wide/ 2- textWide/2;

    appDrawDrawString( add, x, y, afc->afcChoiceText, l );

    return;
    }

/************************************************************************/
/*									*/
/*  Show the current font size.						*/
/*									*/
/************************************************************************/

static void appFontShowSizeInList(
				AppFontChooser *	afc,
				int			fontSizeHalfPoints )
    {
    int		pos;
    int		sizeChosen= -1;

    for ( pos= 0; pos < EditFontToolSizeCount; pos++ )
	{
	if  ( fontSizeHalfPoints > 0				&&
	      EditFontToolSizesHalfPoints[pos] ==
					fontSizeHalfPoints	)
	    { sizeChosen= pos; break;	}
	}

    if  ( sizeChosen >= 0 )
	{ appGuiSelectPositionInListWidget( afc->afcSizeList, sizeChosen ); }
    else{ appGuiRemoveSelectionFromListWidget( afc->afcSizeList );	    }

    return;
    }

static void appFontShowSizeInText(
				AppFontChooser *	afc,
				int			fontSizeHalfPoints )
    {
    char	scratch[80];

    if  ( fontSizeHalfPoints > 0 )
	{
	appFontToolFormatSize( scratch, fontSizeHalfPoints );
	appStringToTextWidget( afc->afcSizeText, scratch );
	}
    else{
	appStringToTextWidget( afc->afcSizeText, "" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get currently selected font.					*/
/*									*/
/************************************************************************/

static int appFontToolGetSize(	int *			pFontSizeHalfPoints,
				int			anyway,
				AppFontChooser *	afc )
    {
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);
    int				sizeHalfPoints= eta->etaFontSizeHalfPoints;

    char *		val;
    char *		s;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    int			isEmpty;

    s= val= appGetStringFromTextWidget( afc->afcSizeText );
    while( isspace( *s ) )
	{ s++;	}
    isEmpty= ! *s;

    appFreeStringFromTextWidget( val );

    if  ( isEmpty )
	{ sizeHalfPoints= -1;	}
    else{
	int		changed= 0;
	int		sizeTwips= sizeHalfPoints;

	if  ( sizeTwips > 0 )
	    { sizeTwips *= 10;	}

	if  ( appGetLengthFromTextWidget( afc->afcSizeText,
				&sizeTwips, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    {
	    if  ( anyway )
		{ sizeHalfPoints= -1;	}
	    else{ return -1;			}
	    }
	else{
	    char	scratch[80];

	    sizeHalfPoints= ( sizeTwips+ 5 )/ 10;

	    appFontToolFormatSize( scratch, sizeHalfPoints );

	    val= appGetStringFromTextWidget( afc->afcSizeText );

	    if  ( strcmp( val, scratch ) )
		{ appStringToTextWidget( afc->afcSizeText, scratch );	}

	    appFreeStringFromTextWidget( val );
	    }
	}

    *pFontSizeHalfPoints= sizeHalfPoints;
    return 0;
    }

static int appFontGetCurrent(	DocumentFont **		pDf,
				int *			pFace,
				int *			pFontSizeHalfPoints,
				int			anyway,
				AppFontChooser *	afc )
    {
    ExpandedTextAttribute *		etaC= &(afc->afcTextAttributeChosen);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    DocumentFont *			df= (DocumentFont *)0;

    int					fontChosen= -1;
    int					faceChosen= afc->afcFaceChosen;

    int					sizeHalfPoints;

    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return -1;	}

    if  ( PROPmaskISSET( chosenMask, TApropDOC_FONT_NUMBER )	&&
	  etaC->etaFontNumber >= 0				)
	{ fontChosen= etaC->etaFontNumber; }

    if  ( fontChosen < 0 && anyway )
	{ fontChosen= 0; }
    if  ( faceChosen < 0 && anyway )
	{ faceChosen= 0; }
    if  ( sizeHalfPoints < 0 && anyway )
	{ sizeHalfPoints= 24; }

    if  ( fontChosen >= 0 )
	{
	df= afc->afcDocumentFontList.dflFonts+ fontChosen;

	if  ( df->dfPsFaceNumber[faceChosen] < 0 )
	    {
	    int		i;

	    for ( i= 0; i < FONTface_COUNT; i++ )
		{
		if  ( df->dfPsFaceNumber[i] >= 0 )
		    {
		    faceChosen= i;
		    etaC->etaFontIsBold= FACE_BOLD( i );
		    etaC->etaFontIsSlanted= FACE_SLANTED( i );
		    break;
		    }
		}
	    }
	}

    if  ( pDf )
	{ *pDf= df;		}
    if  ( pFace )
	{ *pFace= faceChosen;	}
    if  ( pFontSizeHalfPoints )
	{ *pFontSizeHalfPoints= sizeHalfPoints;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Format the current font name.					*/
/*									*/
/************************************************************************/

static void appFontFormatCurrent(	char *			target,
					AppFontChooser *	afc,
					const DocumentFont *	df,
					int			face,
					int			sizeHalfPoints )
    {
    const char *	familyText= "*";
    char		faceName[40];
    char		sizeStr[40];

    faceName[0]= '?';
    faceName[1]= '\0';

    sizeStr[0]= '*';
    sizeStr[1]= '\0';

    if  ( df )
	{ familyText= df->dfName;	}

    if  ( face >= 0		&&
	  face < FONTface_COUNT &&
	  strlen( afc->afcResources.aftrFaces[face] ) < sizeof(faceName) )
	{ strcpy( faceName, afc->afcResources.aftrFaces[face] ); }

    if  ( sizeHalfPoints > 0 )
	{
	if  ( sizeHalfPoints % 2 )
	    { sprintf( sizeStr, "%d.5", sizeHalfPoints/ 2 );	}
	else{ sprintf( sizeStr, "%d", sizeHalfPoints/ 2 );	}
	}

    sprintf( target, "%s,%s,%s", familyText, faceName, sizeStr );

    return;
    }

/************************************************************************/
/*									*/
/*  Reflect the currently selected font.				*/
/*									*/
/************************************************************************/

static void appFontShowExampleOfCurrent(	AppFontChooser *	afc )
    {
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    AppDrawingData *			add= &(afc->afcDrawingData);
    const DocumentFontList *		dfl= &(afc->afcDocumentFontList);

    DocumentFont *	df= (DocumentFont *)0;
    int			fontSizeHalfPoints= -1;
    int			face;

    int			physicalFont;

    TextAttribute	ta;
    PropertyMask	taSetMask;
    PropertyMask	taDoneMask;

    PROPmaskCLEAR( &taSetMask );
    utilInitTextAttribute( &ta );

    PROPmaskADD( &taSetMask, TApropDOC_FONT_NUMBER );
    PROPmaskADD( &taSetMask, TApropFONTSIZE );
    PROPmaskADD( &taSetMask, TApropFONTBOLD );
    PROPmaskADD( &taSetMask, TApropFONTSLANTED );

    docIndirectTextAttribute( &taDoneMask, &ta, etaC, &taSetMask,
							(RGB8Color **)0, 0 );

    if  ( afc->afcFont )
	{
	/*
	appDrawFreeFont( &(afc->afcDrawingData), afc->afcFont );
	*/
	afc->afcFont= (APP_FONT *)0;
	}

    appFontGetCurrent( &df, &face, &fontSizeHalfPoints, 0, afc );

    appFontFormatCurrent( afc->afcChoiceText, afc,
				    df, face, fontSizeHalfPoints );

    appFontGetCurrent( &df, &face, &fontSizeHalfPoints, 1, afc );
    if  ( ta.taFontNumber < 0 && df )
	{ ta.taFontNumber= df->dfDocFontNumber; }
    if  ( ta.taFontNumber < 0 )
	{ ta.taFontNumber= 0; }

    physicalFont= appOpenScreenFont( add, dfl, &ta );
    if  ( physicalFont < 0 )
	{ LDEB(physicalFont);	}
    else{
	const DrawScreenFontList *	apfl= &(add->addScreenFontList);
	const PostScriptFontList *	psfl= add->addPostScriptFontList;
	const AppFontFamily *		psf;
	const AppFontTypeface *		pst;
	DrawScreenFont *		dsf;

	dsf= apfl->apflFonts+ physicalFont;
	afc->afcFont= dsf->apfFontStruct;

	appDrawSetFont( add, dsf->apfFontStruct );

	psf= psfl->psflFamilies+ dsf->apfPsFamilyNumber;
	pst= psf->affFaces+ dsf->apfFaceIndex;

	if  ( appFontXFont( afc->afcX11ChoiceText, add , psf,
			dsf->apfFontEncoding, pst, dsf->apfFullSizePixels ) )
	    { LDEB(1);	}
	else{
	    AfmFontInfo *	afi= pst->aftFontInfo;

	    appGuiChangeLabelText( afc->afcX11Name, afc->afcX11ChoiceText );
	    appGuiChangeLabelText( afc->afcPsName, afi->afiFontName );
	    }
	}

    appExposeRectangle( add, 0, 0, 0, 0 );

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

/************************************************************************/
/*									*/
/*  A font family has been chosen, or set.				*/
/*  Try to find a face that matches the current attributes.		*/
/*									*/
/*  NOTE: The family list is set by the caller.				*/
/*									*/
/************************************************************************/

static int appFontToolGetFaceNumber(	AppFontChooser *	afc )
    {
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    if  ( PROPmaskISSET( chosenMask, TApropFONTBOLD )		&&
	  PROPmaskISSET( chosenMask, TApropFONTSLANTED )	)
	{ return FACE_INDEX(etaC->etaFontIsSlanted,etaC->etaFontIsBold); }

    return -1;
    }

static void appFontReflectFamily(	AppFontChooser *	afc,
					int			charsetIdx )
    {
    AppEncodingMenu *			aem= &(afc->afcEncodingMenu);
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);
    const DocumentFont *		df= (const DocumentFont *)0;

    const DocumentFontList *		dfl= &(afc->afcDocumentFontList);
    DocumentFontFamily *		dff;

    int					face;
    int					set;

    dff= dfl->dflFamilies+ afc->afcFontFamilyChosen;

    appEncodingMenuSetEncoding( aem, dff, charsetIdx );

    if  ( PROPmaskISSET( chosenMask, TApropDOC_FONT_NUMBER )	&&
	  etaC->etaFontNumber >= 0				)
	{ df= dfl->dflFonts+ etaC->etaFontNumber; }

    appGuiEmptyListWidget( afc->afcFaceList );

    for ( face= 0; face < FONTface_COUNT; face++ )
	{
	afc->afcFaceMapFwd[face]= -1;
	afc->afcFaceMapBck[face]= -1;
	}

    set= 0;
    for ( face= 0; face < FONTface_COUNT; face++ )
	{
	if  ( ! df				||
	      df->dfPsFaceNumber[face] >= 0	)
	    {
	    appGuiAddValueToListWidget( afc->afcFaceList,
					afc->afcResources.aftrFaces[face] );

	    afc->afcFaceMapFwd[set]= face;
	    afc->afcFaceMapBck[face]= set;
	    set++;
	    }
	}

    afc->afcFaceChosen= appFontToolGetFaceNumber( afc );

    if  ( afc->afcFaceChosen >= 0 )
	{
	appGuiSelectPositionInListWidget( afc->afcFaceList,
				    afc->afcFaceMapBck[afc->afcFaceChosen] );
	}
    else{
	appGuiRemoveSelectionFromListWidget( afc->afcFaceList );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Current properties have just been set: adapt to them.		*/
/*									*/
/*  1)  If a font family is set, look for it in the list of families.	*/
/*  2)	Position the list accordingly.					*/
/*  3)	Set the encoding menu.						*/
/*  4)	Select the correct font face in the list widget.		*/
/*  5)	Display the font size.						*/
/*  6)	Update the color chooser.					*/
/*  7)	Set underline toggle.						*/
/*  8)	Set strikethrough toggle.					*/
/*  9)	Set the superscript toggle.					*/
/*  10)	Set the subscript toggle.					*/
/*  11)	Set the smallcaps toggle.					*/
/*  12)	Set the capitals toggle.					*/
/*  13)	Show an example of the current font.				*/
/*									*/
/************************************************************************/

static void appFontReflectProperties(	AppFontChooser *	afc )
    {
    int					family;
    int					charsetIdx= -1;
    int					face;

    AppEncodingMenu *			aem= &(afc->afcEncodingMenu);
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    const DocumentFontList *		dfl= &(afc->afcDocumentFontList);
    const DocumentFont *		df= (const DocumentFont *)0;
    int					set;

    /*  1  */
    family= -1;

    if  ( PROPmaskISSET( chosenMask, TApropDOC_FONT_NUMBER )	&&
	  etaC->etaFontNumber >= 0				)
	{
	df= dfl->dflFonts+ etaC->etaFontNumber;
	family= df->dfDocFamilyIndex;

	charsetIdx= df->dfCharsetIndex;
	}

    /*  2  */
    if  ( afc->afcFontFamilyChosen != family )
	{
	afc->afcFontFamilyChosen= family;

	if  ( afc->afcFontFamilyChosen >= 0 )
	    {
	    appGuiSelectPositionInListWidget( afc->afcFamilyList,
						    afc->afcFontFamilyChosen );
	    }
	else{
	    appGuiRemoveSelectionFromListWidget( afc->afcFamilyList );
	    }

	appFontReflectFamily( afc, charsetIdx );
	}

    /*  3  */
    if  ( PROPmaskISSET( chosenMask, TApropDOC_FONT_NUMBER )	&&
	  afc->afcFontFamilyChosen >= 0				)
	{
	DocumentFontFamily *	dff;

	dff= dfl->dflFamilies+ afc->afcFontFamilyChosen;

	appEncodingMenuSetEncoding( aem, dff, charsetIdx );
	}

    /*  4  */
    face= appFontToolGetFaceNumber( afc );

    if  ( afc->afcFaceChosen != face )
	{
	afc->afcFaceChosen= face;

	if  ( afc->afcFaceChosen >= 0 )
	    {
	    appGuiSelectPositionInListWidget( afc->afcFaceList,
				    afc->afcFaceMapBck[afc->afcFaceChosen] );
	    }
	else{ appGuiRemoveSelectionFromListWidget( afc->afcFaceList );	}
	}

    /*  5  */
    {
    int		fontSizeHalfPoints= -1;

    if  ( PROPmaskISSET( chosenMask, TApropFONTSIZE ) )
	{ fontSizeHalfPoints= etaC->etaFontSizeHalfPoints;	}

    appFontShowSizeInList( afc, fontSizeHalfPoints );
    appFontShowSizeInText( afc, fontSizeHalfPoints );
    }

    /*  6  */
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropTEXT_COLOR ) )
	{
	appColorChooserSetColor( &(afc->afcTextColorChooser),
						etaC->etaTextColorExplicit,
						&(etaC->etaTextColor) );
	}
    else{ appColorChooserUnset( &(afc->afcTextColorChooser) );	}

    /*  7  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropTEXTUNDERLINED ) )
	{ set= etaC->etaTextIsUnderlined;	}
    appGuiSetToggleState( afc->afcUnderlinedToggle, set );

    /*  8  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSTRIKETHROUGH ) )
	{ set= etaC->etaHasStrikethrough;	}
    appGuiSetToggleState( afc->afcStrikethroughToggle, set );

    /*  9  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ set= etaC->etaSuperSub == DOCfontSUPERSCRIPT;	}
    appGuiSetToggleState( afc->afcSuperscriptToggle, set );

    /*  10  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ set= etaC->etaSuperSub == DOCfontSUBSCRIPT;	}
    appGuiSetToggleState( afc->afcSubscriptToggle, set );

    /*  11  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSMALLCAPS ) )
	{ set= etaC->etaSmallCaps;	}
    appGuiSetToggleState( afc->afcSmallcapsToggle, set );

    /*  12  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropCAPITALS ) )
	{ set= etaC->etaCapitals;	}
    appGuiSetToggleState( afc->afcCapitalsToggle, set );

    /*  13  */
    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Underline' toggle was activated.				*/
/*  The 'Strikethrough' toggle was activated.				*/
/*  The 'Superscript' toggle was activated.				*/
/*  The 'Subscript' toggle was activated.				*/
/*  The 'Smallcaps' toggle was activated.				*/
/*  The 'Capitals' toggle was activated.				*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appFontUnderlinedToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropTEXTUNDERLINED ) )
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropTEXTUNDERLINED );
    etaC->etaTextIsUnderlined= ( set != 0 );

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontStrikethroughToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSTRIKETHROUGH )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSTRIKETHROUGH );
    etaC->etaHasStrikethrough= ( set != 0 );

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSuperscriptToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSUPERSUB );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( etaC->etaSuperSub == DOCfontSUBSCRIPT )
	    { resetOther= 1;	}

	etaC->etaSuperSub= DOCfontSUPERSCRIPT;

	if  ( resetOther )
	    { appGuiSetToggleState( afc->afcSubscriptToggle, 0 );	}
	}
    else{
	if  ( etaC->etaSuperSub == DOCfontSUPERSCRIPT )
	    { etaC->etaSuperSub= DOCfontREGULAR;	}
	}

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSubscriptToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSUPERSUB );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( etaC->etaSuperSub == DOCfontSUPERSCRIPT )
	    { resetOther= 1;	}

	etaC->etaSuperSub= DOCfontSUBSCRIPT;

	if  ( resetOther )
	    { appGuiSetToggleState( afc->afcSubscriptToggle, 0 );	}
	}
    else{
	if  ( etaC->etaSuperSub == DOCfontSUBSCRIPT )
	    { etaC->etaSuperSub= DOCfontREGULAR;	}
	}

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSmallcapsToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSMALLCAPS )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSMALLCAPS );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( etaC->etaCapitals )
	    { resetOther= 1;	}

	etaC->etaSmallCaps= 1;

	if  ( resetOther )
	    {
	    etaC->etaCapitals= 0;
	    appGuiSetToggleState( afc->afcCapitalsToggle, etaC->etaCapitals );
	    PROPmaskADD( &(afc->afcChosenMask), TApropCAPITALS );
	    }
	}
    else{
	etaC->etaSmallCaps= 0;
	}

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontCapitalsToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropCAPITALS )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropCAPITALS );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( etaC->etaSmallCaps )
	    { resetOther= 1;	}

	etaC->etaCapitals= 1;

	if  ( resetOther )
	    {
	    etaC->etaSmallCaps= 0;
	    appGuiSetToggleState( afc->afcSmallcapsToggle, etaC->etaSmallCaps );
	    PROPmaskADD( &(afc->afcChosenMask), TApropSMALLCAPS );
	    }
	}
    else{
	etaC->etaCapitals= 0;
	}

    appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }


/************************************************************************/
/*									*/
/*  A font family has been chosen					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontFamilyChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const DocumentFontList *	dfl= &(afc->afcDocumentFontList);
    AppEncodingMenu *		aem= &(afc->afcEncodingMenu);

    int				family;
    int				fontNumber= etaC->etaFontNumber;
    int				changed= 0;

    const DocumentFontFamily *	dff;
    const DocumentFont *	df;

    family= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( family < 0 || family >= dfl->dflFamilyCount )
	{ LLDEB(family,dfl->dflFamilyCount); return;	}

    dff= dfl->dflFamilies+ family;

    fontNumber= etaC->etaFontNumber;
    if  ( appEncodingMenuAdaptToFamily( &fontNumber, aem, dff ) )
	{ return;	}

    if  ( etaC->etaFontNumber != fontNumber )
	{ etaC->etaFontNumber= fontNumber; changed= 1;	}

    if  ( ! PROPmaskISSET( &(afc->afcChosenMask), TApropDOC_FONT_NUMBER ) )
	{ changed= 1;	}

    afc->afcFontFamilyChosen= family;
    PROPmaskADD( &(afc->afcChosenMask), TApropDOC_FONT_NUMBER );

    df= dfl->dflFonts+ fontNumber;
    if  ( afc->afcFaceChosen >= 0			&&
	  df->dfPsFaceNumber[afc->afcFaceChosen] < 0	)
	{
	int	i;

	for ( i= 0; i < FONTface_COUNT; i++ )
	    {
	    if  ( df->dfPsFaceNumber[i] >= 0 )
		{
		afc->afcFaceChosen= i;
		etaC->etaFontIsBold= FACE_BOLD( afc->afcFaceChosen );
		etaC->etaFontIsSlanted= FACE_SLANTED( afc->afcFaceChosen );
		break;
		}
	    }
	}

    if  ( changed )
	{
	appFontReflectFamily( afc, df->dfCharsetIndex );

	appFontShowExampleOfCurrent( afc );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A font style has been chosen					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appFontEncodingChosen, w, voidafc )
    {
    int				charsetIdx;
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    int				changed= 0;

    charsetIdx= appGuiGetOptionmenuItemIndex(
			&(afc->afcEncodingMenu.aemEncodingOptionmenu), w );
    if  ( charsetIdx < 0 || charsetIdx >= CHARSETidxCOUNT )
	{ LLDEB(charsetIdx,CHARSETidxCOUNT); return;	}

    if  ( afc->afcFontFamilyChosen >= 0 )
	{
	AppEncodingMenu *		aem= &(afc->afcEncodingMenu);
	ExpandedTextAttribute *		etaC= &(afc->afcTextAttributeChosen);
	const DocumentFontList *	dfl= &(afc->afcDocumentFontList);
	const DocumentFontFamily *	dff;
	int				fontNumber= etaC->etaFontNumber;

	dff= dfl->dflFamilies+ afc->afcFontFamilyChosen;

	if  ( appEncodingMenuAdaptToCharsetIndex( &fontNumber, aem,
							    dff, charsetIdx ) )
	    { return;	}

	if  ( etaC->etaFontNumber != fontNumber )
	    { etaC->etaFontNumber= fontNumber; changed= 1; }

	if  ( changed )
	    {

	    PROPmaskADD( &(afc->afcChosenMask), TApropDOC_FONT_NUMBER );
	    appFontShowExampleOfCurrent( afc );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A font face has been chosen						*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontFaceChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    PropertyMask *		chosenMask= &(afc->afcChosenMask);

    int				i;

    i= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( i == afc->afcFaceChosen )
	{ return;	}

    if  ( i < 0 || i >= FONTface_COUNT )
	{ LLDEB(i,FONTface_COUNT); return;	}

    afc->afcFaceChosen= afc->afcFaceMapFwd[i];

    etaC->etaFontIsBold= ( afc->afcFaceChosen & 0x01 ) != 0;
    etaC->etaFontIsSlanted= ( afc->afcFaceChosen & 0x02 ) != 0;

    PROPmaskADD( chosenMask, TApropFONTBOLD );
    PROPmaskADD( chosenMask, TApropFONTSLANTED );

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  A font size has been chosen in the list widget.			*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontSizeChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);

    int				i;

    char			scratch[80];

    i= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( i < 0 || i >= EditFontToolSizeCount )
	{ LLDEB(i,EditFontToolSizeCount); return; }

    etaC->etaFontSizeHalfPoints= EditFontToolSizesHalfPoints[i];
    PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );

    appFontToolFormatSize( scratch, etaC->etaFontSizeHalfPoints );
    appStringToTextWidget( afc->afcSizeText, scratch );

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The user typed 'Enter' in the text box with the font size.		*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appFontToolSizeChanged, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);

    const int			anyway= 0;
    int				sizeHalfPoints;

    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return;	}

    if  ( sizeHalfPoints > 0 )
	{
	etaC->etaFontSizeHalfPoints= sizeHalfPoints;
	PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );
	}
    else{
	PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );
	}

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The contents of the font size text box changed: Select the new	*/
/*  value in the list.							*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appFontToolSizeTyped, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);

    int				fontSizeHalfPoints= -1;
    int				fontSizeTwips= -1;

    char *			s;

    s= appGetStringFromTextWidget( afc->afcSizeText );

    if  ( ! appGeoLengthFromString( s, UNITtyPOINTS, &fontSizeTwips ) )
	{
	fontSizeHalfPoints= ( fontSizeTwips+ 5 )/ 10;
	PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );
	}
    else{
	PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );
	}

    appFreeStringFromTextWidget( s );

    if  ( eta->etaFontSizeHalfPoints != fontSizeHalfPoints )
	{ appFontShowSizeInList( afc, fontSizeHalfPoints );	}

    return;
    }

/************************************************************************/
/*									*/
/*  A text color was selected.						*/
/*									*/
/************************************************************************/

static void appFontTextColorChosen(
				ColorChooser *			cc,
				int				which,
				void *				voidafc,
				int				choice,
				const RGB8Color *		rgb8Set )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	if  ( which == TApropTEXT_COLOR )
	    {
	    appInspectorShowRgbPage( afc->afcInspector,
			    afc->afcSubjectPage, which, &(eta->etaTextColor) );
	    return;
	    }

	LLDEB(CHOICEccMORE,which);
	return;
	}

    /*  2  */
    if  ( which == TApropTEXT_COLOR )
	{
	int		explicit= choice != CHOICEccDEFAULT;

	PropertyMask	doneMask;

	PROPmaskCLEAR( &doneMask );

	appColorChooserColorChosen( &doneMask, &changed,
			&(eta->etaTextColor), &(eta->etaTextColorExplicit),
			rgb8Set, explicit, TApropTEXT_COLOR );

	PROPmaskADD( &(afc->afcChosenMask), TApropTEXT_COLOR );

	appColorChooserSetColor( &(afc->afcTextColorChooser),
							explicit, rgb8Set );

	appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

	return;
	}

    LDEB(which); return;
    }

static void appFontGotExplicitColor(	void *			voidafc,
					int 			which,
					const RGB8Color *	rgb8Set )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);
    int				changed= 0;

    if  ( which == TApropTEXT_COLOR )
	{
	const int			explicit= 1;

	PropertyMask		doneMask;

	PROPmaskCLEAR( &doneMask );

	appColorChooserColorChosen( &doneMask, &changed,
		    &(eta->etaTextColor), &(eta->etaTextColorExplicit),
		    rgb8Set, explicit, TApropTEXT_COLOR );

	if  ( ! PROPmaskISSET( &(afc->afcChosenMask),
						    TApropTEXT_COLOR )	||
	      changed							)
	    {
	    appColorChooserSetColor( &(afc->afcTextColorChooser),
					    eta->etaTextColorExplicit,
					    &(eta->etaTextColor) );
	    }

	PROPmaskADD( &(afc->afcChosenMask), TApropTEXT_COLOR );

	appGuiEnableWidget( afc->afcSetButton,
				! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

	return;
	}

    LDEB(which); return;
    }

/************************************************************************/
/*									*/
/*  Cleanup of the Font Chooser.					*/
/*									*/
/************************************************************************/

void appFontChooserCleanPage( AppFontChooser *	afc )
    {
    /*
    if  ( afc->afcFont )
	{ appDrawFreeFont( &(afc->afcDrawingData), afc->afcFont );	}
    */

    docCleanExpandedTextAttribute( &(afc->afcTextAttributeSet) );
    docCleanExpandedTextAttribute( &(afc->afcTextAttributeChosen) );

    docCleanFontList( &(afc->afcDocumentFontList) );

    appCleanDrawingData( &(afc->afcDrawingData) );

    appCleanColorChooser( &(afc->afcTextColorChooser) );

    return;
    }

/************************************************************************/
/*									*/
/*  make the font chooser form with three listboxes.			*/
/*									*/
/************************************************************************/

static void appFontMakeChooseForm(	APP_WIDGET			parent,
					const AppFontToolResources *	aftr,
					AppFontChooser *		afc )
    {
    APP_WIDGET		column;
    APP_WIDGET		label;

    APP_WIDGET		chooseRow;

    const int		heightResizable= 1;

    const int		familyPosition= 0;
    const int		familyColspan= 5;
    const int		facePosition= familyPosition+ familyColspan;
    const int		faceColspan= 3;
    const int		sizePosition= facePosition+ faceColspan;
    const int		sizeColspan= 2;
    const int		colcount= sizePosition+ sizeColspan;

    chooseRow= appMakeRowInColumn( parent, colcount, heightResizable );

    /********************************************************************/
    /*									*/
    /*  Parents with listbox and label:					*/
    /*									*/
    /********************************************************************/

    appMakeColumnInRow( &column, chooseRow, familyPosition, familyColspan );

    appMakeLabelInColumn( &label, column, aftr->aftrFamily );

    appGuiMakeListInColumn( &(afc->afcFamilyList), column, 9,
					    appFontFamilyChosen, (void *)afc );

    /***/

    appMakeColumnInRow( &column, chooseRow, facePosition, faceColspan );

    appMakeLabelInColumn( &label, column, aftr->aftrFace );

    appGuiMakeListInColumn( &(afc->afcFaceList), column, 9,
					    appFontFaceChosen, (void *)afc );

    appMakeOptionmenuInColumn( &(afc->afcEncodingMenu.aemEncodingOptionmenu),
								    column );

    {
    const int		textColumns= 5;
    const int		textEnabled= 1;

    appMakeColumnInRow( &column, chooseRow, sizePosition, sizeColspan );

    appMakeLabelInColumn( &label, column, aftr->aftrSize );

    appGuiMakeListInColumn( &(afc->afcSizeList),
				column, 10, appFontSizeChosen, (void *)afc );

    appMakeTextInColumn( &(afc->afcSizeText), column,
						textColumns, textEnabled );

    appGuiSetGotValueCallbackForText( afc->afcSizeText,
					appFontToolSizeChanged, (void *)afc );

    appGuiSetTypingCallbackForText( afc->afcSizeText,
					appFontToolSizeTyped, (void *)afc );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  make the label to preview the font.					*/
/*									*/
/************************************************************************/

static void appFontMakePreviewDrawing(	APP_WIDGET		parent,
					AppFontChooser *	afc,
					double			xfac,
					int			twipsSize )
    {
    const int		wide= -1;
    int			high= 1.3* ( xfac* twipsSize )+ 0.5;
    const int		heightResizable= 0;

    appGuiMakeDrawingAreaInColumn( &(afc->afcSampleDrawing), parent,
		    wide, high, heightResizable, appFontRedraw, (void *)afc );

    
    appMakeLabelInColumn( &(afc->afcX11Name), parent, "?" );
    appMakeLabelInColumn( &(afc->afcPsName), parent, "?" );

    return;
    }

/************************************************************************/
/*									*/
/*  The set button of a font tool has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appFontSetPushed, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;

    int				sizeHalfPoints;
    const int			anyway= 1;

    if  ( ! afc->afcSetFont )
	{ XDEB(afc->afcSetFont); return;	}

    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return;	}

    if  ( sizeHalfPoints < 0 )
	{ PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );	}
    else{ PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );		}

    (*afc->afcSetFont)( afc->afcTarget,
			    &(afc->afcTextAttributeChosen),
			    &(afc->afcChosenMask) );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of a font tool has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appFontRevertPushed, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;

    docCopyExpandedTextAttribute( &(afc->afcTextAttributeChosen),
					    &(afc->afcTextAttributeSet) );

    afc->afcChosenMask= afc->afcSetMask;

    appFontReflectProperties( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  Reflect/Remember the current font of the application.		*/
/*									*/
/*  1)  Adapt the old fashioned internals to new style interface.	*/
/*									*/
/************************************************************************/

static int appFontSetCurrentFont( AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const ExpandedTextAttribute *	etaNew )
    {
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    ExpandedTextAttribute *	etaS= &(afc->afcTextAttributeSet);

    PropertyMask		doneMask;

    PROPmaskCLEAR( &doneMask );
    docUpdateExpandedTextAttribute( &doneMask, etaS, etaNew, newMask );
    docCopyExpandedTextAttribute( etaC, etaS );

    afc->afcSetMask= *newMask;
    afc->afcChosenMask= afc->afcSetMask;

    appFontReflectProperties( afc );

    return 0;
    }

int appFontExpandCurrentFont(	AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const TextAttribute *		taNew,
				unsigned int			documentId,
				const DocumentFontList *	dfl,
				const RGB8Color *		colors,
				int				colorCount )
    {
    DocumentFontList *		dflTo= &(afc->afcDocumentFontList);

    PropertyMask		doneMask;
    ExpandedTextAttribute	eta;

    docInitExpandedTextAttribute( &eta );
    PROPmaskCLEAR( &doneMask );

    if  ( afc->afcCurrentDocumentId != documentId )
	{
	const int			avoidZero= 1;

	DocumentFontFamily *		dff;
	int				i;

	if  ( docCopyFontList( dflTo, dfl ) )
	    { LDEB(1); return -1; }

	appGuiEmptyListWidget( afc->afcFamilyList );

	dff=  dflTo->dflFamilies;
	for ( i= 0; i < dflTo->dflFamilyCount; dff++, i++ )
	    {
	    appGuiAddValueToListWidget( afc->afcFamilyList,
						    dff->dffFamilyName );
	    }

	appColorChooserSuggestPalette( &(afc->afcTextColorChooser),
					    avoidZero, colors, colorCount );

	afc->afcCurrentDocumentId= documentId;
	}

    docExpandTextAttribute( &doneMask, &eta, taNew, newMask, dflTo,
						    colors, colorCount );

    if  ( appFontSetCurrentFont( afc, newMask, &eta ) )
	{ LDEB(1);	}

    docCleanExpandedTextAttribute( &eta );

    return 0;
    }

/************************************************************************/
/*									*/
/*  make a fonts tool.							*/
/*									*/
/************************************************************************/

void appFontToolFillPage(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    int				i;
    int				pos;
    APP_WIDGET			row;
    APP_WIDGET			sep;

    EditApplication *		ea= afc->afcApplication;

    appInitDrawingData( &(afc->afcDrawingData) );

    afc->afcDrawingData.addPostScriptFontList= &(ea->eaPostScriptFontList);

    afc->afcFont= (APP_FONT *)0;
    afc->afcChoiceText[0]= '\0';
    afc->afcX11ChoiceText[0]= '\0';

    afc->afcTarget= (void *)ea;
    afc->afcSetFont= (FontChooserSetFont)0;
    afc->afcSubjectPage= subjectPage;
    afc->afcResources= *aftr;

    is->isPrivate= (void *)afc;
    is->isGotColor= appFontGotExplicitColor;

    appInitColorChooser( &(afc->afcTextColorChooser) );

    for ( i= 0; i < FONTface_COUNT; i++ )
	{
	afc->afcFaceMapFwd[i]= -1;
	afc->afcFaceMapBck[i]= -1;
	}

    {
    double			horPixPerMM;
    double			verPixPerMM;
    double			xfac;
    double			yfac;

    const int			twipsSize= 20* 48;

    appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );

    appFontMakePreviewDrawing( pageWidget, afc, xfac, twipsSize );

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    appFontMakeChooseForm( pageWidget, aftr, afc );

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    appMakeLabelAndColorChooserRow( &(afc->afcColorRow),
		    &(afc->afcTextColorLabel),
		    &(afc->afcTextColorChooser),
		    pageWidget, aftr->aftrTextColor,
		    &(aftr->aftrTextColorChooserResources),
		    appFontTextColorChosen, TApropTEXT_COLOR, (void *)afc );

    appInspectorMakeToggleRow( &row, pageWidget,
					&(afc->afcUnderlinedToggle),
					&(afc->afcStrikethroughToggle),
					aftr->aftrTextUnderlined,
					aftr->aftrTextStrikethrough,
					appFontUnderlinedToggled,
					appFontStrikethroughToggled,
					(void *)afc );

    appInspectorMakeToggleRow( &row, pageWidget,
					&(afc->afcSuperscriptToggle),
					&(afc->afcSubscriptToggle),
					aftr->aftrSuperscript,
					aftr->aftrSubscript,
					appFontSuperscriptToggled,
					appFontSubscriptToggled,
					(void *)afc );

    appInspectorMakeToggleRow( &row, pageWidget,
					&(afc->afcSmallcapsToggle),
					&(afc->afcCapitalsToggle),
					aftr->aftrSmallcaps,
					aftr->aftrCapitals,
					appFontSmallcapsToggled,
					appFontCapitalsToggled,
					(void *)afc );

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    appInspectorMakeButtonRow( &(afc->afcButtonRow), pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			appFontRevertPushed, appFontSetPushed, afc );

    afc->afcRevertButton= is->isRevertButton;
    afc->afcSetButton= is->isApplyButton;
    }

    docInitExpandedTextAttribute( &(afc->afcTextAttributeSet) );
    docInitExpandedTextAttribute( &(afc->afcTextAttributeChosen) );

    docInitFontList( &(afc->afcDocumentFontList) );
    afc->afcCurrentDocumentId= 0;

    PROPmaskCLEAR( &(afc->afcChosenMask) );
    PROPmaskCLEAR( &(afc->afcSetMask) );

    afc->afcFontFamilyChosen= -2;
    afc->afcFaceChosen= -2;

    /**/
    appGuiEmptyListWidget( afc->afcSizeList );

    for ( pos= 0; pos < EditFontToolSizeCount; pos++ )
	{
	char	scratch[80];

	appFontToolFormatSize( scratch, EditFontToolSizesHalfPoints[pos] );

	appGuiAddValueToListWidget( afc->afcSizeList, scratch );
	}
    /**/

    return;
    }

/************************************************************************/
/*									*/
/*  Final stages in making the font tool.				*/
/*									*/
/************************************************************************/

void appFontToolFillChoosers(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr )
    {
    appEncodingMenuFillOptionmenu( aftr->aftrEncodings,
				    appFontEncodingChosen, (void *)afc,
				    &(afc->afcEncodingMenu) );

    return;
    }

void appFontToolFinishPage(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr )
    {
    EditApplication *	ea= afc->afcApplication;

    appOptionmenuRefreshWidth( &(afc->afcEncodingMenu.aemEncodingOptionmenu) );

    appSetDrawingDataForWidget( afc->afcSampleDrawing,
				ea->eaMagnification, &(afc->afcDrawingData) );

    appFinishColorChooser( &(afc->afcTextColorChooser),
				appGuiGetLabelFont( afc->afcTextColorLabel ) );

    return;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_FontToolResourceTable[]=
    {
    APP_RESOURCE( "fontToolFamily",
		offsetof(AppFontToolResources,aftrFamily),
		"Family" ),
    APP_RESOURCE( "fontToolFace",
		offsetof(AppFontToolResources,aftrFace),
		"Typeface" ),
    APP_RESOURCE( "fontToolSize",
		offsetof(AppFontToolResources,aftrSize),
		"Size" ),

    APP_RESOURCE( "fontToolRegular",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceREGULAR]),
		"Regular" ),
    APP_RESOURCE( "fontToolBold",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceBOLD]),
		"Bold" ),
    APP_RESOURCE( "fontToolSlanted",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceSLANTED]),
		"Italic" ),
    APP_RESOURCE( "fontToolBoldSlanted",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceBOLD_SLANTED]),
		"Bold Italic" ),

    APP_RESOURCE( "fontToolTextColor",
		offsetof(AppFontToolResources,aftrTextColor),
		"Text Color" ),

    APP_RESOURCE( "fontToolTextUnderlined",
		offsetof(AppFontToolResources,aftrTextUnderlined),
		"Underlined" ),
    APP_RESOURCE( "fontToolTextStrikethrough",
		offsetof(AppFontToolResources,aftrTextStrikethrough),
		"Strikethrough" ),

    APP_RESOURCE( "fontToolSuperscript",
		offsetof(AppFontToolResources,aftrSuperscript),
		"Superscript" ),
    APP_RESOURCE( "fontToolSubscript",
		offsetof(AppFontToolResources,aftrSubscript),
		"Subscript" ),

    APP_RESOURCE( "fontToolSmallcaps",
		offsetof(AppFontToolResources,aftrSmallcaps),
		"Small caps" ),
    APP_RESOURCE( "fontToolCapitals",
		offsetof(AppFontToolResources,aftrCapitals),
		"All caps" ),

    APP_RESOURCE( "fontToolTextColorChooserAutomatic",
	offsetof(AppFontToolResources,aftrTextColorChooserResources.
							ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "fontToolTextColorChooserMoreColors",
	offsetof(AppFontToolResources,aftrTextColorChooserResources.
							ccrMoreColors),
	"More Colors..." ),

    };

void appFontToolGetResourceTable(	EditApplication *		ea,
					AppFontToolResources *		aftr )
    {
    static int	gotResources= 0;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)aftr,
				APP_FontToolResourceTable,
				sizeof(APP_FontToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    appEncodingMenuGetOptionTexts( aftr->aftrEncodings, ea );

    return;
    }
