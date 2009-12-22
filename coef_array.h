/* dynamic arrays of coefficients are necessary for polynomial
 * remainder sequences and factorisations */

#ifndef _COEF_ARRAY_H_
#define _COEF_ARRAY_H_

#include "polynomial.h"
#include "bigrat.h"

typedef struct coef_array
{
     unsigned size;
     unsigned max_size;
     Coefficient *head;
} CoefArray;

/* make a new empty array with default max size */
CoefArray new_coef_array();
/* put a copy of coefficient at end of array */
CoefPtr ca_push_back(CoefArray *ca, Coefficient nc);
/* put a copy of a polynomial or rational at end */
CoefPtr ca_poly_push_back(CoefArray *ca, Polynomial np);
CoefPtr ca_rational_push_back(CoefArray *ca, BigRat nr);
/* get a polynomial from array, it is an error to use an invalid index
 * ie. index must be < size */
Coefficient ca_get(CoefArray *ca, unsigned index);
/* free array */
void ca_free(CoefArray *ca);

#endif /* _COEF_ARRAY_H_ */
