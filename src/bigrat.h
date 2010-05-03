/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

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
/* make from one bignum */
BigRat make_bigrat2(BigNum num);

BigRat make_bigrat3(SHORT_INT_T val);

/* initialises bigrat, this function must be called for a bigrat
 * before it is used */
void init_bigrat(BigRat *f);
void init_bigrat2(BigRat *f, SHORT_INT_T val);

void free_bigrat(BigRat *f);

/* print with sign ig negative */
void print_bigrat(BigRat f);
/* print bigrat without its sign */
void print_bigrat2(BigRat f);
/* print with sign */
/* void print_bigrat3(BigRat f); */
/* print just sign */
void print_br_sign(BigRat f, int flip);

void print_bigrat_LaTeX(BigRat f);
void print_bigrat_LaTeX2(BigRat f);

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
/* returns true for 1 or -1 */
int br_one2(BigRat f);
int br_neg(BigRat f);
/* returns true if the bigrat is an integer */
int br_int(BigRat f);

#endif /* _BIGRAT_H_ */
