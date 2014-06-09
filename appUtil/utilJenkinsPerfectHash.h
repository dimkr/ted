/*
------------------------------------------------------------------------------
perfect.h: code to generate code for a hash for perfect hashing.
(c) Bob Jenkins, September 1996
You may use this code in any way you wish, and it is free.  No warranty.
I hereby place this in the public domain.
Source is http://burtleburtle.net/bob/c/perfect.h
------------------------------------------------------------------------------
*/

#   include "appUtilConfig.h"

#ifndef UTIL_JENKINS_PERFECT_HASH_H
#define UTIL_JENKINS_PERFECT_HASH_H

# define ub4 UtilUint32
# define ub2 UtilUint16
# define ub1 unsigned char

#define MAXKEYLEN 30                              /* maximum length of a key */
#define USE_SCRAMBLE  4096           /* use scramble if blen >= USE_SCRAMBLE */
#define SCRAMBLE_LEN ((ub4)1<<16)                    /* length of *scramble* */
#define RETRY_INITKEY 2048  /* number of times to try to find distinct (a,b) */
#define RETRY_PERFECT 1     /* number of times to try to make a perfect hash */
#define RETRY_HEX     200               /* RETRY_PERFECT when hex keys given */

/* the generated code for the final hash, assumes initial hash is done */
struct gencode
{
  ub4  genInitlev;
  int  genUseHash2;

  /*
   * The code placed here must declare "ub4 rsl" 
   * and assign it the value of the perfect hash using the function inputs.
   * Later code will be tacked on which returns rsl or manipulates it according
   * to the user directives.
   *
   * This code is at the top of the routine; it may and must declare any
   * local variables it needs.
   *
   * Each way of filling in **line should be given a comment that is a unique
   * tag.  A testcase named with that tag should also be found which tests
   * the generated code.
   */
};

typedef  struct gencode  gencode;

/* user directives: perfect hash? minimal perfect hash? input is an int? */
struct hashform
{
  enum {
    NORMAL_HM                                            /* key is a string */
  } mode;
  enum {
    STRING_HT                                            /* key is a string */
  } hashtype;
  enum {
    NORMAL_HP,                                   /* just find a perfect hash */
    MINIMAL_HP                                /* find a minimal perfect hash */
  } perfect;
  enum {
    FAST_HS,                                                    /* fast mode */
    SLOW_HS                                                     /* slow mode */
  } speed;
};
typedef  struct hashform  hashform;

/* representation of a key */
struct key
{
  const unsigned char *  name_k;                           /* the actual key */
  ub4         len_k;                         /* the length of the actual key */

/* beyond this point is mapping-dependent */

  ub4         a_k;                            /* a, of the key maps to (a,b) */
  ub4         b_k;                            /* b, of the key maps to (a,b) */
  long        i_k;                            /* b, of the key maps to (a,b) */
  struct key *nextb_k;                               /* next key with this b */
};
typedef  struct key  key;

/* things indexed by b of original (a,b) pair */
struct bstuff
{
  ub2  val_b;                                        /* hash=a^tabb[b].val_b */
  key *list_b;                   /* tabb[i].list_b is list of keys with b==i */
  ub4  listlen_b;                                        /* length of list_b */
  ub4  water_b;           /* high watermark of who has visited this map node */
};
typedef  struct bstuff  bstuff;

/* things indexed by final hash value */
struct hstuff
{
  key *key_h;                   /* tabh[i].key_h is the key with a hash of i */
};
typedef  struct hstuff hstuff;

/* things indexed by queue position */
struct qstuff
{
  bstuff *b_q;                        /* b that currently occupies this hash */
  ub4     parent_q;     /* queue position of parent that could use this hash */
  ub2     newval_q;      /* what to change parent tab[b] to to use this hash */
  ub2     oldval_q;                              /* original value of tab[b] */
};
typedef  struct qstuff  qstuff;

extern int utilJenkinsPerfectHash(	const unsigned char **	keyStrings,
					int			keyStringCount,
					const char *		cOutput,
					const char *		cInclude,
					const char *		procName,
					const char *		indexName );

#endif /* UTIL_JENKINS_PERFECT_HASH_H */
