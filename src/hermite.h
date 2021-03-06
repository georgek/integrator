/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

/* the Hermite reduction */

#ifndef _HERMITE_H_
#define _HERMITE_H_

#include "ratfun.h"
#include "tree.h"

void HermiteReduce(RatFun *g, RatFun *h, RatFun AD, char var, int trace);

void HermiteReduceI(node_type *root, char var);

#endif /* _HERMITE_H_ */
