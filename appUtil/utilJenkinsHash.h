/*

Slightly adapted from Source by Bob Jenkins. The original comments 
of the file have been retained. Some unused code has been removed, 
the code was converted to ANSI C and the type system is adapted to 
that of my software. MdD Jan 2003.

--------------------------------------------------------------------
lookupa.c, by Bob Jenkins, December 1996.  Same as lookup2.c
You may use this code in any way you wish.  It has no warranty.
Source is http://burtleburtle.net/bob/c/lookupa.c
--------------------------------------------------------------------

*/

#   ifndef UTIL_JENKINS_HASH_H
#   define UTIL_JENKINS_HASH_H

extern unsigned long utilJenkinsHash(	const unsigned char *	k,
					int			l,
					unsigned long		lev );

extern void  utilJenkinsHash2(		const unsigned char *	k,
					int			l,
					unsigned long *		state );

#   endif
