#include <stdio.h>

#include "tree.h"
#include "prs.h"

void SubResultant(Polynomial *res, CoefArray *prs, Polynomial A, Polynomial B)
{
     unsigned i = 0, j = 0, k = 0;
     int s = 0;
     CoefArray betas = new_coef_array();
     Polynomial Q = {'x', NULL}, R = {'x', NULL};
     Polynomial zero_poly = make_zero_poly(A.variable);
     Coefficient beta = {special}, gamma = {special}, gammat = {special};
     Coefficient r = {special}, rt = {special}, c = {special}, rest = {special};
     int delta = 0;

     if (A.variable != B.variable) {
          printf("Error! A and B have different variables! (PRS)\n");
          return;
     }

     rest.type = polynomial;
     rest.u.poly.head = NULL;

     ca_free(prs);
     *prs = new_coef_array();
     ca_poly_push_back(prs, A);
     ca_poly_push_back(prs, B);
     i = 1;
     /* gamma <-- -1 */
     gamma.type = rational;
     init_bigrat2(&gamma.u.rat, 1);
     negate_bigrat(&gamma.u.rat);
     
     delta = poly_deg(A)-poly_deg(B);

     /* beta <-- (-1)^(delta+1) */
     beta.type = rational;
     init_bigrat2(&beta.u.rat, 1);
     if (delta % 2 == 0) {
          negate_coefficient(&beta);
     }
     ca_poly_push_back(&betas, zero_poly);
     ca_push_back(&betas, beta);

     while (!coef_zero(ca_get(prs, i))) {
          copy_coefficient(&r, poly_lc(ca_get(prs, i).u.poly));
          pseudo_div_polynomials(&Q, &R, ca_get(prs, i-1).u.poly,
                                 ca_get(prs, i).u.poly);
          ca_poly_push_back(prs, R);
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
               coef_power(&gammat, gammat, -delta);
               exact_div_coefficients(&gamma, gammat, rt);
          }
          delta = poly_deg(ca_get(prs, i-1).u.poly) -
               poly_deg(ca_get(prs, i).u.poly);
          
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
     }
     k = i - 1;
     
     if (poly_deg(ca_get(prs, k).u.poly) > 0) {
          copy_poly(res, zero_poly);
          return;
     }
     if (poly_deg(ca_get(prs, k-1).u.poly) == 1) {
          copy_poly(res, ca_get(prs, k).u.poly);
          return;
     }

     copy_coefficient(&rest, ca_get(prs, k));
     coef_power(&rest, rest, poly_deg(ca_get(prs, k-1).u.poly));
     s = 1;
     c.type = rational;
     init_bigrat2(&c.u.rat, 1);
     for (j = 1; j < k; ++j) {
          if (poly_deg(ca_get(prs, j-1).u.poly) % 2 == 1
              && poly_deg(ca_get(prs, j).u.poly) % 2 == 1) {
               s = -s;
          }
          delta = poly_deg(ca_get(prs, j-1).u.poly)-
               poly_deg(ca_get(prs, j).u.poly);
          r = poly_lc(ca_get(prs, j).u.poly);
          coef_power(&rt, r, poly_deg(ca_get(prs, j-1).u.poly)-
                     poly_deg(ca_get(prs, j+1).u.poly));
          mul_coefficients(&rest, rest, rt);
          coef_power(&rt, r, delta + 1);
          coef_power(&rt, rt, poly_deg(ca_get(prs, j).u.poly));
          mul_coefficients(&rest, rest, rt);
          coef_power(&beta, ca_get(&betas, j), poly_deg(ca_get(prs, j).u.poly));
          mul_coefficients(&rest, rest, beta);
     }
     if (s == -1) {
          negate_coefficient(&rest);
     }
     copy_poly(res, rest.u.poly);

     ca_free(&betas);
     free_poly(&Q);
     free_poly(&R);
     free_poly(&zero_poly);
     free_coefficient(&beta);
     free_coefficient(&gamma);
     free_coefficient(&gammat);
     free_coefficient(&r);
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

     R.type = polynomial;
     R.u.poly.head = NULL;

     while (!coef_zero(b)) {
          copy_coefficient(&r, coef_lc(b));
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
     if (coef_neg(*gcd)) {
          negate_coefficient(gcd);
     }

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
     /* Polynomial res = {'x', NULL}; */
     /* BigNum res2 = NULL; */
     Coefficient res;
     
     if (root->type != op2_type || root->u.op2.operator != ',') {
          printf("Error. GCD requires two inputs.\n");
          return;
     }

     /* if (root->u.op2.operand1->type == coef_type */
     /*     && root->u.op2.operand2->type == coef_type) { */
     /*      /\* polynomials *\/ */
     /*      SubResultantGCD(&res, root->u.op2.operand1->u.coef, */
     /*                      root->u.op2.operand2->u.coef, 'x'); */
     /*      print_poly(res); */
     /*      printf("\n"); */
     /*      free_poly(&res); */
     /* } */
     /* else if (root->u.op2.operand1->type == rat_type */
     /*          && root->u.op2.operand2->type == rat_type */
     /*          && bn_one(root->u.op2.operand1->u.rat.den) */
     /*          && bn_one(root->u.op2.operand2->u.rat.den)) { */
     /*      /\* integers *\/ */
     /*      gcd(&res2, root->u.op2.operand1->u.rat.num, */
     /*          root->u.op2.operand2->u.rat.num); */
     /*      print_bignum(res2); */
     /*      printf("\n"); */
     /*      free_bignum(res2); */
     /*      res2 = NULL; */
     /* } */
     if (root->u.op2.operand1->type != coef_type
         || root->u.op2.operand2->type != coef_type) {
          printf("Error. Input to GCD should be two polynomials "
                 "or two integers.\n");
          return;
     }

     coef_gcd(&res, root->u.op2.operand1->u.coef,
              root->u.op2.operand2->u.coef);
     print_coefficient_nonpretty(res);
     printf("\n");
     free_coefficient(&res);
}
