#include <stdio.h>
#include <stdlib.h>

#include "poly_array.h"
#include "polynomial.h"

static const unsigned default_size = 10;

PolyArray new_poly_array()
{
     PolyArray array;
     array.size = 0;
     array.max_size = default_size;
     array.head = malloc(sizeof(Polynomial)*default_size);
     return array;
}

PolyPtr pa_push_back(PolyArray *pa, Polynomial np)
{
     Polynomial *new_head;
     unsigned i;
     
     if (pa->size == pa->max_size) {
          /* double array size */
          pa->max_size = pa->max_size *2;
          new_head = malloc(sizeof(Polynomial)*pa->max_size);
          for (i = 0; i < pa->size; ++i) {
               *(new_head+i) = *(pa->head+i);
          }
          free(pa->head);
          pa->head = new_head;
          new_head = NULL;
     }

     *(pa->head + pa->size) = np;
     ++pa->size;
     return pa->head + pa->size - 1;
}

PolyPtr pa_deep_push_back(PolyArray *pa, Polynomial np)
{
     PolyPtr new;

     new = pa_push_back(pa, np);
     new->head = NULL;
     copy_poly(new, np);

     return new;
}

Polynomial pa_get(PolyArray *pa, unsigned index)
{
     return *(pa->head + index);
}

