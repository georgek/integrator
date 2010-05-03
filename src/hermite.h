/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

/* the Hermite reduction */

#ifndef _HERMITE_H_
#define _HERMITE_H_

#include "ratfun.h"
#include "tree.h"

void HermiteReduce(RatFun *g, RatFun *h, RatFun AD, char var);

void HermiteReduceI(node_type *root, char var);

#endif /* _HERMITE_H_ */
