#include <stdlib.h>
#include <stdio.h>

#include "lrt.h"
#include "polynomial.h"
#include "coef_array.h"
#include "prs.h"
#include "squarefree.h"
#include "ratfun.h"

#define WAIT while (getchar() != '\n')
#define PRINT(S) printf("\t"); printf(S)

void IntRationalLogPart(CoefArray *Qi, CoefArray *Si,
                        Coefficient A, Coefficient D,
                        char var, char newvar,
                        int trace)
{
     Coefficient t = {polynomial}, Qit = {special};
     Coefficient R = {special}, AtDd = {special}, AjQi = {special};
     CoefArray Ri = new_coef_array(), Ai = new_coef_array();
     CoefPtr St = NULL;

     unsigned i, m;

     t.u.poly = make_mono_poly(newvar, 1);

     /* (R, (R_0, ..., R_k, 0)) <- SubResultant_x(D, A-t(dD/dx)) */
     /* AtDd = A - t*(dD/dx) */
     coef_differentiate(&AtDd, D, var);
     mul_coefficients(&AtDd, AtDd, t);
     sub_coefficients(&AtDd, A, AtDd);

     if (trace) {
          PRINT("Calculating resultant and subresultant PRS of:\n");
          PRINT("D = \t\t\t");
          print_coefficient(D);
          printf("\n");
          PRINT("A - t*(dD/dx) = \t");
          print_coefficient(AtDd);
          printf("\n");
          PRINT("with respect to"); printf(" %c\n", var);
          PRINT("(Rothstein-Trager resultant)\n");

          WAIT;
     }

     SubResultant(&R, &Ri, D, AtDd, var);
     /* PRINTC(AtDd); */
     /* PRINTC(R); */

     if (trace) {
          unsigned j;
          
          PRINT("Resultant:\t\t");
          print_coefficient(R);
          printf("\n");
          PRINT("Subresultant PRS:\n");
          for (j = 0; j < Ri.size; ++j) {
               printf("\t\t\t\t");
               print_coefficient(ca_get(&Ri, j));
               printf("\n");
          }

          WAIT;

          PRINT("Now we take the squarefree factorisation of the resultant.\n");

          WAIT;
     }

     /* (Q_1, ..., Q_n) <- Squarefree(R) */
     Squarefree(Qi, R, newvar);

     if (trace) {
          unsigned j;

          printf("\t\t");
          print_coefficient(R);
          printf(" =\t");
          for (j = 0; j < Qi->size; ++j) {
               if (coef_one(ca_get(Qi, j))) {
                    continue;
               }
               print_coefficient(ca_get(Qi, j));
               if (i > 0) {
                    printf("^%u", j+1);
               }
               if (j != Qi->size-1) {
                    printf(" * ");
               }
               else {
                    printf("\n");
               }
          }

          WAIT;
     }

     /* for i <- 1 to n such that deg(Qi, t) > 0 */
     for (i = 1; i <= Qi->size; ++i) {
          if (coef_deg(ca_get(Qi, i-1), newvar) == 0) {
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
                    coef_pp(St, *St, var);
                    break;
               }
          }

          /* PRINTC(*St); */
          /* Squarefree(&Ai, coef_lc(*St, var), newvar); */

          /* for (j = 1; j <= Ai.size; ++j) { */
          /*      /\* PRINTC(ca_get(&Ai, j-1)); *\/ */
          /*      /\* PRINTC(ca_get(Qi, i-1)); *\/ */
          /*      coef_gcd(&AjQi, ca_get(&Ai, j-1), ca_get(Qi, i-1)); */
          /*      coef_power(&AjQi, AjQi, j); */
          /*      /\* PRINTC(*St); *\/ */
          /*      /\* PRINTC(AjQi); *\/ */
          /*      exact_div_coefficients(St, *St, AjQi); */
          /* } */
     }

     /* remove unused Qis */
     for (i = 0; i < Qi->size; ++i) {
          if (coef_deg(ca_get(Qi, i), newvar) > 0) {
               continue;
          }
          /* remove */
          Qit = ca_remove(Qi, i);
          free_coefficient(&Qit);
          --i;
     }

     free_coefficient(&t);
     free_coefficient(&R);
     free_coefficient(&AtDd);
     free_coefficient(&AjQi);
     ca_free(&Ri);
     ca_free(&Ai);
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
                        var, newvar, 0);

     for (i = 0; i < Qi.size; ++i) {
          printf("Q_%d: ", i);
          print_coefficient(ca_get(&Qi, i));
          printf("\n");
     }
     for (i = 0; i < Si.size; ++i) {
          printf("S_%d: ", i);
          print_coefficient(ca_get(&Si, i));
          printf("\n");
     }
}
