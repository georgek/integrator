/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

/* rational functions */

#ifndef _RATFUN_H_
#define _RATFUN_H_

/* useful macro */
#define PRINTR(r) printf(#r ": ");              \
     print_ratfun(r);                           \
     printf("\t\t(rational function)");         \
     printf("\n")

#include "bigrat.h"
#include "polynomial.h"

typedef struct ratfun
{
     Coefficient num;
     Coefficient den;
} RatFun;

void init_ratfun(RatFun *r);

/* the canonical form of a rational function has a non-zero positive
 * denominator, is in lowest terms and has only integer
 * coefficients */
void canonicalise_ratfun(RatFun *r);

void print_ratfun(RatFun r);
void print_ratfun_LaTeX(RatFun r);

void copy_ratfun(RatFun *r, RatFun s);
void free_ratfun(RatFun *r);

int ratfun_zero(RatFun r);

/* arithmetic */
void add_ratfuns(RatFun *res, RatFun left, RatFun right);
void sub_ratfuns(RatFun *res, RatFun left, RatFun right);
void mul_ratfuns(RatFun *res, RatFun left, RatFun right);
void div_ratfuns(RatFun *res, RatFun left, RatFun right);

void negate_ratfun(RatFun *r);
void invert_ratfun(RatFun *r);

void ratfun_power(RatFun *res, RatFun p, SHORT_INT_T power);

void add_ratfun_poly(RatFun *res, RatFun left, Polynomial right);
void sub_ratfun_poly(RatFun *res, RatFun left, Polynomial right);
void mul_ratfun_poly(RatFun *res, RatFun left, Polynomial right);
void div_ratfun_poly(RatFun *res, RatFun left, Polynomial right);

void add_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void sub_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void mul_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void div_ratfun_rat(RatFun *res, RatFun left, BigRat right);


#endif /* _RATFUN_H_ */
