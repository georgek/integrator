/* rational functions */

#ifndef _RATFUN_H_
#define _RATFUN_H_

#include "bigrat.h"
#include "polynomial.h"

typedef struct ratfun
{
     Coefficient num;
     Coefficient den;
} RatFun;

/* the canonical form of a rational function has a non-zero positive
 * denominator, is in lowest terms and has only integer
 * coefficients */
void canonicalise_ratfun(RatFun *r);

void print_ratfun(RatFun r);

void copy_ratfun(RatFun *r, RatFun s);
void free_ratfun(RatFun *r);

/* arithmetic */
void add_ratfuns(RatFun *res, RatFun left, RatFun right);
void sub_ratfuns(RatFun *res, RatFun left, RatFun right);
void mul_ratfuns(RatFun *res, RatFun left, RatFun right);
void div_ratfuns(RatFun *res, RatFun left, RatFun right);

void negate_ratfun(RatFun *r);

void ratfun_power(RatFun *res, RatFun p, SHORT_INT_T power);

void add_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void sub_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void mul_ratfun_rat(RatFun *res, RatFun left, BigRat right);
void div_ratfun_rat(RatFun *res, RatFun left, BigRat right);


#endif /* _RATFUN_H_ */
