#include <stdlib.h>
#include <stdio.h>

#include "lrt.h"
#include "polynomial.h"
#include "coef_array.h"
#include "prs.h"
#include "squarefree.h"
#include "ratfun.h"

void IntRationalLogPart(CoefArray *Qi, CoefArray *Si,
                        Coefficient A, Coefficient D,
                        char var, char newvar)
{
     Coefficient t = {polynomial};
     Coefficient R = {special}, AtDd = {special}, AjQi = {special};
     CoefArray Ri = new_coef_array(), Ai = new_coef_array();
     CoefPtr St = NULL;

     unsigned i, j, m;

     t.u.poly = make_mono_poly(newvar, 1);

     /* (R, (R_0, ..., R_k, 0)) <- SubResultant_x(D, A-t(dD/dx)) */
     /* AtDd = A - t*(dD/dx) */
     coef_differentiate(&AtDd, D, var);
     mul_coefficients(&AtDd, AtDd, t);
     sub_coefficients(&AtDd, A, AtDd);
     SubResultant(&R, &Ri, D, AtDd, var);
     /* PRINTC(AtDd); */
     /* PRINTC(R); */

     /* (Q_1, ..., Q_n) <- Squarefree(R) */
     Squarefree(Qi, R, newvar);

     /* for i <- 1 to n such that deg(Qi, t) > 0 */
     for (i = 1; i <= Qi->size; ++i) {
          if (coef_deg(ca_get(Qi, i-1), newvar) <= 0) {
               continue;
          }

          if (i == coef_deg(D, var)) {
               ca_push_back(Si, D);
               continue;
          }

          /* find R_m where deg_x(R_m) = i */
          for (m = 1; m <= Ri.size - 1; ++m) {
               if (coef_deg(ca_get(&Ri, m), var) == i) {
                    St = ca_push_back(Si, ca_get(&Ri, m));
                    break;
               }
          }

          Squarefree(&Ai, coef_lc(*St, var), newvar);

          for (j = 1; j <= Ai.size; ++j) {
               coef_gcd(&AjQi, ca_get(&Ai, j-1), ca_get(Qi, i-1));
               coef_power(&AjQi, AjQi, j);
               /* PRINTC(AjQi); */
               exact_div_coefficients(St, *St, AjQi);
          }

          /* exact_div_coefficients(St, *St, ca_get(Qi, i-1)); */
          coef_pp(St, *St, var);
     }
}

void IntRationalLogPartI(node_type *root, char var, char newvar)
{
     CoefArray Qi = new_coef_array(), Si = new_coef_array();
     unsigned i;
     
     if (root->type != ratfun_type) {
          printf("Error! LRT algorithm requires a rational function.\n");
          return;
     }

     IntRationalLogPart(&Qi, &Si, root->u.ratfun.num, root->u.ratfun.den,
                        var, newvar);

     for (i = 0; i < Qi.size; ++i) {
          printf("Q_%d: ", i);
          print_coefficient_nonpretty(ca_get(&Qi, i));
          printf("\n");
     }
     for (i = 0; i < Si.size; ++i) {
          printf("S_%d: ", i);
          print_coefficient_nonpretty(ca_get(&Si, i));
          printf("\n");
     }
}
