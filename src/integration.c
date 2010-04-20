#include <stdio.h>

#include "integration.h"
#include "polynomial.h"
#include "hermite.h"
#include "lrt.h"

/* TODO input to hermite must have a primitive denominator so we should remove
 * the content from the rational and/or polynomial part from the rational
 * function first */
/* void IntegrateRationalFunction(node_type *root, char var, char newvar) */
/* { */
/*      RatFun g, h; */
/*      Coefficient Q = {special}, R = {special}; */
/*      CoefArray Qi = new_coef_array(), Si = new_coef_array(); */
/*      unsigned i, j = 0; */
     
/*      init_ratfun(&g); */
/*      init_ratfun(&h); */

/*      if (root->type != ratfun_type) { */
/*           printf("Error! IntegrateRationalFunction" */
/*                  "requires a rational function.\n"); */
/*           return; */
/*      } */

/*      HermiteReduce(&g, &h, root->u.ratfun, var); */
/*      print_ratfun(g); */
/*      printf(" +\n"); */

/*      polydiv_coefficients(&Q, &R, h.num, h.den); */
     
/*      coef_integrate(&Q, Q, var); */
/*      print_coefficient(Q); */

/*      if (!coef_zero(R)) { */
/*           printf(" +\n"); */
/*           IntRationalLogPart(&Qi, &Si, R, h.den, var, newvar); */

/*           for (i = 0; i < Qi.size; ++i) { */
/*                if (coef_deg(ca_get(&Qi, i), newvar) == 0) { */
/*                     continue; */
/*                } */
/*                printf("("); */

/*                printf("sum(%c*ln(", newvar); */
/*                print_coefficient(ca_get(&Si, j++)); */
/*                printf("), %c = rootsOf(", newvar); */
/*                print_coefficient(ca_get(&Qi, i)); */
/*                printf(" = 0))\n"); */

/*                /\* printf(")\n"); *\/ */
               
/*                /\* printf("Q_%d: ", i); *\/ */
/*                /\* print_coefficient_nonpretty(ca_get(&Qi, i)); *\/ */
/*                /\* printf("\n"); *\/ */
/*           } */
/*           /\* for (i = 0; i < Si.size; ++i) { *\/ */
/*           /\*      printf("S_%d: ", i); *\/ */
/*           /\*      print_coefficient_nonpretty(ca_get(&Si, i)); *\/ */
/*           /\*      printf("\n"); *\/ */
/*           /\* } *\/ */
/*      } */
/*      printf("\n"); */
/* } */

void IntegrateRationalFunction(node_type *root, char var, char newvar)
{
     RatFun A, h, content;
     Coefficient R = {special};
     Integral integral;

     init_ratfun(&A);
     init_ratfun(&h);
     init_ratfun(&content);

     init_integral(&integral);

     if (root->type != ratfun_type) {
          printf("Error! IntegrateRationalFunction"
                 "requires a rational function.\n");
          return;
     }

     copy_ratfun(&integral.integrand, root->u.ratfun);
     integral.var = var;
     integral.newvar = newvar;

     if (ratfun_zero(integral.integrand)) {
          return;
     }
     
     /* make numerator and denominator primitive */
     coef_content(&content.num, integral.integrand.num, var);
     exact_div_coefficients(&integral.integrand.num,
                            integral.integrand.num,
                            content.num);
     coef_content(&content.den, integral.integrand.den, var);
     exact_div_coefficients(&integral.integrand.den,
                            integral.integrand.den,
                            content.den);

     HermiteReduce(&integral.rational_part,
                   &h,
                   integral.integrand,
                   var);

     /* put contents back on */
     /* PRINTR(content); */
     mul_ratfuns(&integral.rational_part, integral.rational_part, content);
     mul_ratfuns(&h, h, content);
     /* PRINTR(h); */

     polydiv_coefficients(&integral.poly_part, &R, h.num, h.den);

     coef_integrate(&integral.poly_part, integral.poly_part, var);

     if (!coef_zero(R)) {
          IntRationalLogPart(&integral.Qi, &integral.Si, R, h.den, var, newvar);
     }

     print_integral(integral);
     printf("\nLaTeX format:\n");
     print_integral_LaTeX(integral);

     free_ratfun(&A);
     free_ratfun(&h);
     free_ratfun(&content);
     free_coefficient(&R);
     free_integral(&integral);
}

void init_integral(Integral *integral)
{
     init_ratfun(&integral->integrand);
     init_ratfun(&integral->rational_part);
     integral->poly_part.type = special;
     integral->Qi = new_coef_array();
     integral->Si = new_coef_array();
}

void free_integral(Integral *integral)
{
     free_ratfun(&integral->integrand);
     free_ratfun(&integral->rational_part);
     free_coefficient(&integral->poly_part);
     ca_free(&integral->Qi);
     ca_free(&integral->Si);
}

void print_integral(Integral integral)
{
     int plus = 0;
     unsigned i, j = 0;
     
     if (ratfun_zero(integral.integrand)) {
          printf("0\n");
          return;
     }
     
     if (!ratfun_zero(integral.rational_part)){
          print_ratfun(integral.rational_part);
          plus = 1;
     }

     if (!coef_zero(integral.poly_part)) {
          if (plus) {
               printf(" + ");
          }
          print_coefficient(integral.poly_part);
          plus = 1;
     }

     if (integral.Qi.size > 0) {
          for (i = 0; i < integral.Qi.size; ++i) {
               if (coef_deg(ca_get(&integral.Qi, i), integral.newvar) == 0) {
                    continue;
               }
               if (plus) {
                    printf(" + ");
               }
               printf("sum(%c | ", integral.newvar);
               print_coefficient(ca_get(&integral.Qi, i));
               printf(" = 0) %c*ln(", integral.newvar);
               print_coefficient(ca_get(&integral.Si, j++));
               printf(")");
               plus = 1;
          }
     }

     printf("\n");     
}

void print_integral_LaTeX(Integral integral)
{
     int plus = 0;
     unsigned i, j = 0;
     
     printf("\\int \\! ");
     print_ratfun_LaTeX(integral.integrand);
     printf("\\, d%c = ", integral.var);

     if (ratfun_zero(integral.integrand)) {
          printf("0\n");
          return;
     }
     
     if (!ratfun_zero(integral.rational_part)){
          print_ratfun_LaTeX(integral.rational_part);
          plus = 1;
     }

     if (!coef_zero(integral.poly_part)) {
          if (plus) {
               printf(" + ");
          }
          print_coefficient_LaTeX(integral.poly_part);
          plus = 1;
     }

     if (integral.Qi.size > 0) {
          for (i = 0; i < integral.Qi.size; ++i) {
               if (coef_deg(ca_get(&integral.Qi, i), integral.newvar) == 0) {
                    continue;
               }
               if (plus) {
                    printf(" + ");
               }
               printf("\\sum_{%c | ", integral.newvar);
               print_coefficient_LaTeX(ca_get(&integral.Qi, i));
               printf(" = 0} %c \\ln(", integral.newvar);
               print_coefficient_LaTeX(ca_get(&integral.Si, j++));
               printf(")");
               plus = 1;
          }
     }

     printf("\n");
}
