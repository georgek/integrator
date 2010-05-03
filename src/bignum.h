/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

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
/* signed version for checking sign of bignum */
#define S_SHORT_INT_T signed int
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
/* with no string length */
BigNum make_bignuml(char *string);
/* make new bignum from integer */
BigNum make_bignum2(SHORT_INT_T n);
/* makes a new bignum of a given length initialised to 0 */
BigNum make_zero_bignum(int length);

void free_bignum(BigNum p);

/* print with sign if negative */
void print_bignum(BigNum p);
/* print without sign */
void print_bignum2(BigNum p);
/* print with sign always */
/* void print_bignum3(BigNum p); */
/* print just sign */
void print_sign(BigNum p, int flip);

void bignum_copy(BigNum *d, BigNum s);

int real_length(BigNum);

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

void bn_power(BigNum *res, BigNum p, SHORT_INT_T power);

void half_bignum(BigNum *res, BigNum u);
void double_bignum(BigNum *res, BigNum u);

/* greatest common divisor - binary method */
void gcd(BigNum *gcd, BigNum u, BigNum v);
/* greatest common divisor - Euclid's method */
void gcd2(BigNum *gcd, BigNum u, BigNum v);
/* least common multiple */
void lcm(BigNum *lcm, BigNum u, BigNum v);

/* other */
/* returns true is bignum is equal to zero */
int bn_zero(BigNum u);
/* true if bignum is equal to one */
int bn_one(BigNum u);
/* returns true for 1 or -1 */
int bn_one2(BigNum u);

#endif /* _BIGNUM_H_ */
