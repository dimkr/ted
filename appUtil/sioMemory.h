/************************************************************************/
/*									*/
/*  Simple io streams using blocks of memory.				*/
/*									*/
/************************************************************************/

#   ifndef	SIO_MEMORY_H
#   define	SIO_MEMORY_H

#   include	<sioGeneral.h>
#   include	<utilMemoryBuffer.h>

extern SimpleInputStream * sioInMemoryOpen(	const MemoryBuffer *	mb );

extern SimpleOutputStream * sioOutMemoryOpen(	MemoryBuffer *	mb );

#   endif
