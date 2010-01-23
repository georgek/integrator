/* general bignum functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"

/* utility functions */
static int length(BigNum);
static SHORT_INT_T get_dig(BigNum, int);
static void set_dig(BigNum, int, SHORT_INT_T);
static BigNum add(BigNum, BigNum);
static BigNum sub(BigNum, BigNum);
static BigNum mul(BigNum, BigNum);
static BigNum mul2(BigNum, SHORT_INT_T);
static void dyv(BigNum*, BigNum*, BigNum, BigNum); /* div is a function in stdlib */
static void dyv2(BigNum*, SHORT_INT_T*, BigNum, SHORT_INT_T);
static int lt(BigNum, BigNum);
static void copy(BigNum*, BigNum);

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

BigNum make_bignuml(char *string)
{
     int l = strlen(string);
     return make_bignum(string, l);
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
     free(p);
}

void print_bignum(BigNum p)
{
     int i = length(p);
     int negative = is_neg(p) && !bn_zero(p);

     if (!p) {
          return;
     }

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
          printf("%09u", *(p+i)); /* **TODO** fix to work with all
                                       digit sizes */
          --i;
     }
}

void bignum_copy(BigNum *d, BigNum s)
{
     copy(d, s);
}

int is_neg(BigNum p)
{
     if (!p) {
          return 0;
     }
     return ((S_SHORT_INT_T) *p < 0) ? 1 : 0;
}

int bignum_equal(BigNum left, BigNum right)
{
     int jl=length(left), jr=length(right);

     if (!left || !right) {
          return 0;
     }

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

     *res = mul(left, right);

     /* set sign */
     if ((S_SHORT_INT_T) (*left ^ *right) < 0) {
          negate_bignum(*res);
     }

     /* free old result */
     free_bignum(old_res);
}

void mul_bignums2(BigNum *res, BigNum left, SHORT_INT_T right)
{
     BigNum old_res = *res;

     right = right%RADIX;
     *res = mul2(left, right);

     /* set sign */
     if (is_neg(left)) {
          negate_bignum(*res);
     }

     /* free old result */
     free_bignum(old_res);
}

/* this division gives the same result as "tdiv" or truncate division
 * in GMP, that is to say the quotient will be rounded towards 0 and
 * the remainder will have the same sign as the numerator */
void div_bignums(BigNum *q, BigNum *r, BigNum left, BigNum right)
{
     BigNum old_q = *q;
     BigNum old_r = *r;

     SHORT_INT_T little_r;

     if (bn_zero(right)) {         /* divide by zero */
          printf("ERROR: division by zero");
          return;
     }
     
     if (real_length(right) == 1) { /* short division */
          dyv2(q, &little_r, left, get_dig(right, 0));
          *r = make_bignum2(little_r);

          /* set sign */
          if ((S_SHORT_INT_T) (*left ^ *right) < 0) {
               negate_bignum(*q);
          }
          if (is_neg(left)) {
               negate_bignum(*r);
          }

          /* free old result */
          free_bignum(old_q);
          free_bignum(old_r);

          return;
     }

     if (lt(left, right)) { /* dividend less than divisor */
          *q = make_zero_bignum(1);
          copy(r, left);

          /* set sign */
          if ((S_SHORT_INT_T) (*left ^ *right) < 0) {
               negate_bignum(*q);
          }
          if (is_neg(left)) {
               negate_bignum(*r);
          }

          /* free old result */
          free_bignum(old_q);
          free_bignum(old_r);

          return;
     }
     
     /* long division */
     dyv(q, r, left, right);

     /* set sign */
     if ((S_SHORT_INT_T) (*left ^ *right) < 0) {
          negate_bignum(*q);
     }
     if (is_neg(left)) {
          negate_bignum(*r);
     }

     /* free old result */
     free_bignum(old_q);
     free_bignum(old_r);
}

void div_bignums2(BigNum *q, SHORT_INT_T *r, BigNum left, SHORT_INT_T right)
{
     BigNum old_q = *q;

     right = right%RADIX;
     dyv2(q, r, left, right);

     /* set sign */
     if (is_neg(left)) {
          negate_bignum(*q);
          if (r) *r = -(*r);
     }

     /* free old result */
     free_bignum(old_q);
}

void bn_power(BigNum *res, BigNum p, SHORT_INT_T power)
{
     BigNum temp = NULL;
     SHORT_INT_T mask = 0;

     if (power == 0) {
          free_bignum(*res);
          *res = make_bignum2(1);
          return;
     }
     
     mask = ~(~mask>>1);        /* MSB of short int */

     bignum_copy(&temp, p);

     /* find first 1 in binary representation of power and skip it */
     while (!(mask & power) && mask) {
          mask = mask>>1;
     }
     mask = mask>>1;
     for (; mask; mask = mask>>1) {
          mul_bignums(&temp, temp, temp);
          if (mask & power) {
               mul_bignums(&temp, temp, p);
          }
     }

     bignum_copy(res, temp);
     free_bignum(temp);
}

/* negates a bignum, returns p */
void negate_bignum(BigNum p)
{
     if (p) *p = -((S_SHORT_INT_T) *p);
}

/* halves a bignum, using a right shift */
void half_bignum(BigNum *res, BigNum u)
{
     BigNum old_res = *res;
     int i = real_length(u);
     SHORT_INT_T lsb = 0;
     *res = make_zero_bignum(real_length(u));
     if (is_neg(u)) negate_bignum(*res);
     /* now do the right shift on res */
     while (i > 0) {
          /* add least significant bit from last digit */
          if (lsb) *(*res+i) += RADIX/2;
          /* get least significant bit from this digit */
          lsb = get_dig(u,i-1) & 1;
          /* do right shift */
          *(*res+i) += get_dig(u,i-1) >> 1;
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
          msb = get_dig(u,i-1) / (RADIX/2);
          /* do right shift */
          *(*res+i) += (get_dig(u,i-1)%(RADIX/2)) << 1;
          ++i;
     }
     free_bignum(old_res);
}

/* greatest common divisor
 * implements binary GCD algorithm */
void gcd(BigNum *gcd, BigNum u, BigNum v) 
{
     BigNum nu = NULL, nv = NULL, t = NULL;
     int k = 0;
     int i;

     /* deal with zeros */
     if (bn_zero(u)) {
          copy(gcd, v);
          return;
     }
     if (bn_zero(v)) {
          copy(gcd, u);
          return;
     }

     /* copy u and v */
     copy(&nu, u);
     copy(&nv, v);
     /* make both positive */
     if (is_neg(nu)) {
          negate_bignum(nu);
     }
     if (is_neg(nv)) {
          negate_bignum(nv);
     }

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
          if (!is_neg(t) && !bn_zero(t)) {
               copy(&nu, t);
          }
          else {
               copy(&nv, t);
               negate_bignum(nv);
          }
          sub_bignums(&t, nu, nv);
     } while (!bn_zero(t));
     /* answer is u left shifted k times */
     for (i = 0; i < k; ++i) {
          double_bignum(&nu, nu);
     }
     copy(gcd, nu);

     free_bignum(nu);
     free_bignum(nv);
     free_bignum(t);
}

/* Euclid's algorithm */
void gcd2(BigNum *gcd, BigNum u, BigNum v) 
{
     BigNum onu = NULL, nu = NULL, nv = NULL, q = NULL, r = NULL;
     
     copy(&nu, u);
     copy(&nv, v);
     /* make both positive */
     if (is_neg(nu)) {
          negate_bignum(nu);
     }
     if (is_neg(nv)) {
          negate_bignum(nv);
     }

     while (!bn_zero(nv)) {
          div_bignums(&q, &r, nu, nv);
          onu = nu;
          nu = nv;
          nv = r;
          r = NULL;
          
          free_bignum(onu);
          onu = NULL;
     }
     copy(gcd, nu);

     free_bignum(nu);
     free_bignum(nv);
     free_bignum(q);
}

/* returns length of bignum */
static int length(BigNum p)
{
     if (!p) {
          return 0;
     }
     return abs((int) *p);
}

/* returns real length, excluding leading zero digits */
int real_length(BigNum p)
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

static void set_dig(BigNum p, int i, SHORT_INT_T val)
{
     if (i < length(p)) {
          *(p+i+1) = val;
     }
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

static void dyv(BigNum *q, BigNum *r, BigNum dividend, BigNum divisor)
{
     SHORT_INT_T d;             /* normalisation factor */
     int j, i;                  /* loop counters */
     unsigned n = real_length(divisor);
     unsigned m = real_length(dividend) - n;
     LONG_INT_T t;              /* temp val */
     LONG_INT_T qg;             /* quotient guess */
     LONG_INT_T rg;             /* remainder guess */
     BigNum u = NULL;
     BigNum v = NULL;
     BigNum qgv = NULL;
     BigNum ut = make_zero_bignum(n+1); /* holds temp digits for u
                                         * segment */
     BigNum bc = make_zero_bignum(n+2); /* for doing the b's
                                         * complement */
     int borrow = 0;

     /* copy dividend and divisor */
     copy(&u, dividend);
     copy(&v, divisor);

     /* set up bc */
     set_dig(bc, n+1, 1);

     /* set up quotient */
     *q = make_zero_bignum(m+1);
     /* set up remainder */
     *r = NULL;

     /* normalisation */
     d = (SHORT_INT_T) (RADIX/(get_dig(v, n-1)+1));
     mul_bignums2(&u, u, d);
     mul_bignums2(&v, v, d);

     /* main loop */
     for (j = m; j >= 0; --j) {
          t = (LONG_INT_T) get_dig(u, j+n)*RADIX + get_dig(u, j+n-1);
          qg = t/get_dig(v, n-1);
          rg = t%get_dig(v, n-1);

          /* test guesses */
          while ((qg >= RADIX) ||
                 ((LONG_INT_T) qg*get_dig(v, n-2)
                  > (LONG_INT_T) RADIX*rg+get_dig(u, j+n-2))) {
               --qg;
               rg -= get_dig(v, n-1);

               if (rg >= RADIX) {
                    break;
               } /* rg < b so repeat test */
          }

          /* multiply and subtract */
          mul_bignums2(&qgv, v, (SHORT_INT_T) qg);
          /* set up ut */
          for (i = 0; i <= n; ++i) {
               set_dig(ut, i, get_dig(u, i+j));
          }
          sub_bignums(&ut, ut, qgv);

          if (is_neg(ut)) {
               borrow = 1;
               /* set ut to b's complement of real ut */
               add_bignums(&ut, ut, bc);
          }
          else {
               borrow = 0;
          }
          /* set digits of u */
          for (i = 0; i <= n; ++i) {
               set_dig(u, i+j, get_dig(ut, i));
          }

          set_dig(*q, j, qg);

          /* add back if there was a borrow */
          if (borrow) {
               set_dig(*q, j, qg-1);
               add_bignums(&ut, ut, v);
               /* ignore carry */
               set_dig(ut, n+1, 0);
               /* set digits of u */
               for (i = 0; i <= n; ++i) {
                    set_dig(u, i+j, get_dig(ut, i));
               }
          }
     }

     /* unnormalise */
     div_bignums2(r, NULL, u, d);

     free_bignum(u);
     free_bignum(v);
     free_bignum(ut);
     free_bignum(bc);
     free_bignum(qgv);
}

static void dyv2(BigNum *q, SHORT_INT_T *r, BigNum left, SHORT_INT_T right)
{
     LONG_INT_T t = 0;
     int j = real_length(left)-1;
     SHORT_INT_T rt = 0;
     *q = make_zero_bignum(j+1);
     
     for(; j >= 0; --j) {
          t = (LONG_INT_T) rt*RADIX + get_dig(left,j);
          if (q) *(*q+j+1) = (SHORT_INT_T) (t/right);
          rt = (SHORT_INT_T) (t%right);
     }
     if (r) *r = rt;
}

/* less than, ignores sign */
static int lt(BigNum left, BigNum right)
{
     int jl=length(left), jr=length(right);

     if (!left || !right) {
          return 0;
     }

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

int bn_zero(BigNum u)
{
     if (real_length(u) == 1 && get_dig(u,0) == 0) {
          return 1;
     }
     return 0;
}

int bn_one(BigNum u)
{
     if (real_length(u) == 1 && get_dig(u,0) == 1 && !is_neg(u)) {
          return 1;
     }
     return 0;
}
