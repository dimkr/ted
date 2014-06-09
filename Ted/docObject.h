/************************************************************************/
/*									*/
/*  Text Buffer: Obects inserted in the text.				*/
/*  Objects are independent entities that are embedded in the text.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_H
#   define	DOC_OBJECT_H

#   include	<utilMemoryBuffer.h>

#   include	"docLayoutPosition.h"
#   include	"docDrawingObject.h"
#   include	"docObjectProperties.h"
#   include	"docPictureProperties.h"

struct BufferDocument;
struct BufferItem;
struct TextParticule;

/************************************************************************/
/*									*/
/*  Kinds of object.							*/
/*									*/
/************************************************************************/

typedef struct InsertedObject
    {
    int			ioKind;		/*  Kind of object.		*/
    int			ioResultKind;	/*  Kind of object.		*/
    int			ioTwipsWide;	/*  objw			*/
    int			ioTwipsHigh;	/*  objh			*/
					/*  NOTE: this is a property of	*/
					/*  the object. Not of the way	*/
					/*  in which it is included.	*/
					/*  That is controlled by the	*/
					/*  scale.			*/
    short int		ioScaleXSet;	/*  In %.			*/
    short int		ioScaleYSet;	/*  In %.			*/
    short int		ioScaleXUsed;	/*  In %.			*/
    short int		ioScaleYUsed;	/*  In %.			*/
    int			ioPixelsWide;	/*  Width of object on screen	*/
    int			ioPixelsHigh;	/*  Height of object on screen	*/

    PictureProperties	ioPictureProperties;

    LayoutPosition	ioY0Position;
    int			ioX0Twips;

    unsigned int	ioRtfResultKind:4; /*  From rslt* tags.		*/
    unsigned int	ioRtfEmbedKind:4;  /*  From objemb.. tags.	*/

    unsigned int	ioInline:8;  	/*  Part of the text flow?	*/

    int			ioTopCropTwips;
    int			ioBottomCropTwips;
    int			ioLeftCropTwips;
    int			ioRightCropTwips;

    MemoryBuffer	ioObjectData;
    MemoryBuffer	ioResultData;

    unsigned char *	ioObjectName;
    unsigned char *	ioObjectClass;

    DrawingShape *	ioDrawingShape;

    APP_BITMAP_IMAGE	ioPixmap;
    void *		ioPrivate;
    } InsertedObject;

typedef void (*DOC_CLOSE_OBJECT)(	struct BufferDocument *	bd,
					struct BufferItem *	bi,
					struct TextParticule *	tp,
					void *			voidadd );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docGetBitmapForObject(	InsertedObject *	io );

extern int docObjectSetData(	InsertedObject *		io,
				const unsigned char *		bytes,
				int				size );

extern int docSetResultData(	InsertedObject *		io,
				const unsigned char *		bytes,
				int				size );

extern int docSaveObjectTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern int docSaveResultTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern int docSetObjectName(	InsertedObject *		io,
				const unsigned char *		name,
				int				len );

extern int docSetObjectClass(	InsertedObject *		io,
				const unsigned char *		name,
				int				len );

extern void docInitObject(	InsertedObject *		io );

extern void docCleanObject(	struct BufferDocument *		bd,
				InsertedObject *		io );

extern void docObjectAdaptToPictureGeometry(
				InsertedObject *		io,
				const PictureProperties *	pip );

extern int docReadMetafileObject(	InsertedObject *	io,
					const char *		filename );

extern int docReadBitmapObject(		InsertedObject *	io,
					const char *		filename );

#   endif	/*  DOC_OBJECT_H	*/
