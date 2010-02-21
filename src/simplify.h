#ifndef _SIMPLIFY_H_
#define _SIMPLIFY_H_

#include "tree.h"

void simple_simplify(node_type **root);

void extract_polys(node_type **root);

void extract_ratfuns(node_type **root);

#endif /* _SIMPLIFY_H_ */
