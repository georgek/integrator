/* big rational numbers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"
#include "bigrat.h"

void make_bigrat(BigRat *f, BigNum num, BigNum den)
{
     free_bigrat(f);

     bignum_copy(&f->num, num);
     bignum_copy(&f->den, den);
     reduce_bigrat(f);
     /* set sign */
     if (is_neg(f->den)) {
          negate_bignum(f->num);
          negate_bignum(f->den);
     }
}

void init_bigrat(BigRat *f)
{
     f->num = make_bignum2(0);
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
          print_bignum(f.den);
     }
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

