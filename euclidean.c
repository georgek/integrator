#include <stdio.h>

#include "euclidean.h"
#include "polynomial.h"

void Euclidean(Polynomial *res, Polynomial a, Polynomial b)
{
     Polynomial na, nb, q, r;

     if (a.variable != b.variable) {
          printf("Warning! Polynomials have different variables (Euclidean)\n");
          return;
     }

     na = make_zero_poly(a.variable);
     nb = make_zero_poly(b.variable);
     q = make_zero_poly(a.variable);
     r = make_zero_poly(a.variable);
     copy_poly(&na, a);
     copy_poly(&nb, b);

     while (!poly_zero(nb)) {
          div_polynomials(&q, &r, na, nb);
          copy_poly(&na, nb);
          copy_poly(&nb, r);
     }

     copy_poly(res, na);

     free_poly(&na);
     free_poly(&nb);
     free_poly(&q);
     free_poly(&r);
}

/* "half/full" version */
void ExtendedEuclidean(Polynomial *g, Polynomial *s, Polynomial *t,
                       Polynomial a, Polynomial b)
{
     Polynomial temp;

     if (a.variable != b.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Extended Euclidean)\n");
          return;
     }

     temp = make_zero_poly(a.variable);

     HalfExtendedEuclidean(g, s, a, b);
     mul_polynomials(&temp, *s, a);
     sub_polynomials(&temp, *g, temp);
     div_polynomials(t, &temp, temp, b);

     free_poly(&temp);
}

void HalfExtendedEuclidean(Polynomial *g, Polynomial *s,
                           Polynomial a, Polynomial b)
{
     Polynomial a1, b1, r1, na, nb, q, r;

     if (a.variable != b.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Half Extended Euclidean)\n");
          return;
     }
     
     a1 = make_one_poly(a.variable);
     b1 = make_zero_poly(a.variable);
     r1 = make_zero_poly(a.variable);
     na = make_zero_poly(a.variable);
     nb = make_zero_poly(b.variable);
     q = make_zero_poly(a.variable);
     r = make_zero_poly(a.variable);
     copy_poly(&na, a);
     copy_poly(&nb, b);

     while (!poly_zero(nb)) {
          div_polynomials(&q, &r, na, nb);
          copy_poly(&na, nb);
          copy_poly(&nb, r);
          mul_polynomials(&r1, q, b1);
          sub_polynomials(&r1, a1, r1);
          copy_poly(&a1, b1);
          copy_poly(&b1, r1);
     }

     copy_poly(g, na);
     copy_poly(s, a1);

     free_poly(&a1);
     free_poly(&b1);
     free_poly(&r1);
     free_poly(&na);
     free_poly(&nb);
     free_poly(&q);
     free_poly(&r);
}

void SolveDiophantineEquation(Polynomial *s, Polynomial *t,
                              Polynomial a, Polynomial b, Polynomial c)
{
     Polynomial temp;

     if (a.variable != b.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Solve Diophantine Equation)\n");
          return;
     }

     temp = make_zero_poly(a.variable);
     
     HalfSolveDiophantineEquation(s, a, b, c);
     mul_polynomials(&temp, *s, a);
     sub_polynomials(&temp, c, temp);
     div_polynomials(t, &temp, temp, b);

     free_poly(&temp);
}

void HalfSolveDiophantineEquation(Polynomial *s,
                                  Polynomial a, Polynomial b, Polynomial c)
{
     Polynomial g, q, r;

     if (a.variable != b.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Extended Euclidean)");
          return;
     }

     g = make_zero_poly(a.variable);
     q = make_zero_poly(a.variable);
     r = make_zero_poly(a.variable);

     HalfExtendedEuclidean(&g, s, a, b);
     div_polynomials(&q, &r, c, g);

     if (!poly_zero(r)) {
          printf("Error! c is not an ideal generated by a and b!"
                 "(Solve Diophantine Equation)\n");
          return;
     }

     mul_polynomials(s, q, *s);
     if (!poly_zero(*s) && poly_deg(*s) >= poly_deg(b)) {
          div_polynomials(&q, &r, *s, b);
          copy_poly(s, r);
     }

     free_poly(&g);
     free_poly(&q);
     free_poly(&r);
}
