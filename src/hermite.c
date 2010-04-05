#define PRINTC(c) printf(#c ": ");              \
     print_coefficient(c);                      \
     printf("\n")
#define PRINTR(r) printf(#r ": ");              \
     print_ratfun(r);                           \
     printf("\n")

#include <stdio.h>

#include "euclidean.h"
#include "hermite.h"

void HermiteReduce(RatFun *g, RatFun *h, RatFun AD)
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
     PRINTC(A);
     copy_coefficient(&D, AD.den);
     PRINTC(D);

     coef_differentiate(&Dd, D);
     PRINTC(Dd);
     coef_gcd(&Dm, D, Dd);
     PRINTC(Dm);
     exact_div_coefficients(&Ds, D, Dm);
     PRINTC(Ds);

     while (coef_deg(Dm) > 0) {
          coef_differentiate(&Dmd, Dm);
          PRINTC(Dmd);
          coef_gcd(&Dm2, Dm, Dmd);
          PRINTC(Dm2);
          exact_div_coefficients(&Dms, Dm, Dm2);
          PRINTC(Dms);

          mul_coefficients(&mDsDmd, Ds, Dmd);
          exact_div_coefficients(&mDsDmd, mDsDmd, Dm);
          negate_coefficient(&mDsDmd);
          PRINTC(mDsDmd);

          PRINTC(Dms);
          PRINTC(A);
          SolveDiophantineEquation(&B, &C, mDsDmd, Dms, A);
          PRINTC(B);
          PRINTC(C);

          exact_div_coefficients(&A, Ds, Dms);
          coef_differentiate(&Bd, B);
          mul_coefficients(&A, A, Bd);
          sub_coefficients(&A, C, A);
          PRINTC(A);

          t.num = B;
          t.den = Dm;
          PRINTR(*g);
          PRINTR(t);
          add_ratfuns(g, *g, t);
          PRINTR(*g);

          copy_coefficient(&Dm, Dm2);
          PRINTC(Dm);
          printf("-----\n");
     }

     h->num = A;
     h->den = Ds;
     canonicalise_ratfun(h);

     free_ratfun(&old_g);
     free_ratfun(&old_h);
}

void HermiteReduceI(node_type *root)
{
     RatFun g, h;
     init_ratfun(&g);
     init_ratfun(&h);

     if (root->type != ratfun_type) {
          printf("Error! Hermite reduce requires a rational function.\n");
          return;
     }

     HermiteReduce(&g, &h, root->u.ratfun);

     printf("\n");
     print_ratfun(g);
     printf(" + int(");
     print_ratfun(h);
     printf(")\n");
}
