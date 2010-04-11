#include <stdio.h>

#include "integration.h"
#include "polynomial.h"
#include "hermite.h"
#include "lrt.h"

void IntegrateRationalFunction(node_type *root, char var, char newvar)
{
     RatFun g, h;
     Coefficient Q = {special}, R = {special};
     CoefArray Qi = new_coef_array(), Si = new_coef_array();
     unsigned i, j = 0;
     
     init_ratfun(&g);
     init_ratfun(&h);

     if (root->type != ratfun_type) {
          printf("Error! IntegrateRationalFunction"
                 "requires a rational function.\n");
          return;
     }

     HermiteReduce(&g, &h, root->u.ratfun, var);
     print_ratfun(g);
     printf(" +\n");

     polydiv_coefficients(&Q, &R, h.num, h.den);
     
     coef_integrate(&Q, Q, var);
     print_coefficient_nonpretty(Q);

     if (!coef_zero(R)) {
          printf(" +\n");
          IntRationalLogPart(&Qi, &Si, R, h.den, var, newvar);

          for (i = 0; i < Qi.size; ++i) {
               if (coef_zero(ca_get(&Qi, i))) {
                    continue;
               }
               printf("(");

               printf("sum(%c*ln(", newvar);
               print_coefficient_nonpretty(ca_get(&Si, j++));
               printf("), %c = rootsOf(", newvar);
               print_coefficient_nonpretty(ca_get(&Qi, i));
               printf(" = 0))\n");

               /* printf(")\n"); */
               
               /* printf("Q_%d: ", i); */
               /* print_coefficient_nonpretty(ca_get(&Qi, i)); */
               /* printf("\n"); */
          }
          /* for (i = 0; i < Si.size; ++i) { */
          /*      printf("S_%d: ", i); */
          /*      print_coefficient_nonpretty(ca_get(&Si, i)); */
          /*      printf("\n"); */
          /* } */
     }
     printf("\n");
}
