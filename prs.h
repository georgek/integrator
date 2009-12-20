/* algorithms for calculating subresultant polynomial remainder
 * sequences, gcds and resultants */

#ifndef _PRS_H_
#define _PRS_H_

#include "polynomial.h"
#include "poly_array.h"

/* implementation of subresultant algorithm from Bronstein's _Symbolic
 * Integration I_ p. 24 */
void SubResultant(Polynomial *res, PolyArray *prs, Polynomial A, Polynomial B);


#endif /* _PRS_H_ */
