#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "bignum.h"
#include "simplify.h"
#include "y.tab.h"
#include "polynomial.h"

/* this function performs a simplficiation (ie. replacing 3*2 with 5
 * etc.) and reduces the tree into rationals and polynomials if
 * possible */
void simple_simplify(node_type **root)
{
     node_type *r = *root;

     switch (r->type) {
     case op2_type:
          simple_simplify(&(r->u.op2.operand1));
          simple_simplify(&(r->u.op2.operand2));

          if (r->u.op2.operand1->type == rat_type) {
               /* left operand is a rational */
               if (br_zero(r->u.op2.operand1->u.rat)) {
                    /* left operand is zero */
                    switch (r->u.op2.operator) {
                    case '+':
                         /* 0+x = x */
                         *root = r->u.op2.operand2;
                         r->u.op2.operand2 = NULL;
                         free_tree(r);
                         break;
                    case '-':
                         /* 0-x = -x */
                         *root = add_op1(UMINUS, r->u.op2.operand2);
                         r->u.op2.operand2 = NULL;
                         free_tree(r);
                         break;
                    case '*':
                         /* 0*x = 0 */
                    case '/':
                         /* 0/x = 0 */
                         *root = add_rat(make_bignum2(0));
                    free_tree(r);
                    break;
                    case '^':
                         if (((r->u.op2.operand2->type == rat_type)
                              && (br_zero(r->u.op2.operand2->u.rat)))) {
                              /* 0^A = 1 where A = 0 */
                              *root = add_rat(make_bignum2(1));
                              free_tree(r);
                         }
                         else {
                              /* 0^A = 0 where A =/= 0 */
                              *root = add_rat(make_bignum2(0));
                              free_tree(r);
                         }
                         break;
                    default:
                         break;
                    }
               }
               else if (r->u.op2.operand2->type == rat_type) {
                    /* right operand is also a rational */
                    switch (r->u.op2.operator) {
                    case '+':
                         add_bigrats(&r->u.op2.operand1->u.rat,
                                     r->u.op2.operand1->u.rat,
                                     r->u.op2.operand2->u.rat);
                         *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                    case '-':
                         sub_bigrats(&r->u.op2.operand1->u.rat,
                                     r->u.op2.operand1->u.rat,
                                     r->u.op2.operand2->u.rat);
                         *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                    case '*':
                         mul_bigrats(&r->u.op2.operand1->u.rat,
                                     r->u.op2.operand1->u.rat,
                                     r->u.op2.operand2->u.rat);
                         *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                    case '/':
                         div_bigrats(&r->u.op2.operand1->u.rat,
                                     r->u.op2.operand1->u.rat,
                                     r->u.op2.operand2->u.rat);
                         *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                         /* case '^': */
                         /*      *root = add_int( */
                         /*           (int) pow(r->u.op2.operand1->u.intg.value, */
                         /*                     r->u.op2.operand2->u.intg.value)); */
                         /*      free_tree(r); */
                         /*      break; */
                    default:
                         break;
                    }
               }
               else if (br_one(r->u.op2.operand1->u.rat)) {
                    /* left operand is 1 */
                    switch (r->u.op2.operator) {
                    case '*':
                         /* 1*x = x */
                         *root = r->u.op2.operand2;
                         r->u.op2.operand2 = NULL;
                         free_tree(r);
                         break;
                    case '^':
                         /* 1^x = 1 */
                         *root = add_rat(make_bignum2(1));
                         free_tree(r);
                         break;
                    default:
                         break;
                    }
               }
          }
          else {
               /* left operand is not numeric */
               if (r->u.op2.operand2->type == rat_type) {
                    /* right operand is an integer */
                    if (br_zero(r->u.op2.operand2->u.rat)) {
                         /* right operand is 0 */
                         switch (r->u.op2.operator) {
                         case '+':
                              /* x+0 = x */
                         case '-':
                              /* x-0 = x */
                              *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                         case '*':
                              /* x*0 = 0 */
                              *root = add_rat(make_bignum2(0));
                              free_tree(r);
                              break;
                         case '^':
                              /* x^0 = 1 */
                              *root = add_rat(make_bignum2(1));
                              free_tree(r);
                              break;
                         default:
                              break;
                         }
                    }
                    else if (br_one(r->u.op2.operand2->u.rat)) {
                         /* right operand is 1 */
                         switch (r->u.op2.operator) {
                         case '*':
                              /* x*1 = x */
                         case '/':
                              /* x/1 = x */
                         case '^':
                              /* x^1 = x */
                              *root = r->u.op2.operand1;
                         r->u.op2.operand1 = NULL;
                         free_tree(r);
                         break;
                         default:
                              break;
                         }
                    }
               }
          }
          break;
     case op1_type:
          simple_simplify(&(r->u.op1.operand));
          /* remove unecessary unary minuses */
          if (r->u.op1.operator == UMINUS) {
               if (r->u.op1.operand->type == rat_type) {
                    negate_bigrat(&r->u.op1.operand->u.rat);
                    *root = r->u.op1.operand;
                    r->u.op1.operand = NULL;
                    free_tree(r);
               }
          }
          break;
     default:
          break;
     }
}

void extract_polys(node_type **root)
{
     node_type *r = *root;

     switch (r->type) {
     case op2_type:
          extract_polys(&r->u.op2.operand1);
          extract_polys(&r->u.op2.operand2);

          switch (r->u.op2.operator) {
          case '^':
               if (r->u.op2.operand1->type == poly_type
                   && r->u.op2.operand2->type == rat_type) {
                    /* poly ^ rat = poly */
                    if (real_length(r->u.op2.operand2->u.rat.num)
                        != 1) {
                         printf("Error! Multiple precision indices are not "
                                "supported!\n");
                         exit(1);
                    }
                    poly_power(&r->u.op2.operand1->u.poly,
                               r->u.op2.operand1->u.poly,
                               *(r->u.op2.operand2->u.rat.num+1));
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               break;
               
          case '*':
               if (r->u.op2.operand1->type == rat_type
                   && r->u.op2.operand2->type == poly_type) {
                    /* rat * poly = poly */
                    mul_poly_rat(&r->u.op2.operand2->u.poly,
                                 r->u.op2.operand2->u.poly,
                                 r->u.op2.operand1->u.rat);
                    *root = r->u.op2.operand2;
                    r->u.op2.operand2 = NULL;
                    free_tree(r);
               }
               else if (r->u.op2.operand1->type == poly_type
                        && r->u.op2.operand2->type == poly_type) {
                    /* poly * poly = poly */
                    mul_polynomials(&r->u.op2.operand1->u.poly,
                                    r->u.op2.operand1->u.poly,
                                    r->u.op2.operand2->u.poly);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               else if (r->u.op2.operand1->type == poly_type
                        && r->u.op2.operand2->type == rat_type) {
                    /* poly * rat = poly */
                    mul_poly_rat(&r->u.op2.operand1->u.poly,
                                 r->u.op2.operand1->u.poly,
                                 r->u.op2.operand2->u.rat);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               break;

          case '/':
               if (r->u.op2.operand1->type == poly_type
                   && r->u.op2.operand2->type == rat_type) {
                    /* poly / rat = poly */
                    div_poly_rat(&r->u.op2.operand1->u.poly,
                                 r->u.op2.operand1->u.poly,
                                 r->u.op2.operand2->u.rat);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               break;

          case '+':
               if (r->u.op2.operand1->type == poly_type
                   && r->u.op2.operand2->type == poly_type) {
                    /* poly + poly = poly */
                    poly_splice_add(&r->u.op2.operand1->u.poly,
                                    &r->u.op2.operand2->u.poly);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               /* TODO: these two cases can be done more efficiently
                * by splicing */
               else if (r->u.op2.operand1->type == poly_type
                        && r->u.op2.operand2->type == rat_type) {
                    /* poly + rat = poly */
                    add_poly_rat(&r->u.op2.operand1->u.poly,
                                 r->u.op2.operand1->u.poly,
                                 r->u.op2.operand2->u.rat);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               else if (r->u.op2.operand1->type == rat_type
                        && r->u.op2.operand2->type == poly_type) {
                    /* rat + poly = poly */
                    add_poly_rat(&r->u.op2.operand2->u.poly,
                                 r->u.op2.operand2->u.poly,
                                 r->u.op2.operand1->u.rat);
                    *root = r->u.op2.operand2;
                    r->u.op2.operand2 = NULL;
                    free_tree(r);
               }
               break;
               
          case '-':
               if (r->u.op2.operand1->type == poly_type
                   && r->u.op2.operand2->type == poly_type) {
                    /* poly + poly = poly */
                    poly_splice_sub(&r->u.op2.operand1->u.poly,
                                    &r->u.op2.operand2->u.poly);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               /* TODO: these two cases can be done more efficiently
                * by splicing */
               else if (r->u.op2.operand1->type == poly_type
                        && r->u.op2.operand2->type == rat_type) {
                    /* poly + rat = poly */
                    sub_poly_rat(&r->u.op2.operand1->u.poly,
                                 r->u.op2.operand1->u.poly,
                                 r->u.op2.operand2->u.rat);
                    *root = r->u.op2.operand1;
                    r->u.op2.operand1 = NULL;
                    free_tree(r);
               }
               else if (r->u.op2.operand1->type == rat_type
                        && r->u.op2.operand2->type == poly_type) {
                    /* rat + poly = poly */
                    sub_poly_rat(&r->u.op2.operand2->u.poly,
                                 r->u.op2.operand2->u.poly,
                                 r->u.op2.operand1->u.rat);
                    *root = r->u.op2.operand2;
                    r->u.op2.operand2 = NULL;
                    free_tree(r);
               }
               break;
               
          default:
               break;
          }
          
          break;
     case op1_type:
          extract_polys(&r->u.op1.operand);

          /* deal with unary minus */
          if (r->u.op1.operator == UMINUS
              && r->u.op1.operand->type == poly_type) {
               negate_polynomial(&r->u.op1.operand->u.poly);
               *root = r->u.op1.operand;
               r->u.op1.operand = NULL;
               free_tree(r);
          }
          
          break;
     case var_type:
          /* this is a monomial */
          *root = add_poly(make_mono_poly(r->u.var,1));
          free_tree(r);
          break;
     default:
          break;
     }
     
}

