#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"

BigNum make_bignum(char *string, int length)
{
     char seg[DIGPERWORD+1];      /* string to hold segments */
     BigNum bignum, p;
     int bignum_length;
     char *s;

     /* init segment string */
     seg[DIGPERWORD] = '\0';

     /* allocate memory for bignum */
     bignum_length = (length%DIGPERWORD) ? length/DIGPERWORD+2 :
          length/DIGPERWORD+1;
     bignum = malloc(sizeof(SHORT_INT_T)*bignum_length);
     
     /* set length and sign */
     p = bignum;
     *p = bignum_length-1;
     ++p;
     /* set digits */
     s = string+length;
     while (s-string >= DIGPERWORD) {
          s -= DIGPERWORD;
          strncpy(seg, s, DIGPERWORD);
          *p = (unsigned) strtoul(seg, (char**)NULL, 10);
          ++p;
     }
     /* might be one more */
     if (s != string) {
          strncpy(seg, string, s-string);
          seg[s-string] = '\0';
          *p = (unsigned) strtoul(seg, (char**)NULL, 10);
     }
     return bignum;
}

void free_bignum(BigNum p)
{
     free(p);
}

void print_bignum(BigNum p)
{
     unsigned length = *p;
     BigNum i;
     i = p + length;
     printf("%u", *i);          /* first digit is not padded */
     --i;
     while(i != p) {
          printf("%09u", *i);
          --i;
     }
}

