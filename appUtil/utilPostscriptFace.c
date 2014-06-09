/************************************************************************/
/*									*/
/*  Keep a list of font faces.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<time.h>

#   include	<utilTree.h>
#   include	<utilPostscriptFace.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Collect the fonts in a document					*/
/*  This has two functions:						*/
/*	1)  They are needed for the DSC comments.			*/
/*	2)  This makes it possible to define shorthands in the prologue	*/
/*									*/
/************************************************************************/

int utilRememberPostsciptFace(		PostScriptTypeList *	pstl,
					int			encoding,
					const AfmFontInfo *	afi,
					const TextAttribute *	ta,
					const char *		prefix,
					int			appearsInText )
    {
    PostScriptFace *	psf;
    const char *	fontName;
    int			faceIndex;

    char		faceId[FACElenID+1];
    char *		s;

    int			ref;
    FaceReference *	fr;

    if  ( ! pstl->pstlFaceTree )
	{
	const int	ownKeys= 1;

	pstl->pstlFaceTree= utilTreeMakeTree( ownKeys );
	if  ( ! pstl->pstlFaceTree )
	    { XDEB(pstl->pstlFaceTree); return -1;	}
	}

    psf= utilTreeGetEQ( pstl->pstlFaceTree, &fontName, afi->afiFontName );
    if  ( ! psf )
	{
	int	enc;
	void *	prevPsf= (void *)0;

	psf= malloc( sizeof(PostScriptFace) );
	if  ( ! psf )
	    { XDEB(psf); return -1;	}

	psf->psfAfi= afi;

	for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	    { psf->psfEncodingUsed[enc]= 0; }

	psf->psfReferences= (FaceReference *)0;
	psf->psfReferenceCount= 0;
	psf->psfAppearsInText= 0;
	psf->psfEmbed= PSembedUNKNOWN;
	psf->psfFontFileName= (char *)0;
	psf->psfFontFileNameLength= 0;

	if  ( utilTreeStoreValue( pstl->pstlFaceTree, &prevPsf,
					&fontName, afi->afiFontName,  psf ) )
	    { SDEB(afi->afiFontName); free( psf ); return -1; }
	}

    faceIndex= FACE_INDEX( ta->taFontIsSlanted, ta->taFontIsBold );

    s= faceId;
    sprintf( s, "%s%d", prefix, ta->taFontNumber );
    s += strlen( s );
    if  ( ta->taFontIsBold )
	{ strcpy( s, "b" ); s++;	}
    if  ( ta->taFontIsSlanted )
	{ strcpy( s, "i" ); s++;	}

    fr= psf->psfReferences;
    for ( ref= 0; ref < psf->psfReferenceCount; fr++, ref++ )
	{
	if  ( fr->frDocFontIndex == ta->taFontNumber	&&
	      fr->frDocFaceIndex == faceIndex		&&
	      ! strcmp( fr->frFaceId, faceId )		)
	    { break;	}
	}

    if  ( ref >= psf->psfReferenceCount )
	{
	fr= (FaceReference *)realloc( psf->psfReferences,
					    (ref+1)* sizeof(FaceReference) );
	if  ( ! fr )
	    { LXDEB(psf->psfReferenceCount,fr); return -1;	}
	psf->psfReferences= fr;
	fr += psf->psfReferenceCount++;

	fr->frDocFontIndex= ta->taFontNumber;
	fr->frDocFaceIndex= faceIndex;
	strcpy( fr->frFaceId, faceId );
	fr->frEncoding= encoding;
	fr->frAppearsInText= appearsInText;
	}

    if  ( fr->frEncoding != encoding )
	{ LLDEB(fr->frEncoding,encoding);	}

    if  ( appearsInText )
	{
	fr->frAppearsInText= appearsInText;
	psf->psfAppearsInText= appearsInText;
	}

    if  ( encoding >= 0			&&
	  encoding < ENCODINGps_COUNT	)
	{
	psf->psfEncodingUsed[encoding]= 1;
	pstl->pstlEncodingUsed[encoding]= 1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void utilInitPostScriptFaceList( PostScriptTypeList *	pstl )
    {
    int		enc;

    pstl->pstlFaceTree= (void *)0;

    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	{ pstl->pstlEncodingUsed[enc]= 0; }

    pstl->pstlFontDirectory= (const char *)0;

    return;
    }

static int utilFreePostScriptFace(	const char *	key,
					void *		vpsf,
					void *		through )
    {
    PostScriptFace *	psf= (PostScriptFace *)vpsf;

    if  ( psf )
	{
	if  ( psf->psfReferences )
	    { free( psf->psfReferences );	}
	if  ( psf->psfFontFileName )
	    { free( psf->psfFontFileName );	}

	free( psf );
	}

    return 0;
    }

void utilCleanPostScriptFaceList( PostScriptTypeList *	pstl )
    {
    if  ( pstl->pstlFaceTree )
	{
	utilTreeFreeTree( pstl->pstlFaceTree,
				    utilFreePostScriptFace, (void *)0 );
	}

    return;
    }

