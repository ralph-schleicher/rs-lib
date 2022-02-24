/* rs-expr.h --- arithmetic expressions

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

#ifndef RS_EXPR_H
#define RS_EXPR_H

#ifdef __cplusplus
#define RS_EXPR_BEGIN_DECL extern "C" {
#define RS_EXPR_END_DECL }
#else /* not __cplusplus */
#define RS_EXPR_BEGIN_DECL
#define RS_EXPR_END_DECL
#endif /* not __cplusplus */

RS_EXPR_BEGIN_DECL

/* Symbolic token codes.  */
enum
  {
    RS_EXPR_NUL,
    RS_EXPR_NUM,
    RS_EXPR_POS,
    RS_EXPR_NEG,
    RS_EXPR_ADD,
    RS_EXPR_SUB,
    RS_EXPR_MUL,
    RS_EXPR_DIV,
    RS_EXPR_POW,
    RS_EXPR_BEG,
    RS_EXPR_END,
    RS_EXPR_SEP,
    RS_EXPR_SYM,
    RS_EXPR_SY1,
    RS_EXPR_FUN,
    RS_EXPR_BIN,
    RS_EXPR_VAR,
    RS_EXPR_IF1,
    RS_EXPR_IF2
  };

/* Control request commands.  */
enum
  {
    RS_EXPR_PUNCT_CHAR,
    RS_EXPR_SCAN_NUM,
    RS_EXPR_SCAN_SYM,
    RS_EXPR_COMPAR_VAR,
    RS_EXPR_EVAL_VAR,
    RS_EXPR_FREE_VAR,

    RS_EXPR_CONTROLS
  };

/* Utility structure for scanning symbolic word tokens.  */
struct rs_expr_sym
  {
    /* Symbol name.  */
    char const *e_name;

    /* Token code.  */
    int e_tok;

    /* Reference.  */
    void *e_ref;
  };

/* Base of the natural logarithm.  */
extern double const rs_expr_e;

/* Ratio of a circle's circumference to its diameter.  */
extern double const rs_expr_pi;

/* Positive infinity.  */
extern double const rs_expr_inf;

/* Not-a-number.  */
extern double const rs_expr_nan;

/* Create an arithmetic expression.  */
extern void *rs_expr_new (void);

/* Destroy an arithmetic expression.  */
extern void rs_expr_delete (void *__expr);

/* Set user data structure.  */
extern void rs_expr_set_data (void *__expr, void *__data);

/* Get user data structure.  */
extern void *rs_expr_get_data (void *__expr);

/* Perform an arithmetic expression control request.  */
extern int rs_expr_control (void *__expr, int __command, ...);

/* Convert the initial part of STRING to an arithmetic expression.  */
extern void *rs_expr_scan (void *__expr, char const *__string, char **__end);

/* The default scanner for numeric word tokens.  */
extern char *rs_expr_scan_num (void *__expr, char const *__start, int *__tok, double *__val);

/* The default scanner for symbolic word tokens.  */
extern char *rs_expr_scan_sym (void *__expr, char const *__start, int *__tok, void **__ref);

/* Evaluate an arithmetic expression.  */
extern double rs_expr_eval (void *__expr);

/* Call FUNC for each known symbolic variable.  */
extern void rs_expr_walk_var (void *__expr, void *__func, void *__arg);

RS_EXPR_END_DECL

#endif /* not RS_EXPR_H */
