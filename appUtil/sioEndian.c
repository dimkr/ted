#   include	"appUtilConfig.h"
#   include	<sioEndian.h>

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

int sioEndianPutBeInt32(	long			l,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    sioOutPutCharacter( b0, sos );
    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b2, sos );
    sioOutPutCharacter( b3, sos );

    return 0;
    }

int sioEndianPutLeInt32(	long			l,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    sioOutPutCharacter( b3, sos );
    sioOutPutCharacter( b2, sos );
    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b0, sos );

    return 0;
    }

int sioEndianPutBeInt16(	int			i,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    sioOutPutCharacter( b0, sos );
    sioOutPutCharacter( b1, sos );

    return 0;
    }

int sioEndianPutLeInt16(	int			i,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b0, sos );

    return 0;
    }

int sioEndianPutBeUint32(	unsigned long		ul,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    sioOutPutCharacter( b0, sos );
    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b2, sos );
    sioOutPutCharacter( b3, sos );

    return 0;
    }

int sioEndianPutLeUint32(	unsigned long		ul,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    sioOutPutCharacter( b3, sos );
    sioOutPutCharacter( b2, sos );
    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b0, sos );

    return 0;
    }

int sioEndianPutBeUint16(	unsigned int		ui,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    sioOutPutCharacter( b0, sos );
    sioOutPutCharacter( b1, sos );

    return 0;
    }

int sioEndianPutLeUint16(	unsigned int		ui,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    sioOutPutCharacter( b1, sos );
    sioOutPutCharacter( b0, sos );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/************************************************************************/

int sioEndianGetLeInt16(	SimpleInputStream *	sis )
    {
    int			i;

    unsigned char	b0,b1;

    b1= sioInGetCharacter( sis );
    b0= sioInGetCharacter( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

int sioEndianGetBeInt16(	SimpleInputStream *	sis )
    {
    int			i;

    unsigned char	b0,b1;

    b0= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

unsigned int sioEndianGetLeUint16(	SimpleInputStream *	sis )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b1= sioInGetCharacter( sis );
    b0= sioInGetCharacter( sis );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

unsigned int sioEndianGetBeUint16(	SimpleInputStream *	sis )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b0= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

long sioEndianGetLeInt32(	SimpleInputStream *	sis )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b3= sioInGetCharacter( sis );
    b2= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );
    b0= sioInGetCharacter( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

long sioEndianGetBeInt32(	SimpleInputStream *	sis )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b0= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );
    b2= sioInGetCharacter( sis );
    b3= sioInGetCharacter( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

unsigned long sioEndianGetLeUint32(	SimpleInputStream *	sis )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b3= sioInGetCharacter( sis );
    b2= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );
    b0= sioInGetCharacter( sis );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

unsigned long sioEndianGetBeUint32(	SimpleInputStream *	sis )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b0= sioInGetCharacter( sis );
    b1= sioInGetCharacter( sis );
    b2= sioInGetCharacter( sis );
    b3= sioInGetCharacter( sis );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

