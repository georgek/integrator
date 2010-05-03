/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

/* squarefree factorisation */

#ifndef _SQUAREFREE_H_
#define _SQUAREFREE_H_

#include "coef_array.h"
#include "tree.h"

void Squarefree(CoefArray *f, Coefficient A, char var);

void SquarefreeI(node_type *root, char var);

#endif /* _SQUAREFREE_H_ */
