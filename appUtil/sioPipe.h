/************************************************************************/
/*									*/
/*  sioPipe.[ch]:	sio equivalents to popen/pclose.		*/
/*									*/
/************************************************************************/

#   ifndef	SIO_PIPE_H
#   define	SIO_PIPE_H

#   include     <appSystem.h>
#   include     <sioGeneral.h>

extern SimpleOutputStream * sioOutPipeOpen(	const char *	command );

#   endif
