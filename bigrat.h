/* big rational numbers */

#ifndef _BIGRAT_H_
#define _BIGRAT_H_

#include <limits.h>

#include "bignum.h"

typedef struct 
{
     BigNum mum;                /* numerator */
     BigNum den;                /* denominator */
} BigRat;

/* make new bigrat from two bignums */
BigRat make_bigrat(BigNum num, BigNum den);

void free_bigrat(BigRat f);

void print_bigrat(BigRat f);



#endif /* _BIGRAT_H_ */
