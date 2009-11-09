/* bignums are arbitrary precision integers */
/* **TODO**: make sure this works with any system not just mine,
     probably using a configure script to work out which types to use
     and the radix */

#ifndef _BIGNUM_H_
#define _BIGNUM_H_

#include <limits.h>

/* long int type for result of multiplication */
#define LONG_INT_T unsigned long int
/* and the argument type for printf */
#define PRINT_LI(var) printf("%lu", var)

/* short int type for storage */
#define SHORT_INT_T unsigned int
/* and its argument for printf */
#define PRINT_SI(var) printf("%u", var)

/* find good value for radix,
 * this is good for 32 bit short int (for 64 bit system) */
#define RADIX 1000000000
/* this is good for a 16 bit short int (for 32 bit system) */
/* #define RADIX 10000 */
#define DIGPERWORD 9
/* word length is the actual length of words in bits */
#define WORD_LENGTH 30

typedef SHORT_INT_T *BigNum;

/* makes a new bignum from a string, returns pointer to array */
BigNum make_bignum(char *string, int length);
/* make new bignum from integer */
BigNum make_bignum2(SHORT_INT_T n);
/* makes a new bignum of a given length initialised to 0 */
BigNum make_zero_bignum(int length);

void free_bignum(BigNum p);

void print_bignum(BigNum p);

/* logical operations */
int is_neg(BigNum p);
int bignum_equal(BigNum left, BigNum right);
int bignum_lt(BigNum left, BigNum right);
int bignum_gt(BigNum left, BigNum right);
int bignum_lte(BigNum left, BigNum right);
int bignum_gte(BigNum left, BigNum right);

/* arithmetic */
void negate_bignum(BigNum p);
void add_bignums(BigNum *res, BigNum left, BigNum right);
void sub_bignums(BigNum *res, BigNum left, BigNum right);
void mul_bignums(BigNum *res, BigNum left, BigNum right);
void mul_bignums2(BigNum *res, BigNum left, SHORT_INT_T right);
void div_bignums(BigNum *q, BigNum *r, BigNum left, BigNum right);
void div_bignums2(BigNum *q, SHORT_INT_T *r, BigNum left, SHORT_INT_T right);

void half_bignum(BigNum *res, BigNum u);
void double_bignum(BigNum *res, BigNum u);

/* greatest common divisor */
void gcd(BigNum *gcd, BigNum u, BigNum v);


#endif /* _BIGNUM_H_ */
