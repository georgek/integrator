#ifndef _TREE_H_
#define _TREE_H_

#include "bignum.h"
#include "bigrat.h"
#include "polynomial.h"
#include "ratfun.h"

typedef enum node_types {
     rat_type, var_type, poly_type, ratfun_type, op1_type, op2_type
} node_types;

/* contents of nodes for each type */
/* rational */
typedef BigRat rat_node_type;

/* variable */
typedef char var_node_type;

/* monomial */
typedef Polynomial poly_node_type;

/* rational function */
typedef RatFun ratfun_node_type;

/* unary operator */
typedef struct {
     int operator;
     struct node_type * operand;
} op1_node_type;

/* binary operator */
typedef struct {
     int operator;
     struct node_type *operand1;
     struct node_type *operand2;
} op2_node_type;

typedef struct node_type {
     node_types type;
     /* node contents */
     union {
          rat_node_type rat;
          var_node_type var;
          poly_node_type poly;
          ratfun_node_type ratfun;
          op1_node_type op1;
          op2_node_type op2;
     } u;
} node_type;


/* functions for adding nodes to the tree */

node_type *add_rat(BigNum value);
node_type *add_var(char name);
node_type *add_poly(Polynomial poly);
node_type *add_ratfun(Polynomial num, Polynomial den);
node_type *add_op1(int operator, node_type *operand);
node_type *add_op2(int operator, node_type *operand1, node_type *operand2);

/* frees the tree from memory */
void free_tree(node_type *p);

/* functions for printing a tree */

void print_prefix(node_type *root);
void traverse_prefix(node_type *p);

void print_postfix(node_type *root);
void traverse_postfix(node_type *p);

void print_prefix_lisp(node_type *root);
void traverse_prefix_lisp(node_type *p, int prev_op);

void print_infix(node_type *root);
void traverse_infix(node_type *root);
/* utility functions for infix */
int op_priority(int i);
int higher_priority(int i, int j);

void print_operator(int id);



#endif /* _TREE_H_ */
