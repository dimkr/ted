/************************************************************************/
/*									*/
/*  Define a mapping between fonts and their faces as they appear in an	*/
/*  office document and the family/encoding/face combinations as they	*/
/*  are used in printing, and indirectly in showing a document on	*/
/*  screen.								*/
/*									*/
/*  1)  Indirection to the family in the administration of PostScript	*/
/*	fonts.								*/
/*  2)  Indirection to the face in the family.				*/
/*  3)  Shortcut to the metrics of the face.				*/
/*  4)  Attributes of the face.						*/
/*  5)  Encoding for which this entry is used.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_POSTSCRIPT_FACE_H
#   define	UTIL_POSTSCRIPT_FACE_H

#   include	<utilDocFont.h>
#   include	<psFont.h>

#   define	FACElenID	24

typedef struct FaceReference
    {
		/*  KEY  */
    int		frDocFontIndex;
    int		frDocFaceIndex;
    char	frFaceId[FACElenID+1];

		/*  DEP  */
    int		frEncoding;
    int		frAppearsInText;
    } FaceReference;

typedef struct PostScriptFace
    {
    const AfmFontInfo *	psfAfi;				/*  3		*/
    unsigned char	psfEncodingUsed[ENCODINGps_COUNT];

    FaceReference *	psfReferences;
    int			psfReferenceCount;
    int			psfAppearsInText;
    int			psfEmbed;
#			define	PSembedNO	-1
#			define	PSembedUNKNOWN	0
#			define	PSembedCOPY	1
#			define	PSembedBTOA	2
#			define	PSembedTTFTO42	3
    char *		psfFontFileName;
    int			psfFontFileNameLength;
    } PostScriptFace;

typedef struct PostScriptTypeList
    {
    void *		pstlFaceTree;
    int			pstlEncodingUsed[ENCODINGps_COUNT];
    const char *	pstlFontDirectory;
    } PostScriptTypeList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilRememberPostsciptFace(	PostScriptTypeList *	pstl,
					int			encoding,
					const AfmFontInfo *	afi,
					const TextAttribute *	ta,
					const char *		prefix,
					int			appearsInText );

extern void utilInitPostScriptFaceList( PostScriptTypeList *	pstl );
extern void utilCleanPostScriptFaceList( PostScriptTypeList *	pstl );

#   endif	/*  UTIL_POSTSCRIPT_FACE_H	*/
