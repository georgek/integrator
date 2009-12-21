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
void init_bigrat2(BigRat *f, SHORT_INT_T val);

void free_bigrat(BigRat *f);

void print_bigrat(BigRat f);

void bigrat_copy(BigRat *f, BigRat g);

void reduce_bigrat(BigRat *f);

/* arithmetic */
void negate_bigrat(BigRat *f);
void add_bigrats(BigRat *res, BigRat left, BigRat right);
void sub_bigrats(BigRat *res, BigRat left, BigRat right);
void mul_bigrats(BigRat *res, BigRat left, BigRat right);
void mul_bigrats2(BigRat *res, BigRat left, SHORT_INT_T right);
void div_bigrats(BigRat *res, BigRat left, BigRat right);
void div_bigrats2(BigRat *res, BigRat left, SHORT_INT_T right);

void br_power(BigRat *res, BigRat p, SHORT_INT_T power);

int br_zero(BigRat f);
int br_one(BigRat f);

#endif /* _BIGRAT_H_ */
