#include <stdio.h>

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
          r = poly_lc(ca_get(prs, i).u.poly);
          pseudo_div_polynomials(&Q, &R, ca_get(prs, i-1).u.poly,
                                 ca_get(prs, i).u.poly);
          ca_poly_push_back(prs, R);
          div_coefficients(prs->head+i+1, *(prs->head+i+1),
                           ca_get(&betas, i));
          ++i;
          copy_coefficient(&rt, r);
          negate_coefficient(&rt);
          copy_coefficient(&gammat, gamma);
          if (delta > 0) {
               coef_power(&rt, rt, delta);
               coef_power(&gammat, gammat, delta-1);
               div_coefficients(&gamma, rt, gammat);
          }
          else {
               coef_power(&rt, rt, -delta);
               coef_power(&gammat, gammat, -delta);
               div_coefficients(&gamma, gammat, rt);
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
               div_coefficients(&beta, beta, gammat);
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

