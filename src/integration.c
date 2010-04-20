#include <stdio.h>

#include "integration.h"
#include "polynomial.h"
#include "hermite.h"
#include "lrt.h"

/* TODO input to hermite must have a primitive denominator so we should remove
 * the content from the rational and/or polynomial part from the rational
 * function first */
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
     print_coefficient(Q);

     if (!coef_zero(R)) {
          printf(" +\n");
          IntRationalLogPart(&Qi, &Si, R, h.den, var, newvar);

          for (i = 0; i < Qi.size; ++i) {
               if (coef_deg(ca_get(&Qi, i), newvar) == 0) {
                    continue;
               }
               printf("(");

               printf("sum(%c*ln(", newvar);
               print_coefficient(ca_get(&Si, j++));
               printf("), %c = rootsOf(", newvar);
               print_coefficient(ca_get(&Qi, i));
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

void IntegrateRationalFunctionLaTeX(node_type *root, char var, char newvar)
{
     RatFun A, g, h, content;
     Coefficient Q = {special}, R = {special};
     CoefArray Qi = new_coef_array(), Si = new_coef_array();
     unsigned i, j = 0;
     int plus = 0;

     init_ratfun(&A);
     init_ratfun(&g);
     init_ratfun(&h);
     init_ratfun(&content);

     if (root->type != ratfun_type) {
          printf("Error! IntegrateRationalFunction"
                 "requires a rational function.\n");
          return;
     }

     copy_ratfun(&A, root->u.ratfun);

     printf("\\int \\! ");
     print_ratfun_LaTeX(A);
     printf("\\, d%c = ", var);

     if (ratfun_zero(A)) {
          printf("0\n");
          return;
     }
     
     /* make numerator and denominator primitive */
     coef_content(&content.num, A.num, var);
     exact_div_coefficients(&A.num, A.num, content.num);
     coef_content(&content.den, A.den, var);
     exact_div_coefficients(&A.den, A.den, content.den);

     HermiteReduce(&g, &h, A, var);
     if (!ratfun_zero(g)){
          print_ratfun_LaTeX(g);
          plus = 1;
     }

     /* put contents back on */
     /* PRINTR(content); */
     mul_ratfuns(&g, g, content);
     mul_ratfuns(&h, h, content);
     /* PRINTR(h); */

     polydiv_coefficients(&Q, &R, h.num, h.den);

     coef_integrate(&Q, Q, var);

     if (!coef_zero(Q)) {
          if (plus) {
               printf(" + ");
          }
          print_coefficient_LaTeX(Q);
          plus = 1;
     }

     if (!coef_zero(R)) {
          IntRationalLogPart(&Qi, &Si, R, h.den, var, newvar);

          for (i = 0; i < Qi.size; ++i) {
               if (coef_deg(ca_get(&Qi, i), newvar) == 0) {
                    continue;
               }
               if (plus) {
                    printf(" + ");
               }
               printf("\\sum_{%c | ", newvar);
               print_coefficient_LaTeX(ca_get(&Qi, i));
               printf(" = 0} %c \\ln(", newvar);
               print_coefficient_LaTeX(ca_get(&Si, j++));
               printf(")");
               plus = 1;
          }
     }

     printf("\n");
}
