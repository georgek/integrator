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

#include "bigrat.h"

typedef enum coef_types {
     special, polynomial, rational
} coef_types;

typedef struct polynomial *PolyPtr;

typedef struct monomial *MonoPtr;

typedef struct coefficient
{
     coef_types type;
     union {
          BigRat rat;
          PolyPtr poly;
     } u;
} Coefficient;

typedef struct polynomial
{
     char variable;
     MonoPtr head;
} Polynomial;

typedef struct monomial
{
     int degree;
     Coefficient coeff;
     MonoPtr next;
} Monomial;

/* makes the special zero polynomial, which just contains the special
 * zero monomial */
Polynomial make_zero_poly(char variable);

void free_poly(Polynomial *p);

void copy_poly(Polynomial *p, Polynomial s);

void print_poly(Polynomial p);

void add_monomial(Polynomial* p, int degree, Coefficient coef);
void sub_monomial(Polynomial* p, int degree, Coefficient coef);

/* general coefficient functions */
void print_coefficient(Coefficient c);

void copy_coefficient(Coefficient *c, Coefficient s);

void free_coefficient(Coefficient *c);

int coef_zero(Coefficient c);
int coef_one(Coefficient c);

/* general coefficient arithmetic functions */
void add_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void mul_coefficients(Coefficient *res, Coefficient left, Coefficient right);
void mul_coefficients2(Coefficient *res, Coefficient left, SHORT_INT_T right);
void div_coefficients(Coefficient *res, Coefficient left, Coefficient right);

/* polynomials */
int poly_zero(Polynomial p);

int poly_deg(Polynomial p);
const Coefficient poly_lc(Polynomial p);

/* polynomial arithmetic */
void add_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void sub_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void mul_polynomials(Polynomial *res, Polynomial left, Polynomial right);
void div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A, Polynomial B);

void poly_differentiate(Polynomial *pd, Polynomial p);

#endif /* _POLYNOMIAL_H_ */
