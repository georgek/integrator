/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

/* polynomials
 *
 * these are implemented as circular linked lists of monomials, the
 * list head points to a special monomial which is inclueded for
 * convenience
 *
 * the monomials consist of a degree and a coefficient which is either
 * a rational number of another polynomial (in a different variable */

#ifndef _POLYNOMIAL_H_
#define _POLYNOMIAL_H_

/* useful macro for printing polys */
#define PRINTP(p) printf(#p ": ");              \
     printf("\t\t");                            \
     print_poly(p);                             \
     printf("\n")


/* useful macro for printing coefficients */
#define PRINTC(c) printf(#c ": ");              \
     printf("\t\t");                            \
     print_coefficient(c);                      \
     printf("\t\t");                            \
     if ((c).type == rational)                  \
     printf("(rational)");                      \
     else if ((c).type == polynomial)           \
          printf("(polynomial)");               \
     printf("\n")

#include "bigrat.h"

typedef enum coef_types {
     special, polynomial, rational
} coef_types;

typedef struct polynomial *PolyPtr;

typedef struct coefficient *CoefPtr;

typedef struct monomial *MonoPtr;

typedef struct polynomial
{
     char variable;
     MonoPtr head;
} Polynomial;

typedef struct coefficient
{
     coef_types type;
     union {
          BigRat rat;
          Polynomial poly;
     } u;
} Coefficient;

typedef struct monomial
{
     int degree;
     Coefficient coeff;
     MonoPtr next;
} Monomial;

/* makes the special zero polynomial, which just contains the special
 * zero monomial */
Polynomial make_zero_poly(char variable);
Polynomial make_one_poly(char variable);
Polynomial make_const_poly(char variable, BigRat r);

/* makes a polynomial with one unit monomial */
Polynomial make_mono_poly(char variable, int degree);

void free_poly(Polynomial *p);

void copy_poly(Polynomial *p, Polynomial s);

/* for testing, not pretty */
/* void print_poly_simple(Polynomial p); */

/* prettier but not fully pretty */
/* void print_poly_nonpretty(Polynomial p); */

/* pretty printing */
void print_poly(Polynomial p);
void print_poly2(Polynomial p, int flip);
/* void print_poly3(Polynomial p); */
void print_poly_sign(Polynomial p, int flip);

/* LaTeX printing */
void print_poly_LaTeX(Polynomial p);
void print_poly_LaTeX2(Polynomial p, int flip);

void add_monomial(Polynomial* p, int degree, Coefficient coef);
void sub_monomial(Polynomial* p, int degree, Coefficient coef);

/* general coefficient functions */
/* non-pretty testing version */
/* void print_coefficient_simple(Coefficient c); */
/* prettier but not very pretty version, for now until the pretty version is
 * done properly */
/* void print_coefficient_nonpretty(Coefficient c); */
/* print coefficient with its sign if negative */
void print_coefficient(Coefficient c);
/* print coefficient without its sign */
void print_coefficient2(Coefficient c, int flip);
/* print coefficient with sign always */
/* void print_coefficient3(Coefficient c); */
/* print just sign */
int print_coef_sign(Coefficient c, int flip);

void print_coefficient_LaTeX(Coefficient c);
void print_coefficient_LaTeX2(Coefficient c, int flip);

void copy_coefficient(Coefficient *c, Coefficient s);

void free_coefficient(Coefficient *c);

int coef_zero(Coefficient c);
int coef_one(Coefficient c);
/* returns true for 1 or -1 */
int coef_one2(Coefficient c);

int coef_deg(Coefficient c, char var);

/* general coefficient arithmetic functions */
void negate_coefficient(Coefficient *c);
void add_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void mul_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void mul_coefficients2(Coefficient *res, Coefficient left, SHORT_INT_T right);
/* exact division of coefficients, always works for rationals, might not for
 * polys, fails if the division is not exact */
void exact_div_coefficients(Coefficient *res,
                            Coefficient left, Coefficient right);
/* mainly for dividing coefs by powers ie. in integration */
void div_coefficients2(Coefficient *res,
                       Coefficient left, SHORT_INT_T right);
/* Euclidean division with remainder, might not work for multivariate polys */
void polydiv_coefficients(Coefficient *q, Coefficient *r,
                          Coefficient left, Coefficient right);
/* pseudo-division with pseudo-remainder */
void pseudo_div_coefficients(Coefficient *q, Coefficient *r,
                             Coefficient left, Coefficient right);

void coef_power(Coefficient *res, Coefficient coef, SHORT_INT_T power);

void coef_gcd(Coefficient *res, Coefficient a, Coefficient b);

int coef_neg(Coefficient c);

BigRat coef_rat_part(Coefficient c);

/* reduces polynomial to rational if it is constant */
void coef_const_canonicalise(Coefficient *c);

void coef_differentiate(Coefficient *cd, Coefficient c, char var);
void coef_integrate(Coefficient *ci, Coefficient c, char var);

/* substitute solution */
void subst_var_coef(Coefficient *c, Coefficient sol, char var);

/* content and primitive part */
void coef_content(Coefficient *cont, Coefficient p, char var);
void coef_pp(Coefficient *pp, Coefficient p, char var);

/* leading coefficient */
const Coefficient coef_lc(Coefficient c, char var);
unsigned coef_num_monomials(Coefficient c);

/* polynomials */
int poly_zero(Polynomial p);
int poly_one(Polynomial p);
/* returns true for 1 or -1 */
int poly_one2(Polynomial p);

int poly_deg(Polynomial p);
int poly_deg2(Polynomial p, char var);
const Coefficient poly_lc(Polynomial p, char var);
unsigned poly_num_monomials(Polynomial p);

/* polynomial arithmetic */
void negate_polynomial(Polynomial *p);
void add_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void sub_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void mul_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A,
                     Polynomial B);
void pseudo_div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A,
                            Polynomial B);
void exact_div_polynomials(Polynomial *Q, Polynomial A, Polynomial B);

void add_poly_rat(Polynomial *res, Polynomial left, BigRat right);
void sub_poly_rat(Polynomial *res, Polynomial left, BigRat right);
void mul_poly_rat(Polynomial *res, Polynomial left, BigRat right);
void div_poly_rat(Polynomial *res, Polynomial left, BigRat right);

void poly_power(Polynomial *res, Polynomial p, SHORT_INT_T power);

/* more efficient way to add/sub when we don't want to keep the RHS poly */
void poly_splice_add(Polynomial *left, Polynomial *right);
void poly_splice_sub(Polynomial *left, Polynomial *right);

int poly_neg(Polynomial p);

/* content and primitive part */
void poly_content(Coefficient *cont, Polynomial p, char var);
void poly_pp(Polynomial *pp, Polynomial p, char var);

void poly_differentiate(Polynomial *pd, Polynomial p, char var);
void poly_integrate(Polynomial *pi, Polynomial p, char var);

/* solve linear poly */
void solve_linear_poly(Coefficient *sol, Polynomial p);
/* substitute solution */
void subst_var_poly(Polynomial *p, Coefficient sol, char var);

/* is poly univariate? */
int poly_univar(Polynomial p);

/* rational part of polynomial ie. lcm of coef denominators */
BigRat poly_rat_part(Polynomial p);

#endif /* _POLYNOMIAL_H_ */
