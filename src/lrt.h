/* the Lazard-Rioboo-Trager algorithm for calculating the logarithmic part of
 * a rational function integral */

#ifndef H_LRT
#define H_LRT

#include "polynomial.h"
#include "coef_array.h"
#include "tree.h"

void IntRationalLogPart(CoefArray *Qi, CoefArray *Si,
                        Coefficient A, Coefficient D,
                        char var, char newvar);

void IntRationalLogPartI(node_type *root, char var, char newvar);

#endif /* H_LRT */
