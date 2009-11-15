/* big rational numbers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"
#include "bigrat.h"

BigRat make_bigrat(BigNum num, BigNum den)
{
     BigRat rat;
     BigNum n = NULL, d = NULL, g = NULL, r = NULL;
     bignum_copy(&n, num);
     bignum_copy(&d, den);
     /* find gcd */
     gcd(&g, n, d);
     div_bignums(&n, &r, n, g);
     div_bignums(&d, &r, d, g);
     rat.num = n;
     rat.den = d;
     return rat;
}

void free_bigrat(BigRat f)
{
     free_bignum(f.num);
     free_bignum(f.den);
}

void print_bigrat(BigRat f)
{
     printf("(");
     print_bignum(f.num);
     printf("/");
     print_bignum(f.den);
     printf(")");
}

