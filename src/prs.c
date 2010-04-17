#include <stdio.h>

#include "coef_array.h"
#include "tree.h"
#include "prs.h"

void SubResultant(Coefficient *res, CoefArray *prs,
                  Coefficient A, Coefficient B, char var)
{
     unsigned i = 0, j = 0, k = 0;
     int s = 0;
     CoefArray betas = new_coef_array();
     Coefficient Q = {special}, R = {special};
     Coefficient zero = {rational};
     Coefficient beta = {special}, gamma = {special}, gammat = {special};
     Coefficient r = {special}, rt = {special}, c = {special}, rest = {special};
     int delta = 0;

     /* if (A.variable != B.variable) { */
     /*      printf("Error! A and B have different variables! (PRS)\n"); */
     /*      return; */
     /* } */

     zero.u.rat = make_bigrat3(0);

     rest.type = special;

     ca_free(prs);
     *prs = new_coef_array();
     ca_push_back(prs, A);
     ca_push_back(prs, B);
     i = 1;
     /* gamma <-- -1 */
     gamma.type = rational;
     gamma.u.rat = make_bigrat3(1);
     negate_coefficient(&gamma);
     
     delta = coef_deg(A, var)-coef_deg(B, var);

     /* beta <-- (-1)^(delta+1) */
     beta.type = rational;
     init_bigrat2(&beta.u.rat, 1);
     if (delta % 2 == 0) {
          negate_coefficient(&beta);
     }
     ca_push_back(&betas, zero);
     ca_push_back(&betas, beta);

     while (!coef_zero(ca_get(prs, i))) {
          /* printf("***** i: %d *****\n", i); */
          /* PRINTC(ca_get(prs, i)); */
          /* PRINTC(gamma); */
          
          copy_coefficient(&r, coef_lc(ca_get(prs, i), var));
          pseudo_div_coefficients(&Q, &R, ca_get(prs, i-1), ca_get(prs, i));
          ca_push_back(prs, R);
          exact_div_coefficients(prs->head+i+1, *(prs->head+i+1),
                                 ca_get(&betas, i));
          ++i;
          copy_coefficient(&rt, r);
          negate_coefficient(&rt);
          copy_coefficient(&gammat, gamma);
          
          if (delta > 0) {
               coef_power(&rt, rt, delta);
               coef_power(&gammat, gammat, delta-1);
               exact_div_coefficients(&gamma, rt, gammat);
          }
          else {
               coef_power(&rt, rt, -delta);
               coef_power(&gammat, gammat, 1-delta);
               exact_div_coefficients(&gamma, gammat, rt);
          }
          delta = coef_deg(ca_get(prs, i-1), var) -
               coef_deg(ca_get(prs, i), var);
          
          copy_coefficient(&beta, r);
          negate_coefficient(&beta);
          if (delta > 0) {
               coef_power(&gammat, gamma, delta);
               mul_coefficients(&beta, beta, gammat);
          }
          else if (delta < 0) {
               coef_power(&gammat, gamma, -delta);
               exact_div_coefficients(&beta, beta, gammat);
          }
          ca_push_back(&betas, beta);

          /* printf("delta: %d\n", delta); */
          /* PRINTC(ca_get(&betas, i-1)); */
          /* PRINTC(r); */
     }
     k = i - 1;
     
     if (coef_deg(ca_get(prs, k), var) > 0) {
          copy_coefficient(res, zero);
          return;
     }
     if (coef_deg(ca_get(prs, k-1), var) == 1) {
          copy_coefficient(res, ca_get(prs, k));
          return;
     }

     copy_coefficient(&rest, ca_get(prs, k));
     coef_power(&rest, rest, coef_deg(ca_get(prs, k-1), var));
     s = 1;
     c.type = rational;
     c.u.rat = make_bigrat3(1);
     for (j = 1; j < k; ++j) {
          if (coef_deg(ca_get(prs, j-1), var) % 2 == 1
              && coef_deg(ca_get(prs, j), var) % 2 == 1) {
               s = -s;
          }
          delta = coef_deg(ca_get(prs, j-1), var)-
               coef_deg(ca_get(prs, j), var);
          r = coef_lc(ca_get(prs, j), var);
          coef_power(&rt, r, coef_deg(ca_get(prs, j-1), var)-
                     coef_deg(ca_get(prs, j+1), var));
          mul_coefficients(&rest, rest, rt);
          coef_power(&rt, r, delta + 1);
          coef_power(&rt, rt, coef_deg(ca_get(prs, j), var));
          mul_coefficients(&rest, rest, rt);
          coef_power(&beta, ca_get(&betas, j), coef_deg(ca_get(prs, j), var));
          mul_coefficients(&rest, rest, beta);
     }

     copy_coefficient(res, rest);
     mul_coefficients(res, *res, c);
     if (s == -1) {
          negate_coefficient(res);
     }

     ca_free(&betas);
     free_coefficient(&Q);
     free_coefficient(&R);
     free_coefficient(&zero);
     free_coefficient(&beta);
     free_coefficient(&gamma);
     free_coefficient(&gammat);
     free_coefficient(&rt);
     free_coefficient(&c);
     free_coefficient(&rest);
}

void SubResultantGCD(Coefficient *gcd, Coefficient A, Coefficient B, char var)
{
     Coefficient Q = {special}, a = {special}, b = {special};
     Coefficient beta = {special}, gamma = {special}, gammat = {special};
     Coefficient R = {special}, r = {special}, rt = {special}, c = {special};
     int delta = 0;

     /* if (A.variable != B.variable) { */
     /*      printf("Error! A and B have different variables! (PRS)\n"); */
     /*      return; */
     /* } */

     /* printf("*** begin subresultant PRS ***, var: %c\n", var); */

     copy_coefficient(&a, A);
     copy_coefficient(&b, B);

     /* gamma <-- -1 */
     gamma.type = rational;
     init_bigrat2(&gamma.u.rat, 1);
     negate_bigrat(&gamma.u.rat);
     
     delta = coef_deg(A, var)-coef_deg(B, var);

     /* beta <-- (-1)^(delta+1) */
     beta.type = rational;
     init_bigrat2(&beta.u.rat, 1);
     if (delta % 2 == 0) {
          negate_coefficient(&beta);
     }

     while (!coef_zero(b)) {
          copy_coefficient(&r, coef_lc(b, var));
          /* PRINTC(a); */
          /* PRINTC(b); */
          pseudo_div_coefficients(&Q, &R, a, b);
          /* PRINTC(Q); */
          /* PRINTC(R); */

          exact_div_coefficients(&R, R, beta);
          copy_coefficient(&a, b);
          copy_coefficient(&b, R);
          if (coef_zero(b)) {
               break;
          }

          copy_coefficient(&rt, r);
          negate_coefficient(&rt);
          copy_coefficient(&gammat, gamma);
          if (delta > 0) {
               coef_power(&rt, rt, delta);
               coef_power(&gammat, gammat, delta-1);
               exact_div_coefficients(&gamma, rt, gammat);
               /* PRINTC(rt); */
               /* PRINTC(gammat); */
          }
          else {
               coef_power(&rt, rt, -delta);
               coef_power(&gammat, gammat, -delta);
               exact_div_coefficients(&gamma, gammat, rt);
          }
          /* PRINTC(a); */
          /* PRINTC(b); */
          delta = coef_deg(a, var) - coef_deg(b, var);
          /* printf("delta: %d\n",delta); */
          
          copy_coefficient(&beta, r);
          negate_coefficient(&beta);
          if (delta > 0) {
               coef_power(&gammat, gamma, delta);
               mul_coefficients(&beta, beta, gammat);
          }
          else if (delta < 0) {
               coef_power(&gammat, gamma, -delta);
               /* PRINTC(beta); */
               /* PRINTC(gammat); */
               exact_div_coefficients(&beta, beta, gammat);
               /* PRINTC(beta); */
          }
     }

     copy_coefficient(gcd, a);

     /* make primitive */
     /* printf("*** the answer:\n"); */
     /* PRINTC(*gcd); */
     coef_pp(gcd, *gcd, var);
     /* PRINTC(*gcd); */
     /* take the positive gcd */
     /* if (coef_neg(*gcd)) { */
     /*      negate_coefficient(gcd); */
     /* } */

     /* printf("*** end subresultant PRS ***\n"); */

     free_coefficient(&Q);
     free_coefficient(&beta);
     free_coefficient(&gamma);
     free_coefficient(&gammat);
     free_coefficient(&r);
     free_coefficient(&rt);
     free_coefficient(&c);
}

void GCDI(node_type *root)
{
     Coefficient res = {special};
     
     if (root->type != op2_type || root->u.op2.operator != ',') {
          printf("Error. GCD requires two inputs.\n");
          return;
     }

     if (root->u.op2.operand1->type != coef_type
         || root->u.op2.operand2->type != coef_type) {
          printf("Error. Input to GCD should be polynomials "
                 "or integers.\n");
          return;
     }

     coef_gcd(&res, root->u.op2.operand1->u.coef,
              root->u.op2.operand2->u.coef);
     print_coefficient(res);
     printf("\n");
     free_coefficient(&res);
}

void SubResultantPRSI(node_type *root, char var)
{
     Coefficient res = {special};
     CoefArray prs = {0,0,NULL};
     int i;
     
     if (root->type != op2_type || root->u.op2.operator != ',') {
          printf("Error. PRS requires two inputs.\n");
          return;
     }

     if (root->u.op2.operand1->type != coef_type
         || root->u.op2.operand2->type != coef_type) {
          printf("Error. Input to PRS should be polynomials "
                 "or integers.\n");
          return;
     }

     prs = new_coef_array();

     SubResultant(&res, &prs,
                  root->u.op2.operand1->u.coef,
                  root->u.op2.operand2->u.coef,
                  var);
     printf("Resultant: ");
     print_coefficient(res);
     printf("\n");
     printf("PRS:\n");
     for (i = 0; i < prs.size; ++i) {
          printf("R_%d: ", i);
          print_coefficient(ca_get(&prs, i)); printf("\n");
     }

     printf("\n");
     free_coefficient(&res);
     ca_free(&prs);
}
