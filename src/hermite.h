/* the Hermite reduction */

#ifndef _HERMITE_H_
#define _HERMITE_H_

#include "ratfun.h"
#include "tree.h"

void HermiteReduce(RatFun *g, RatFun *h, RatFun AD, char var);

void HermiteReduceI(node_type *root, char var);

#endif /* _HERMITE_H_ */
