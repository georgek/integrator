/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

#include <stdio.h>

#include "integration.h"
#include "polynomial.h"
#include "hermite.h"
#include "lrt.h"

#define WAIT while (getchar() != '\n')

void IntegrateRationalFunction(node_type *root, char var, char newvar,
                               int trace)
{
     RatFun A, h, content;
     Coefficient R = {special}, rat_part = {rational}, solution = {special};
     Coefficient *Qit = NULL, *Sit = NULL;
     Coefficient Qit2 = {special}, Sit2 = {special};
     Integral integral;
     unsigned i;

     init_ratfun(&A);
     init_ratfun(&h);
     init_ratfun(&content);

     init_bigrat(&rat_part.u.rat);

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
          goto print;
     }

     /* make numerator and denominator primitive */
     coef_content(&content.num, root->u.ratfun.num, var);
     exact_div_coefficients(&root->u.ratfun.num,
                            root->u.ratfun.num,
                            content.num);
     coef_content(&content.den, root->u.ratfun.den, var);
     exact_div_coefficients(&root->u.ratfun.den,
                            root->u.ratfun.den,
                            content.den);

     if (trace) {
          printf("Computing rational part of integral using "
                 "Hermite reduction...\n");
          WAIT;
     }

     HermiteReduce(&integral.rational_part,
                   &h,
                   root->u.ratfun,
                   var,
                   trace);

     /* put contents back on */
     /* PRINTR(content); */
     mul_ratfuns(&integral.rational_part, integral.rational_part, content);
     mul_ratfuns(&h, h, content);
     /* PRINTR(h); */

     if (trace) {
          printf("Found rational part:\t\t");
          print_ratfun(integral.rational_part);
          printf("\n");

          printf("\nHermite reduction leaves:\t");
          print_ratfun(h);
          printf("\n");
          WAIT;

          printf("Remove polynomial part...\n");
          WAIT;
     }

     polydiv_coefficients(&integral.poly_part, &R, h.num, h.den);

     if (trace) {
          printf("Polynomial part:\t\t");
          print_coefficient(integral.poly_part);
          printf("\n");
     }

     coef_integrate(&integral.poly_part, integral.poly_part, var);

     if (trace) {
          printf("Integrate it:\t\t\t");
          print_coefficient(integral.poly_part);
          printf("\n");
          WAIT;

          printf("Left over:\t\t\t");
          print_coefficient(R);
          printf("/");
          print_coefficient(h.den);
          printf("\n");
     }

     if (!coef_zero(R) && coef_deg(h.den, var) > coef_deg(R, var)) {

          if (trace) {
               printf("\nComputing logarithmic part using "
                      "Lazard-Rioboo-Trager algorithm...\n");
               WAIT;
          }

          IntRationalLogPart(&integral.Qi, &integral.Si, R, h.den, var, newvar,
                             trace);
          
          /* make Qi and Si primitive */
          for (i = 0; i < integral.Qi.size; ++i) {
               Qit = ca_get2(&integral.Qi, i);
               Sit = ca_get2(&integral.Si, i);
               
               rat_part.u.rat = coef_rat_part(*Qit);
               mul_coefficients(Qit, *Qit, rat_part);
               free_bigrat(&rat_part.u.rat);
               
               /* rat_part.u.rat = coef_rat_part(*Sit); */
               /* mul_coefficients(Sit, *Sit, rat_part); */
               /* free_bigrat(&rat_part.u.rat); */

               coef_pp(Qit, *Qit, newvar);
               /* coef_pp(Sit, *Sit, var); */
          }

          if (trace) {
               printf("Found logarithmic part.\n");
               printf("%d sum(s) over roots:\t\t", integral.Qi.size);
               if (integral.Qi.size > 0) {
                    for (i = 0; i < integral.Qi.size; ++i) {
                         printf("sum(%c | ", integral.newvar);
                         print_coefficient(ca_get(&integral.Qi, i));
                         printf(" = 0) %c*ln(", integral.newvar);
                         print_coefficient(ca_get(&integral.Si, i));
                         printf(")");
                         if (i < integral.Qi.size-1) {
                              printf(" + ");
                         }
                    }
               }
               printf("\n");
               WAIT;

               printf("Solve the linear univariate Qis to get explicit sums...\n");
          }

          /* solve linear univariate Qis */
          for (i = 0; i < integral.Qi.size; ++i) {
               if (coef_deg(ca_get(&integral.Qi, i), newvar) != 1
                   || !poly_univar(ca_get(&integral.Qi, i).u.poly)) {
                    continue;
               }
               solve_linear_poly(&solution, ca_get(&integral.Qi, i).u.poly);
               subst_var_coef(ca_get2(&integral.Si, i), solution, newvar);

               /* move to solved arrays */
               ca_push_back(&integral.QiS, solution);
               ca_push_back(&integral.SiS, ca_get(&integral.Si, i));

               /* remove from normal arrays */
               Qit2 = ca_remove(&integral.Qi, i);
               Sit2 = ca_remove(&integral.Si, i);
               free_coefficient(&Qit2);
               free_coefficient(&Sit2);
               --i;
          }

          /* make SiS primitive */
          for (i = 0; i < integral.SiS.size; ++i) {
               Sit = ca_get2(&integral.SiS, i);
               
               rat_part.u.rat = coef_rat_part(*Sit);
               mul_coefficients(Sit, *Sit, rat_part);
               free_bigrat(&rat_part.u.rat);

               coef_pp(Sit, *Sit, var);
          }

          if (trace) {
               printf("Found %d explicit sum(s):\t", integral.QiS.size);
               if (integral.QiS.size > 0) {
                    for (i = 0; i < integral.QiS.size; ++i) {
                         if (!coef_one(ca_get(&integral.QiS, i))) {
                              print_coefficient(ca_get(&integral.QiS, i));
                              printf("*ln(");
                         }
                         else {
                              printf("ln(");
                         }
                         print_coefficient(ca_get(&integral.SiS, i));
                         printf(")");
                         if (i < integral.QiS.size-1) {
                              printf(" + ");
                         }
                    }
               }
               printf("\n");
               WAIT;
          }
     }
     else if (coef_deg(h.den, var) == 0) {
          /* this is actually just a poly over a constant,
           * so integrate it trivially */
          coef_integrate(&R, R, var);
          /* move this to the numerator of h */
          free_coefficient(&h.num);
          h.num = R;
          R.type = special;
          /* add h to the rational part of integral */
          add_ratfuns(&integral.rational_part, integral.rational_part, h);
     }
     else {
          /* this shouldn't happen */
          printf("Error!  Invalid ratfun following Hermite reduction!\n");
          PRINTR(h);
          printf("\n");
          PRINTC(R);
          printf("\n");
     }

print:
     if (trace) {
          printf("\nFinal answer:\n");
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
     integral->QiS = new_coef_array();
     integral->SiS = new_coef_array();
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
     unsigned i;
     
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

     /* print explicit sums */
     if (integral.QiS.size > 0) {
          for (i = 0; i < integral.QiS.size; ++i) {
               if (plus) {
                    printf(" + ");
               }
               if (!coef_one(ca_get(&integral.QiS, i))) {
                    print_coefficient(ca_get(&integral.QiS, i));
                    printf("*ln(");
               }
               else {
                    printf("ln(");
               }
               print_coefficient(ca_get(&integral.SiS, i));
               printf(")");
               plus = 1;
          }
     }

     /* print sums over roots */
     if (integral.Qi.size > 0) {
          for (i = 0; i < integral.Qi.size; ++i) {
               if (plus) {
                    printf(" + ");
               }
               printf("sum(%c | ", integral.newvar);
               print_coefficient(ca_get(&integral.Qi, i));
               printf(" = 0) %c*ln(", integral.newvar);
               print_coefficient(ca_get(&integral.Si, i));
               printf(")");
               plus = 1;
          }
     }

     printf("\n");     
}

void print_integral_LaTeX(Integral integral)
{
     int plus = 0;
     unsigned i;
     
     printf("\\int \\! ");
     print_ratfun_LaTeX(integral.integrand);
     printf(" \\, d%c = ", integral.var);

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

     /* print explicit sums */
     if (integral.QiS.size > 0) {
          for (i = 0; i < integral.QiS.size; ++i) {
               if (plus) {
                    if (!coef_neg(ca_get(&integral.QiS, i))) {
                         printf(" + ");
                    }
                    else {
                         printf(" - ");
                    }
               }
               if (!coef_one2(ca_get(&integral.QiS, i))) {
                    print_coefficient_LaTeX2(ca_get(&integral.QiS, i), 1);
                    printf("\\ln(");
               }
               else if (coef_neg(ca_get(&integral.QiS, i))
                        && coef_one2(ca_get(&integral.QiS, i))) {
                    printf("-\\ln(");
               }
               else {
                    printf("\\ln(");
               }
               print_coefficient_LaTeX(ca_get(&integral.SiS, i));
               printf(")");
               plus = 1;
          }
     }

     /* print sums over roots */
     if (integral.Qi.size > 0) {
          for (i = 0; i < integral.Qi.size; ++i) {
               if (plus) {
                    printf(" + ");
               }
               printf("\\sum_{%c | ", integral.newvar);
               print_coefficient_LaTeX(ca_get(&integral.Qi, i));
               printf(" = 0} %c \\ln(", integral.newvar);
               print_coefficient_LaTeX(ca_get(&integral.Si, i));
               printf(")");
               plus = 1;
          }
     }

     printf("\n");
}
