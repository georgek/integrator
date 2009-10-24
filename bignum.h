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

/* find good value for radix, this is good for 32 bit short int */
#define RADIX 1000000000
#define DIGPERWORD 9

typedef SHORT_INT_T *BigNum;

/* makes a new bignum from a string, returns pointer to array */
BigNum make_bignum(char *string, int length);
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
void div_bignums(BigNum *q, BigNum *r, BigNum left, BigNum right);

#endif /* _BIGNUM_H_ */
