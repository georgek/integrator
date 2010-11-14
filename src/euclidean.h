/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

/* Euclidean algorithms for polynomials */

#ifndef _EUCLIDIAN_H_
#define _EUCLIDIAN_H_

#include "polynomial.h"

/* normal Euclidean algorithm for calculating gcd(a,b) */
void Euclidean(Coefficient *res, Coefficient a, Coefficient b);

/* extended Euclidean for calculating gcd(a,b), s and t such that
 * gcd(a,b) = sa + tb */
void ExtendedEuclidean(Coefficient *g, Coefficient *s, Coefficient *t,
                       Coefficient a, Coefficient b);

/* half extended version only calculates s */
void HalfExtendedEuclidean(Coefficient *g, Coefficient *s,
                           Coefficient a, Coefficient b);

/* solves a diophantine equation of the form sa + tb = c */
void SolveDiophantineEquation(Coefficient *s, Coefficient *t,
                              Coefficient a, Coefficient b, Coefficient c);
/* half version only calculates s */
void HalfSolveDiophantineEquation(Coefficient *s,
                                  Coefficient a, Coefficient b, Coefficient c);

#endif /* _EUCLIDIAN_H_ */
