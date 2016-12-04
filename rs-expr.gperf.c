/* C code produced by gperf version 3.0.4 */
/* Command-line: gperf rs-expr.gperf  */
/* Computed positions: -k'1-2,4' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "rs-expr.gperf"

/* rs-expr.gperf --- symbol table for arithmetic expressions.	-*- C -*-

   Copyright (C) 1993 Ralph Schleicher

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in
        the documentation and/or other materials provided with the
        distribution.

      * Neither the name of the copyright holder nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.  */

#if defined (__GLIBC__) && __GLIBC__ >= 2
#ifndef HAVE_TRUNC
#define HAVE_TRUNC 1
#endif
#ifndef HAVE_DREM
#define HAVE_DREM 1
#endif
#ifndef HAVE_REMAINDER
#define HAVE_REMAINDER 1
#endif
#ifndef HAVE_FMIN
#define HAVE_FMIN 1
#endif
#ifndef HAVE_FMAX
#define HAVE_FMAX 1
#endif
#ifndef HAVE_LOG10
#define HAVE_LOG10 1
#endif
#ifndef HAVE_LOG2
#define HAVE_LOG2 1
#endif
#ifndef HAVE_CBRT
#define HAVE_CBRT 1
#endif
#ifndef HAVE_HYPOT
#define HAVE_HYPOT 1
#endif
#ifndef HAVE_ASINH
#define HAVE_ASINH 1
#endif
#ifndef HAVE_ACOSH
#define HAVE_ACOSH 1
#endif
#ifndef HAVE_ATANH
#define HAVE_ATANH 1
#endif
#ifndef HAVE_ERF
#define HAVE_ERF 1
#endif
#ifndef HAVE_ERFC
#define HAVE_ERFC 1
#endif
#ifndef HAVE_TGAMMA
#define HAVE_TGAMMA 1
#endif
#ifndef HAVE_LGAMMA
#define HAVE_LGAMMA 1
#endif
#endif /* __GLIBC__ */

#if defined (__CYGWIN__) || defined (__MINGW32__)
#ifndef HAVE_TRUNC
#define HAVE_TRUNC 1
#endif
#ifndef HAVE_REMAINDER
#define HAVE_REMAINDER 1
#endif
#ifndef HAVE_FMIN
#define HAVE_FMIN 1
#endif
#ifndef HAVE_FMAX
#define HAVE_FMAX 1
#endif
#ifndef HAVE_LOG10
#define HAVE_LOG10 1
#endif
#ifndef HAVE_LOG2
#define HAVE_LOG2 1
#endif
#ifndef HAVE_CBRT
#define HAVE_CBRT 1
#endif
#ifndef HAVE_HYPOT
#define HAVE_HYPOT 1
#endif
#ifndef HAVE_ASINH
#define HAVE_ASINH 1
#endif
#ifndef HAVE_ACOSH
#define HAVE_ACOSH 1
#endif
#ifndef HAVE_ATANH
#define HAVE_ATANH 1
#endif
#ifndef HAVE_ERF
#define HAVE_ERF 1
#endif
#ifndef HAVE_ERFC
#define HAVE_ERFC 1
#endif
#ifndef HAVE_TGAMMA
#define HAVE_TGAMMA 1
#endif
#ifndef HAVE_LGAMMA
#define HAVE_LGAMMA 1
#endif
#endif /* __CYGWIN__ or __MINGW32__ */

#ifdef _MSC_VER
#ifndef HAVE_HYPOT
#define HAVE_HYPOT 1
#define hypot _hypot
#endif
#define copysign _copysign
#endif /* _MSC_VER */

/* Truncation function.  */
#if HAVE_TRUNC
#else /* not HAVE_TRUNC */
static double
trunc (double x)
{
  if (x == x)
    {
      if (x > 0.0)
	return floor (x);

      if (x < 0.0)
	return ceil (x);
    }

  return x;
}
#endif /* not HAVE_TRUNC */

/* Remainder function.  */
#if HAVE_DREM
#else /* not HAVE_DREM */
#if HAVE_REMAINDER
#define drem remainder
#else /* not HAVE_REMAINDER */
#define drem fmod
#endif /* not HAVE_REMAINDER */
#endif /* not HAVE_DREM */

/* Minimum function.  */
#if HAVE_FMIN
#else /* not HAVE_FMIN */
static double
fmin (double x, double y)
{
  if (x != x || y != y)
    return rs_expr_nan;

  return (x < y) ? x : y;
}
#endif /* not HAVE_FMIN */

/* Maximum function.  */
#if HAVE_FMAX
#else /* not HAVE_FMAX */
static double
fmax (double x, double y)
{
  if (x != x || y != y)
    return rs_expr_nan;

  return (x > y) ? x : y;
}
#endif /* not HAVE_FMAX */

/* Decadic logarithm function.  */
#if HAVE_LOG10
#else /* not HAVE_LOG10 */
#ifndef M_LOG10E
#define M_LOG10E 0.43429448190325182765
#endif
static double
log10 (double x)
{
  return log (x) * M_LOG10E;
}
#endif /* not HAVE_LOG10 */

/* Binary logarithm function.  */
#if HAVE_LOG2
#else /* not HAVE_LOG2 */
#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634074
#endif
static double
log2 (double x)
{
  return log (x) * M_LOG2E;
}
#endif /* not HAVE_LOG2 */

/* Cube root function.  */
#if HAVE_CBRT
#else /* not HAVE_CBRT */
static double
cbrt (double x)
{
  if (x != x)
    return x;

  return copysign (pow (fabs (x), 1.0 / 3.0), x);
}
#endif /* not HAVE_CBRT */

/* Hypotenuse function.  */
#if HAVE_HYPOT
#else /* not HAVE_HYPOT */
#if HAVE_LIBGSL
#define hypot gsl_hypot
#else /* not HAVE_LIBGSL */
static double
hypot (double x, double y)
{
  double a, b;

  if (x != x || y != y)
    return rs_expr_nan;

  x = fabs (x);
  y = fabs (y);

  if (x > y)
    {
      a = x;
      b = y / x;
    }
  else
    {
      if (y == 0.0)
	return 0.0;

      a = y;
      b = x / y;
    }

  return a * sqrt (1.0 + b * b);
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_HYPOT */

/* Inverse hyperbolic sine function.  */
#if HAVE_ASINH
#else /* not HAVE_ASINH */
#if HAVE_LIBGSL
#define asinh gsl_asinh
#else /* not HAVE_LIBGSL */
static double
asinh (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_ASINH */

/* Inverse hyperbolic cosine function.  */
#if HAVE_ACOSH
#else /* not HAVE_ACOSH */
#if HAVE_LIBGSL
#define acosh gsl_acosh
#else /* not HAVE_LIBGSL */
static double
acosh (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_ACOSH */

/* Inverse hyperbolic tangent function.  */
#if HAVE_ATANH
#else /* not HAVE_ATANH */
#if HAVE_LIBGSL
#define atanh gsl_atanh
#else /* not HAVE_LIBGSL */
static double
atanh (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_ATANH */

/* Error function.  */
#if HAVE_ERF
#else /* not HAVE_ERF */
#if HAVE_LIBGSL
#define erf gsl_sf_erf
#else /* not HAVE_LIBGSL */
static double
erf (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_ERF */

/* Complementary error function.  */
#if HAVE_ERFC
#else /* not HAVE_ERFC */
#if HAVE_LIBGSL
#define erfc gsl_sf_erfc
#else /* not HAVE_LIBGSL */
static double
erfc (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_ERFC */

/* Gamma function.  */
#if HAVE_TGAMMA
#else /* not HAVE_TGAMMA */
#if HAVE_LIBGSL
#define tgamma gsl_sf_gamma
#else /* not HAVE_LIBGSL */
#if HAVE_LGAMMA
static double
tgamma (double x)
{
  double tem = lgamma (x);
  return signgam * exp (tem);
}
#else /* not HAVE_LGAMMA */
static double
tgamma (double x __attribute__ ((unused)))
{
  return rs_expr_nan;
}
#endif /* not HAVE_LGAMMA */
#endif /* not HAVE_LIBGSL */
#endif /* not HAVE_TGAMMA */
#line 365 "rs-expr.gperf"
struct rs_expr_sym;
/* maximum key range = 85, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
rs_expr_sym1_hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86,  0,
      25, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 10, 30,  5,
       0,  0, 25,  0, 15, 40,  5, 86, 10, 35,
       5, 20, 15, 86, 30,  0, 15,  0, 86, 86,
      86, 10,  0, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86, 86, 86, 86,
      86, 86, 86, 86, 86, 86, 86
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]+1];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct rs_expr_sym *
rs_expr_sym1_lookup (str, len)
     register const char *str;
     register unsigned int len;
{
  enum
    {
      TOTAL_KEYWORDS = 47,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 85
    };

  static const struct rs_expr_sym wordlist[] =
    {
      {"",RS_EXPR_NUL,NULL},
#line 375 "rs-expr.gperf"
      {"e",       RS_EXPR_SYM, (void *) &rs_expr_e},
      {"",RS_EXPR_NUL,NULL},
#line 419 "rs-expr.gperf"
      {"erf",     RS_EXPR_FUN, (void *) erf},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
#line 388 "rs-expr.gperf"
      {"sin",     RS_EXPR_FUN, (void *) sin},
#line 420 "rs-expr.gperf"
      {"erfc",    RS_EXPR_FUN, (void *) erfc},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
#line 405 "rs-expr.gperf"
      {"ld",      RS_EXPR_FUN, (void *) log2},
#line 398 "rs-expr.gperf"
      {"exp",     RS_EXPR_FUN, (void *) exp},
#line 393 "rs-expr.gperf"
      {"acos",    RS_EXPR_FUN, (void *) acos},
#line 415 "rs-expr.gperf"
      {"acosh",   RS_EXPR_FUN, (void *) acosh},
#line 392 "rs-expr.gperf"
      {"arcsin",  RS_EXPR_FUN, (void *) asin},
#line 404 "rs-expr.gperf"
      {"lb",      RS_EXPR_FUN, (void *) log2},
#line 379 "rs-expr.gperf"
      {"abs",     RS_EXPR_FUN, (void *) fabs},
#line 395 "rs-expr.gperf"
      {"atan",    RS_EXPR_FUN, (void *) atan},
#line 417 "rs-expr.gperf"
      {"atanh",   RS_EXPR_FUN, (void *) atanh},
#line 394 "rs-expr.gperf"
      {"arccos",  RS_EXPR_FUN, (void *) acos},
#line 376 "rs-expr.gperf"
      {"pi",      RS_EXPR_SYM, (void *) &rs_expr_pi},
#line 389 "rs-expr.gperf"
      {"cos",     RS_EXPR_FUN, (void *) cos},
#line 410 "rs-expr.gperf"
      {"sinh",    RS_EXPR_FUN, (void *) sinh},
#line 383 "rs-expr.gperf"
      {"trunc",   RS_EXPR_FUN, (void *) trunc},
#line 418 "rs-expr.gperf"
      {"artanh",  RS_EXPR_FUN, (void *) atanh},
#line 402 "rs-expr.gperf"
      {"lg",      RS_EXPR_FUN, (void *) log10},
#line 399 "rs-expr.gperf"
      {"log",     RS_EXPR_FUN, (void *) log},
#line 408 "rs-expr.gperf"
      {"cbrt",    RS_EXPR_FUN, (void *) cbrt},
#line 401 "rs-expr.gperf"
      {"log10",   RS_EXPR_FUN, (void *) log10},
#line 396 "rs-expr.gperf"
      {"arctan",  RS_EXPR_FUN, (void *) atan},
#line 400 "rs-expr.gperf"
      {"ln",      RS_EXPR_FUN, (void *) log},
#line 406 "rs-expr.gperf"
      {"pow",     RS_EXPR_BIN, (void *) pow},
#line 391 "rs-expr.gperf"
      {"asin",    RS_EXPR_FUN, (void *) asin},
#line 413 "rs-expr.gperf"
      {"asinh",   RS_EXPR_FUN, (void *) asinh},
#line 416 "rs-expr.gperf"
      {"arcosh",  RS_EXPR_FUN, (void *) acosh},
      {"",RS_EXPR_NUL,NULL},
#line 378 "rs-expr.gperf"
      {"nan",     RS_EXPR_SYM, (void *) &rs_expr_nan},
#line 411 "rs-expr.gperf"
      {"cosh",    RS_EXPR_FUN, (void *) cosh},
#line 409 "rs-expr.gperf"
      {"hypot",   RS_EXPR_BIN, (void *) hypot},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
#line 386 "rs-expr.gperf"
      {"min",     RS_EXPR_BIN, (void *) fmin},
#line 380 "rs-expr.gperf"
      {"ceil",    RS_EXPR_FUN, (void *) ceil},
#line 397 "rs-expr.gperf"
      {"angle",   RS_EXPR_BIN, (void *) atan2},
      {"",RS_EXPR_NUL,NULL},
#line 381 "rs-expr.gperf"
      {"ceiling", RS_EXPR_FUN, (void *) ceil},
#line 390 "rs-expr.gperf"
      {"tan",     RS_EXPR_FUN, (void *) tan},
#line 407 "rs-expr.gperf"
      {"sqrt",    RS_EXPR_FUN, (void *) sqrt},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL},
#line 384 "rs-expr.gperf"
      {"mod",     RS_EXPR_BIN, (void *) fmod},
#line 403 "rs-expr.gperf"
      {"log2",    RS_EXPR_FUN, (void *) log2},
      {"",RS_EXPR_NUL,NULL},
#line 414 "rs-expr.gperf"
      {"arsinh",  RS_EXPR_FUN, (void *) asinh},
      {"",RS_EXPR_NUL,NULL},
#line 385 "rs-expr.gperf"
      {"rem",     RS_EXPR_BIN, (void *) drem},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
#line 377 "rs-expr.gperf"
      {"inf",     RS_EXPR_SYM, (void *) &rs_expr_inf},
#line 412 "rs-expr.gperf"
      {"tanh",    RS_EXPR_FUN, (void *) tanh},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL},
#line 387 "rs-expr.gperf"
      {"max",     RS_EXPR_BIN, (void *) fmax},
      {"",RS_EXPR_NUL,NULL},
#line 421 "rs-expr.gperf"
      {"gamma",   RS_EXPR_FUN, (void *) tgamma},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
      {"",RS_EXPR_NUL,NULL}, {"",RS_EXPR_NUL,NULL},
#line 382 "rs-expr.gperf"
      {"floor",   RS_EXPR_FUN, (void *) floor}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = rs_expr_sym1_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].e_name;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
