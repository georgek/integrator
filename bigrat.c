/* big rational numbers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"
#include "bigrat.h"

BigRat make_bigrat(BigNum num, BigNum den)
{
     BigRat rat;
     rat.num = NULL;
     rat.den = NULL;
     bignum_copy(&rat.num, num);
     bignum_copy(&rat.den, den);
     reduce_bigrat(&rat);
     /* set sign */
     if (is_neg(rat.den)) {
          negate_bignum(rat.num);
          negate_bignum(rat.den);
     }
     return rat;
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
     if (one(f.den)) {
          print_bignum(f.num);
     }
     else {
          printf("(");
          print_bignum(f.num);
          printf("/");
          print_bignum(f.den);
          printf(")");
     }
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

