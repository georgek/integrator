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
void free_bignum(BigNum p);

void print_bignum(BigNum p);

#endif /* _BIGNUM_H_ */
