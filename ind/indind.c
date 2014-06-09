#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<appDebugon.h>

#   ifdef NeXT
#	include	<libc.h>
#	include	<mach/mach.h>
#	define	IND_MAP_FD
#   endif

#   ifdef __linux__
#	include	<sys/types.h>
#	include	<sys/mman.h>
#	include	<errno.h>
#	ifndef MAP_FAILED
#	    define	MAP_FAILED	((void *)-1)
#	endif
#	define	IND_MMAP
#   endif

#   include	<unistd.h>
#   include	<fcntl.h>
#   include	<utilEndian.h>

/************************************************************************/
/*  Swap a buffer							*/
/************************************************************************/
static void	indSwapInt( unsigned char * buf )
    {
    register unsigned char	c;

    c= buf[0]; buf[0]= buf[3]; buf[3]= c;
    c= buf[1]; buf[1]= buf[2]; buf[2]= c;
    }

static void	indSwapInd( IND * ind )
    {
    indSwapInt( (unsigned char *)&(ind->ind_magic) );
    indSwapInt( (unsigned char *)&(ind->ind_fd) );
    indSwapInt( (unsigned char *)&(ind->ind_start) );
    indSwapInt( (unsigned char *)&(ind->ind_readonly) );
    indSwapInt( (unsigned char *)&(ind->ind_nnode) );
    indSwapInt( (unsigned char *)&(ind->indAllocatedNodes) );
    indSwapInt( (unsigned char *)&(ind->indAllocatedLinks) );
    indSwapInt( (unsigned char *)&(ind->ind_lfree) );
    indSwapInt( (unsigned char *)&(ind->ind_lfull) );
    indSwapInt( (unsigned char *)&(ind->ind_ccount) );
    indSwapInt( (unsigned char *)&(ind->ind_cfree) );
    indSwapInt( (unsigned char *)&(ind->ind_cfull) );

    return;
    }

/************************************************************************/
/*  Make an index.							*/
/************************************************************************/

IND * indINDmake( int	read_only )
    {
    IND *	ind= (IND *)malloc( sizeof( IND ) );

    if  ( ind )
	{
	ind->ind_magic= INDMAGIC;
	ind->ind_fd= -1;

	ind->ind_start= -1;
	ind->ind_readonly= read_only;

	ind->ind_nodes= (TrieNode *)0;
	ind->indNodePages= (TrieNode **)0;
	ind->ind_nnode= 0;
	ind->indAllocatedNodes= 0;

	ind->ind_links= (TrieLink *)0;
	ind->indLinkPages= (TrieLink **)0;
	ind->indAllocatedLinks= 0;
	ind->ind_lfree= 0;
	ind->ind_lfull= 0;

	ind->ind_classes= (int *)0;
	ind->ind_cpages= (int **)0;
	ind->ind_ccount= 0;
	ind->ind_cfree= 0;
	ind->ind_cfull= 0;

	ind->indMmappedFile= (unsigned char *)0;
	ind->indMmappedSize= 0;
	}

    return ind;
    }

void	indINDfree( IND *	ind )
    {
    int		i;
    int		page;
    char *	p;
    int		pagesAreMemoryMapped= 0;

#   ifdef IND_MAP_FD
    if  ( ind->indMmappedFile )
	{
	kern_return_t	ret;

	ret= vm_deallocate( task_self(),
		    (vm_address_t)ind->indMmappedFile, ind->indMmappedSize );
	if  ( ret != KERN_SUCCESS )
	    { LDEB(ret); }

	pagesAreMemoryMapped= 1;
	}
#   endif


#   ifdef IND_MMAP
    if  ( ind->indMmappedFile )
	{
	int	ret;

	ret= munmap( (void *)ind->indMmappedFile, ind->indMmappedSize );
	if  ( ret )
	    { LDEB(ret); }

	pagesAreMemoryMapped= 1;
	}
#   endif

    if  ( ind->indNodePages )
	{
	page= 0;
	if  ( ! pagesAreMemoryMapped )
	    {
	    for ( i= 0; i < ind->indAllocatedNodes; i += TNsBLOCK )
		{
		p= (char *)ind->indNodePages[page++];
		if  ( p )
		    { free( p );	}
		}
	    }

	free( (char *)ind->indNodePages );
	}

    if  ( ind->indLinkPages )
	{
	page= 0;
	if  ( ! pagesAreMemoryMapped )
	    {
	    for ( i= 0; i < ind->indAllocatedLinks; i += TLsBLOCK )
		{
		p= (char *)ind->indLinkPages[page++];
		if  ( p )
		    { free( p );	}
		}
	    }

	free( (char *)ind->indLinkPages );
	}

    if  ( ind->ind_cpages )
	{
	page= 0;
	if  ( ! pagesAreMemoryMapped )
	    {
	    for ( i= 0; i < ind->ind_ccount; i += ITsBLOCK )
		{
		p= (char *)ind->ind_cpages[page++];
		if  ( p )
		    { free( p );	}
		}
	    }

	free( (char *)ind->ind_cpages );
	}

    free( (char *)ind );
    }

/************************************************************************/
/*  Write an index to file						*/
/************************************************************************/
static int	indWriteData(	IND *		ind,
				int		fd,
				int		swap )
    {
    int		n;
    int		i;
    unsigned	sz;
    int		page;

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{
	sz= TNsBLOCK * sizeof( TrieNode );
	if  ( swap )
	    {
	    for ( i= 0; i < TNsBLOCK; i++ )
		{
		TrieNode *	tn= &(ind->indNodePages[page][i]);

		indSwapInt( (unsigned char *)&tn->tn_transitions );
		indSwapInt( (unsigned char *)&tn->tn_items );
		}
	    }

	if  ( write( fd, (char *)ind->indNodePages[page], sz ) != sz )
	    { return -1; }

	if  ( swap )
	    {
	    for ( i= 0; i < TNsBLOCK; i++ )
		{
		TrieNode *	tn= &(ind->indNodePages[page][i]);

		indSwapInt( (unsigned char *)&tn->tn_transitions );
		indSwapInt( (unsigned char *)&tn->tn_items );
		}
	    }
	page++;
	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{
	sz= TLsBLOCK * sizeof( TrieLink );
	if  ( swap )
	    {
	    for ( i= 0; i < TLsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->indLinkPages[page][i] ); }
	    }
	if  ( write( fd, (char *)ind->indLinkPages[page], sz ) != sz )
	    { LDEB(fd); return -1; }
	if  ( swap )
	    {
	    for ( i= 0; i < TLsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->indLinkPages[page][i] ); }
	    }
	page++;
	}

    page= 0;
    for ( n= 0; n < ind->ind_ccount; n += ITsBLOCK )
	{
	sz= ITsBLOCK * sizeof( int );
	if  ( swap )
	    {
	    for ( i= 0; i < ITsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->ind_cpages[page][i] ); }
	    }
	if  ( write( fd, (char *)ind->ind_cpages[page], sz ) != sz )
	    { LDEB(fd); return -1; }
	if  ( swap )
	    {
	    for ( i= 0; i < ITsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->ind_cpages[page][i] ); }
	    }
	page++;
	}

    return 0;
    }

int	indINDwrite(	IND *		ind,
			const char *	filename )
    {
    int		fd= creat( filename, 0666 );
    int		rval= 0;
    IND	swapped;

    if  ( fd < 0 )
	{ return -1; }

#   if 0
    appDebug( "WRITE: ROOT= %d, nnode= %d, ncount= %d lcount= %d\n",
	ind->ind_start,	ind->ind_nnode, ind->indAllocatedNodes,
	ind->indAllocatedLinks );
#   endif

    if  ( offsetof(IND,indMmappedFile) != 72 )
	{ LLDEB(offsetof(IND,indMmappedFile),72); return -1;	}
    if  ( sizeof(TrieLink) != 4 )
	{ LLDEB(sizeof(TrieLink),4); return -1;		}
    if  ( sizeof(TrieNode) != 12 )
	{ LLDEB(sizeof(TrieNode),12); return -1;	}

    if  ( write( fd, ind, offsetof(IND,indMmappedFile) ) !=
					    offsetof(IND,indMmappedFile) )
	{ LDEB(fd); rval= -1;	}
    else{
	if  ( indWriteData( ind, fd, 0 ) )
	    { LDEB(fd); rval= -1;	}
	}

    if  ( ! rval )
	{
	swapped= *ind;
	indSwapInd( &swapped );

	if  ( write( fd, &swapped, offsetof(IND,indMmappedFile) ) !=
						offsetof(IND,indMmappedFile) )
	    { LDEB(fd); rval= -1;	}
	else{
	    if  ( indWriteData( ind, fd, 1 ) )
		{ LDEB(fd); rval= -1;	}
	    }
	}

    if  ( close( fd ) )
	{ rval= -1;	}

    return rval;
    }

/************************************************************************/
/*  Read an index from file						*/
/************************************************************************/
IND *	indINDread(	const char *	filename,
			int		read_only )
    {
    int			fd;

    IND *		ind;
    TrieNode *		nodes;
    TrieLink *		links;
    int *		items;
    int			n;
    unsigned		sz;
    int			page;
    long		fileOff= 0;
    long		memOff= 0;
    unsigned long	mmapSize;
    unsigned long	mmapOffset;
    int			memory_mapped= 0;
    int			oppositeEndian= 0;

    unsigned short	one= 1;
    int			machineBigEndian= ( *(unsigned char *)&one == 0 );

    unsigned char	hd[72];

    if  ( sizeof(TrieNode) != 12 )
	{ LLDEB(sizeof(TrieNode),12); return (IND *)0;	}
    if  ( sizeof(TrieLink) != 4 )
	{ LLDEB(sizeof(TrieLink),4); return (IND *)0;	}

    fd= open( filename, 0 );
    if  ( fd < 0 )
	{ SLDEB(filename,read_only); return (IND *)0;	}

    ind= indINDmake( read_only );

    if  ( ! ind )
	{ XDEB(ind); goto failure;	}

    ind->ind_readonly= read_only;

    if  ( read( fd, hd, sizeof(hd) ) != sizeof(hd) )
	{ LDEB(sizeof(hd)); goto failure;	}

    if  ( machineBigEndian )
	{
	ind->ind_magic= utilEndianExtractBeInt32( hd );

	if  ( ind->ind_magic == INDMAGIC )
	    {
	    ind->ind_start= utilEndianExtractBeInt32( hd+ 8 );
	    ind->ind_nnode= utilEndianExtractBeInt32( hd+ 24 );
	    ind->indAllocatedNodes= utilEndianExtractBeInt32( hd+ 28 );
	    ind->indAllocatedLinks= utilEndianExtractBeInt32( hd+ 40 );
	    ind->ind_lfree= utilEndianExtractBeInt32( hd+ 44 );
	    ind->ind_lfull= utilEndianExtractBeInt32( hd+ 48 );
	    ind->ind_ccount= utilEndianExtractBeInt32( hd+ 60 );
	    ind->ind_cfree= utilEndianExtractBeInt32( hd+ 64 );
	    ind->ind_cfull= utilEndianExtractBeInt32( hd+ 68 );

	    fileOff += sizeof(hd);
	    }
	else{
	    if  ( ind->ind_magic == INDMAGIC_R )
		{
		fileOff= sizeof(hd);
		fileOff += utilEndianExtractLeInt32( hd+ 28 )* sizeof(TrieNode);
		fileOff += utilEndianExtractLeInt32( hd+ 40 )* sizeof(TrieLink);
		fileOff += utilEndianExtractLeInt32( hd+ 60 )* sizeof(int);

		if  ( lseek( fd, fileOff, SEEK_SET ) != fileOff	||
		      read( fd, hd, sizeof(hd) ) != sizeof(hd)	)
		    { LDEB(fileOff); goto failure;	}

		ind->ind_magic= utilEndianExtractBeInt32( hd );
		if  ( ind->ind_magic != INDMAGIC )
		    { XXDEB(ind->ind_magic,INDMAGIC); goto failure;	}

		ind->ind_start= utilEndianExtractBeInt32( hd+ 8 );
		ind->ind_nnode= utilEndianExtractBeInt32( hd+ 24 );
		ind->indAllocatedNodes= utilEndianExtractBeInt32( hd+ 28 );
		ind->indAllocatedLinks= utilEndianExtractBeInt32( hd+ 40 );
		ind->ind_lfree= utilEndianExtractBeInt32( hd+ 44 );
		ind->ind_lfull= utilEndianExtractBeInt32( hd+ 48 );
		ind->ind_ccount= utilEndianExtractBeInt32( hd+ 60 );
		ind->ind_cfree= utilEndianExtractBeInt32( hd+ 64 );
		ind->ind_cfull= utilEndianExtractBeInt32( hd+ 68 );

		fileOff += sizeof(hd);

		oppositeEndian= 1;
		}
	    else{ LDEB(ind->ind_magic); goto failure;	}
	    }
	}
    else{
	ind->ind_magic= utilEndianExtractLeInt32( hd );

	if  ( ind->ind_magic == INDMAGIC )
	    {
	    ind->ind_start= utilEndianExtractLeInt32( hd+ 8 );
	    ind->ind_nnode= utilEndianExtractLeInt32( hd+ 24 );
	    ind->indAllocatedNodes= utilEndianExtractLeInt32( hd+ 28 );
	    ind->indAllocatedLinks= utilEndianExtractLeInt32( hd+ 40 );
	    ind->ind_lfree= utilEndianExtractLeInt32( hd+ 44 );
	    ind->ind_lfull= utilEndianExtractLeInt32( hd+ 48 );
	    ind->ind_ccount= utilEndianExtractLeInt32( hd+ 60 );
	    ind->ind_cfree= utilEndianExtractLeInt32( hd+ 64 );
	    ind->ind_cfull= utilEndianExtractLeInt32( hd+ 68 );

	    fileOff += sizeof(hd);
	    }
	else{
	    if  ( ind->ind_magic == INDMAGIC_R )
		{
		fileOff= sizeof(hd);
		fileOff += utilEndianExtractBeInt32( hd+ 28 )* sizeof(TrieNode);
		fileOff += utilEndianExtractBeInt32( hd+ 40 )* sizeof(TrieLink);
		fileOff += utilEndianExtractBeInt32( hd+ 60 )* sizeof(int);

		if  ( lseek( fd, fileOff, SEEK_SET ) != fileOff	||
		      read( fd, hd, sizeof(hd) ) != sizeof(hd)	)
		    { LDEB(fileOff); goto failure;	}

		ind->ind_magic= utilEndianExtractLeInt32( hd );
		if  ( ind->ind_magic != INDMAGIC )
		    { XXDEB(ind->ind_magic,INDMAGIC); goto failure;	}

		ind->ind_start= utilEndianExtractLeInt32( hd+ 8 );
		ind->ind_nnode= utilEndianExtractLeInt32( hd+ 24 );
		ind->indAllocatedNodes= utilEndianExtractLeInt32( hd+ 28 );
		ind->indAllocatedLinks= utilEndianExtractLeInt32( hd+ 40 );
		ind->ind_lfree= utilEndianExtractLeInt32( hd+ 44 );
		ind->ind_lfull= utilEndianExtractLeInt32( hd+ 48 );
		ind->ind_ccount= utilEndianExtractLeInt32( hd+ 60 );
		ind->ind_cfree= utilEndianExtractLeInt32( hd+ 64 );
		ind->ind_cfull= utilEndianExtractLeInt32( hd+ 68 );

		fileOff += sizeof(hd);

		oppositeEndian= 1;
		}
	    else{ LDEB(ind->ind_magic); goto failure;	}
	    }
	}

#   if 0
    appDebug( "READ:  ROOT= %d, nnode= %d, ncount= %d lcount= %d\n",
	ind->ind_start,	ind->ind_nnode, ind->indAllocatedNodes,
	ind->indAllocatedLinks );
#   endif

    if  ( oppositeEndian )
	{ memOff= fileOff % 512;	}
    else{ memOff= fileOff; 		}

    mmapSize=	memOff+
		ind->indAllocatedNodes* sizeof(TrieNode)+
		ind->indAllocatedLinks* sizeof(TrieLink)+
		ind->ind_ccount* sizeof( int );
    mmapOffset= fileOff- memOff;

#   ifdef IND_MAP_FD
    if  ( read_only )
	{
	kern_return_t	ret;

	ret= map_fd( fd, (vm_offset_t)mmapOffset,
		    (vm_address_t *)&(ind->indMmappedFile), 1, mmapSize );
	if  ( ret != KERN_SUCCESS )
	    { LDEB(ret);	}
	else{
	    memory_mapped= 1;
	    ind->indMmappedSize= mmapSize;
	    }

	}
#   endif

#   ifdef IND_MMAP
    if  ( read_only )
	{
	void *	mmapResult;

	mmapResult= mmap( (void *)0, mmapSize,
			    PROT_READ, MAP_SHARED, fd, mmapOffset );

	if  ( mmapResult == MAP_FAILED )
	    { LSDEB(errno,strerror(errno));	}
	else{
	    ind->indMmappedFile= mmapResult;
	    ind->indMmappedSize= mmapSize;
	    memory_mapped= 1;
	    }
	}
#   endif

    if  ( memory_mapped )
	{
	ind->ind_nodes= (TrieNode *)( ind->indMmappedFile+ memOff );
	memOff += ind->indAllocatedNodes* sizeof(TrieNode);

	ind->ind_links= (TrieLink *)( ind->indMmappedFile+ memOff );
	memOff += ind->indAllocatedLinks* sizeof(TrieLink);

	ind->ind_classes= (int *)( ind->indMmappedFile+ memOff );
	memOff += ind->ind_ccount* sizeof(int);
	}

    sz= (ind->indAllocatedNodes/TNsBLOCK)* sizeof( TrieNode * );
    ind->indNodePages= (TrieNode **)malloc( sz );

    if  ( ! ind->indNodePages )
	{ goto failure;	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{ ind->indNodePages[page++]= (TrieNode *)0; }

    sz= (ind->indAllocatedLinks/TLsBLOCK)* sizeof( TrieLink * );
    ind->indLinkPages= (TrieLink **)malloc( sz );

    if  ( ! ind->indLinkPages )
	{ goto failure;	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{ ind->indLinkPages[page++]= (TrieLink *)0; }

    if  ( ind->ind_ccount > 0 )
	{
	sz= (ind->ind_ccount/ITsBLOCK)* sizeof( int * );
	ind->ind_cpages= (int **)malloc( sz );

	if  ( ! ind->ind_cpages )
	    { goto failure;	}

	page= 0;
	for ( n= 0; n < ind->ind_ccount; n += ITsBLOCK )
	    { ind->ind_cpages[page++]= (int *)0; }
	}
    else{ ind->ind_cpages= (int **)0;	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{
	if  ( ! memory_mapped )
	    {
	    sz= TNsBLOCK * sizeof( TrieNode );
	    ind->indNodePages[page++]= nodes= (TrieNode *)malloc( sz );
	    if  ( ! nodes )
		{ goto failure;	}

	    if  ( read( fd, (char *)nodes, sz ) != sz )
		{ goto failure;	}

	    fileOff += sz;
	    }
	else{ ind->indNodePages[page++]= ind->ind_nodes+ n;	}
	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{
	if  ( ! memory_mapped )
	    {
	    sz= TLsBLOCK * sizeof( TrieLink );
	    ind->indLinkPages[page++]= links= (TrieLink *)malloc( sz );
	    if  ( ! links )
		{ goto failure;	}

	    if  ( read( fd, (char *)links, sz ) != sz )
		{ goto failure;	}

	    fileOff += sz;
	    }
	else{ ind->indLinkPages[page++]= ind->ind_links+ n;	}
	}

    page= 0;
    for ( n= 0; n < ind->ind_ccount; n += ITsBLOCK )
	{
	if  ( ! memory_mapped )
	    {
	    sz= ITsBLOCK * sizeof( int );
	    ind->ind_cpages[page++]= items= (int *)malloc( sz );
	    if  ( ! items )
		{ goto failure;	}

	    if  ( read( fd, (char *)items, sz ) != sz )
		{ goto failure;	}

	    fileOff += sz;
	    }
	else{ ind->ind_cpages[page]= ind->ind_classes+ n; page++; }
	}

    if  ( close( fd ) )
	{ indINDfree( ind ); return (IND *)0; }

    return ind;

failure:
    close( fd );
    if  ( ind )
	{ indINDfree( ind );	}
    return (IND *)0;
    }
