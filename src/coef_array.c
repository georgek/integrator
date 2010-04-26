#include <stdio.h>
#include <stdlib.h>

#include "coef_array.h"
#include "polynomial.h"

static const unsigned default_size = 10;

CoefArray new_coef_array()
{
     CoefArray array;
     array.size = 0;
     array.max_size = default_size;
     array.head = malloc(sizeof(Coefficient)*default_size);
     return array;
}

CoefPtr ca_push_back(CoefArray *ca, Coefficient nc)
{
     Coefficient *new_head;
     unsigned i;
     
     if (ca->size == ca->max_size) {
          /* double array size */
          ca->max_size = ca->max_size *2;
          new_head = malloc(sizeof(Coefficient)*ca->max_size);
          for (i = 0; i < ca->size; ++i) {
               *(new_head+i) = *(ca->head+i);
          }
          free(ca->head);
          ca->head = new_head;
          new_head = NULL;
     }

     (ca->head + ca->size)->type = special;
     copy_coefficient(ca->head + ca->size, nc);

     return ca->head + ca->size++;
}

CoefPtr ca_poly_push_back(CoefArray *ca, Polynomial np)
{
     Coefficient nc;
     nc.type = polynomial;
     nc.u.poly = np;
     return ca_push_back(ca, nc);
}

CoefPtr ca_rational_push_back(CoefArray *ca, BigRat nr)
{
     Coefficient nc;
     nc.type = rational;
     nc.u.rat = nr;
     return ca_push_back(ca, nc);
}

Coefficient ca_get(CoefArray *ca, unsigned index)
{
     return *(ca->head + index);
}

Coefficient *ca_get2(CoefArray *ca, unsigned index)
{
     return ca->head + index;
}

void ca_free(CoefArray *ca)
{
     ca->size = 0;
     ca->max_size = 0;
     free(ca->head);
     ca->head = NULL;
}

