#ifndef _TREE_H_
#define _TREE_H_

typedef enum node_types {
     int_type, var_type, op1_type, op2_type
} node_types;

/* contents of nodes for each type */
/* integer */
typedef struct {
     int value;
} int_node_type;

/* variable */
typedef struct {
     char name;
} var_node_type;

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
          int_node_type intg;
          var_node_type var;
          op1_node_type op1;
          op2_node_type op2;
     } u;
} node_type;


/* functions for adding nodes to the tree */

node_type *add_int(int value);
node_type *add_var(char name);
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
