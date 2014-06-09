#   include	"appUtilConfig.h"
#   include	<stdio.h>
#   include	<appDebugon.h>
#   include	"utilEndian.h"

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

void utilPutBeInt32(	long	l,
			FILE *	f )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    putc( b0, f ); putc( b1, f ); putc( b2, f ); putc( b3, f );
    }

void utilPutLeInt32(	long	l,
			FILE *	f )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    putc( b3, f ); putc( b2, f ); putc( b1, f ); putc( b0, f );
    }

void utilPutBeInt16(	int	i,
			FILE *	f )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    putc( b0, f ); putc( b1, f );
    }

void utilPutLeInt16(	int	i,
			FILE *	f )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    putc( b1, f ); putc( b0, f );
    }

void utilPutBeUint32(	unsigned long		ul,
			FILE *			f )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    putc( b0, f );
    putc( b1, f );
    putc( b2, f );
    putc( b3, f );

    return;
    }

void utilPutLeUint32(	unsigned long		ul,
			FILE *			f )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    putc( b3, f );
    putc( b2, f );
    putc( b1, f );
    putc( b0, f );

    return;
    }

void utilPutBeUint16(	unsigned int		ui,
			FILE *			f )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    putc( b0, f );
    putc( b1, f );

    return;
    }

void utilPutLeUint16(	unsigned int		ui,
			FILE *			f )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    putc( b1, f );
    putc( b0, f );

    return;
    }


/************************************************************************/
/*									*/
/*  Buffer Extraction.							*/
/*									*/
/************************************************************************/

long utilEndianExtractLeInt32(	const unsigned char *	b )
    {
    long	l;

    if  ( b[3] > 127 )
	{
	NEGVALUE32(b[3],b[2],b[1],b[0],l,long)
	}
    else{
	POSVALUE32(b[3],b[2],b[1],b[0],l,long)
	}

    return l;
    }

long utilEndianExtractBeInt32(	const unsigned char *	b )
    {
    long	l;

    if  ( b[0] > 127 )
	{
	NEGVALUE32(b[0],b[1],b[2],b[3],l,long)
	}
    else{
	POSVALUE32(b[0],b[1],b[2],b[3],l,long)
	}

    return l;
    }

unsigned long utilEndianExtractLeUint32(	const unsigned char *	b )
    {
    unsigned long	ul;

    POSVALUE32(b[3],b[2],b[1],b[0],ul,unsigned long)

    return ul;
    }

unsigned long utilEndianExtractBeUint32(	const unsigned char *	b )
    {
    unsigned long	ul;

    POSVALUE32(b[0],b[1],b[2],b[3],ul,unsigned long)

    return ul;
    }

int utilEndianExtractLeInt16(	const unsigned char *	b )
    {
    int		i;

    if  ( b[1] > 127 )
	{
	NEGVALUE16(b[1],b[0],i,int)
	}
    else{
	POSVALUE16(b[1],b[0],i,int)
	}

    return i;
    }

int utilEndianExtractBeInt16(	const unsigned char *	b )
    {
    int		i;

    if  ( b[0] > 127 )
	{
	NEGVALUE16(b[0],b[1],i,int)
	}
    else{
	POSVALUE16(b[0],b[1],i,int)
	}

    return i;
    }

unsigned int utilEndianExtractLeUint16( const unsigned char *	b )
    {
    unsigned int	i;

    POSVALUE16(b[1],b[0],i,unsigned int)

    return i;
    }

unsigned int utilEndianExtractBeUint16(	const unsigned char *	b )
    {
    unsigned int	i;

    POSVALUE16(b[0],b[1],i,unsigned int)

    return i;
    }

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/************************************************************************/

int utilGetLeInt16(	FILE *	f )
    {
    int			i;

    unsigned char	b0,b1;

    b1= getc( f );
    b0= getc( f );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

int utilGetBeInt16(	FILE *	f )
    {
    int			i;

    unsigned char	b0,b1;

    b0= getc( f );
    b1= getc( f );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

unsigned int utilGetLeUint16(	FILE *	f )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b1= getc( f );
    b0= getc( f );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

unsigned int utilGetBeUint16(	FILE *	f )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b0= getc( f );
    b1= getc( f );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

long utilGetLeInt32(	FILE *	f )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b3= getc( f );
    b2= getc( f );
    b1= getc( f );
    b0= getc( f );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

long utilGetBeInt32(	FILE *	f )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b0= getc( f );
    b1= getc( f );
    b2= getc( f );
    b3= getc( f );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

unsigned long utilGetLeUint32(	FILE *	f )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b3= getc( f );
    b2= getc( f );
    b1= getc( f );
    b0= getc( f );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

unsigned long utilGetBeUint32(	FILE *	f )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b0= getc( f );
    b1= getc( f );
    b2= getc( f );
    b3= getc( f );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

/************************************************************************/
/*									*/
/*  Insert in buffers							*/
/*									*/
/************************************************************************/

void utilEndianStoreBeInt32(	long		l,
				unsigned char *	b )
    {
    if  ( l < 0 )
	{
	NEGBYTES32(b[3],b[2],b[1],b[0],l)
	}
    else{
	POSBYTES32(b[3],b[2],b[1],b[0],l)
	}
    }

void utilEndianStoreLeInt32(	long		l,
				unsigned char *	b )
    {
    if  ( l < 0 )
	{
	NEGBYTES32(b[0],b[1],b[2],b[3],l)
	}
    else{
	POSBYTES32(b[0],b[1],b[2],b[3],l)
	}
    }

void utilEndianStoreBeInt16(	int		i,
				unsigned char *	b )
    {
    if  ( i < 0 )
	{
	NEGBYTES16(b[1],b[0],i)
	}
    else{
	POSBYTES16(b[1],b[0],i)
	}
    }

void utilEndianStoreLeInt16(	int		i,
				unsigned char *	b )
    {
    if  ( i < 0 )
	{
	NEGBYTES16(b[0],b[1],i)
	}
    else{
	POSBYTES16(b[0],b[1],i)
	}
    }

