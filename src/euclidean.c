#include <stdio.h>

#include "euclidean.h"
#include "polynomial.h"

void Euclidean(Coefficient *res, Coefficient a, Coefficient b)
{
     Polynomial na, nb, q, r;
     Coefficient old_res = *res;

     if (b.type == rational) {
          copy_coefficient(res, b);
          return;
     }
     else if (a.type == rational) {
          copy_coefficient(res, a);
     }

     if (a.u.poly.variable != b.u.poly.variable) {
          printf("Warning! Polynomials have different variables (Euclidean)\n");
          return;
     }

     na = make_zero_poly(a.u.poly.variable);
     nb = make_zero_poly(b.u.poly.variable);
     q = make_zero_poly(a.u.poly.variable);
     r = make_zero_poly(a.u.poly.variable);
     copy_poly(&na, a.u.poly);
     copy_poly(&nb, b.u.poly);

     while (!poly_zero(nb)) {
          div_polynomials(&q, &r, na, nb);
          copy_poly(&na, nb);
          copy_poly(&nb, r);
     }

     res->type = polynomial;
     res->u.poly.head = NULL;
     copy_poly(&res->u.poly, na);

     free_poly(&na);
     free_poly(&nb);
     free_poly(&q);
     free_poly(&r);
     free_coefficient(&old_res);
}

/* "half/full" version */
void ExtendedEuclidean(Coefficient *g, Coefficient *s, Coefficient *t,
                       Coefficient a, Coefficient b)
{
     Polynomial temp;
     Coefficient old_g = *g, old_s = *s, old_t = *t;

     if (b.type == rational) {
          copy_coefficient(g, b);
          free_coefficient(s);
          s->type = rational;
          s->u.rat = make_bigrat3(0);
          free_coefficient(t);
          t->type = rational;
          t->u.rat = make_bigrat3(1);
          return;
     }
     else if (a.type == rational) {
          copy_coefficient(g, a);
          free_coefficient(s);
          s->type = rational;
          s->u.rat = make_bigrat3(1);
          free_coefficient(t);
          t->type = rational;
          t->u.rat = make_bigrat3(0);
     }

     if (a.u.poly.variable != b.u.poly.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Extended Euclidean)\n");
          return;
     }

     temp = make_zero_poly(a.u.poly.variable);

     g->type = polynomial;
     g->u.poly.head = NULL;
     s->type = polynomial;
     s->u.poly.head = NULL;
     t->type = polynomial;
     t->u.poly.head = NULL;

     HalfExtendedEuclidean(g, s, a, b);
     mul_polynomials(&temp, s->u.poly, a.u.poly);
     sub_polynomials(&temp, g->u.poly, temp);
     div_polynomials(&t->u.poly, &temp, temp, b.u.poly);

     free_poly(&temp);
     free_coefficient(&old_g);
     free_coefficient(&old_s);
     free_coefficient(&old_t);
}

void HalfExtendedEuclidean(Coefficient *g, Coefficient *s,
                           Coefficient a, Coefficient b)
{
     Coefficient a1 = {rational}, b1 = {rational}, r1 = {special};
     Coefficient na = {special}, nb = {special}, q = {special}, r = {special};

     if (b.type == rational) {
          copy_coefficient(g, b);
          free_coefficient(s);
          s->type = rational;
          s->u.rat = make_bigrat3(0);
          return;
     }
     else if (a.type == rational) {
          copy_coefficient(g, a);
          free_coefficient(s);
          s->type = rational;
          s->u.rat = make_bigrat3(1);
     }

     if (a.u.poly.variable != b.u.poly.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Extended Euclidean)\n");
          return;
     }

     copy_coefficient(&na, a);
     copy_coefficient(&nb, b);

     a1.u.rat = make_bigrat3(1);
     b1.u.rat = make_bigrat3(0);

     while (!coef_zero(nb)) {
          polydiv_coefficients(&q, &r, na, nb);
          copy_coefficient(&na, nb);
          copy_coefficient(&nb, r);
          mul_coefficients(&r1, q, b1);
          sub_coefficients(&r1, a1, r1);
          copy_coefficient(&a1, b1);
          copy_coefficient(&b1, r1);
     }

     copy_coefficient(g, na);
     copy_coefficient(s, a1);

     free_coefficient(&a1);
     free_coefficient(&b1);
     free_coefficient(&r1);
     free_coefficient(&na);
     free_coefficient(&nb);
     free_coefficient(&q);
     free_coefficient(&r);
}

void SolveDiophantineEquation(Coefficient *s, Coefficient *t,
                              Coefficient a, Coefficient b, Coefficient c)
{
     Coefficient temp = {special};

     HalfSolveDiophantineEquation(s, a, b, c);
     /* printf("diophantine:\n"); */
     /* PRINTC(a); */
     /* PRINTC(b); */
     /* PRINTC(c); */
     /* PRINTC(*s); */
     mul_coefficients(&temp, *s, a);
     /* PRINTC(temp); */
     sub_coefficients(&temp, c, temp);
     /* PRINTC(temp); */
     exact_div_coefficients(t, temp, b);
     /* PRINTC(*t); */
     /* printf("end dio\n"); */

     free_coefficient(&temp);
}

void HalfSolveDiophantineEquation(Coefficient *s,
                                  Coefficient a, Coefficient b, Coefficient c)
{
     Coefficient g, q, r;
     Coefficient old_s = *s;

     if (b.type == rational) {
          free_coefficient(s);
          s->type = rational;
          s->u.rat = make_bigrat3(0);
          return;
     }
     else if (a.type == rational) {
          exact_div_coefficients(s, c, a);
          return;
     }

     if (a.u.poly.variable != b.u.poly.variable) {
          printf("Warning! Polynomials have different variables"
                 "(Extended Euclidean)\n");
          return;
     }

     g.type = special;
     q.type = special;
     r.type = special;

     s->type = polynomial;
     s->u.poly.head = NULL;

     HalfExtendedEuclidean(&g, s, a, b);
     polydiv_coefficients(&q, &r, c, g);

     if (!coef_zero(r)) {
          printf("Error! c is not in the ideal generated by a and b!"
                 "(Solve Diophantine Equation)\n");
          return;
     }

     mul_coefficients(s, q, *s);
     if (!coef_zero(*s) && coef_deg(*s, 'x') >= coef_deg(b, 'x')) { /* TODO vars */
          polydiv_coefficients(&q, &r, *s, b);
          copy_coefficient(s, r);
     }

     free_coefficient(&g);
     free_coefficient(&q);
     free_coefficient(&r);
     free_coefficient(&old_s);
}
