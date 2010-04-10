#include <stdio.h>
#include <stdlib.h>

#include "bigrat.h"
#include "coef_array.h"
#include "euclidean.h"
#include "polynomial.h"
#include "prs.h"
#include "squarefree.h"
#include "tree.h"

void Squarefree(CoefArray *f, Coefficient A, char var)
{
     Coefficient c = {special};
     Coefficient *Ak = NULL;
     Coefficient S = {special},Sd = {special},Sm = {special};
     Coefficient Ss = {special}, Ssd = {special}, Y = {special};
     Coefficient Z = {special}, At = {special};
     unsigned i = 1;

     /* c <-- content(A) */
     coef_content(&c, A, var);
     PRINTC(c);
     /* S <-- pp(A) */
     exact_div_coefficients(&S, A, c);
     coef_differentiate(&Sd, S, var);
     PRINTC(Sd);
     coef_gcd(&Sm, S, Sd);
     PRINTC(Sm);
     exact_div_coefficients(&Ss, S, Sm);
     exact_div_coefficients(&Y, Sd, Sm);
     PRINTC(Y);

     coef_differentiate(&Ssd, Ss, var);
     PRINTC(Ssd);
     sub_coefficients(&Z, Y, Ssd);
     while (!coef_zero(Z)) {
          coef_gcd(&At, Ss, Z);
          PRINTC(At);
          Ak = ca_push_back(f, At);
          
          printf("%d\n", i++);
          PRINTC(Ss);
          PRINTC(Y);
          PRINTC(Z);
          PRINTC(*Ak);
          
          exact_div_coefficients(&Ss, Ss, *Ak);
          exact_div_coefficients(&Y, Z, *Ak);

          coef_differentiate(&Ssd, Ss, var);
          sub_coefficients(&Z, Y, Ssd);
     }

     printf("%d\n", i++);
     PRINTC(Ss);
     PRINTC(Y);
     PRINTC(Z);
     if (Ak) {
          PRINTC(*Ak);
     }
          
     ca_push_back(f, Ss);
     mul_coefficients(f->head, *f->head, c);
}

void SquarefreeI(node_type *root, char var)
{
     CoefArray arr;
     int i;
     
     /* input must be one polynomial */
     if (root->type != coef_type) {
          printf("Error. Input to Squarefree must be a polynomial.\n");
          return;
     }

     arr = new_coef_array();

     Squarefree(&arr, root->u.coef, var);
     printf("...\n");
     print_coefficient_nonpretty(root->u.coef);
     printf(" = ");
     for (i = 0; i < arr.size; ++i) {
          if (coef_one(ca_get(&arr, i))) {
               continue;
          }
          print_coefficient_nonpretty(ca_get(&arr, i));
          if (i > 0) {
               printf("^%u", i+1);
          }
          if (i != arr.size-1) {
               printf(" * ");
          }
          else {
               printf("\n");
          }
     }

     ca_free(&arr);
}
