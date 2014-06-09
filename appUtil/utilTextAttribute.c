/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilPropMask.h"
#   include	"utilTextAttribute.h"

/************************************************************************/
/*									*/
/*  Set text attributes to a default value.				*/
/*									*/
/************************************************************************/

void utilInitTextAttribute(	TextAttribute *	ta	)
    {
    ta->taFontNumber= -1;
    ta->taFontSizeHalfPoints= 0;
    ta->taTextColorNumber= 0;
    ta->taTextStyleNumber= 0;
    ta->taFontIsBold= 0;
    ta->taFontIsSlanted= 0;
    ta->taTextIsUnderlined= 0;
    ta->taSuperSub= DOCfontREGULAR;
    ta->taSmallCaps= 0;
    ta->taHasStrikethrough= 0;
    ta->taCapitals= 0;
    }

/************************************************************************/
/*									*/
/*  Get text attribute difference mask.					*/
/*									*/
/************************************************************************/

void utilAttributeDifference(		PropertyMask *		pDoneMask,
					const TextAttribute *	taTo,
					const TextAttribute *	taFrom,
					const PropertyMask *	setMask )
    {
    PropertyMask	doneMask;

    if  ( ! setMask )
	{ XDEB(setMask); return;	}

    PROPmaskCLEAR( &doneMask );

    if  ( PROPmaskISSET( setMask, TApropDOC_FONT_NUMBER )			&&
	  taTo->taFontNumber != taFrom->taFontNumber			)
	{ PROPmaskADD( &doneMask, TApropDOC_FONT_NUMBER ); }

    if  ( PROPmaskISSET( setMask, TApropFONTSIZE )			&&
	  taTo->taFontSizeHalfPoints != taFrom->taFontSizeHalfPoints	)
	{ PROPmaskADD( &doneMask, TApropFONTSIZE ); }

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR )			&&
	  taTo->taTextColorNumber != taFrom->taTextColorNumber		)
	{ PROPmaskADD( &doneMask, TApropTEXT_COLOR ); }

    if  ( PROPmaskISSET( setMask, TApropTEXT_STYLE )			&&
	  taTo->taTextStyleNumber != taFrom->taTextStyleNumber		)
	{ PROPmaskADD( &doneMask, TApropTEXT_STYLE ); }

    if  ( PROPmaskISSET( setMask, TApropFONTBOLD )			&&
	  taTo->taFontIsBold != taFrom->taFontIsBold			)
	{ PROPmaskADD( &doneMask, TApropFONTBOLD ); }

    if  ( PROPmaskISSET( setMask, TApropFONTSLANTED )			&&
	  taTo->taFontIsSlanted != taFrom->taFontIsSlanted		)
	{ PROPmaskADD( &doneMask, TApropFONTSLANTED ); }

    if  ( PROPmaskISSET( setMask, TApropTEXTUNDERLINED )		&&
	  taTo->taTextIsUnderlined != taFrom->taTextIsUnderlined	)
	{ PROPmaskADD( &doneMask, TApropTEXTUNDERLINED ); }

    if  ( PROPmaskISSET( setMask, TApropSUPERSUB )			&&
	  taTo->taSuperSub != taFrom->taSuperSub			)
	{ PROPmaskADD( &doneMask, TApropSUPERSUB ); }

    if  ( PROPmaskISSET( setMask, TApropSMALLCAPS )			&&
	  taTo->taSmallCaps != taFrom->taSmallCaps			)
	{ PROPmaskADD( &doneMask, TApropSMALLCAPS ); }

    if  ( PROPmaskISSET( setMask, TApropCAPITALS )			&&
	  taTo->taCapitals != taFrom->taCapitals			)
	{ PROPmaskADD( &doneMask, TApropCAPITALS ); }

    if  ( PROPmaskISSET( setMask, TApropSTRIKETHROUGH )			&&
	  taTo->taHasStrikethrough != taFrom->taHasStrikethrough	)
	{ PROPmaskADD( &doneMask, TApropSTRIKETHROUGH ); }

    *pDoneMask= doneMask;
    return;
    }

void utilUpdateTextAttribute(		PropertyMask *		pDoneMask,
					TextAttribute *		taTo,
					const TextAttribute *	taFrom,
					const PropertyMask *	setMask )
    {
    PropertyMask	doneMask= *pDoneMask;

    if  ( PROPmaskISSET( setMask, TApropDOC_FONT_NUMBER )			&&
	  taTo->taFontNumber != taFrom->taFontNumber			)
	{
	taTo->taFontNumber= taFrom->taFontNumber;
	PROPmaskADD( &doneMask, TApropDOC_FONT_NUMBER );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSIZE )			&&
	  taTo->taFontSizeHalfPoints != taFrom->taFontSizeHalfPoints	)
	{
	taTo->taFontSizeHalfPoints= taFrom->taFontSizeHalfPoints;
	PROPmaskADD( &doneMask, TApropFONTSIZE );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR )			&&
	  taTo->taTextColorNumber != taFrom->taTextColorNumber		)
	{
	taTo->taTextColorNumber= taFrom->taTextColorNumber;
	PROPmaskADD( &doneMask, TApropTEXT_COLOR );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_STYLE )			&&
	  taTo->taTextStyleNumber != taFrom->taTextStyleNumber		)
	{
	taTo->taTextStyleNumber= taFrom->taTextStyleNumber;
	PROPmaskADD( &doneMask, TApropTEXT_STYLE );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTBOLD )			&&
	  taTo->taFontIsBold != taFrom->taFontIsBold			)
	{
	taTo->taFontIsBold= taFrom->taFontIsBold;	
	PROPmaskADD( &doneMask, TApropFONTBOLD );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSLANTED )			&&
	  taTo->taFontIsSlanted != taFrom->taFontIsSlanted		)
	{
	taTo->taFontIsSlanted= taFrom->taFontIsSlanted;
	PROPmaskADD( &doneMask, TApropFONTSLANTED );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXTUNDERLINED )		&&
	  taTo->taTextIsUnderlined != taFrom->taTextIsUnderlined	)
	{
	taTo->taTextIsUnderlined= taFrom->taTextIsUnderlined;
	PROPmaskADD( &doneMask, TApropTEXTUNDERLINED );
	}

    if  ( PROPmaskISSET( setMask, TApropSUPERSUB )			&&
	  taTo->taSuperSub != taFrom->taSuperSub			)
	{
	taTo->taSuperSub= taFrom->taSuperSub;
	PROPmaskADD( &doneMask, TApropSUPERSUB );
	}

    if  ( PROPmaskISSET( setMask, TApropSMALLCAPS )			&&
	  taTo->taSmallCaps != taFrom->taSmallCaps			)
	{
	taTo->taSmallCaps= taFrom->taSmallCaps;
	PROPmaskADD( &doneMask, TApropSMALLCAPS );
	}

    if  ( PROPmaskISSET( setMask, TApropCAPITALS )			&&
	  taTo->taCapitals != taFrom->taCapitals			)
	{
	taTo->taCapitals= taFrom->taCapitals;
	PROPmaskADD( &doneMask, TApropCAPITALS );
	}

    if  ( PROPmaskISSET( setMask, TApropSTRIKETHROUGH )			&&
	  taTo->taHasStrikethrough != taFrom->taHasStrikethrough	)
	{
	taTo->taHasStrikethrough= taFrom->taHasStrikethrough;
	PROPmaskADD( &doneMask, TApropSTRIKETHROUGH );
	}

    *pDoneMask= doneMask;
    return;
    }

