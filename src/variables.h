/* the variable table stores the names of variables and their rank */

#ifndef H_VARIABLES
#define H_VARIABLES

/* makes a new variable table, destroying any old one */
void make_var_tab();
void add_var_cell(char name);
void set_top_var(char name);
/* gets the variable rank, returns -1 if var is not in table */
int var_rank(char name);

void print_var_tab();

#endif /* H_VARIABLES */
