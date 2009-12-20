#include "prs.h"

void SubResultant(Polynomial *res, PolyArray *prs, Polynomial A, Polynomial B)
{
     unsigned i, j, k;

     *prs = new_poly_array();   /* TODO: free old array? */
     pa_deep_push_back(prs, A);
     pa_deep_push_back(prs, B);
     
}

