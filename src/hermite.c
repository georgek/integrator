/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

#include <stdio.h>

#include "euclidean.h"
#include "hermite.h"

#define WAIT while (getchar() != '\n')
#define PRINT(S) printf("\t"); printf(S)

void HermiteReduce(RatFun *g, RatFun *h, RatFun AD, char var, int trace)
{
     Coefficient A = {special}, D = {special}, Dd = {special}, Dm = {special};
     Coefficient Ds = {special}, Dmd = {special}, Dm2 = {special};
     Coefficient Dms = {special}, B = {polynomial}, Bd = {special};
     Coefficient C = {polynomial}, mDsDmd = {special};
     RatFun old_g = *g, old_h = *h, t = {{special}, {special}};

     g->num.type = rational;
     g->num.u.rat = make_bigrat3(0);
     g->den.type = rational;
     g->den.u.rat = make_bigrat3(1);
     init_ratfun(h);

     B.u.poly = make_zero_poly('x');
     C.u.poly = make_zero_poly('x');

     copy_coefficient(&A, AD.num);
     /* PRINTC(A); */
     copy_coefficient(&D, AD.den);
     /* PRINTC(D); */

     coef_differentiate(&Dd, D, var);
     /* PRINTC(Dd); */
     coef_gcd(&Dm, D, Dd);
     /* PRINTC(Dm); */
     exact_div_coefficients(&Ds, D, Dm);
     /* PRINTC(Ds); */

     while (coef_deg(Dm, var) > 0) {
          coef_differentiate(&Dmd, Dm, var);
          /* PRINTC(Dmd); */
          coef_gcd(&Dm2, Dm, Dmd);
          /* PRINTC(Dm2); */
          exact_div_coefficients(&Dms, Dm, Dm2);
          /* PRINTC(Dms); */

          mul_coefficients(&mDsDmd, Ds, Dmd);
          exact_div_coefficients(&mDsDmd, mDsDmd, Dm);
          negate_coefficient(&mDsDmd);
          /* PRINTC(mDsDmd); */

          /* PRINTC(Dms); */
          /* PRINTC(A); */
          if (trace) {
               PRINT("Solving diophantine equation:\t");
               printf("S * ");
               print_coefficient(mDsDmd);
               printf(" + T * ");
               print_coefficient(Dms);
               printf(" = ");
               print_coefficient(A);
               printf("\n\n");
          }

          SolveDiophantineEquation(&B, &C, mDsDmd, Dms, A);

          if (trace) {
               PRINT("Solution:\t\t\t");
               printf("S = ");
               print_coefficient(B);
               printf(",\tT = ");
               print_coefficient(C);
               printf("\n");
               WAIT;
          }
          
          /* PRINTC(B); */
          /* PRINTC(C); */

          /* PRINTC(Ds); */
          /* PRINTC(Dms); */
          exact_div_coefficients(&A, Ds, Dms);
          /* PRINTC(A); */
          /* PRINTC(B); */
          coef_differentiate(&Bd, B, var);
          /* PRINTC(Bd); */
          mul_coefficients(&A, A, Bd);
          sub_coefficients(&A, C, A);
          /* PRINTC(A); */

          t.num = B;
          t.den = Dm;
          /* PRINTR(*g); */
          /* PRINTR(t); */
          add_ratfuns(g, *g, t);
          /* PRINTR(*g); */

          copy_coefficient(&Dm, Dm2);
          /* PRINTC(Dm); */
          /* printf("-----\n"); */
     }

     h->num = A;
     h->den = Ds;
     canonicalise_ratfun(h);

     free_ratfun(&old_g);
     free_ratfun(&old_h);
}

void HermiteReduceI(node_type *root, char var)
{
     RatFun g, h;
     init_ratfun(&g);
     init_ratfun(&h);

     if (root->type != ratfun_type) {
          printf("Error! Hermite reduce requires a rational function.\n");
          return;
     }

     HermiteReduce(&g, &h, root->u.ratfun, var, 0);

     printf("\n");
     print_ratfun(g);
     printf(" + int(");
     print_ratfun(h);
     printf(")\n");
}
