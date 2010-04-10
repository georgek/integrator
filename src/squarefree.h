/* squarefree factorisation */

#ifndef _SQUAREFREE_H_
#define _SQUAREFREE_H_

#include "coef_array.h"
#include "tree.h"

void Squarefree(CoefArray *f, Coefficient A, char var);

void SquarefreeI(node_type *root, char var);

#endif /* _SQUAREFREE_H_ */
