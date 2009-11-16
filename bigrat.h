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
BigRat make_bigrat(BigNum num, BigNum den);

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
