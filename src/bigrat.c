/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

/* big rational numbers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"
#include "bigrat.h"

BigRat make_bigrat(BigNum num, BigNum den)
{
     BigRat new = {NULL, NULL};

     bignum_copy(&new.num, num);
     bignum_copy(&new.den, den);
     reduce_bigrat(&new);
     /* set sign */
     if (is_neg(new.den)) {
          negate_bignum(new.num);
          negate_bignum(new.den);
     }

     return new;
}

BigRat make_bigrat2(BigNum num)
{
     BigRat new = {NULL, NULL};

     bignum_copy(&new.num, num);
     new.den = make_bignum2(1);
     reduce_bigrat(&new);

     return new;
}

BigRat make_bigrat3(SHORT_INT_T val)
{
     BigRat new = {NULL, NULL};
     init_bigrat2(&new, val);
     return new;
}

void init_bigrat(BigRat *f)
{
     f->num = make_bignum2(0);
     f->den = make_bignum2(1);
}

void init_bigrat2(BigRat *f, SHORT_INT_T val)
{
     f->num = make_bignum2(val);
     f->den = make_bignum2(1);
}

void free_bigrat(BigRat *f)
{
     free_bignum(f->num);
     free_bignum(f->den);
     f->num = NULL;
     f->den = NULL;
}

void print_bigrat(BigRat f)
{
     if (bn_one(f.den)) {
          print_bignum(f.num);
     }
     else {
          print_bignum(f.num);
          printf("/");
          print_bignum2(f.den);
     }
}

void print_bigrat2(BigRat f)
{
     if (bn_one(f.den)) {
          print_bignum2(f.num);
     }
     else {
          print_bignum2(f.num);
          printf("/");
          print_bignum2(f.den);
     }
}

void print_bigrat_LaTeX(BigRat f)
{
     if (bn_one(f.den)) {
          print_bignum(f.num);
     }
     else {
          printf("\\frac{");
          print_bignum(f.num);
          printf("}{");
          print_bignum2(f.den);
          printf("}");
     }
}

void print_bigrat_LaTeX2(BigRat f)
{
     if (bn_one(f.den)) {
          print_bignum2(f.num);
     }
     else {
          printf("\\frac{");
          print_bignum2(f.num);
          printf("}{");
          print_bignum2(f.den);
          printf("}");
     }
}

/* void print_bigrat3(BigRat f) */
/* { */
/*      if (bn_one(f.den)) { */
/*           print_bignum3(f.num); */
/*      } */
/*      else { */
/*           print_bignum3(f.num); */
/*           printf("/"); */
/*           print_bignum2(f.den); */
/*      } */
/* } */

void print_br_sign(BigRat f, int flip)
{
     print_sign(f.num, flip);
}

void bigrat_copy(BigRat *f, BigRat g)
{
     bignum_copy(&f->num, g.num);
     bignum_copy(&f->den, g.den);
}

void reduce_bigrat(BigRat *f)
{
     BigNum g = NULL, r = NULL;
     gcd(&g, f->num, f->den);
     div_bignums(&f->num, &r, f->num, g);
     div_bignums(&f->den, &r, f->den, g);
     if (is_neg(f->den)) {
          negate_bignum(f->num);
          negate_bignum(f->den);
     }
     free_bignum(g);
     free_bignum(r);
}

void negate_bigrat(BigRat *f)
{
     negate_bignum(f->num);
}

void add_bigrats(BigRat *res, BigRat left, BigRat right)
{
     BigRat t;
     init_bigrat(&t);

     /* use t.num and t.den to hold temporary values */
     mul_bignums(&t.num, left.num, right.den);
     mul_bignums(&t.den, left.den, right.num);

     /* w = uv' + u'v */
     add_bignums(&t.num, t.num, t.den);
     /* w' = u'v' */
     mul_bignums(&t.den, left.den, right.den);

     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void sub_bigrats(BigRat *res, BigRat left, BigRat right)
{
     BigRat t;
     init_bigrat(&t);

     /* use t.num and t.den to hold temporary values */
     mul_bignums(&t.num, left.num, right.den);
     mul_bignums(&t.den, left.den, right.num);

     /* w = uv' - u'v */
     sub_bignums(&t.num, t.num, t.den);
     /* w' = u'v' */
     mul_bignums(&t.den, left.den, right.den);

     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void mul_bigrats(BigRat *res, BigRat left, BigRat right)
{
     BigRat t;
     init_bigrat(&t);

     mul_bignums(&t.num, left.num, right.num);
     mul_bignums(&t.den, left.den, right.den);
     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void mul_bigrats2(BigRat *res, BigRat left, SHORT_INT_T right)
{
     BigRat t;
     init_bigrat(&t);

     mul_bignums2(&t.num, left.num, right);
     bignum_copy(&t.den, left.den);
     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void div_bigrats(BigRat *res, BigRat left, BigRat right)
{
     /* same as multiply except inverts right */
     BigRat t;
     init_bigrat(&t);

     mul_bignums(&t.num, left.num, right.den);
     mul_bignums(&t.den, left.den, right.num);
     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void div_bigrats2(BigRat *res, BigRat left, SHORT_INT_T right)
{
     BigRat t;
     init_bigrat(&t);

     bignum_copy(&t.num, left.num);
     mul_bignums2(&t.den, left.den, right);
     reduce_bigrat(&t);

     free_bigrat(res);
     res->num = t.num;
     res->den = t.den;
}

void br_power(BigRat *res, BigRat p, SHORT_INT_T power)
{
     BigRat temp;
     SHORT_INT_T mask = 0;

     if (power == 0) {
          free_bigrat(res);
          res->num = make_bignum2(1);
          res->den = make_bignum2(1);
          return;
     }
     
     mask = ~(~mask>>1);        /* MSB of short int */

     init_bigrat(&temp);
     bigrat_copy(&temp, p);

     /* find first 1 in binary representation of power and skip it */
     while (!(mask & power) && mask) {
          mask = mask>>1;
     }
     mask = mask>>1;
     for (; mask; mask = mask>>1) {
          mul_bigrats(&temp, temp, temp);
          if (mask & power) {
               mul_bigrats(&temp, temp, p);
          }
     }

     bigrat_copy(res, temp);
     free_bigrat(&temp);
}

int br_zero(BigRat f)
{
     return bn_zero(f.num);
}

int br_one(BigRat f)
{
     if (bn_one(f.num) && bn_one(f.den)) {
          return 1;
     }
     return 0;
}

int br_one2(BigRat f)
{
     if (bn_one2(f.num) && bn_one(f.den)) {
          return 1;
     }
     return 0;
}

int br_neg(BigRat f)
{
     return is_neg(f.num);
}

int br_int(BigRat f)
{
     return bn_one(f.den);
}

