/************************************************************************/
/*									*/
/*  Printf like functionality of SimpleOutputStream's			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdarg.h>
#   include	<limits.h>

#   include	"sioGeneral.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Printf like functionality.						*/
/*									*/
/************************************************************************/

static const char	SIO_PrintfLowerDigits[16]= "0123456789abcdef";
static const char	SIO_PrintfUpperDigits[16]= "0123456789ABCDEF";

# define FLAG_LEFT		0x0001
# define FLAG_SIGNED		0x0002
# define FLAG_SPACE		0x0004
# define FLAG_ALTERNATE		0x0008
# define FLAG_ZEROPAD		0x0010
# define FLAG_SHORT		0x0020
# define FLAG_LONG		0x0040
# define FLAG_LONGD		0x0080
# define FLAG_WIDTH_INDIR	0x0100
# define FLAG_PREC_INDIR	0x0200
# define FLAG_HAS_PRECISION	0x0400

/************************************************************************/
/*									*/
/*  Extract a conversion specification. Everything after the initial	*/
/*  '%' including the conversion character is extracted.		*/
/*									*/
/************************************************************************/

static void sioOutPrintfGetConversion(	unsigned int *	pFlags,
					int *		pMinWidth,
					int *		pPrecision,
					char *		pConversion,
					const char **	pPast,
					const char *	format )
    {
    unsigned int	flags= 0;
    int			minWidth= 0;
    int			precision= 0;
    char		conversion;

    for (;;)
	{
	switch( format[0] )
	    {
	    case '-': flags |= FLAG_LEFT;	format++; continue;
	    case '+': flags |= FLAG_SIGNED;	format++; continue;
	    case ' ': flags |= FLAG_SPACE;	format++; continue;
	    case '#': flags |= FLAG_ALTERNATE;	format++; continue;
	    case '0': flags |= FLAG_ZEROPAD;	format++; continue;

	    default:
		break;
	    }

	break;
	}

    if  ( format[0] == '*' )
	{ flags |= FLAG_WIDTH_INDIR; format++;	}
    else{
	while( isdigit( format[0] ) )
	    { minWidth= 10* minWidth+ ( format[0]- '0' ); format++; }
	}

    if  ( format[0] == '.' )
	{
	format++;
	flags |= FLAG_HAS_PRECISION;

	if  ( format[0] == '*' )
	    { flags |= FLAG_PREC_INDIR; format++;	}
	else{
	    while( isdigit( format[0] ) )
		{ precision= 10* precision+ ( format[0]- '0' ); format++; }
	    }
	}

    switch( format[0] )
	{
	case 'h': flags |= FLAG_SHORT;	format++; break;
	case 'l': flags |= FLAG_LONG;	format++; break;
	case 'L': flags |= FLAG_LONGD;	format++; break;

	default:
	    break;
	}

    conversion= *(format++);

    *pPast= format;

    *pFlags= flags;
    *pMinWidth= minWidth;
    *pPrecision= precision;
    *pConversion= conversion;

    return;
    }

/************************************************************************/
/*									*/
/*  Implemetation of %d, %i, %x, %X, %o, %u.				*/
/*									*/
/*  0)  Impose a maximum on the precision to avoid crashes. This is	*/
/*	allowed by the ANSI C spec.					*/
/*  1)  Default precision is 1.						*/
/*  2)  Collect digits.							*/
/*  3)  Leading zeros in to make up the precision.			*/
/*  3a) If the value does not start with the prefix for the alternate	*/
/*	format, make it start that way. It was not clear to me whether	*/
/*	this should be done before or after the SIGN and SPACE flags.	*/
/*	NOTE that for convenience, the prefix is given in reverse	*/
/*	order.								*/
/*  4)  Emit sign of negative number.					*/
/*  5)  Emit sign of positive number if the '+' flag was set.		*/
/*  6)  Emit a space when the space flag is set. Close reading of the	*/
/*	text about the space flag in 7.9.6.1 reveals that a space	*/
/*	should be emitted when no sign is emitted. The caller makes	*/
/*	sure not to pass flags with FLAG_SPACE set for unsigned		*/
/*	conversions.							*/
/*  7)  Emit leading space or zeros. Zeros should be preceded by the	*/
/*	sign, then by the prefix belonging to the 'alternate form'.	*/
/*  8)  Emit digits. If the sign and the prefix are not emitted before	*/
/*	any padding zeros, those are emitted first.			*/
/*  7)  Emit trailing space.						*/
/*									*/
/*  A)  The space flag only applies for signed conversions. Remove it	*/
/*	for the unsigned ones.						*/
/*  B)  Determine sign and proceed do the generic routine.		*/
/*									*/
/************************************************************************/

# define STACK_SIZE 509

static int sioOutPrintf_dixou(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				int			base,
				const char *		digits,
				const char *		altPrefixRev,
				int			sign,
				unsigned long		value )
    {
    char	stack[STACK_SIZE+3];
    int		digStacked= 0;
    int		totStacked= 0;

    int		emitted= 0;

    /*  0  */
    if  ( precision > STACK_SIZE )
	{ LLDEB(precision,STACK_SIZE); precision= STACK_SIZE;	}

    /*  1  */
    if  ( ! ( flags & FLAG_HAS_PRECISION ) )
	{ precision= 1;	}

    /*  2  */
    while( value > 0 )
	{
	stack[digStacked++]= digits[ value % base ];
	value= value/ base;
	}

    /*  3  */
    while( digStacked < precision )
	{ stack[digStacked++]= '0';	}

    totStacked= digStacked;

    /*  3a  */
    if  ( flags & FLAG_ALTERNATE )
	{
	int		pos= totStacked- 1;
	const char *	s;

	s= altPrefixRev;
	while( *s && pos >= 0 )
	    {
	    if  ( stack[pos] != *s )
		{ break;	}

	    s++; pos--;
	    }

	if  ( *s )
	    {
	    s= altPrefixRev;
	    while( *s )
		{ stack[totStacked++]= *(s++);	}
	    }
	}

    /*  4  */
    if  ( sign < 0 )
	{ stack[totStacked++]= '-';		}
    else{
	/*  5  */
	if  ( sign >= 0 && ( flags & FLAG_SIGNED ) )
	    { stack[totStacked++]= '+';	}
	else{
	    /*  6  */
	    if  ( flags & FLAG_SPACE )
		{ stack[totStacked++]= ' ';	}
	    }
	}

    /*  7  */
    if  ( ! ( flags & FLAG_LEFT ) )
	{
	if  ( ( flags & FLAG_ZEROPAD ) && ! ( flags & FLAG_HAS_PRECISION ) )
	    {
	    while( totStacked > digStacked )
		{ sioOutPutCharacter( stack[--totStacked], sos ); emitted++; }

	    while( emitted+ totStacked < minWidth )
		{ sioOutPutCharacter( '0', sos ); emitted++; }
	    }
	else{
	    while( emitted+ totStacked < minWidth )
		{ sioOutPutCharacter( ' ', sos ); emitted++; }
	    }
	}

    /*  8  */
    while( totStacked > 0 )
	{ sioOutPutCharacter( stack[--totStacked], sos ); emitted++; }

    /*  9  */
    while( emitted < minWidth )
	{ sioOutPutCharacter( ' ', sos ); emitted++; }

    return emitted;
    }

static int sioOutPrintf_xou(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				int			base,
				const char *		digits,
				const char *		altPrefixRev,
				unsigned long		value )
    {
    int		sign= 0;

    /*  A  */
    flags &= ~FLAG_SPACE;

    /*  B  */
    if  ( value > 0 )
	{ sign= +1;	}

    return sioOutPrintf_dixou( sos, flags, minWidth, precision,
				    base, digits, altPrefixRev, sign, value );
    }

static int sioOutPrintf_di(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				int			base,
				const char *		digits,
				long			value )
    {
    int			sign= 0;
    const char *	altPrefixRev= "";

    /*  B  */
    if  ( value > 0 )
	{ sign= +1;	}
    if  ( value < 0 )
	{ sign= -1; value= -value;	}

    return sioOutPrintf_dixou( sos, flags, minWidth, precision,
				    base, digits, altPrefixRev, sign, value );
    }


/************************************************************************/
/*									*/
/*  Floating point formatting. Just use sprintf(). Writing this myself	*/
/*  is not worth the hassle.						*/
/*									*/
/*  1)  Paranoia checks against crashes.				*/
/*  2)  Save format.							*/
/*  3)  Convert using sprintf()						*/
/*  4)  Emit result.							*/
/*									*/
/************************************************************************/

static int sioOutPrintf_efg(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				const char *		format,
				const char *		past,
				double			value )
    {
    int		emitted= 0;

    char	sf[STACK_SIZE+3];
    char	sv[STACK_SIZE+3];

    /*  1  */
    if  ( minWidth > STACK_SIZE )
	{ LLDEB(minWidth,STACK_SIZE); return -1;	}
    if  ( precision > STACK_SIZE )
	{ LLDEB(precision,STACK_SIZE); return -1;	}

    /*  1  */
    if  ( past- format > STACK_SIZE )
	{ LLDEB(past-format,STACK_SIZE); return -1;	}

    /*  2  */
    strncpy( sf, format, past- format )[past- format]= '\0';

    /*  3  */
    sprintf( sv, sf, value );

    /*  4  */
    while( sv[emitted] )
	{ sioOutPutCharacter( sv[emitted], sos ); emitted++; }

    return emitted;
    }

/************************************************************************/
/*									*/
/*  %c and %% conversions.						*/
/*									*/
/*  1)  Emit leading spaces.						*/
/*  2)  Emit the character.						*/
/*  3)  Emit trailing spaces.						*/
/*									*/
/************************************************************************/

static int sioOutPrintf_c(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				int			c )
    {
    int		emitted= 0;

    /*  1  */
    if  ( ! ( flags & FLAG_LEFT ) )
	{
	while( emitted+ 1 < minWidth )
	    { sioOutPutCharacter( ' ', sos ); emitted++; }
	}

    /*  2  */
    sioOutPutCharacter( c, sos );
    emitted++;

    /*  3  */
    while( emitted < minWidth )
	{ sioOutPutCharacter( ' ', sos ); emitted++; }

    return emitted;
    }

/************************************************************************/
/*									*/
/*  %s conversion.							*/
/*									*/
/*  1)  Determine the length of the input if necessary.			*/
/*  2)  Emit leading space.						*/
/*  3)  Emit the input string.						*/
/*  3)  Emit trailing space.						*/
/*									*/
/************************************************************************/

static int sioOutPrintf_s(	SimpleOutputStream *	sos,
				unsigned int		flags,
				int			minWidth,
				int			precision,
				const char *		value )
    {
    const char *	s;

    int			emitted= 0;

    if  ( ! ( flags & FLAG_HAS_PRECISION ) )
	{ precision= INT_MAX;	}

    /*  1  */
    if  ( ! ( flags & FLAG_LEFT ) )
	{
	int		leading= minWidth;

	s= value;
	while( *s && s- value < precision )
	    { s++; leading--;	}

	/*  2  */
	while( emitted < leading )
	    { sioOutPutCharacter( ' ', sos ); emitted++; }
	}

    /*  3  */
    s= value;
    while( *s && s- value < precision )
	{
	sioOutPutCharacter( *s, sos );
	s++; emitted++;
	}

    /*  4  */
    while( emitted < minWidth )
	{ sioOutPutCharacter( ' ', sos ); emitted++; }

    return emitted;
    }

/************************************************************************/
/*									*/
/*  Printf formatting on output streams.				*/
/*									*/
/*  1)  Scan the format string.						*/
/*  2)  Emit everything upto the next conversion or the end of the	*/
/*	format without any interference.				*/
/*  3)  Extract a conversion specifier.					*/
/*  4)  '*' for minWidth.						*/
/*  5)  '*' for precision.						*/
/*									*/
/************************************************************************/

int sioOutPrintf(	SimpleOutputStream *	sos,
			const char *		format,
			... )
    {
    int		emitted= 0;
    va_list	valist;

    va_start( valist, format );

    /*  1  */
    while( format[0] )
	{
	unsigned int		flags;
	char			conversion;
	int			minWidth;
	int			precision;

	int			res;
	const char *		past;

	const char *		altPrefixRev;

	/*  2  */
	while( format[0] && format[0] != '%' )
	    {
	    sioOutPutCharacter( format[0], sos );
	    format++; emitted++;
	    }

	if  ( ! format[0] )
	    { break;	}

	/*  3  */
	sioOutPrintfGetConversion( &flags, &minWidth,
						&precision, &conversion,
						&past, format+ 1 );

	/*  4  */
	if  ( flags & FLAG_WIDTH_INDIR )
	    {
	    minWidth= va_arg( valist, int );

	    if  ( minWidth < 0 )
		{ minWidth= 0; }
	    }

	/*  5  */
	if  ( flags & FLAG_PREC_INDIR )
	    {
	    precision= va_arg( valist, int );

	    if  ( precision < 0 )
		{ flags &= ~FLAG_HAS_PRECISION; precision= 0; }
	    }

	switch( conversion )
	    {
	    case 'd': case 'i':
		if  ( flags & FLAG_LONG )
		    {
		    res= sioOutPrintf_di( sos, flags, minWidth, precision,
					    10, SIO_PrintfLowerDigits,
					    va_arg( valist, long ) );
		    }
		else{
		    res= sioOutPrintf_di( sos, flags, minWidth, precision,
					    10, SIO_PrintfLowerDigits,
					    (long)( va_arg( valist, int ) ) );
		    }

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    case 'o':
		altPrefixRev= "0";

		if  ( flags & FLAG_LONG )
		    {
		    res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    8, SIO_PrintfLowerDigits, altPrefixRev,
			    va_arg( valist, unsigned long ) );
		    }
		else{
		    res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    8, SIO_PrintfLowerDigits, altPrefixRev,
			    (unsigned long)( va_arg( valist, unsigned int ) ) );
		    }

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;
					
	    case 'u':
		altPrefixRev= "";

		if  ( flags & FLAG_LONG )
		    {
		    res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    10, SIO_PrintfLowerDigits, altPrefixRev,
			    va_arg( valist, unsigned long ) );
		    }
		else{
		    res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    10, SIO_PrintfLowerDigits, altPrefixRev,
			    (unsigned long)( va_arg( valist, unsigned int ) ) );
		    }

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;
					
	    case 'x':
		{
		unsigned long	value;

		altPrefixRev= "x0";

		if  ( flags & FLAG_LONG )
		    { value= va_arg( valist, unsigned long );	}
		else{ value= (unsigned long)va_arg( valist, unsigned int ); }

		if  ( value == 0 )
		    { altPrefixRev= "";	}

		res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    16, SIO_PrintfLowerDigits, altPrefixRev, value );
		}

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;
					
	    case 'X':
		{
		unsigned long	value;

		altPrefixRev= "X0";

		if  ( flags & FLAG_LONG )
		    { value= va_arg( valist, unsigned long );	}
		else{ value= (unsigned long)va_arg( valist, unsigned int ); }

		if  ( value == 0 )
		    { altPrefixRev= "";	}

		res= sioOutPrintf_xou( sos, flags, minWidth, precision,
			    16, SIO_PrintfUpperDigits, altPrefixRev, value );
		}

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;
					
	    case 'f':
	    case 'e': case 'E':
	    case 'g': case 'G':
		res= sioOutPrintf_efg( sos, flags, minWidth, precision,
				    format, past, va_arg( valist, double ) );

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    case 'c':
		res= sioOutPrintf_c( sos, flags, minWidth, precision,
						    va_arg( valist, int ) );
		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    case 's':
		res= sioOutPrintf_s( sos, flags, minWidth, precision,
					    va_arg( valist, const char * ) );
		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    case 'P':
		flags |= FLAG_ALTERNATE;
		res= sioOutPrintf_xou( sos, flags, minWidth, precision,
				16, SIO_PrintfLowerDigits, "0x",
				(unsigned long)va_arg( valist, void * ) );

		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    case 'n':
		if  ( flags & FLAG_SHORT )
		    { *(va_arg( valist, short * ))= emitted;	}
		else{
		    if  ( flags & FLAG_LONG )
			{ *(va_arg( valist, long * ))= emitted;	}
		    else{ *(va_arg( valist, int * ))= emitted;	}
		    }

		break;

	    case '%':
		if  ( past != format+ 2 )
		    { SSDEB(format,past);	}

		res= sioOutPrintf_c( sos, flags, minWidth, precision, '%' );
		if  ( res < 0 )
		    { LDEB(res); emitted= -1; goto ready;	}

		emitted += res;
		break;

	    default:
		CDEB(conversion);
		while( format < past )
		    {
		    sioOutPutCharacter( format[0], sos );
		    format++; emitted++;
		    }
		break;
	    }

	format= past;
	}

  ready:

    va_end( valist );
    return emitted;
    }
