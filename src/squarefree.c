#include <stdio.h>
#include <stdlib.h>

#include "bigrat.h"
#include "coef_array.h"
#include "euclidean.h"
#include "polynomial.h"
#include "prs.h"
#include "squarefree.h"
#include "tree.h"

void Squarefree(CoefArray *f, Polynomial A)
{
     Coefficient c = {special};
     Coefficient *Ak = NULL;
     Polynomial S = {'x', NULL},Sd = {'x', NULL},Sm = {'x', NULL};
     Polynomial Ss = {'x', NULL}, Ssd = {'x', NULL}, Y = {'x', NULL};
     Polynomial Z = {'x', NULL}, At = {'x', NULL};
     /* unsigned i = 1; */

     /* c <-- content(A) */
     poly_content(&c, A);
     /* printf("c = "); */
     /* print_coefficient(c); */
     /* printf("\n"); */
     /* S <-- pp(A) */
     poly_pp(&S, A);
     poly_differentiate(&Sd, S);
     /* printf("S' = "); */
     /* print_poly(Sd); */
     /* printf("\n"); */
     SubResultantGCD(&Sm, S, Sd);
     /* printf("S- = "); */
     /* print_poly(Sm); */
     /* printf("\n"); */
     exact_div_polynomials(&Ss, S, Sm);
     exact_div_polynomials(&Y, Sd, Sm);
     /* printf("Y = "); */
     /* print_poly(Y); */
     /* printf("\n"); */

     poly_differentiate(&Ssd, Ss);
     /* printf("S*' = "); */
     /* print_poly(Ssd); */
     /* printf("\n"); */
     sub_polynomials(&Z, Y, Ssd);
     while (!poly_zero(Z)) {
          SubResultantGCD(&At, Ss, Z);
          Ak = ca_poly_push_back(f, At);
          
          /* printf("%d\n", i++); */
          /* printf("S* = "); */
          /* print_poly(Ss); */
          /* printf("\nY = "); */
          /* print_poly(Y); */
          /* printf("\nZ = "); */
          /* print_poly(Z); */
          /* printf("\nA_k = "); */
          /* print_poly(Ak->u.poly); */
          /* printf("\n"); */
          
          exact_div_polynomials(&Ss, Ss, Ak->u.poly);
          exact_div_polynomials(&Y, Z, Ak->u.poly);

          poly_differentiate(&Ssd, Ss);
          sub_polynomials(&Z, Y, Ssd);
     }

     /* printf("%d\n", i++); */
     /* printf("S* = "); */
     /* print_poly(Ss); */
     /* printf("\nY = "); */
     /* print_poly(Y); */
     /* printf("\nZ = "); */
     /* print_poly(Z); */
     /* printf("\nA_k = "); */
     /* print_poly(Ss); */
     /* printf("\n"); */
          
     ca_poly_push_back(f, Ss);
     mul_coefficients(f->head, *f->head, c);
}

void SquarefreeI(node_type *root)
{
     CoefArray arr;
     int i;
     
     /* input must be one polynomial */
     if (root->type != poly_type) {
          printf("Error. Input to Squarefree must be a polynomial.\n");
          return;
     }

     arr = new_coef_array();

     Squarefree(&arr, root->u.poly);

     for (i = 0; i < arr.size; ++i) {
          if (coef_one(ca_get(&arr, i))) {
               continue;
          }
          print_coefficient(ca_get(&arr, i));
          if (i > 0) {
               printf("^%u", i+1);
          }
          if (i != arr.size-1) {
               printf("*");
          }
          else {
               printf("\n");
          }
     }

     ca_free(&arr);
}

