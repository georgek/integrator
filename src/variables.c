/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

#include <stdio.h>
#include <stdlib.h>

#include "variables.h"

typedef struct var_cell
{
     char name;
     int rank;
} Var;

static Var *var_table = NULL;
static int num_vars = 0;
static int max_size = 0;
static int default_size = 5;

void make_var_tab()
{
     if (var_table != NULL) {
          free(var_table);
          var_table = NULL;
     }

     num_vars = 0;
     max_size = default_size;
     var_table = malloc(sizeof(Var)*max_size);
}

void add_var_cell(char name)
{
     Var *new_var_table;
     int loc;
     unsigned i;

     if (var_table == NULL) {
          make_var_tab();
     }
     
     /* already here? */
     if (var_rank(name) != -1) {
          return;
     }

     if (num_vars == max_size) {
          /* double array size */
          max_size = max_size *2;
          new_var_table = malloc(sizeof(Var)*max_size);
          for (i = 0; i < num_vars; ++i) {
               *(new_var_table+i) = *(var_table+i);
          }
          free(var_table);
          var_table = new_var_table;
          new_var_table = NULL;
     }

     /* do a straight insertion */
     loc = num_vars;
     while (loc > 0 && name < (var_table+loc-1)->name) {
          *(var_table+loc) = *(var_table+loc-1);
          --loc;
     }
     (var_table+loc)->name = name;
     (var_table+loc)->rank = num_vars+1;
     ++num_vars;
}

void set_top_var(char name)
{
     int beg = 0;
     int mid;
     int end = num_vars - 1;

     add_var_cell(name);

     while(beg < end) {
          mid = (beg+end)/2;
          if(name <= (var_table+mid)->name) {
               end = mid;
          }
          else {
               beg = mid+1;
          }
     }

     (var_table+beg)->rank = 0;
}

int var_rank(char name)
{
     int beg = 0;
     int mid;
     int end = num_vars - 1;

     while(beg < end) {
          mid = (beg+end)/2;
          if(name <= (var_table+mid)->name) {
               end = mid;
          }
          else {
               beg = mid+1;
          }
     }

     return (name == (var_table+beg)->name) ?
          (var_table+beg)->rank : -1;
}

void print_var_tab()
{
     unsigned i;
     for (i = 0; i < num_vars; ++i) {
          printf("name: %c, rank: %d\n", (var_table+i)->name, (var_table+i)->rank);
     }
     printf("\n");
}

