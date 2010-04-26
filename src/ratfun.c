#include <stdlib.h>
#include <stdio.h>

#include "bignum.h"
#include "ratfun.h"
#include "polynomial.h"
#include "bigrat.h"
#include "prs.h"

void init_ratfun(RatFun *r)
{
     r->num.type = special;
     r->den.type = special;
}

void canonicalise_ratfun(RatFun *r)
{
     Coefficient t = {rational, {{NULL, NULL}}};
     /* Coefficient s = {rational, {{NULL, NULL}}}; */

     /* printf("canonicalising ratfun...\n"); */
     /* PRINTC(r->num); */
     /* PRINTC(r->den); */

     if (coef_zero(r->num)){
          free_coefficient(&r->den);
          r->den.type = rational;
          r->den.u.rat = make_bigrat3(1);
          return;
     }

     if (coef_neg(r->den)) {
          negate_coefficient(&r->num);
          negate_coefficient(&r->den);
     }

     /* remove rational coefficients */
     t.u.rat = coef_rat_part(r->num);
     mul_coefficients(&r->num, r->num, t);
     mul_coefficients(&r->den, r->den, t);
     free_coefficient(&t);

     t.u.rat = coef_rat_part(r->den);
     mul_coefficients(&r->num, r->num, t);
     mul_coefficients(&r->den, r->den, t);
     free_coefficient(&t);

     /* reduce */
     if (r->num.type == polynomial && r->den.type == polynomial) {
          /* printf("reducing ratfun...\n"); */
          /* PRINTC(r->num); */
          /* PRINTC(r->den); */
          coef_gcd(&t, r->num, r->den);
          /* PRINTC(r->num); */
          /* PRINTC(r->den); */
          /* PRINTC(t); */
          exact_div_coefficients(&r->num, r->num, t);
          exact_div_coefficients(&r->den, r->den, t);
          free_coefficient(&t);
     }
}

void print_ratfun(RatFun r)
{
     if (coef_zero(r.num)) {
          printf("zero");
          return;
     }
     if (coef_one(r.den)) {
          print_coefficient(r.num);
          return;
     }
     print_coefficient(r.num);
     printf("/");
     print_coefficient(r.den);
}

void print_ratfun_LaTeX(RatFun r)
{
     Coefficient t = {special};

     if (coef_zero(r.num)) {
          printf("0");
          return;
     }
     if (coef_one(r.den)) {
          print_coefficient_LaTeX(r.num);
     }
     else {
          copy_coefficient(&t, r.num);
          if (coef_neg(r.num)) {
               negate_coefficient(&t);
               printf("-");
          }
          printf("\\frac{");
          print_coefficient_LaTeX(t);
          printf("}{");
          print_coefficient_LaTeX(r.den);
          printf("}");
     }
     free_coefficient(&t);
}

void copy_ratfun(RatFun *r, RatFun s)
{
     copy_coefficient(&r->num, s.num);
     copy_coefficient(&r->den, s.den);
}

void free_ratfun(RatFun *r)
{
     free_coefficient(&r->num);
     free_coefficient(&r->den);
}

int ratfun_zero(RatFun r)
{
     return coef_zero(r.num);
}

void add_ratfuns(RatFun *res, RatFun left, RatFun right)
{
     RatFun t;
     init_ratfun(&t);

     /* use t.num and t.den to hold temporary values */
     mul_coefficients(&t.num, left.num, right.den);
     mul_coefficients(&t.den, left.den, right.num);

     /* w = uv' + u'v */
     add_coefficients(&t.num, t.num, t.den);
     /* w' = u'v' */
     mul_coefficients(&t.den, left.den, right.den);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void sub_ratfuns(RatFun *res, RatFun left, RatFun right)
{
     RatFun t;
     init_ratfun(&t);

     /* use t.num and t.den to hold temporary values */
     mul_coefficients(&t.num, left.num, right.den);
     mul_coefficients(&t.den, left.den, right.num);

     /* w = uv' - u'v */
     sub_coefficients(&t.num, t.num, t.den);
     /* w' = u'v' */
     mul_coefficients(&t.den, left.den, right.den);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void mul_ratfuns(RatFun *res, RatFun left, RatFun right)
{
     RatFun t;
     init_ratfun(&t);

     mul_coefficients(&t.num, left.num, right.num);
     mul_coefficients(&t.den, left.den, right.den);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void div_ratfuns(RatFun *res, RatFun left, RatFun right)
{
     /* same as multiply except inverts right */
     RatFun t;
     init_ratfun(&t);

     mul_coefficients(&t.num, left.num, right.den);
     mul_coefficients(&t.den, left.den, right.num);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void negate_ratfun(RatFun *r)
{
     negate_coefficient(&r->num);
}

void invert_ratfun(RatFun *r)
{
     Coefficient t = r->num;
     r->num = r->den;
     r->den = t;
}

/* TODO this is probably really inefficient, would be more
 * efficient to reduce fraction each time */
void ratfun_power(RatFun *res, RatFun p, SHORT_INT_T power)
{
     copy_ratfun(res, p);
     coef_power(&res->num, res->num, power);
     coef_power(&res->den, res->den, power);
     canonicalise_ratfun(res);
}

void add_ratfun_poly(RatFun *res, RatFun left, Polynomial right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = polynomial;
     tc.u.poly = right;

     /* use t.num and t.den to hold temporary values */
     mul_coefficients(&t.den, left.den, tc);

     /* w = uv' + u'v */
     add_coefficients(&t.num, left.num, t.den);
     /* w' = u'v' */
     t.den.type = special;
     copy_coefficient(&t.den, left.den);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void sub_ratfun_poly(RatFun *res, RatFun left, Polynomial right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = polynomial;
     tc.u.poly = right;

     /* use t.num and t.den to hold temporary values */
     mul_coefficients(&t.den, left.den, tc);

     /* w = uv' - u'v */
     sub_coefficients(&t.num, left.num, t.den);
     /* w' = u'v' */
     t.den.type = special;
     copy_coefficient(&t.den, left.den);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void mul_ratfun_poly(RatFun *res, RatFun left, Polynomial right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = polynomial;
     tc.u.poly = right;

     mul_coefficients(&t.num, left.num, tc);
     t.den.type = special;
     copy_coefficient(&t.den, left.den);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void div_ratfun_poly(RatFun *res, RatFun left, Polynomial right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = polynomial;
     tc.u.poly = right;

     mul_coefficients(&t.den, left.den, tc);
     t.num.type = special;
     copy_coefficient(&t.num, left.num);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;
}

void add_ratfun_rat(RatFun *res, RatFun left, BigRat right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = rational;
     tc.u.rat.den = make_bignum2(1);

     /* use t.num and t.den to hold temporary values */
     tc.u.rat.num = right.den;
     mul_coefficients(&t.num, left.num, tc);
     tc.u.rat.num = right.num;
     mul_coefficients(&t.den, left.den, tc);

     /* w = uv' + u'v */
     add_coefficients(&t.num, t.num, t.den);
     /* w' = u'v' */
     tc.u.rat.num = right.den;
     mul_coefficients(&t.den, left.den, tc);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;

     free_bignum(tc.u.rat.den);
}

void sub_ratfun_rat(RatFun *res, RatFun left, BigRat right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = rational;
     tc.u.rat.den = make_bignum2(1);

     /* use t.num and t.den to hold temporary values */
     tc.u.rat.num = right.den;
     mul_coefficients(&t.num, left.num, tc);
     tc.u.rat.num = right.num;
     mul_coefficients(&t.den, left.den, tc);

     /* w = uv' - u'v */
     sub_coefficients(&t.num, t.num, t.den);
     /* w' = u'v' */
     tc.u.rat.num = right.den;
     mul_coefficients(&t.den, left.den, tc);

     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;

     free_bignum(tc.u.rat.den);
}

void mul_ratfun_rat(RatFun *res, RatFun left, BigRat right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = rational;
     tc.u.rat.den = make_bignum2(1);

     tc.u.rat.num = right.num;
     mul_coefficients(&t.num, left.num, tc);
     tc.u.rat.num = right.den;
     mul_coefficients(&t.den, left.den, tc);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;

     free_bignum(tc.u.rat.den);
}

void div_ratfun_rat(RatFun *res, RatFun left, BigRat right)
{
     RatFun t;
     Coefficient tc;
     init_ratfun(&t);
     tc.type = rational;
     tc.u.rat.den = make_bignum2(1);

     tc.u.rat.num = right.den;
     mul_coefficients(&t.num, left.num, tc);
     tc.u.rat.num = right.num;
     mul_coefficients(&t.den, left.den, tc);
     canonicalise_ratfun(&t);

     free_ratfun(res);
     res->num = t.num;
     res->den = t.den;

     free_bignum(tc.u.rat.den);
}
