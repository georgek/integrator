/* dynamic arrays of polynomials are necessary for polynomial
 * remainder sequences and factorisations */

#ifndef _POLY_ARRAY_H_
#define _POLY_ARRAY_H_

#include "polynomial.h"

typedef struct poly_array
{
     unsigned size;
     unsigned max_size;
     Polynomial *head;
} PolyArray;

/* make a new empty array with default max size */
PolyArray new_poly_array();
/* push back a new polynomial, increasing size by one, makes a shallow
 * copy */
PolyPtr pa_push_back(PolyArray *pa, Polynomial np);
/* push back, but make a deep copy of polynomial */
PolyPtr pa_deep_push_back(PolyArray *pa, Polynomial np);
/* get a polynomial from array, it is an error to use an invalid index
 * ie. index must be < size */
Polynomial pa_get(PolyArray *pa, unsigned index);


#endif /* _POLY_ARRAY_H_ */
