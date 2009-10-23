#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"

/* utility functions */
static int length(BigNum);
static SHORT_INT_T get_dig(BigNum, int);
static BigNum add(BigNum, BigNum);
static BigNum sub(BigNum, BigNum);
static BigNum mul(BigNum, BigNum);
static int lt(BigNum, BigNum);
/* static BigNum trim_bignum(BigNum); */

BigNum make_bignum(char *string, int length)
{
     char seg[DIGPERWORD+1];      /* string to hold segments */
     BigNum bignum, p;
     int bignum_length, negative;
     char *s;

     /* init segment string */
     seg[DIGPERWORD] = '\0';

     /* set sign */
     if (*string == '-') {
          /* negative */
          negative = 1;
          /* advance one character */
          string += 1;
          length -= 1;
     }
     else {
          negative = 0;
     }

     /* allocate memory for bignum */
     bignum_length = (length%DIGPERWORD) ? length/DIGPERWORD+2 :
          length/DIGPERWORD+1;
     bignum = malloc(sizeof(SHORT_INT_T)*bignum_length);
     
     /* set length */
     p = bignum;
     *p = negative ? -(bignum_length-1) : bignum_length-1;
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

BigNum make_zero_bignum(int length)
{
     BigNum new = calloc(length+1, sizeof(BigNum));
     *new = length;
     return new;
}

void free_bignum(BigNum p)
{
     if (p) {
          free(p);
     }
}

void print_bignum(BigNum p)
{
     int length = abs(*p);
     BigNum i;
     int negative = is_neg(p);

     i = p + length;
     if (negative) {
          printf("%c", '-');    /* print sign */
     }
     /* skip zero digits */
     while (*i == 0) {
          --i;
     }
     printf("%u", *i);          /* first digit is not padded */
     --i;
     while(i != p) {
          printf("%09u", *i);
          --i;
     }
}

int is_neg(BigNum p)
{
     return ((int) *p < 0) ? 1 : 0;
}

int bignum_equal(BigNum left, BigNum right)
{
     int jl=length(left), jr=length(right);
     if (!(is_neg(left) && is_neg(right))) {
          return 0;
     }
     /* find first non-zero digit of each */
     while (*(left+jl) == 0) {
          --jl;
     }
     while (*(right+jr) == 0) {
          --jr;
     }
     if (jl != jr) {
          /* different lengths therefore not equal */
          return 0;
     }
     /* compare each digit */
     while (jl > 0) {
          if (*(left+jl) != *(right+jr)) {
               return 0;
          }
          --jl;
          --jr;
     }
     /* all digits equal */
     return 1;
}

int bignum_lt(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return lt(left, right);
          }
          else {
               return 0;
          }
     }
     else {
          if (!is_neg(right)) {
               return 1;
          }
          else {
               return lt(right, left);
          }
     }     
}

int bignum_gt(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return lt(right, left);
          }
          else {
               return 1;
          }
     }
     else {
          if (!is_neg(right)) {
               return 0;
          }
          else {
               return lt(left, right);
          }
     }     
}

int bignum_lte(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return !lt(right, left);
          }
          else {
               return 0;
          }
     }
     else {
          if (!is_neg(right)) {
               return 1;
          }
          else {
               return !lt(left, right);
          }
     }     
}

int bignum_gte(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return !lt(left, right);
          }
          else {
               return 1;
          }
     }
     else {
          if (!is_neg(right)) {
               return 0;
          }
          else {
               return !lt(right, left);
          }
     }     
}

BigNum add_bignums(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return add(left, right); /* a+b */
          }
          else {                            /* a-b */
               if (lt(left,right)) {        /* make sure lhs is
                                             * greater than rhs */
                    return negate_bignum(sub(right, left));
               }
               else {
                    return sub(left, right);
               }
          }
     }
     else {
          if (!is_neg(right)) {             /* b-a */
               if (lt(right,left)) {        /* make sure lhs is
                                             * greater than rhs */
                    return negate_bignum(sub(left, right));
               }
               else {
                    return sub(right, left);
               }
          }
          else {
               return negate_bignum(add(left, right)); /* -(a+b) */
          }
     }
}

BigNum sub_bignums(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {             /* a-b */
               if (lt(left,right)) {        /* make sure lhs is
                                             * greater than rhs */
                    return negate_bignum(sub(right, left));
               }
               else {
                    return sub(left, right);
               }
          }
          else {
               return add(left, right); /* a+b */
          }
     }
     else {
          if (!is_neg(right)) {
               return negate_bignum(add(left, right)); /* -(a+b) */
          }
          else {                            /* b-a */
               if (lt(right,left)) {        /* make sure lhs is
                                             * greater than rhs */
                    return negate_bignum(sub(left, right));
               }
               else {
                    return sub(right, left);
               }
          }
     }
}

BigNum mul_bignums(BigNum left, BigNum right)
{
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               return mul(left, right); /* a*b */
          }
          else {
               return negate_bignum(mul(left, right)); /* -(a*b) */
          }
     }
     else {
          if (!is_neg(right)) {
               return negate_bignum(mul(left, right)); /* -(a*b) */
          }
          else {
               return mul(left, right); /* a*b */
          }
     }
}
/* negates a bignum, returns p */
BigNum negate_bignum(BigNum p)
{
     *p = -(*p);
     return p;
}

/* returns length of bignum */
static int length(BigNum p)
{
     return abs((int) *p);
}

static SHORT_INT_T get_dig(BigNum p, int i)
{
     if (i < length(p)) {
          return *(p+i+1);
     }
     else return 0;
}

/* these static functions always ignore the sign and assume both
 * bignums are positive, negative numbers can be dealt with in terms
 * of positive numbers and the negate */

/* the classical algorithms */

/* does an actual addition of two nonnegative bignums */
static BigNum add(BigNum left, BigNum right) 
{
     /* result length could be one longer than the longest operand */
     int result_length = (length(left) > length(right)) ? length(left)+1 :
          length(right)+1;
     LONG_INT_T result_dig;
     int j = 0;                 /* j runs through digit positions */
     SHORT_INT_T k = 0;                 /* k holds carry */
     BigNum result;
     
     result = make_zero_bignum(result_length);

     while (j < (length(result)-1)) {
          result_dig = (LONG_INT_T) get_dig(left, j) + get_dig(right, j) + k;
          *(result+j+1) = (SHORT_INT_T) (result_dig % RADIX);
          k = (SHORT_INT_T) (result_dig / RADIX);
          ++j;
     }
     *(result+j+1) = k;
     
     return result;
}

/* does subtraction of two nonnegative bignums where left > right */
static BigNum sub(BigNum left, BigNum right)
{
     /* result length could be as long as the longest operand */
     int result_length = (length(left) > length(right)) ? length(left) :
          length(right);
     SHORT_INT_T result_dig;
     int j = 0;                 /* j runs through digit positions */
     SHORT_INT_T k = 1;                 /* k holds carry+1 */
     BigNum result;
     
     result = make_zero_bignum(result_length);

     while (j < (length(result))) {
          result_dig = RADIX + get_dig(left, j) - get_dig(right, j) + k - 1;
          *(result+j+1) = result_dig % RADIX;
          k = result_dig / RADIX;
          ++j;
     }
     *(result+j+1) = k;
     
     return result;     
}

static BigNum mul(BigNum left, BigNum right)
{
     int result_length = length(left) + length(right);
     LONG_INT_T t;
     SHORT_INT_T k;
     int i, j;
     BigNum result;

     result = make_zero_bignum(result_length);

     j = 0;
     while (j < length(right)) {
          if (get_dig(right, j) == 0) {
               *(result+j+length(left)+1) = 0;
               ++j;
               continue;
          }
          i = 0;
          k = 0;
          while (i < length(left)) {
               t = (LONG_INT_T) get_dig(left, i) * get_dig(right, j) +
                    get_dig(result, i+j) + k;
               *(result+i+j+1) = (SHORT_INT_T) (t % RADIX);
               k = (SHORT_INT_T) (t / RADIX);
               ++i;
          }
          *(result+j+length(left)+1) = k;
          ++j;
     }
     return result;
}
/* less than, ignores sign */
static int lt(BigNum left, BigNum right)
{
     int jl=length(left), jr=length(right);
     /* find first non-zero digit of each */
     while (*(left+jl) == 0) {
          --jl;
     }
     while (*(right+jr) == 0) {
          --jr;
     }
     if (jl < jr) {
          /* left is shorter so less */
          return 1;
     }
     if (jl > jr) {
          /* left is longer so more */
          return 0;
     }
     /* compare each digit to see if any are less than or greater
      * than */
     while (jl > 0) {
          if (*(left+jl) < *(right+jr)) {
               /* less than, so true */
               return 1;
          }
          if (*(left+jl) > *(right+jr)) {
               /* greater than, so return false */
               return 0;
          }
          --jl;
          --jr;
     }
     /* must be equal, so return false */
     return 0;
}

/* shortens array by copying, if necessary */
/* static BigNum trim_bignum(BigNum p) */
/* { */
/*      int i = length(p); */
/* } */


