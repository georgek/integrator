/* big rational numbers */

#ifndef _BIGRAT_H_
#define _BIGRAT_H_

#include <limits.h>

#include "bignum.h"

typedef struct {
     BigNum num;                /* numerator */
     BigNum den;                /* denominator */
} BigRat;

/* make new bigrat from two bignums */
void make_bigrat(BigRat *f, BigNum num, BigNum den);

/* initialises bigrat, this function must be called for a bigrat
 * before it is used */
void init_bigrat(BigRat *f);

void free_bigrat(BigRat *f);

void print_bigrat(BigRat f);

void reduce_bigrat(BigRat *f);

/* arithmetic */
void negate_bigrat(BigRat *f);
void add_bigrats(BigRat *res, BigRat left, BigRat right);
void sub_bigrats(BigRat *res, BigRat left, BigRat right);
void mul_bigrats(BigRat *res, BigRat left, BigRat right);
void div_bigrats(BigRat *res, BigRat left, BigRat right);


#endif /* _BIGRAT_H_ */
