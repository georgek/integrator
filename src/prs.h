/* algorithms for calculating subresultant polynomial remainder
 * sequences, gcds and resultants */

#ifndef _PRS_H_
#define _PRS_H_

#include "polynomial.h"
#include "coef_array.h"
#include "tree.h"

/* implementation of subresultant algorithm from Bronstein's _Symbolic
 * Integration I_ p. 24 */
void SubResultant(Polynomial *res, CoefArray *prs, Polynomial A, Polynomial B);
/* uses the subresultant PRS algorithm to calculate just the GCD */
void SubResultantGCD(Coefficient *gcd, Coefficient A, Coefficient B);
void GCDI(node_type *root);

#endif /* _PRS_H_ */
