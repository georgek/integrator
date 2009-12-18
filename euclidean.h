/* Euclidean algorithms for polynomials */

#ifndef _EUCLIDIAN_H_
#define _EUCLIDIAN_H_

#include "polynomial.h"

/* normal Euclidean algorithm for calculating gcd(a,b) */
void Euclidean(Polynomial *res, Polynomial a, Polynomial b);

/* extended Euclidean for calculating gcd(a,b), s and t such that
 * gcd(a,b) = sa + tb */
void ExtendedEuclidean(Polynomial *g, Polynomial *s, Polynomial *t,
                       Polynomial a, Polynomial b);

/* half extended version only calculates s */
void HalfExtendedEuclidean(Polynomial *g, Polynomial *s,
                           Polynomial a, Polynomial b);

#endif /* _EUCLIDIAN_H_ */
