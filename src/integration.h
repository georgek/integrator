/* general integration stuff */

#ifndef H_INTEGRATION
#define H_INTEGRATION

#include "tree.h"
#include "ratfun.h"
#include "polynomial.h"
#include "coef_array.h"

typedef struct integral {
     RatFun integrand;
     char var;
     char newvar;
     RatFun rational_part;
     Coefficient poly_part;
     CoefArray Qi;
     CoefArray Si;
     CoefArray QiS;             /* solutions to linear Qis */
     CoefArray SiS;
} Integral;

void IntegrateRationalFunction(node_type *root, char var, char newvar);
/* void IntegrateRationalFunctionLaTeX(node_type *root, char var, char newvar); */

void init_integral(Integral *integral);
void free_integral(Integral *integral);

void print_integral(Integral integral);
void print_integral_LaTeX(Integral integral);

#endif /* H_INTEGRATION */
