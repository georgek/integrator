/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

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
/* remove coef from array, returning the coef for freeing etc. */
Coefficient ca_remove(CoefArray *ca, unsigned index);
/* put a copy of a polynomial or rational at end */
CoefPtr ca_poly_push_back(CoefArray *ca, Polynomial np);
CoefPtr ca_rational_push_back(CoefArray *ca, BigRat nr);
/* get a polynomial from array, it is an error to use an invalid index
 * ie. index must be < size */
Coefficient ca_get(CoefArray *ca, unsigned index);
/* returns pointer instead (for modifying them) */
Coefficient *ca_get2(CoefArray *ca, unsigned index);
/* free array */
void ca_free(CoefArray *ca);

#endif /* _COEF_ARRAY_H_ */
