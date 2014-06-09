/************************************************************************/
/*									*/
/*  Simple io streams, encoding/decoding into/from 12 bits LZW format.	*/
/*									*/
/*  NOTE that the initial 'CodeSize' byte of the LZW stream is NOT	*/
/*  read nor written by this library. It is passed as an argument to	*/
/*  the stream open routines. This allows us to ignore the fact that	*/
/*  files have the compressed data in a special format. The indirection	*/
/*  GIF stream takes care of that.					*/
/*									*/
/*  This is an isolated and optimized version of code found in 'giflib'	*/
/*  The Original Authors:						*/
/*									*/
/*  14 Jun 89 - Version 1.0 by Gershon Elber.				*/
/*   3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89,	*/
/*		Unique names).						*/
/*  26 Jun 96 - Version 3.0 by Eric S. Raymond (Full GIF89 support)	*/
/*									*/
/*  Also includes fragments borrowed from the ImageMagick code.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioLzw.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Common LZW Compression/Decompression state variables.		*/
/*									*/
/************************************************************************/

#   define LZ_MAX_CODE		4095	/* Biggest code possible in 12 bits. */
#   define LZ_BITS		12
#   define FIRST_CODE		4097    /* Impossible code, to signal first. */
#   define NO_SUCH_CODE		4098    /* Impossible code, to signal empty. */

typedef struct LzwState
    {
    int			lsCurrentCode;
			/************************************************/
			/*  The code last issued, or a bogus value to	*/
			/*  indicate that we are at the beginning of	*/
			/*  the output.					*/
			/************************************************/
    int			lsNextCode;
			/************************************************/
			/*  The next code the algorithm can generate.	*/
			/************************************************/
    int			lsCurrentCodeSize;
			/************************************************/
			/*  The number of bits required to represent	*/
			/*  RunningCode.				*/
			/************************************************/
    int			lsMaxCode1;
			/************************************************/
			/*  1 bigger than max. possible code, in	*/
			/*  RunningBits bits.				*/
			/************************************************/
    int			lsInitialCodeSize;
			/************************************************/
			/*  The number of bits needed to hold all	*/
			/*  symbold in the original alphabet.		*/
			/************************************************/
    int			lsClearCode;
    int			lsEOFCode;
			/************************************************/
			/*  Special Codes.				*/
			/************************************************/
    int			lsBitsAccumulated;
    unsigned long	lsAccumulator;
			/************************************************/
			/*  For collecting codes in bytes.		*/
			/*  -   Number of bits in CrntShiftDWord	*/
			/*  -   To hold the byte values.		*/
			/************************************************/
    } LzwState;

/************************************************************************/
/*									*/
/*  Initialise the state of a compressor/decompressor.			*/
/*									*/
/************************************************************************/

static void sioLzwInitState(	LzwState *	ls,
				int		codeSize )
    {
    ls->lsInitialCodeSize= codeSize;
    ls->lsClearCode= ( 1 << codeSize );
    ls->lsEOFCode= ls->lsClearCode+ 1;
    ls->lsNextCode= ls->lsEOFCode+ 1;
    ls->lsCurrentCodeSize= codeSize + 1;
    ls->lsMaxCode1= 1 << ls->lsCurrentCodeSize;
    ls->lsBitsAccumulated= 0;
    ls->lsAccumulator= 0;

    ls->lsCurrentCode= FIRST_CODE;

    return;
    }

/************************************************************************/
/*									*/
/*  12 bits LZW decompression specific data.				*/
/*									*/
/************************************************************************/

typedef struct LzwInputStream
    {
    LzwState		lisState;
    SimpleInputStream *	lisSisLzw;

    int			lisExhausted;
    int			lisLastCode;
			/************************************************/
			/*  The code before the current code.		*/
			/************************************************/
    int			lisStackPtr;
			/************************************************/
			/*  For character stack (see below).		*/
			/************************************************/
    unsigned char	lisStack[LZ_MAX_CODE];
			/************************************************/
			/*  Decoded pixels are stacked here.		*/
			/************************************************/
    unsigned char	lisSuffix[LZ_MAX_CODE+1];
    unsigned int	lisPrefix[LZ_MAX_CODE+1];
			/************************************************/
			/*  Keep track of the meaning of the Codes.	*/
			/************************************************/
    } LzwInputStream;


/************************************************************************/
/*									*/
/*  Decompress a 12 bits LZW input stream.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  The LZ decompression input routine:					*/
/*									*/
/*  This routine is responsable for the decompression of the bit	*/
/*  stream from 8 bits (bytes) packets, into the real codes.		*/
/*  Returns 0 if read succesfully, -1 otherwise.			*/
/*									*/
/*  1)  As long as we need to get more bytes from input stream for	*/
/*	next code							*/
/*  2)  Extract the code.						*/
/*  3)  And shift it away.						*/
/*  4)  Increment the next code to be implicitly expected. (What the	*/
/*	encoder is supposed to send next) and if it is beyond what fits	*/
/*	in the current number of bits increment the number of bits.	*/
/*	NOTE however that codes above 4095 are used for special		*/
/*	signaling.							*/
/*									*/
/************************************************************************/

static int sioInLzwReadCode(	LzwInputStream *	lis,
				int *			pCode )

{
    LzwState *			ls= &(lis->lisState);

    static unsigned int CodeMasks[]=
	{
	0x0000,
	0x0001,
	0x0003,
	0x0007,
	0x000f,
	0x001f,
	0x003f,
	0x007f,
	0x00ff,
	0x01ff,
	0x03ff,
	0x07ff,
	0x0fff
	};

    /*  1  */
    while( ls->lsBitsAccumulated < ls->lsCurrentCodeSize )
	{
	int			byte;

	byte= sioInGetCharacter( lis->lisSisLzw );
	if  ( byte == EOF )
	    { XDEB(byte); return -1; }

	ls->lsAccumulator |= ((unsigned long) byte) << ls->lsBitsAccumulated;
	ls->lsBitsAccumulated += 8;
	}

    /*  2  */
    *pCode = ls->lsAccumulator & CodeMasks[ls->lsCurrentCodeSize];

    /*  3  */
    ls->lsAccumulator >>= ls->lsCurrentCodeSize;
    ls->lsBitsAccumulated -=  ls->lsCurrentCodeSize;

    /*  4  */
    if  ( ++ls->lsNextCode > ls->lsMaxCode1	&&
	  ls->lsCurrentCodeSize < LZ_BITS		)
	{
	ls->lsMaxCode1 <<= 1;
	ls->lsCurrentCodeSize++;
	}

    return 0;
}

/************************************************************************/
/*									*/
/*  Routine to trace the Prefixes linked list until we get a prefix	*/
/*  which is not a code, but a pixel value (less than ClearCode).	*/
/*  Returns that pixel value.						*/
/*  If the image is defective, we might loop here forever, so we limit	*/
/*  the loops to the maximum possible when the image is image Ok:	*/
/*  LZ_MAX_CODE times.							*/
/*									*/
/************************************************************************/

static int sioInLzwGetPrefixChar(	const unsigned int *	Prefix,
					int			Code,
					int			ClearCode )
{
    int i = 0;

    while( Code > ClearCode	&&
	   i++ <= LZ_MAX_CODE	)
	{ Code = Prefix[Code];	}

    return Code;
}

/************************************************************************/
/*									*/
/*  The LZ decompression routine:					*/
/*									*/
/*  This version decompress the given gif file into Line of length	*/
/*  LineLen.								*/
/*  This routine can be called few times (one per scan line, for	*/
/*  example), in order to complete the whole image.			*/
/*									*/
/*  1)  To not try to continue after the end of input has been found	*/
/*	nor after a previous error.					*/
/*  2)  Pop the contents of the stack before reading the input stream.	*/
/*  3)  Until a sufficient number of bytes is returned, read codes from	*/
/*	the input stream and process them.				*/
/*									*/
/************************************************************************/

static int sioInLzwReadBytes(	void *		voidlis,
				unsigned char *	buffer,
				int		count )
{
    int				i = 0;
    int				j;
    int				CrntCode;
    int				EOFCode;
    int				ClearCode;
    int				CrntPrefix;
    int				LastCode;
    int				StackPtr;
    unsigned char *		Stack;
    unsigned char *		Suffix;
    unsigned int *		Prefix;

    LzwInputStream *		lis= (LzwInputStream *)voidlis;
    LzwState *			ls= &(lis->lisState);

    StackPtr= lis->lisStackPtr;
    Prefix = lis->lisPrefix;
    Suffix = lis->lisSuffix;
    Stack = lis->lisStack;
    EOFCode = ls->lsEOFCode;
    ClearCode = ls->lsClearCode;
    LastCode= lis->lisLastCode;

    /*  1  */
    if  ( lis->lisExhausted )
	{ LDEB(lis->lisExhausted); return 0; }

    /*  2  */
    while( StackPtr > 0 && i < count )
	{ buffer[i++]= Stack[--StackPtr];	}

    /*  3  */
    while( i < count )
	{
	if  ( sioInLzwReadCode( lis, &CrntCode ) )
    	    { LDEB(1); return -1;	}

	if  ( CrntCode == EOFCode )
	    { lis->lisExhausted= 1; i++; break; }
	else if (CrntCode == ClearCode) {
	    /* We need to start over again: */
	    for (j = 0; j <= LZ_MAX_CODE; j++) Prefix[j] = NO_SUCH_CODE;
	    ls->lsNextCode = ls->lsEOFCode + 1;
	    ls->lsCurrentCodeSize = ls->lsInitialCodeSize + 1;
	    ls->lsMaxCode1 = 1 << ls->lsCurrentCodeSize;
	    LastCode= lis->lisLastCode= NO_SUCH_CODE;
	}
	else {
	    /* Its regular code - if in pixel range simply add it to output  */
	    /* stream, otherwise trace to codes linked list until the prefix */
	    /* is in pixel range:					     */
	    if (CrntCode < ClearCode) {
		/* This is simple - its pixel scalar, so add it to output:   */
		buffer[i++] = CrntCode;
	    }
	    else {
		/* Its a code to needed to be traced: trace the linked list  */
		/* until the prefix is a pixel, while pushing the suffix     */
		/* pixels on our stack. If we done, pop the stack in reverse */
		/* (thats what stack is good for!) order to output.	     */
		if (Prefix[CrntCode] == NO_SUCH_CODE) {
		    /* Only allowed if CrntCode is exactly the running code: */
		    /* In that case CrntCode = XXXCode, CrntCode or the	     */
		    /* prefix code is last code and the suffix char is	     */
		    /* exactly the prefix of last code!			     */
		    if (CrntCode == ls->lsNextCode - 2) {
			CrntPrefix = LastCode;
			Suffix[ls->lsNextCode - 2] =
			Stack[StackPtr++] = sioInLzwGetPrefixChar(Prefix,
							LastCode, ClearCode);
		    }
		    else {
			LDEB(1); return -1;
		    }
		}
		else
		    CrntPrefix = CrntCode;

		/* Now (if image is O.K.) we should not get an NO_SUCH_CODE  */
		/* During the trace. As we might loop forever, in case of    */
		/* defective image, we count the number of loops we trace    */
		/* and stop if we got LZ_MAX_CODE. obviously we can not      */
		/* loop more than that.					     */
		j = 0;
		while (j++ <= LZ_MAX_CODE &&
		       CrntPrefix > ClearCode &&
		       CrntPrefix <= LZ_MAX_CODE) {
		    Stack[StackPtr++] = Suffix[CrntPrefix];
		    CrntPrefix = Prefix[CrntPrefix];
		}
		if (j >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE) {
		    LLLDEB(j,CrntPrefix,LZ_MAX_CODE);
		    return -1;
		}
		/* Push the last character on stack: */
		Stack[StackPtr++] = CrntPrefix;

		/* Now lets pop all the stack into output: */
		while (StackPtr != 0 && i < count)
		    buffer[i++] = Stack[--StackPtr];
	    }
	    if (LastCode != NO_SUCH_CODE) {
		Prefix[ls->lsNextCode - 2] = LastCode;

		if (CrntCode == ls->lsNextCode - 2) {
		    /* Only allowed if CrntCode is exactly the running code: */
		    /* In that case CrntCode = XXXCode, CrntCode or the	     */
		    /* prefix code is last code and the suffix char is	     */
		    /* exactly the prefix of last code!			     */
		    Suffix[ls->lsNextCode - 2] =
			sioInLzwGetPrefixChar(Prefix, LastCode, ClearCode);
		}
		else {
		    Suffix[ls->lsNextCode - 2] =
			sioInLzwGetPrefixChar(Prefix, CrntCode, ClearCode);
		}
	    }
	    LastCode = CrntCode;
	}
    }

    lis->lisLastCode= LastCode;
    lis->lisStackPtr= StackPtr;

    return i;
}


/************************************************************************/
/*									*/
/*  Close an Lzw Input stream.						*/
/*									*/
/*  1)  First drain it.							*/
/*									*/
/************************************************************************/

static int sioInLzwClose(	void *	voidlis )
    {
    int				rval= 0;
    LzwInputStream *		lis= (LzwInputStream *)voidlis;

    /*  1 */
    while( ! lis->lisExhausted )
	{
	unsigned char	buffer[SIOsizBUF];

	if  ( sioInLzwReadBytes( voidlis, buffer, SIOsizBUF ) < 0 )
	    { LDEB(1); rval= -1; break;	}
	}

    free( voidlis );

    return rval;
    }

SimpleInputStream * sioInLzwGifOpen(	SimpleInputStream *	sisLzw,
					int			codeSize )
    {
    SimpleInputStream *		sis;
    LzwInputStream *		lis;

    int				i;
    unsigned int *		p;

    lis= (LzwInputStream *)malloc( sizeof(LzwInputStream) );
    if  ( ! lis )
	{ XDEB(lis); return (SimpleInputStream *)0;	}

    lis->lisSisLzw= sisLzw;
    lis->lisExhausted= 0;

    lis->lisStackPtr= 0;
    lis->lisLastCode= NO_SUCH_CODE;

    sioLzwInitState( &(lis->lisState), codeSize );

    p= lis->lisPrefix;
    for ( i = 0; i <= LZ_MAX_CODE; p++, i++ )
	{ *p= NO_SUCH_CODE;	}

    sis= sioInOpen( (void *)lis, sioInLzwReadBytes, sioInLzwClose );

    if  ( ! sis )
	{ XDEB(sis); free( lis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  SimpleOutputStreams doing LZW compression.				*/
/*									*/
/*  1)  Mask nonsense bits from the input of the compressor.		*/
/*									*/
/************************************************************************/

static const unsigned char SIO_LZW_OutMasks[]=
    {
    0x00,
    0x01,
    0x03,
    0x07,
    0x0f,
    0x1f,
    0x3f,
    0x7f,
    0xff
    };

#define HT_SIZE			8192	   /* 12bits = 4096 or twice as big! */
#define HT_KEY_MASK		0x1FFF			      /* 13bits keys */
#define HT_KEY_NUM_BITS		13			      /* 13bits keys */
#define HT_MAX_KEY		8191	/* 13bits - 1, maximal code possible */
#define HT_MAX_CODE		4095	/* Biggest code possible in 12 bits. */

/* The 32 bits of the long are divided into two parts for the key & code:   */
/* 1. The code is 12 bits as our compression algorithm is limited to 12bits */
/* 2. The key is 12 bits Prefix code + 8 bit new char or 20 bits.	    */
#define HT_GET_KEY(l)	(l >> 12)
#define HT_GET_CODE(l)	(l & 0x0FFF)
#define HT_PUT_KEY(l)	(l << 12)
#define HT_PUT_CODE(l)	(l & 0x0FFF)

typedef struct GifHashTableType {
    unsigned long HTable[HT_SIZE];
} GifHashTableType;

/******************************************************************************
* Routine to generate an HKey for the hashtable out of the given unique key.  *
* The given Key is assumed to be 20 bits as follows: lower 8 bits are the     *
* new postfix character, while the upper 12 bits are the prefix code.	      *
* Because the average hit ratio is only 2 (2 hash references per entry),      *
* evaluating more complex keys (such as twin prime keys) is not worth it!     *
******************************************************************************/

# define KeyItem(Item) \
    ((((unsigned long)(Item) >> 12) ^ (unsigned long)(Item)) & HT_KEY_MASK)

/******************************************************************************
* Routine to clear the HashTable to an empty state.			      *
* This part is a little machine depended. Use the commented part otherwise.   *
******************************************************************************/

static void _ClearHashTable(GifHashTableType *HashTable)
{
    memset(HashTable -> HTable, 0xFF, HT_SIZE * sizeof(long));
}

/******************************************************************************
* Routine to insert a new Item into the HashTable. The data is assumed to be  *
* new one.								      *
******************************************************************************/

static void _InsertHashTable(GifHashTableType *HashTable, unsigned long Key, int Code)
{
    int HKey = KeyItem(Key);
    unsigned long *HTable = HashTable -> HTable;

    while (HT_GET_KEY(HTable[HKey]) != 0xFFFFFL) {
	HKey = (HKey + 1) & HT_KEY_MASK;
    }
    HTable[HKey] = HT_PUT_KEY(Key) | HT_PUT_CODE(Code);
}

/******************************************************************************
* Routine to test if given Key exists in HashTable and if so returns its code *
* Returns the Code if key was found, -1 if not.				      *
******************************************************************************/

static int _ExistsHashTable(	GifHashTableType *	HashTable,
				unsigned long		Key )
{
    int			HKey= KeyItem(Key);
    unsigned long *	HTable= HashTable -> HTable, HTKey;

    while ((HTKey = HT_GET_KEY(HTable[HKey])) != 0xFFFFFL) {
	if (Key == HTKey) return HT_GET_CODE(HTable[HKey]);
	HKey = (HKey + 1) & HT_KEY_MASK;
    }

    return -1;
}

/************************************************************************/
/*									*/
/*  Actual Output stream object.					*/
/*									*/
/************************************************************************/

typedef struct LzwGifOutputStream
    {
    GifHashTableType		losHashTable;
    LzwState			losState;
    SimpleOutputStream *	losSosLzw;
    } LzwGifOutputStream;

/************************************************************************/
/*									*/
/*  The actual output routine to the (blocked) stream: 			*/
/*  This routine is responsible for the translation of the bit stream	*/
/*  into 8 bits (byte) packets.						*/
/*									*/
/*  1)  Emit full bytes.						*/
/*									*/
/************************************************************************/

static int sioOutLzwGifWriteCode(	LzwGifOutputStream *	los,
					int			code )
    {
    LzwState *			ls= &(los->losState);
    int				rval= 0;

    ls->lsAccumulator |= ( (long)code ) << ls->lsBitsAccumulated;
    ls->lsBitsAccumulated += ls->lsCurrentCodeSize;

    while( ls->lsBitsAccumulated >= 8 )
	{
	/*  1  */
	if  ( sioOutPutCharacter( ls->lsAccumulator & 0xff,
						    los->losSosLzw ) == EOF )
	    { rval= -1;	}

	ls->lsAccumulator >>= 8;
	ls->lsBitsAccumulated -= 8;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close an LZW output stream.						*/
/*									*/
/*  1)  As the encoding process is one code behind, issue the last	*/
/*	code.								*/
/*  2)  Issue the EOF code.						*/
/*  3)  Emit all bits that are heaped up in the encoder.		*/
/*  4)  Free memory.							*/
/*									*/
/************************************************************************/

static int sioOutLzwGifClose(	void *		voidlos )
    {
    int				rval= 0;
    LzwGifOutputStream *	los= (LzwGifOutputStream *)voidlos;
    LzwState *			ls= &(los->losState);

    /*  1  */
    if  ( sioOutLzwGifWriteCode( los, ls->lsCurrentCode ) )
	{ XDEB(ls->lsCurrentCode); rval= -1;	}

    /*  2  */
    if  ( sioOutLzwGifWriteCode( los, ls->lsEOFCode ) )
	{ XDEB(ls->lsEOFCode); rval= -1;	}

    /*  3  */
    while( ls->lsBitsAccumulated > 0 )
	{
	if  ( sioOutPutCharacter( ls->lsAccumulator & 0xff,
						    los->losSosLzw ) == EOF )
	    { LDEB(1); rval= -1;	}

	ls->lsAccumulator >>= 8;
	ls->lsBitsAccumulated -=  8;
	}

    /*  4  */
    free( los );

    return rval;
    }

/************************************************************************/
/*									*/
/*  The LZ compression routine:						*/
/*  This version compress the given buffer of length count.		*/
/*  This routine can be called several times (one per scan line, for	*/
/*  example), in order to compress the whole image			*/
/*									*/
/*  1)  If the Current code is the initial value that serves to		*/
/*	flag this, set the current code to the first character to	*/
/*	compress. (and consume it)					*/
/*  2)  Make a new key to search the hash table.			*/
/*  3)  Lookup in hash table.						*/
/*  4)  If it is there, simply take the new code as the current code.	*/
/*  5)  If not, emit the current code and set the current code to the	*/
/*	pixel value.							*/
/*      If next code cannot fit into the current code size, must raise	*/
/*	the code size.							*/
/*  6)  BUT.. If the hash table is full, send a Clear code and clear	*/
/*	the hash table as well.						*/
/*									*/
/************************************************************************/

static int sioOutLzwGifWriteBytes(
				void *			voidlos,
				const unsigned char *	buffer,
				int			count )
{
    LzwGifOutputStream *	los= (LzwGifOutputStream *)voidlos;
    LzwState *			ls= &(los->losState);

    int				i= 0;

    /*  1  */
    if  ( ls->lsCurrentCode == FIRST_CODE )
	{
	ls->lsCurrentCode=
		    buffer[i++] & SIO_LZW_OutMasks[ls->lsInitialCodeSize];
	}

    while( i < count )
	{
	int	val= buffer[i++] & SIO_LZW_OutMasks[ls->lsInitialCodeSize];
	int	nextKey;
	int	nextCode;

	/*  2  */
	nextKey= ( ((unsigned long) ls->lsCurrentCode ) << 8) + val;

	/*  3  */
	nextCode= _ExistsHashTable( &(los->losHashTable), nextKey );

	/*  4  */
	if  ( nextCode >= 0 )
	    { ls->lsCurrentCode= nextCode;	}
	else{
	    /*  5  */
	    if  ( sioOutLzwGifWriteCode( los, ls->lsCurrentCode ) )
		{ LDEB(ls->lsCurrentCode); return -1;	}

	    ls->lsCurrentCode= val;

	    if  ( ls->lsNextCode >= ls->lsMaxCode1 )
		{ ls->lsMaxCode1= 1 << ++(ls->lsCurrentCodeSize);	}

	    /*  6  */
	    if  ( ls->lsCurrentCodeSize > 12 )
		{
		ls->lsCurrentCodeSize= 12;

		if  ( sioOutLzwGifWriteCode( los, ls->lsClearCode ) )
		    { LDEB(ls->lsCurrentCode); return -1;	}

		ls->lsNextCode= ls->lsEOFCode+ 1;
		ls->lsCurrentCodeSize= ls->lsInitialCodeSize+ 1;
		ls->lsMaxCode1= 1 << ls->lsCurrentCodeSize;

		_ClearHashTable( &(los->losHashTable) );
		}
	    else{
		_InsertHashTable( &(los->losHashTable),
					    nextKey, ls->lsNextCode );
		ls->lsNextCode++;
		}
	   }
	}

    return count;
}

/************************************************************************/
/*									*/
/*  Open a 12 bit LZW output stream.					*/
/*									*/
/*  NOTE that the initial code size has been communicated outside the	*/
/*	compressed stream. In the GIF case even before the packeted	*/
/*	stream packet header of the first packet.			*/
/*									*/
/*  1)  Allocate memory and remember the output stream.			*/
/*  2)  Initialize the hash table by clearing it.			*/
/*  3)  Initialize the general LZW administration for the current code	*/
/*	size.								*/
/*  4)  General SioOut administration.					*/
/*  Z)  Start with a clear code to be sure that the decoder has also	*/
/*	cleared its hash table.						*/
/*									*/
/************************************************************************/

SimpleOutputStream * sioOutLzwGifOpen(	SimpleOutputStream *	sosLzw,
					int			codeSize )
    {
    SimpleOutputStream *	sos;
    LzwGifOutputStream *	los;

    /*  1  */
    los= (LzwGifOutputStream *)malloc( sizeof(LzwGifOutputStream) );
    if  ( ! los )
	{ XDEB(los); return (SimpleOutputStream *)0;	}

    los->losSosLzw= sosLzw;

    /*  2  */
    _ClearHashTable( &(los->losHashTable) );

    /*  3  */
    sioLzwInitState( &(los->losState), codeSize );

    /*  4  */
    sos= sioOutOpen( (void *)los, sioOutLzwGifWriteBytes,
					    (SIOoutSEEK)0, sioOutLzwGifClose );

    if  ( ! sos )
	{ XDEB(sos); free( los ); return (SimpleOutputStream *)0; }

    /*  Z  */
    if  ( sioOutLzwGifWriteCode( los, los->losState.lsClearCode ) )
	{ LDEB(1); sioOutClose( sos ); return (SimpleOutputStream *)0; }

    return sos;
    }

