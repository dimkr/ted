/*
------------------------------------------------------------------------------
perfect.c: code to generate code for a hash for perfect hashing.
(c) Bob Jenkins, September 1996, December 1999
You may use this code in any way you wish, and it is free.  No warranty.
I hereby place this in the public domain.
Source is http://burtleburtle.net/bob/c/perfect.c

This generates a minimal perfect hash function.  That means, given a
set of n keys, this determines a hash function that maps each of
those keys into a value in 0..n-1 with no collisions.

The perfect hash function first uses a normal hash function on the key
to determine (a,b) such that the pair (a,b) is distinct for all
keys, then it computes a^scramble[tab[b]] to get the final perfect hash.
tab[] is an array of 1-byte values and scramble[] is a 256-term array of 
2-byte or 4-byte values.  If there are n keys, the length of tab[] is a 
power of two between n/3 and n.

I found the idea of computing distinct (a,b) values in "Practical minimal 
perfect hash functions for large databases", Fox, Heath, Chen, and Daoud, 
Communications of the ACM, January 1992.  They found the idea in Chichelli 
(CACM Jan 1980).  Beyond that, our methods differ.

The key is hashed to a pair (a,b) where a in 0..*alen*-1 and b in
0..*blen*-1.  A fast hash function determines both a and b
simultaneously.  Any decent hash function is likely to produce
hashes so that (a,b) is distinct for all pairs.  I try the hash
using different values of *salt* until all pairs are distinct.

The final hash is (a XOR scramble[tab[b]]).  *scramble* is a
predetermined mapping of 0..255 into 0..smax-1.  *tab* is an
array that we fill in in such a way as to make the hash perfect.

First we fill in all values of *tab* that are used by more than one
key.  We try all possible values for each position until one works.

This leaves m unmapped keys and m values that something could hash to.
If you treat unmapped keys as lefthand nodes and unused hash values
as righthand nodes, and draw a line connecting each key to each hash
value it could map to, you get a bipartite graph.  We attempt to
find a perfect matching in this graph.  If we succeed, we have
determined a perfect hash for the whole set of keys.

*scramble* is used because (a^tab[i]) clusters keys around *a*.
------------------------------------------------------------------------------
*/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"appUtilConfig.h"
#   include	"utilJenkinsHash.h"
#   include	"utilJenkinsPerfectHash.h"

#   include	<appDebugon.h>

#   define	TRUE		1
#   define	FALSE		0
#   define	UB4BITS		32
#   define	CHECKSTATE	8
#   define	UB1MAXVAL	0xff
#   define	UB2MAXVAL	0xffff

/*
------------------------------------------------------------------------------
Find the mapping that will produce a perfect hash
------------------------------------------------------------------------------
*/

/* return the ceiling of the log (base 2) of val */
static ub4 ilog2( ub4 val )
{
  ub4 i;
  for (i=0; ((ub4)1<<i) < val; ++i)
    ;
  return i;
}

/*

compute p(x), where p is a permutation of 0..(1<<nbits)-1
permute(0)=0.  This is intended and useful.

ub4 x;           input, a value in some range
ub4 nbits;       input, number of bits in range

*/

static ub4  permute(	ub4 x,
			ub4 nbits )
{
  int i;
  int mask   = ((ub4)1<<nbits)-1;                                /* all ones */
  int const2 = 1+nbits/2;
  int const3 = 1+nbits/3;
  int const4 = 1+nbits/4;
  int const5 = 1+nbits/5;
  for (i=0; i<20; ++i)
  {
    x = (x+(x<<const2)) & mask; 
    x = (x^(x>>const3));
    x = (x+(x<<const4)) & mask;
    x = (x^(x>>const5));
  }
  return x;
}

/*

initialize scramble[] with distinct random values in 0..smax-1

ub4      *scramble;          hash is a^scramble[tab[b]]
ub4       smax;              scramble values should be in 0..smax-1

*/

static void scrambleinit(	ub4      *scramble,
				ub4       smax )
{
  ub4 i;

  /* fill scramble[] with distinct random integers in 0..smax-1 */
  for (i=0; i<SCRAMBLE_LEN; ++i)
  {
    scramble[i] = permute(i, ilog2(smax));
  }
}

/* 
 * Check if key1 and key2 are the same. 
 * We already checked (a,b) are the same.
 */
static int checkdup(	key      *key1,
			key      *key2,
			hashform *form )
{
  switch(form->hashtype)
  {
  case STRING_HT:
    if ((key1->len_k == key2->len_k) &&
	!memcmp(key1->name_k, key2->name_k, (size_t)key1->len_k))
    {
      appDebug("fatal error: Duplicates keys!  %.*s\n",
				 (int)key1->len_k, key1->name_k);
      return -1;
    }
    break;
  default:
    appDebug("fatal error: Illegal hash type %lu\n",
					    (unsigned long)form->hashtype);
    return -1;
  }

  return 0;
}


/* 
 * Put keys in tabb according to key->b_k
 * Also check if the initial hash might work 
 *
 * bstuff   *tabb;		output, list of keys with b for (a,b)
 * ub4       blen;		length of tabb
 * key      *keys;		list of keys already hashed
 * hashform *form;		user directives
 * int       ignoreCollisions;	TRUE means to complete init despite collisions
 *
 *  1)  Initialize the b table.
 *  2)  Scan all keys.
 *  3)  Scan all keys with the same b value that we have found until now.
 *  4)  If the a value also is the same, we have a collision.
 *  5)  In a paranoya mood.. Check whether this was not a duplicate key.
 *  6)  Give up on collisions. [unless ignored]
 *  7)  Prepend the current key to the list of keys with this b value.
 *  8)  Report whether no collisions were found.
 *
 */

static int inittab(	bstuff   *tabb,
			ub4       blen,
			key      *keys,
			int	 keyCount,
			hashform *form,
			int       ignoreCollisions )
    {
    int		nocollision= TRUE;

    key *	mykey;
    int		ki;

    /*  1  */
    memset( (void *)tabb, 0, (size_t)(sizeof(bstuff)*blen) );

    /*  2  */
    mykey= keys;
    for ( ki= 0; ki < keyCount; mykey++, ki++ )
	{
	key *	otherkey;

	/*  3  */
	for ( otherkey=tabb[mykey->b_k].list_b; 
	      otherkey; 
	      otherkey= otherkey->nextb_k )
	    {
	    /*  4  */
	    if  ( mykey->a_k == otherkey->a_k )
		{
		nocollision= FALSE;

		/*  5  */
		if  ( checkdup( mykey, otherkey, form ) < 0 )
		    { return -1;	}

		/*  6  */
		if  ( ! ignoreCollisions )
		    { return FALSE;	}
		}
	    }

	/*  7  */
	mykey->nextb_k= tabb[mykey->b_k].list_b;
	tabb[mykey->b_k].list_b= mykey;
	tabb[mykey->b_k].listlen_b++;
	}

    /*  8 */
    return nocollision;
    }


/*

Do the initial hash for normal mode (use utilJenkinsHash and utilJenkinsHash2)
I.E. Calculate (a,b) values for every key.

keys, keyCount:		The array of keys.
alen:			(a,b) has a in 0..alen-1, a power of 2
blen:			(a,b) has b in 0..blen-1, a power of 2
smax:			maximum range of computable hash values
salt:			used to initialize the hash function
final:			output, code for the final hash

1)  We have an enourmous number of keys: Use the expensive hash function.
2)  Use the arbitrary 'golden ratio' times the salt as an initial value
    for the hash.
3)  Calculate (a,b) values for every key in this round.

*/

static void initnorm(	key      *keys,
			int      keyCount,
			ub4       alen,
			ub4       blen,
			ub4       smax,
			ub4       salt,
			gencode  *final )
{
    key *	mykey;
    int		ki;

    ub4		logalen= ilog2( alen );

    /*  1  */
    if  ( logalen+ ilog2( blen ) > UB4BITS )
	{
	final->genUseHash2= 1;
	/*  2  */
	final->genInitlev= salt* 0x9e3779b9;

	/*  3  */
	mykey= keys;
	for ( ki= 0; ki < keyCount; mykey++, ki++ )
	    {
	    unsigned long	state[CHECKSTATE];
	    ub4			i;

	    for ( i=0; i<CHECKSTATE; ++i )
		{ state[i] = final->genInitlev;	}

	    utilJenkinsHash2( mykey->name_k, mykey->len_k, state );
	    mykey->a_k = state[0]&(alen-1);
	    mykey->b_k = state[1]&(blen-1);
	    }
	}
    else{
	final->genUseHash2= 0;
	/*  2  */
	final->genInitlev= salt*0x9e3779b9;

	/*  3  */
	mykey= keys;
	for ( ki= 0; ki < keyCount; mykey++, ki++ )
	    {
	    ub4 hash = utilJenkinsHash(
			    mykey->name_k, mykey->len_k, final->genInitlev );

	    mykey->a_k = (logalen > 0) ? hash>>(UB4BITS-logalen) : 0;
	    mykey->b_k = (blen > 1) ? hash & (blen-1) : 0;
	    }
	}
}

/* 
 * Run a hash function on the key to get a and b 
 * Returns:
 *   0: didn't find distinct (a,b) for all keys
 *   1: found distinct (a,b) for all keys, put keys in tabb[]
 *   2: found a perfect hash, no need to do any more work

key      *keys;         list of all keys
ub4       nkeys;        total number of keys
bstuff   *tabb;         stuff indexed by b
ub4       alen;         (a,b) has a in 0..alen-1, a power of 2
ub4       blen;         (a,b) has b in 0..blen-1, a power of 2
ub4       smax;         range of computable hash values
ub4       salt;         used to initialize the hash function
hashform *form;         user directives
gencode  *final;        code for final hash

 */

static ub4 initkey(	key      *keys,
			ub4       nkeys,
			bstuff   *tabb,
			ub4       alen,
			ub4       blen,
			ub4       smax,
			ub4       salt,
			hashform *form,
			gencode  *final )
{
  /* Do the initial hash of the keys */
  switch(form->mode)
  {
  case NORMAL_HM:
    initnorm(keys, nkeys, alen, blen, smax, salt, final);
    break;
  default:
    appDebug("fatal error: illegal mode\n"); 
    exit(1);
  }

  if (nkeys <= 1)
  {
    return 2;
  }

  return inittab(tabb, blen, keys, nkeys, form, FALSE);
}

/*

Print an error message and exit if there are duplicates

bstuff   *tabb;         array of lists of keys with the same b
ub4       blen;         length of tabb, a power of 2
key      *keys;
hashform *form;         user directives

*/

static int duplicates(		bstuff   *tabb,
				ub4       blen,
				key      *keys,
				int       keyCount,
				hashform *form )
{
  ub4  i;
  key *key1;
  key *key2;

  (void)inittab(tabb, blen, keys, keyCount, form, TRUE);

  /* for each b, do nested loops through key list looking for duplicates */
  for (i=0; i<blen; ++i)
    for (key1=tabb[i].list_b; key1; key1=key1->nextb_k)
      for (key2=key1->nextb_k; key2; key2=key2->nextb_k)
	if  ( checkdup(key1, key2, form) )
	    { return -1;	}

  return 0;
}


/*
Try to apply an augmenting list

int     rollback;        FALSE applies augmenting path, TRUE rolls back

*/

static int apply(	bstuff *tabb,
			hstuff *tabh,
			qstuff *tabq,
			ub4     blen,
			ub4    *scramble,
			ub4     tail,
			int     rollback )
{
  ub4     hash;
  key    *mykey;
  bstuff *pb;
  ub4     child;
  ub4     parent;
  ub4     stabb;                                         /* scramble[tab[b]] */

  /* walk from child to parent */
  for (child=tail-1; child; child=parent)
  {
    parent = tabq[child].parent_q;                    /* find child's parent */
    pb     = tabq[parent].b_q;             /* find parent's list of siblings */

    /* erase old hash values */
    stabb = scramble[pb->val_b];
    for (mykey=pb->list_b; mykey; mykey=mykey->nextb_k)
    {
      hash = mykey->a_k^stabb;
      if (mykey == tabh[hash].key_h)
      {                            /* erase hash for all of child's siblings */
	tabh[hash].key_h = (key *)0;
      }
    }

    /* change pb->val_b, which will change the hashes of all parent siblings */
    pb->val_b = (rollback ? tabq[child].oldval_q : tabq[child].newval_q);

    /* set new hash values */
    stabb = scramble[pb->val_b];
    for (mykey=pb->list_b; mykey; mykey=mykey->nextb_k)
    {
      hash = mykey->a_k^stabb;
      if (rollback)
      {
	if (parent == 0) continue;                  /* root never had a hash */
      }
      else if (tabh[hash].key_h)
      {
	/* very rare: roll back any changes */
	(void)apply(tabb, tabh, tabq, blen, scramble, tail, TRUE);
	return FALSE;                                  /* failure, collision */
      }
      tabh[hash].key_h = mykey;
    }
  }
  return TRUE;
}


/*

-------------------------------------------------------------------------------
augment(): Add item to the mapping.

Construct a spanning tree of *b*s with *item* as root, where each
parent can have all its hashes changed (by some new val_b) with 
at most one collision, and each child is the b of that collision.

I got this from Tarjan's "Data Structures and Network Algorithms".  The
path from *item* to a *b* that can be remapped with no collision is 
an "augmenting path".  Change values of tab[b] along the path so that 
the unmapped key gets mapped and the unused hash value gets used.

Assuming 1 key per b, if m out of n hash values are still unused, 
you should expect the transitive closure to cover n/m nodes before 
an unused node is found.  Sum(i=1..n)(n/i) is about nlogn, so expect
this approach to take about nlogn time to map all single-key b's.
-------------------------------------------------------------------------------

bstuff   *tabb;      stuff indexed by b
hstuff   *tabh;      which key is associated with which hash, indexed by hash
qstuff   *tabq;      queue of *b* values, this is the spanning tree
ub4       blen;      length of tabb
ub4      *scramble;  final hash is a^scramble[tab[b]]
ub4       smax;      highest value in scramble
bstuff   *item;      &tabb[b] for the b to be mapped
ub4       nkeys;     final hash must be in 0..nkeys-1
ub4       highwater; a value higher than any now in tabb[].water_b
hashform *form;      TRUE if we should do a minimal perfect hash

*/

static int augment(	bstuff   *tabb,
			hstuff   *tabh,
			qstuff   *tabq,
			ub4       blen,
			ub4      *scramble,
			ub4       smax,
			bstuff   *item,
			ub4       nkeys,
			ub4       highwater,
			hashform *form )
{
  ub4  q;                      /* current position walking through the queue */
  ub4  tail;              /* tail of the queue.  0 is the head of the queue. */
  ub4  limit=((blen < USE_SCRAMBLE) ? smax : UB1MAXVAL+1);
  ub4  highhash = ((form->perfect == MINIMAL_HP) ? nkeys : smax);
  int  trans = (form->speed == SLOW_HS || form->perfect == MINIMAL_HP);

  /* initialize the root of the spanning tree */
  tabq[0].b_q = item;
  tail = 1;

  /* construct the spanning tree by walking the queue, add children to tail */
  for (q=0; q<tail; ++q)
  {
    bstuff *myb = tabq[q].b_q;                        /* the b for this node */
    ub4     i;                              /* possible value for myb->val_b */

    if (!trans && (q == 1)) 
      break;                                  /* don't do transitive closure */

    for (i=0; i<limit; ++i)
    {
      bstuff *childb = (bstuff *)0;             /* the b that this i maps to */
      key    *mykey;                       /* for walking through myb's keys */

      for (mykey = myb->list_b; mykey; mykey=mykey->nextb_k)
      {
	key    *childkey;
	ub4 hash = mykey->a_k^scramble[i];

	if (hash >= highhash) break;                        /* out of bounds */
	childkey = tabh[hash].key_h;

	if (childkey)
	{
	  bstuff *hitb = &tabb[childkey->b_k];

	  if (childb)
	  {
	    if (childb != hitb) break;            /* hit at most one child b */
	  }
	  else
	  {
	    childb = hitb;                        /* remember this as childb */
	    if (childb->water_b == highwater) break;     /* already explored */
	  }
	}
      }
      if (mykey) continue;             /* myb with i has multiple collisions */

      /* add childb to the queue of reachable things */
      if (childb) childb->water_b = highwater;
      tabq[tail].b_q      = childb;
      tabq[tail].newval_q = i;     /* how to make parent (myb) use this hash */
      tabq[tail].oldval_q = myb->val_b;            /* need this for rollback */
      tabq[tail].parent_q = q;
      ++tail;

      if (!childb)
      {                                  /* found an *i* with no collisions? */
	/* try to apply the augmenting path */
	if (apply(tabb, tabh, tabq, blen, scramble, tail, FALSE))
	  return TRUE;        /* success, item was added to the perfect hash */

	--tail;                    /* don't know how to handle such a child! */
      }
    }
  }
  return FALSE;
}


/*

find a mapping that makes this a perfect hash

1)  Initialize: Clean memory from previous attemts.
2)  Find the highest number of collisions in b value.
3)  Try to map all b's. Do so in the descending order of keys per b value.
    This seems to be a silly approach. As maxkeys usually is much smaller 
    than nkeys, the choice to loop this way is justified.
4)  Found!

*/

static int perfect(	bstuff *	tabb,
			hstuff *	tabh,
			int		hCount,
			qstuff *	tabq,
			ub4		blen,
			ub4		smax,
			ub4 *		scramble,
			ub4		nkeys,
			hashform *	form )
{
    ub4 maxkeys;                        /* maximum number of keys for any b */
    ub4 i, j;

    /*  1  */
    memset((void *)tabh, 0, (size_t)( hCount* sizeof(hstuff) ) );
    memset((void *)tabq, 0, (size_t)(sizeof(qstuff)*(blen+1)));

    /*  2  */
    for ( maxkeys=0,i=0; i<blen; ++i ) 
	{
	if  ( maxkeys < tabb[i].listlen_b )
	    { maxkeys = tabb[i].listlen_b;	}
	}

    /*  3  */
    for ( j= maxkeys; j > 0; --j )
	{
	for ( i=0; i < blen; ++i )
	    {
	    if  ( tabb[i].listlen_b != j )
		{ continue;	}

	    if  ( ! augment( tabb, tabh, tabq,
			blen, scramble, smax, &tabb[i], nkeys, i+1, form ) )
		{
		appDebug( "fail to map group of size %ld for tab size %ld\n", 
							(long)j, (long)blen);
		return FALSE;
		}
	    }
	}

    /* Success!  We found a perfect hash of all keys into 0..nkeys-1. */
    return TRUE;
}


/*
guess initial values for alen and blen

ub4      *alen;    output, initial alen
ub4      *blen;    output, initial blen
ub4      *smax;    input, power of two greater or equal to max hash value
ub4       nkeys;   number of keys being hashed
hashform *form;    user directives

*/

static void initalen(	ub4      *alen,
			ub4      *blen,
			ub4      *smax,
			ub4       nkeys,
			hashform *form )
{
  /*
   * Find initial *alen, *blen
   * Initial alen and blen values were found empirically.  Some factors:
   *
   * If smax<256 there is no scramble, so tab[b] needs to cover 0..smax-1.
   *
   * alen and blen must be powers of 2 because the values in 0..alen-1 and
   * 0..blen-1 are produced by applying a bitmask to the initial hash function.
   *
   * alen must be less than smax, in fact less than nkeys, because otherwise
   * there would often be no i such that a^scramble[i] is in 0..nkeys-1 for
   * all the *a*s associated with a given *b*, so there would be no legal
   * value to assign to tab[b].  This only matters when we're doing a minimal
   * perfect hash.
   *
   * It takes around 800 trials to find distinct (a,b) with nkey=smax*(5/8)
   * and alen*blen = smax*smax/32.
   *
   * Values of blen less than smax/4 never work, and smax/2 always works.
   *
   * We want blen as small as possible because it is the number of bytes in
   * the huge array we must create for the perfect hash.
   *
   * When nkey <= smax*(5/8), blen=smax/4 works much more often with 
   * alen=smax/8 than with alen=smax/4.  Above smax*(5/8), blen=smax/4
   * doesn't seem to care whether alen=smax/8 or alen=smax/4.  I think it
   * has something to do with 5/8 = 1/8 * 5.  For example examine 80000, 
   * 85000, and 90000 keys with different values of alen.  This only matters
   * if we're doing a minimal perfect hash.
   *
   * When alen*blen <= 1<<UB4BITS, the initial hash must produce one integer.
   * Bigger than that it must produce two integers, which increases the
   * cost of the hash per character hashed.
   */
  if (form->perfect == NORMAL_HP)
  {
    if ((form->speed == FAST_HS) && (nkeys > *smax*0.8))
      *smax = *smax * 2;

    *alen = (0 && *smax>131072) ? 
      ((ub4)1<<(UB4BITS-ilog2(*blen))) :   /* distinct keys => distinct (A,B) */
      *smax;                         /* no reason to restrict alen to smax/2 */
    if (0 && *smax < 32)
      *blen = *smax;                      /* go for function speed not space */
    else if (*smax/4 <= (1<<14))
      *blen = ((nkeys <= *smax*0.56) ? *smax/32 :
	       (nkeys <= *smax*0.74) ? *smax/16 : *smax/8);
    else
      *blen = ((nkeys <= *smax*0.6) ? *smax/16 : 
	       (nkeys <= *smax*0.8) ? *smax/8 : *smax/4);

    if ((form->speed == FAST_HS) && (*blen < *smax/8))
      *blen = *smax/8;

    if (*alen < 1) *alen = 1;
    if (*blen < 1) *blen = 1;
  }
  else
  {
    switch(ilog2(*smax))
    {
    case 0:
      *alen = 1;
      *blen = 1;
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
      *alen = (form->perfect == NORMAL_HP) ? *smax : *smax/2;
      *blen = *smax/2;
      break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
      if (form->speed == FAST_HS)
      {
	*alen = *smax/2;
	*blen = *smax/4;
      }
      if (*smax/4 < USE_SCRAMBLE)
      {
	*alen = ((nkeys <= *smax*0.52) ? *smax/8 : *smax/4);
	*blen = ((nkeys <= *smax*0.52) ? *smax/8 : *smax/4);
      }
      else
      {
	*alen = ((nkeys <= *smax*(5.0/8.0)) ? *smax/8 : 
		 (nkeys <= *smax*(3.0/4.0)) ? *smax/4 : *smax/2);
	*blen = *smax/4;                /* always give the small size a shot */
      }
      break;
    case 18:
      if (form->speed == FAST_HS)
      {
	*alen = *smax/2;
	*blen = *smax/2;
      }
      else
      {
	*alen = *smax/8;                 /* never require the multiword hash */
	*blen = (nkeys <= *smax*(5.0/8.0)) ? *smax/4 : *smax/2;
      }
      break;
    case 19:
    case 20:
      *alen = (nkeys <= *smax*(5.0/8.0)) ? *smax/8 : *smax/2;
      *blen = (nkeys <= *smax*(5.0/8.0)) ? *smax/4 : *smax/2;
      break;
    default:
      *alen = *smax/2;              /* just find a hash as quick as possible */
      *blen = *smax/2;     /* we'll be thrashing virtual memory at this size */
      break;
    }
  }
}

/* 

** Try to find a perfect hash function.  
** Return the successful initializer for the initial hash. 
** Return 0 if no perfect hash could be found.

bstuff  **tabb;           output, tab[] of the perfect hash, length *blen
ub4      *alen;           output, 0..alen-1 is range for a of (a,b)
ub4      *blen;           output, 0..blen-1 is range for b of (a,b)
ub4      *salt;           output, initializes initial hash
gencode  *final;          code for final hash
ub4      *scramble;       input, hash = a^scramble[tab[b]]
ub4      *smax;           input, scramble[i] in 0..smax-1
key      *keys;           input, keys to hash
ub4       nkeys;          input, number of keys being hashed
hashform *form;           user directives

1)  guess initial values for smax, alen and blen.
    smax: The first 2 power >= ........
    alen: Number of possible a values.
    blen: Number of possible b values.
2)  .....
3)  .....
4)  Number of possible hash values.
5)  Allocate working memory.
6)  Try to find a perfect hash: Retry until found.
7)  Try to find distinct (A,B) for all keys
8)  initkey() found a perfect hash by accident.
9)  Could not find distint (a,b) for all keys..
10) Retry RETRY_INITKEY times.
11) If we are over the maximum number of retries, try to put more bits 
    in (A,B) to make distinct (A,B) more likely.
12) More bits available for A?
13) More bits available for B?
14) No more bits for A or B available: Everything is hopeless.
    Before we give up: check wheter this is caused by duplicate keys.
15) Now we have found distinct (A,B) pairs for all keys. Try to derive 
    a perfect hash from it. If found, we are done.
16) Retry RETRY_PERFECT times.
17) If we are over the maximum number of retries, try to put more bits 
    in B to make a working table more likely. There is no reason yet to 
    reject this salt value, so frustrate the increment of the loop.

*/

static int findhash(	bstuff  **tabb,
			hstuff  **pTabh,
			int *	pHCount,
			ub4      *alen,
			ub4      *blen,
			gencode  *final,
			ub4      *scramble,
			ub4      *smax,
			key      *keys,
			ub4       nkeys,
			hashform *form )
{
    ub4 bad_initkey;                  /* how many times did initkey fail? */
    ub4 bad_perfect;                  /* how many times did perfect fail? */
    ub4 trysalt;                      /* trial initializer for initial hash */
    ub4 maxalen;
    hstuff *tabh;                     /* table of keys indexed by hash value */
    qstuff *tabq;  /* table of stuff indexed by queue value, used by augment */

    /*  1  */
    *smax = ((ub4)1<<ilog2(nkeys));
    initalen(alen, blen, smax, nkeys, form);

    /*  2  */
    scrambleinit(scramble, *smax);

    /*  3  */
    maxalen = (form->perfect == MINIMAL_HP) ? *smax/2 : *smax;

    /*  4  */
    *pHCount= (form->perfect == MINIMAL_HP ? nkeys : *smax );

    /*  5  */
    *tabb = (bstuff *)malloc((size_t)(sizeof(bstuff)*(*blen)));
    tabq  = (qstuff *)malloc(sizeof(qstuff)*(*blen+1));
    tabh  = (hstuff *)malloc(sizeof(hstuff)* *pHCount );

    /*  6  */
    bad_initkey = 0;
    bad_perfect = 0;
    for ( trysalt=1; ; ++trysalt )
	{
	int rslinit;

	/* reset some results */
	final->genUseHash2= 0;

	/*  7  */
	rslinit= initkey( keys, nkeys, *tabb, *alen, *blen, *smax, trysalt,
								form, final );
	if  ( rslinit < 0 )
	    { LDEB(rslinit); return -1;	}

	/*  8  */
	if  ( rslinit == 2 )
	    { *blen = 0; break; }

	/*  9  */
	if  ( rslinit == 0 )
	    {
	    bad_initkey++;

	    /*  10  */
	    if  ( bad_initkey < RETRY_INITKEY )
		{ continue;	}

	    /*  11, 12  */
	    if  ( *alen < maxalen )
		{
		*alen *= 2;

		bad_initkey = 0;
		bad_perfect = 0;
		continue;
		} 

	    /*  13  */
	    if  ( *blen < *smax )
		{
		*blen *= 2;
		*tabb  = (bstuff *)realloc( *tabb,
				    (size_t)(sizeof(bstuff)*(*blen)));
		tabq  = (qstuff *)realloc( tabq,
				    (size_t)(sizeof(qstuff)*(*blen+1)));

		bad_initkey = 0;
		bad_perfect = 0;
		continue;
		}

	    /*  14  */
	    if  ( duplicates( *tabb, *blen, keys, nkeys, form ) < 0 )
		{ return -1;	}
	    appDebug(
	      "fatal error: Cannot perfect hash: cannot find distinct (A,B)\n");

	    return -1;
	    }

	appDebug("found distinct (A,B) on attempt %ld\n", (long)trysalt );

	/*  15  */
	if  ( perfect( *tabb, tabh, *pHCount, tabq,
				*blen, *smax, scramble, nkeys, form ) )
	    { break;	}

	bad_perfect++;

	/*  16  */
	if  ( bad_perfect < RETRY_PERFECT )
	    { continue;	}

	/*  17  */
	if  ( *blen < *smax )
	    {
	    *blen *= 2;

	    *tabb  = (bstuff *)realloc( *tabb,
				    (size_t)(sizeof(bstuff)*(*blen)));
	    tabq  = (qstuff *)realloc( tabq,
				    (size_t)(sizeof(qstuff)*(*blen+1)));
	    --trysalt;
	    bad_perfect= 0;
	    continue;
	    }

	/*  18  */
	appDebug("fatal error: Cannot perfect hash: cannot build tab[]\n");
	exit( 1 );
	}

    appDebug("built perfect hash table of size %ld\n", (long)*blen);

    /* free working memory */
    *pTabh= tabh;

    free((void *)tabq);

    return 0;
}

/************************************************************************/
/*									*/
/*  Utility functions for writing the various arrays.			*/
/*									*/
/************************************************************************/

static void writeScramble_4(	FILE *		f,
				ub4 *		scramble )
    {
    int		i;

    fprintf(f, "static ub4 scramble[] = {\n");

    for ( i=0; i <= UB1MAXVAL; i += 4 )
	{
	fprintf(f, "0x%.8lx, 0x%.8lx, 0x%.8lx, 0x%.8lx,\n",
					(unsigned long)scramble[i+0],
					(unsigned long)scramble[i+1],
					(unsigned long)scramble[i+2],
					(unsigned long)scramble[i+3]);
	}

    fprintf(f, "};\n");
    }

static void writeScramble_2(	FILE *		f,
				ub4 *		scramble )
    {
    int		i;

    fprintf(f, "static ub2 scramble[] = {\n");

    for ( i=0; i <= UB1MAXVAL; i += 8 )
	{
	fprintf(f, 
		"0x%.4lx, 0x%.4lx, 0x%.4lx, 0x%.4lx, "
		"0x%.4lx, 0x%.4lx, 0x%.4lx, 0x%.4lx,\n",
					(unsigned long)scramble[i+0],
					(unsigned long)scramble[i+1],
					(unsigned long)scramble[i+2],
					(unsigned long)scramble[i+3],
					(unsigned long)scramble[i+4],
					(unsigned long)scramble[i+5],
					(unsigned long)scramble[i+6],
					(unsigned long)scramble[i+7] );
	}

    fprintf(f, "};\n");
    }

static void writeSTab_w16(	FILE *		f,
				const char *	type,
				const bstuff *	tabb,
				int		blen,
				const ub4 *	scramble,
				ub4		tabMask )
    {
    int		i;

    fprintf( f, "static const %s tab[] = {\n", type );

    for (i=0; i<blen; i+=16)
	{
	fprintf(f,
	    "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
	    "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,\n",
		(long)(scramble[tabb[i+ 0].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 1].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 2].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 3].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 4].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 5].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 6].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 7].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 8].val_b] & tabMask ),
		(long)(scramble[tabb[i+ 9].val_b] & tabMask ),
		(long)(scramble[tabb[i+10].val_b] & tabMask ),
		(long)(scramble[tabb[i+11].val_b] & tabMask ),
		(long)(scramble[tabb[i+12].val_b] & tabMask ),
		(long)(scramble[tabb[i+13].val_b] & tabMask ),
		(long)(scramble[tabb[i+14].val_b] & tabMask ),
		(long)(scramble[tabb[i+15].val_b] & tabMask ) ); 
	}

    fprintf( f, "};\n" );

    return;
    }

static void writeSTab_w8(	FILE *		f,
				const char *	type,
				const bstuff *	tabb,
				int		blen,
				const ub4 *	scramble,
				ub4		tabMask )
    {
    int		i;

    fprintf( f, "static const %s tab[] = {\n", type );

    for (i=0; i<blen; i+=8)
	{
	fprintf(f, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,\n",
		(long)(scramble[tabb[i+0].val_b] & tabMask ),
		(long)(scramble[tabb[i+1].val_b] & tabMask ), 
		(long)(scramble[tabb[i+2].val_b] & tabMask ),
		(long)(scramble[tabb[i+3].val_b] & tabMask ), 
		(long)(scramble[tabb[i+4].val_b] & tabMask ),
		(long)(scramble[tabb[i+5].val_b] & tabMask ), 
		(long)(scramble[tabb[i+6].val_b] & tabMask ),
		(long)(scramble[tabb[i+7].val_b] & tabMask ) ); 
	}

    fprintf( f, "};\n" );

    return;
    }

static void writeTab_w16(	FILE *		f,
				const char *	type,
				const bstuff *	tabb,
				int		blen,
				ub4		tabMask )
    {
    int		i;

    fprintf( f, "static const %s tab[] = {\n", type );

    for (i=0; i<blen; i+=16)
	{
	fprintf(f,
	    "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
	    "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,\n",
		(long)(tabb[i+ 0].val_b & tabMask ),
		(long)(tabb[i+ 1].val_b & tabMask ), 
		(long)(tabb[i+ 2].val_b & tabMask ),
		(long)(tabb[i+ 3].val_b & tabMask ), 
		(long)(tabb[i+ 4].val_b & tabMask ),
		(long)(tabb[i+ 5].val_b & tabMask ), 
		(long)(tabb[i+ 6].val_b & tabMask ),
		(long)(tabb[i+ 7].val_b & tabMask ), 
		(long)(tabb[i+ 8].val_b & tabMask ),
		(long)(tabb[i+ 9].val_b & tabMask ), 
		(long)(tabb[i+10].val_b & tabMask ),
		(long)(tabb[i+11].val_b & tabMask ), 
		(long)(tabb[i+12].val_b & tabMask ),
		(long)(tabb[i+13].val_b & tabMask ), 
		(long)(tabb[i+14].val_b & tabMask ),
		(long)(tabb[i+15].val_b & tabMask ) ); 
	}

    fprintf( f, "};\n" );

    return;
    }

/*

make the .c file

cOutput			   Output file name.
bstuff   *tabb;            table indexed by b
ub4       smax;            range of scramble[]
ub4       blen;            b in 0..blen-1, power of 2
ub4      *scramble;        used in final hash
gencode  *final;           code for the final hash
hashform *form;            user directives

1)  Preliminaries, both for printing the function and the 
    map back table. [+ testing]
2)  Open the file anf print header.
3)  Print scramble table.
4)  Print table indexed by b.

*/

static int make_c(	const char *		cOutput,
			const char *		cInclude,
			const char *		procName,
			const char *		indexName,
			const unsigned char **	keyStrings,
			key *			keys,
			int			keyStringCount,
			const bstuff *		tabb,
			hstuff *		tabh,
			int			hCount,
			ub4			smax,
			ub4			alen,
			ub4			blen,
			ub4 *			scramble,
			const gencode *		final,
			const hashform *	form )
{
    ub4			i;
    FILE *		f;

			/*  1  */
    int			valshift= UB4BITS- ilog2( alen );
    unsigned int	bMask= blen- 1;
    ub4			tabMask= 0;

    /*  2  */
    f= fopen( cOutput, "w" );
    if  ( ! f )
	{ SXDEB(cOutput,f); return -1;	}

    fprintf( f, "#   include %s\n\n", "<appUtilConfig.h>" );
    fprintf( f, "#   include %s\n\n", "<utilJenkinsHash.h>" );

    if  ( cInclude )
	{ fprintf( f, "#   include %s\n\n", cInclude );	}

    fprintf( f, "#   define ub4 UtilUint32\n" );
    fprintf( f, "#   define ub2 UtilUint16\n" );
    fprintf( f, "#   define ub1 unsigned char\n" );

    fprintf( f, "\n" );

    fprintf( f, "#   define CHECKSTATE 8\n" );

    fprintf( f, "\n" );

    fprintf( f, "/* smax= %ld, blen= %ld */\n\n", (long)smax, (long)blen );

    /*  3  */
    if (blen >= USE_SCRAMBLE)
    {
	fprintf(f, "/* A way to make the 1-byte values in tab bigger */\n");

	if  ( smax > UB2MAXVAL+1 )
	    { writeScramble_4( f, scramble );	}
	else{ writeScramble_2( f, scramble );	}

	fprintf(f, "\n");
    }

    /*  4  */
    if  ( blen > 0 )
	{
	const char * type= "ehh";

	fprintf( f,
	    "/* small adjustments to _a_ to make values distinct */\n" );

	if  ( smax <= UB1MAXVAL+1 || blen >= USE_SCRAMBLE )
	    {
	    type= "ub1";
	    tabMask= 0xff;
	    }
	else{
	    type= "ub2";
	    tabMask= 0xffff;
	    }

	if  ( blen < 16 )
	    {
	    fprintf( f, "static const %s tab[] = {\n", type );

	    for ( i=0; i < blen; i++ )
		{ fprintf(f, "%3d,", scramble[tabb[i].val_b] & tabMask ); }
	    fprintf(f, "};\n");
	    }
	else if (blen <= 1024)
	    {
	    writeSTab_w16( f, type, tabb, blen, scramble, tabMask );
	    }
	else if (blen < USE_SCRAMBLE)
	    {
	    writeSTab_w8( f, type, tabb, blen, scramble, tabMask );
	    }
	else{
	    writeTab_w16( f, type, tabb, blen, tabMask );
	    }

	fprintf(f, "\n");
	}

#   if 0
    {
    int k;

    fprintf( f, "/*\n\n" );

    for ( i= 0; i < hCount; i++ )
	{ tabh[i].key_h= (key *)0;	}

    for ( k= 0; k < keyStringCount; k++ )
	{
	ub4 initlev= final->genInitlev;
	const unsigned char * keyv= keys[k].name_k;
	int len= keys[k].len_k;
	ub4 tab_b;
	ub4 rsl= 0;

	if  ( final->genUseHash2 )
	    {
	    unsigned long state[CHECKSTATE];
	    ub4 a;
	    ub4 b;

	    for ( i=0; i < CHECKSTATE; ++i ) state[i]= initlev;

	    utilJenkinsHash2( keyv, len, state );

	    a= state[0]&(alen-1);
	    b= state[1]&(blen-1);

	    if  ( a != keys[k].a_k )
		{ LLDEB(a,keys[k].a_k);	}
	    if  ( b != keys[k].b_k )
		{ LLDEB(b,keys[k].b_k);	}

	    if  ( blen < USE_SCRAMBLE )
		{ tab_b= scramble[tabb[b].val_b] & tabMask;	}
	    else{ tab_b= tabb[b].val_b & tabMask;		}

	    rsl= (a^scramble[tab_b]);
	    }
	else{
	    ub4 val = utilJenkinsHash( keyv, len, initlev );
	    ub4 a= val>>valshift;
	    ub4 b= val&bMask;

	    if  ( a != keys[k].a_k )
		{ LLDEB(a,keys[k].a_k);	}
	    if  ( b != keys[k].b_k )
		{ LLDEB(b,keys[k].b_k);	}

	    if  ( blen < USE_SCRAMBLE )
		{ tab_b= scramble[tabb[b].val_b] & tabMask;	}
	    else{ tab_b= tabb[b].val_b & tabMask;		}

	    if  ( smax > 1 )
		{
		if  ( blen < USE_SCRAMBLE )
		    { rsl = (a^tab_b);			}
		else{ rsl = (a^scramble[tab_b]);	}
		}
	    }

	if  ( rsl >= hCount )
	    { LLDEB(rsl,hCount);	}
	else{
	    if  ( tabh[rsl].key_h )
		{ LLSDEB(k,tabh[rsl].key_h->i_k,(char *)keyStrings[k]);	}

	    tabh[rsl].key_h= &(keys[k]);
	    }

	fprintf( f, "%7d\t%7ld\t\"%s\"\n", k, (long)rsl, keyStrings[k] );
	}

    fprintf( f, "\n*/\n\n" );

    }
#   endif

    if  ( indexName )
	{
	fprintf(f, "/* map back array */\n\n");

	fprintf( f, "static int mapBack[]=\n" );
	fprintf( f, "    {\n" );

	for ( i= 0; i < hCount; i++ )
	    {
	    if  ( tabh[i].key_h )
		{
		fprintf( f, "    %4ldL,\t/* %5d: \"%s\" */\n",
				    tabh[i].key_h->i_k,
				    i, keyStrings[tabh[i].key_h->i_k] );
		}
	    else{
		fprintf( f, "    -1L,\n" );
		}
	    }

	fprintf( f, "    };\n\n" );
	}

    fprintf(f, "/* The hash function */\n\n");

    fprintf(f, "unsigned long %s(const unsigned char * key, int len )\n",
								    procName );
    fprintf(f, "{\n");

    fprintf(f, "    const ub4 initlev= 0x%lx;\n",
				    (unsigned long)final->genInitlev );
    fprintf(f, "    ub4 rsl= 0;\n" );

    if  ( final->genUseHash2 )
	{
	fprintf( f, "    unsigned long state[CHECKSTATE];\n" );
	fprintf( f, "    ub4 i;\n" );
	fprintf( f, "\n" );

	fprintf( f, "    for (i=0; i<CHECKSTATE; ++i) state[i]=initlev;\n" );
	fprintf( f, "\n" );

	fprintf( f, "    utilJenkinsHash2(key, len, state);\n" );
	fprintf( f, "\n" );

	fprintf( f,
	    "    rsl= ((state[0]&0x%x)^scramble[tab[state[1]&0x%x]]);\n",
							    alen-1, blen-1);
	}
    else{
	fprintf( f, "    ub4 val = utilJenkinsHash( key, len, initlev );\n" );

	if  ( smax > 1 )
	    {
	    if  ( blen < USE_SCRAMBLE )
		{
		fprintf( f, "    rsl = ((val>>%d)^tab[val&0x%x]);\n",
							  valshift, bMask );
		}
	    else{
		fprintf( f, "    rsl = ((val>>%d)^scramble[tab[val&0x%x]]);\n",
							  valshift, bMask );
		}
	    }
	}

    fprintf(f, "    return rsl;\n");
    fprintf(f, "}\n\n");

    if  ( indexName )
	{
	fprintf( f, "int %s( const unsigned char * key, int len )\n",
								indexName );
	fprintf(f, "{\n");
	fprintf(f, "    ub4 rsl= %s( key, len );\n", procName );
	fprintf( f, "\n" );

	fprintf( f, "    if  ( rsl >= sizeof(mapBack)/sizeof(int) )\n" );
	fprintf( f, "\t{ return -1;	}\n" );
	fprintf( f, "\n" );

	fprintf( f, "    return mapBack[rsl];\n" );

	fprintf(f, "}\n\n");
	}

    fclose(f);

    return 0;
}

/*
------------------------------------------------------------------------------
External interface
------------------------------------------------------------------------------
*/

extern int utilJenkinsPerfectHash(	const unsigned char **	keyStrings,
					int			keyStringCount,
					const char *		cOutput,
					const char *		cInclude,
					const char *		procName,
					const char *		indexName )
    {
    int		rval= 0;
    hashform	form;

    key *	keys= (key *)0;
    key *	mykey;
    int		ki;

    bstuff *	tabb= (bstuff *)0;
    hstuff *	tabh= (hstuff *)0;

    int		hCount;

    ub4       smax;          /* scramble[] values in 0..smax-1, a power of 2 */
    ub4       alen;                          /* a in 0..alen-1, a power of 2 */
    ub4       blen;                          /* b in 0..blen-1, a power of 2 */
    gencode   final;                                  /* code for final hash */
    ub4       scramble[SCRAMBLE_LEN];         /* used in final hash function */

    /* default behavior */
    form.mode= NORMAL_HM;
    form.hashtype= STRING_HT;
    form.perfect= MINIMAL_HP;
    form.speed= SLOW_HS;

    /* set up code for final hash */
    final.genUseHash2= 0;

    keys= (key *)malloc( keyStringCount* sizeof( key ) );
    if  ( ! keys )
	{ LXDEB(keyStringCount,keys); rval= -1; goto ready; }

    mykey= keys;
    for ( ki= 0; ki < keyStringCount; mykey++, ki++ )
	{
	memset( mykey, 0, sizeof(key) );
	mykey->name_k = keyStrings[ki];
	mykey->len_k  = (ub4)(strlen((char *)mykey->name_k));
	mykey->i_k  = (ub4)ki;
	}

    /* find the hash */
    if  ( findhash(&tabb, &tabh, &hCount, &alen, &blen, &final, 
		       scramble, &smax, keys, keyStringCount, &form ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( make_c( cOutput, cInclude, procName, indexName,
			    keyStrings, keys, keyStringCount,
			    tabb, tabh, hCount,
			    smax, alen, blen, scramble, &final, &form ) )
	{ SDEB(cOutput); rval= -1; goto ready;	}

  ready:

    if  ( keys )
	{ free( keys );	}
    if  ( tabb )
	{ free( tabb );	}
    if  ( tabh )
	{ free( tabh );	}

    return rval;
    }
