#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"

/* utility functions */
static int length(BigNum);
static int real_length(BigNum);
static SHORT_INT_T get_dig(BigNum, int);
static SHORT_INT_T get_dig2(BigNum, int);
static BigNum add(BigNum, BigNum);
static BigNum sub(BigNum, BigNum);
static BigNum mul(BigNum, BigNum);
static BigNum mul2(BigNum, SHORT_INT_T);
static void dyv(BigNum*, BigNum*, BigNum, BigNum); /* div is a function in stdlib */
static int lt(BigNum, BigNum);
static void copy(BigNum*, BigNum);
static int zero(BigNum);
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

BigNum make_bignum2(SHORT_INT_T n)
{
     BigNum bignum = malloc(sizeof(SHORT_INT_T)*2);
     *bignum = 1;
     *(bignum+1) = n;
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
     int i = length(p);
     int negative = is_neg(p);

     if (negative) {
          printf("%c", '-');    /* print sign */
     }
     /* skip zero digits */
     while (*(p+i) == 0 && i > 1) {
          --i;
     }
     printf("%u", *(p+i));      /* first digit is not padded */
     --i;
     while(i > 0) {
          printf("%09u", *(p+i));
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

void add_bignums(BigNum *res, BigNum left, BigNum right)
{
     BigNum old_res = *res;
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               *res = add(left, right); /* a+b */
          }
          else {                            /* a-b */
               if (lt(left,right)) {        /* make sure lhs is
                                             * greater than rhs */
                    *res = sub(right, left);
                    negate_bignum(*res);
               }
               else {
                    *res = sub(left, right);
               }
          }
     }
     else {
          if (!is_neg(right)) {             /* b-a */
               if (lt(right,left)) {        /* make sure lhs is
                                             * greater than rhs */
                    *res = sub(left, right);
                    negate_bignum(*res);
               }
               else {
                    *res = sub(right, left);
               }
          }
          else {
               *res = add(left, right); /* -(a+b) */
               negate_bignum(*res);
          }
     }
     /* free old result */
     free_bignum(old_res);
}

void sub_bignums(BigNum *res, BigNum left, BigNum right)
{
     BigNum old_res = *res;
     if (!is_neg(left)) {
          if (!is_neg(right)) {             /* a-b */
               if (lt(left,right)) {        /* make sure lhs is
                                             * greater than rhs */
                    *res = sub(right, left);
                    negate_bignum(*res);
               }
               else {
                    *res = sub(left, right);
               }
          }
          else {
               *res = add(left, right); /* a+b */
          }
     }
     else {
          if (!is_neg(right)) {
               *res = add(left, right); /* -(a+b) */
               negate_bignum(*res);
          }
          else {                            /* b-a */
               if (lt(right,left)) {        /* make sure lhs is
                                             * greater than rhs */
                    *res = sub(left, right);
                    negate_bignum(*res);
               }
               else {
                    *res = sub(right, left);
               }
          }
     }
     /* free old result */
     free_bignum(old_res);
}

void mul_bignums(BigNum *res, BigNum left, BigNum right)
{
     BigNum old_res = *res;
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               *res = mul(left, right); /* a*b */
          }
          else {
               *res = mul(left, right); /* -(a*b) */
               negate_bignum(*res);
          }
     }
     else {
          if (!is_neg(right)) {
               *res = mul(left, right); /* -(a*b) */
               negate_bignum(*res);
          }
          else {
               *res = mul(left, right); /* a*b */
          }
     }
     /* free old result */
     free_bignum(old_res);
}

void mul_bignums2(BigNum *res, BigNum left, SHORT_INT_T right)
{
     BigNum old_res = *res;
     if (!is_neg(left)) {
          *res = mul2(left, right); /* a*b */
     }
     else {
          *res = mul2(left, right); /* -(a*b) */
          negate_bignum(*res);
     }
     /* free old result */
     free_bignum(old_res);
}

void div_bignums(BigNum *q, BigNum *r, BigNum left, BigNum right)
{
     BigNum old_q = *q;
     BigNum old_r = *r;
     if (!is_neg(left)) {
          if (!is_neg(right)) {
               dyv(q, r, left, right); /* a/b */
          }
          else {
               dyv(q,r, left, right); /* -(a/b) */
               negate_bignum(*q);
               negate_bignum(*r);
          }
     }
     else {
          if (!is_neg(right)) {
               dyv(q,r, left, right); /* -(a/b) */
               negate_bignum(*q);
               negate_bignum(*r);
          }
          else {
               dyv(q, r, left, right); /* a/b */
          }
     }
     /* free old result */
     free_bignum(old_q);
     free_bignum(old_r);
}

/* negates a bignum, returns p */
void negate_bignum(BigNum p)
{
     *p = -((int) *p);
}

/* halves a bignum, using a right shift */
void half_bignum(BigNum *res, BigNum u)
{
     BigNum old_res = *res;
     int i = length(u);
     SHORT_INT_T lsb = 0;
     *res = make_zero_bignum(real_length(u));
     if (is_neg(u)) negate_bignum(*res);
     /* now do the right shift on res */
     while (i > 0) {
          /* add least significant bit from last digit */
          if (lsb) *(*res+i) += RADIX/2;
          /* get least significant bit from this digit */
          lsb = *(u+i) & 1;
          /* do right shift */
          *(*res+i) += *(u+i) >> 1;
          --i;
     }
     free_bignum(old_res);
}

/* doubles a bignum, using a left shift */
void double_bignum(BigNum *res, BigNum u)
{
     BigNum old_res = *res;
     int i = 1;
     SHORT_INT_T msb = 0;

     *res = make_zero_bignum(real_length(u)+1);

     if (is_neg(u)) negate_bignum(*res);
     /* now do the left shift on res */
     while (i < length(*res)+1) {
          /* add msb from last digit */
          if (msb) *(*res+i) += 1;
          /* get most significant bit from this digit */
          msb = *(u+i) / (RADIX/2);
          /* do right shift */
          *(*res+i) += (*(u+i)%(RADIX/2)) << 1;
          ++i;
     }
     free_bignum(old_res);
}

/* greatest common divisor
 * implements binary GCD algorithm */
void gcd(BigNum *gcd, BigNum u, BigNum v) 
{
     BigNum nu, nv, t;
     int k = 0;
     int i;
     /* copy u and v */
     nu = make_zero_bignum(1);
     nv = make_zero_bignum(1);
     copy(&nu, u);
     copy(&nv, v);

     t = make_zero_bignum(1);
     /* find power of 2, keep dividing both until at least one is
      * odd */
     while (!((get_dig(nu, 0)&1) || (get_dig(nv, 0)&1))) { /* while both even */
          ++k;
          half_bignum(&nu, nu);
          half_bignum(&nv, nv);
     }
     if (get_dig(nu, 0)&1) {
          copy(&t, nv);
          negate_bignum(t);
     }
     else {
          copy(&t, nu);
     }
     do {
          /* halve t until it is odd */
          while (!(get_dig(t,0)&1)) {
               half_bignum(&t, t);
          }
          /* reset max(u,v) */
          if (!is_neg(t) && !zero(t)) {
               copy(&nu, t);
          }
          else {
               copy(&nv, t);
               negate_bignum(nv);
          }
          sub_bignums(&t, nu, nv);
     } while (!zero(t));
     /* answer is u left shifted k times */
     for (i = 0; i < k; ++i) {
          double_bignum(&nu, nu);
     }
     copy(gcd, nu);
}

/* returns length of bignum */
static int length(BigNum p)
{
     return abs((int) *p);
}

/* returns real length, excluding leading zero digits */
static int real_length(BigNum p)
{
     int i = length(p);
     
     /* skip leading zeros */
     while (*(p+i) == 0 && i > 1) {
          --i;
     }
     return i;
}

/* digits are numbered from 0 starting from least significant */
static SHORT_INT_T get_dig(BigNum p, int i)
{
     if (i < length(p)) {
          return *(p+i+1);
     }
     else return 0;
}

static SHORT_INT_T get_dig2(BigNum p, int i)
{
     if (i < length(p)) {
          return *(p + length(p) - i);
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

static BigNum mul2(BigNum left, SHORT_INT_T right)
{
     int result_length = length(left) + 1;
     LONG_INT_T t;
     SHORT_INT_T k;
     int i;
     BigNum result;

     if (right == 0) {
          result = make_zero_bignum(1);
          return result;
     }
     
     result = make_zero_bignum(result_length);
     i = 0;
     k = 0;
     while (i < length(left)) {
          t = (LONG_INT_T) get_dig(left, i) * right + k;
          *(result+i+1) = (SHORT_INT_T) (t % RADIX);
          k = (SHORT_INT_T) (t / RADIX);
          ++i;
     }
     *(result+length(left)+1) = k;

     return result;
}

static void dyv(BigNum *q, BigNum *r, BigNum u, BigNum v)
{
     BigNum nu, nv, tv;
     SHORT_INT_T d;
     SHORT_INT_T j, qd, rd;
     SHORT_INT_T m = length(u) - length(v);
     SHORT_INT_T n = length(v);

     /* allocate bignums for answer */
     *q = make_zero_bignum(m+1);
     *r = make_zero_bignum(n);

     tv = make_zero_bignum(1);

     /* normalise */
     d = RADIX/(get_dig2(v, n-1)+1);
     mul_bignums2(&nu, u, d);
     mul_bignums2(&nv, v, d);

     j = m;
     while (j >= 0) {
          qd = (get_dig2(nu, j+n)*RADIX + get_dig2(nu, j+n-1))/get_dig2(nv, n-1);
          rd = (get_dig2(nu, j+m)*RADIX + get_dig2(nu, j+n-1))%get_dig2(nv, n-1);
          /* test trial qd */
          while (qd >= RADIX ||
                 qd*get_dig2(nv, n-2) > (RADIX*rd + get_dig2(nu, j+n-2))) {
               --qd;
               rd += get_dig2(nv, n-1);
               if (rd < RADIX) {
                    continue;
               }
               else {
                    break;
               }
          }
          /* multiply and subtract */
          mul_bignums2(&tv, nv, qd);
          
     }
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

static void copy(BigNum *res, BigNum u)
{
     BigNum old_res = *res;
     int i;
     *res = malloc(sizeof(SHORT_INT_T) * (length(u)+1));
     for (i = 0; i < (length(u)+1); ++i) {
          *(*res+i) = *(u+i);
     }
     free_bignum(old_res);
}

static int zero(BigNum u)
{
     int i = 0;
     while (i < length(u)) {
          if (get_dig(u, i)) {
               return 0;
          }
          ++i;
     }
     return 1;
}

