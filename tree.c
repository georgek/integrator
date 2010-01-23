#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "bignum.h"
#include "tree.h"
#include "y.tab.h"

void yyerror(char *s);

/* size of node without contents */
#define SIZEOF_NODE ((char *)&p->u.rat - (char *)p)

node_type *add_rat(BigNum value)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(rat_node_type);
     if ((p = malloc(node_size)) == NULL) {
          yyerror("out of memory");
     }

     /* initialise */
     p->type = rat_type;
     p->u.rat.value.num = value;
     p->u.rat.value.den = make_bignum2(1);

     return p;
}

node_type *add_var(char name)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(var_node_type);
     if ((p = malloc(node_size)) == NULL) {
          yyerror("out of memory");
     }
     
     /* copy information */
     p->type = var_type;
     p->u.var.name = name;
     
     return p;
}

node_type *add_poly(Polynomial poly)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(poly_node_type);
     if ((p = malloc(node_size)) == NULL) {
          yyerror("out of memory");
     }
     
     /* copy information */
     p->type = poly_type;
     p->u.poly.poly.variable = poly.variable;
     p->u.poly.poly.head = poly.head;
     
     return p;
}

node_type *add_op1(int operator, node_type *operand)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(op1_node_type);
     if ((p = malloc(node_size)) == NULL) {
          yyerror("out of memory");
     }
     
     /* copy information */
     p->type = op1_type;
     p->u.op1.operator = operator;
     p->u.op1.operand = operand;
     
     return p;
}

node_type *add_op2(int operator, node_type *operand1, node_type *operand2)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(op2_node_type);
     if ((p = malloc(node_size)) == NULL) {
          yyerror("out of memory");
     }
     
     /* copy information */
     p->type = op2_type;
     p->u.op2.operator = operator;
     p->u.op2.operand1 = operand1;
     p->u.op2.operand2 = operand2;
     
     return p;
}

void free_tree(node_type *p) 
{
     if (!p) {
          return;
     }
     else if (p->type == rat_type) {
          free_bigrat(&p->u.rat.value);
     }
     else if (p->type == op1_type) {
          free_tree(p->u.op1.operand);
     }
     else if (p->type == op2_type) {
          free_tree(p->u.op2.operand1);
          free_tree(p->u.op2.operand2);
     }
     free(p);
}

void print_prefix_lisp(node_type *root)
{
     traverse_prefix_lisp(root, 0);
     printf("\n");
}

void traverse_prefix_lisp(node_type *p, int prev_op)
{
     if (!p) return;
     switch (p->type) {
     case rat_type:
          print_bigrat(p->u.rat.value);
          break;
     case var_type:
          printf("%c", p->u.var.name);
          break;
     case poly_type:
          print_poly(p->u.poly.poly);
          break;
     case op1_type:
          printf("(");
          print_operator(p->u.op1.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op1.operand, p->u.op1.operator);
          printf(")");
          break;
     case op2_type:
          /* if last operator was the same as this one, don't print it again */
          if (p->u.op2.operator == prev_op && prev_op != '^') {
               traverse_prefix_lisp(p->u.op2.operand1,
                                    p->u.op2.operator);
               printf(" ");
               traverse_prefix_lisp(p->u.op2.operand2,
                                    p->u.op2.operator);
               break;
          }
          printf("(");
          print_operator(p->u.op2.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op2.operand1, p->u.op2.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op2.operand2, p->u.op2.operator);
          printf(")");
          break;
     }
}

void print_prefix(node_type *root)
{
     traverse_prefix(root);
     printf("\n");
}

void traverse_prefix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case rat_type:
          print_bigrat(p->u.rat.value);
          break;
     case var_type:
          printf("%c", p->u.var.name);
          break;
     case poly_type:
          print_poly(p->u.poly.poly);
          break;
     case op1_type:
          print_operator(p->u.op1.operator);
          printf(" ");
          traverse_prefix(p->u.op1.operand);
          break;
     case op2_type:
          print_operator(p->u.op2.operator);
          printf(" ");
          traverse_prefix(p->u.op2.operand1);
          printf(" ");
          traverse_prefix(p->u.op2.operand2);
          break;
     }
}

void print_postfix(node_type *root)
{
     traverse_postfix(root);
     printf("\n");
}

void traverse_postfix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case rat_type:
          print_bigrat(p->u.rat.value);
          break;
     case var_type:
          printf("%c", p->u.var.name);
          break;
     case poly_type:
          print_poly(p->u.poly.poly);
          break;
     case op1_type:
          traverse_postfix(p->u.op1.operand);
          printf(" ");
          print_operator(p->u.op1.operator);
          break;
     case op2_type:
          traverse_postfix(p->u.op2.operand1);
          printf(" ");
          traverse_postfix(p->u.op2.operand2);
          printf(" ");
          print_operator(p->u.op2.operator);
          break;
     }
}

void print_infix(node_type *root)
{
     traverse_infix(root);
     printf("\n");
}

void traverse_infix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case rat_type:
          print_bigrat(p->u.rat.value);
          break;
     case var_type:
          printf("%c", p->u.var.name);
          break;
     case poly_type:
          print_poly(p->u.poly.poly);
          break;
     case op1_type:
          /* deal with unary minus */
          if (p->u.op1.operator == UMINUS) {
               printf("-");
               if ((p->u.op1.operand->type == op2_type)
                   && (higher_priority(p->u.op1.operator,
                                       p->u.op1.operand->u.op2.operator)
                       ==1)) {
                    printf("(");
                    traverse_infix(p->u.op1.operand);
                    printf(")");
               }
               else {
                    traverse_infix(p->u.op1.operand);
               }
          }
          else {
               print_operator(p->u.op1.operator);
               printf("(");
               traverse_infix(p->u.op1.operand);
               printf(")");
          }
          break;
     case op2_type:
          /* if one of the operands is an operator of lower priority
           * than this operator, it must go in brackets */
          if ((p->u.op2.operand1->type == op2_type)
              && (higher_priority(p->u.op2.operator,
                                  p->u.op2.operand1->u.op2.operator)
                   == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand1);
               printf(")");
          }
          else if ((p->u.op2.operand1->type == op1_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand1->u.op1.operator)
                        == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand1);
               printf(")");
          }
          else {
               traverse_infix(p->u.op2.operand1);
          }

          /* printf(" "); */
          print_operator(p->u.op2.operator);
          /* printf(" "); */

          if ((p->u.op2.operand2->type == op2_type)
              && (higher_priority(p->u.op2.operator,
                                  p->u.op2.operand2->u.op2.operator)
                   == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          else if ((p->u.op2.operand2->type == op1_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand2->u.op1.operator)
                        == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          else {
               traverse_infix(p->u.op2.operand2);
          }

          break;
     }
}

/* returns operator's precedence for infix mode */
int op_priority(int i)
{
     int p;
     switch (i) 
     {
     case '-': p=1; break;
     case '+': p=1; break;
     case '*': p=2; break; 
     case '/': p=3; break;
     case UMINUS: p=4; break;
     case '^': p=5; break;
     default: p=6;break;       
     }                   
     return (p);
}

/* compares operator priority for infix mode */
int higher_priority(int i, int j)
{
        int p1, p2;
        p1=op_priority(i); p2=op_priority(j);
        if (p1>p2) return 1 ;
        else if (p1<p2) return -1 ;
        else return 0;
}

void print_operator(int id)
{
     switch (id) {
     case LN:
          printf("ln");
          break;
     case EXP:
          printf("exp");
          break;
     case SIN:
          printf("sin");
          break;
     case COS:
          printf("cos");
          break;
     case TAN:
          printf("tan");
          break;
     case EVAL:
          printf("eval");
          break;
     case DIFF:
          printf("diff");
          break;
     case UMINUS:
          printf("~");
          break;
     case UPLUS:
          printf("P");
          break;
     default:
          printf("%c", id);
          break;
     }
}

