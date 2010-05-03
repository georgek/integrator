/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

/* algorithms for calculating subresultant polynomial remainder
 * sequences, gcds and resultants */

#ifndef _PRS_H_
#define _PRS_H_

#include "polynomial.h"
#include "coef_array.h"
#include "tree.h"

/* implementation of subresultant algorithm from Bronstein's _Symbolic
 * Integration I_ p. 24 */
void SubResultant(Coefficient *res, CoefArray *prs,
                  Coefficient A, Coefficient B, char var);
/* uses the subresultant PRS algorithm to calculate just the GCD */
void SubResultantGCD(Coefficient *gcd, Coefficient A, Coefficient B, char var);
void GCDI(node_type *root);
void SubResultantPRSI(node_type *root, char var);

#endif /* _PRS_H_ */
