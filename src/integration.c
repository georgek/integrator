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
          return;
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

     HermiteReduce(&integral.rational_part,
                   &h,
                   root->u.ratfun,
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
          /* make Qi and Si primitive */
          for (i = 0; i < integral.Qi.size; ++i) {
               Qit = ca_get2(&integral.Qi, i);
               Sit = ca_get2(&integral.Si, i);
               
               rat_part.u.rat = coef_rat_part(*Qit);
               mul_coefficients(Qit, *Qit, rat_part);
               free_bigrat(&rat_part.u.rat);
               
               rat_part.u.rat = coef_rat_part(*Sit);
               mul_coefficients(Sit, *Sit, rat_part);
               free_bigrat(&rat_part.u.rat);

               coef_pp(Qit, *Qit, newvar);
               coef_pp(Sit, *Sit, var);
          }

          /* solve linear univariate Qis */
          for (i = 0; i < integral.Qi.size; ++i) {
               if (ca_get(&integral.Qi, i).type != polynomial
                   || ca_get(&integral.Qi, i).u.poly.head->next->coeff.type
                   != rational
                   || ca_get(&integral.Qi, i).u.poly.head->next->next->coeff.type
                   != rational) {
                    continue;
               }
               printf("Solving:\n");
               PRINTC(ca_get(&integral.Qi, i));
               solve_linear_poly(&solution, ca_get(&integral.Qi, i).u.poly);
               PRINTC(solution);
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

     /* print explicit sums */
     if (integral.QiS.size > 0) {
          for (i = 0; i < integral.QiS.size; ++i) {
               if (plus && !coef_neg(ca_get(&integral.QiS, i))) {
                    printf(" + ");
               }
               if (!coef_one2(ca_get(&integral.QiS, i))) {
                    printf(" - ");
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
