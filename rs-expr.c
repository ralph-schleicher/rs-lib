/* rs-expr.c --- arithmetic expressions.

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

/* Commentary:

   Here is a sort of YACC grammer for the implemented parser.

	%token NUM SYM FUN BIN VAR
	%right ? :
	%left + -
	%left * /
	%left NEG
	%right ^

	expr:	  NUM
		| expr ? expr : expr
		| expr + expr
		| expr - expr
		| expr * expr
		| expr / expr
		| expr ^ expr
		| + expr %prec NEG
		| - expr %prec NEG
		| ( expr )
		| SYM
		| FUN ( expr )
		| BIN ( expr , expr )
		| VAR
		;

   Hope it helps for understanding the code.  Or, in the words of the
   Jargon file:

	:recursion: See {recursion}.  */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "tsearch.h"
#include "rs-expr.h"

#if ! defined (__GNUC__) || (__GNUC__ < 2)
#ifndef __attribute__
#define __attribute__(arg)
#endif /* not __attribute__ */
#endif /* not __GNUC__ or __GNUC__ < 2 */

/* Mathematical constants.  */
#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Use this macro if an array subscript C is of type ‘char’.  */
#define CHAR_SUBSCRIPT(c) ((unsigned char) (c))

/* Set the ‘errno’ variable to the error code E
   and return back to the caller.  */
#define set_errno_and_return(e)						\
do									\
  {									\
    errno = (e);							\
    return;								\
  }									\
while (0)

/* Set the ‘errno’ variable to the error code E
   and return VAL back to the caller.  */
#define set_errno_and_return_value(e,val)				\
do									\
  {									\
    errno = (e);							\
    return (val);							\
  }									\
while (0)

struct node;
struct var;

/* Data structure for an arithmetic expression object.  */
struct expr
  {
    /* Root node.  */
    struct node *root;

    /* Flag for fatal errors.  */
    int err;

    /* Punctuation token character codes.  */
    int pos, neg, add, sub, mul, div, pow, beg, end, sep, if1, if2;

    /* Call-back function for scanning numeric word tokens.  */
    char *(*num) (void *__expr, char const *__start, int *__tok, double *__val);

    /* Call-back function for scanning symbolic word tokens.  */
    char *(*sym) (void *__expr, char const *__start, int *__tok, void **__ref);

    /* User data.  */
    void *data;

    /* Tree of symbolic variables.  */
    void *var;

    /* Call-back function for comparing symbolic variables.  */
    int (*compar) (void const *__a, void const *__b);

    /* Call-back function for evaluating symbolic variables.  */
    double (*eval) (void *__data);

    /* Call-back function for deleting symbolic variables.  */
    void (*free) (void *__data);

    /* Call-back function for traversing symbolic variables.  */
    void *func, *arg;
  };

/* Node of an arithmetic expression (a binary tree).  */
struct node
  {
    /* Token code.  */
    int tok;

    /* Numeric constant.  */
    double num;

    /* Reference to external symbols.  */
    union
      {
	/* Symbolic constant (variable reference).  */
	double *sym;

	/* Symbolic constant (function reference).  */
	double (*sy1) (void);

	/* Unary function.  */
	double (*fun) (double __x);

	/* Binary function.  */
	double (*bin) (double __x, double __y);

	/* Symbolic variable.  */
	struct var *var;
      }
    ref;

    /* Expression operands.  */
    struct node *op[3];
  };

/* Data structure for a symbolic variable.  */
struct var
  {
    /* Numeric value.  */
    double val;

    /* Arithmetic expression object.  */
    struct expr *expr;

    /* Client data structure.  */
    void *data;
  };

/* Mathematical constants.  The expressions for positive infinity and
   not-a-number may produce a compiler warning message.  */
#if defined (__GNUC__)
#define NEED_INIT 0
#else
#define NEED_INIT 1
#endif

#if NEED_INIT
static int init = 0;

double const rs_expr_e;
double const rs_expr_pi;
double const rs_expr_inf;
double const rs_expr_nan;
#else /* not NEED_INIT */
double const rs_expr_e = M_E;
double const rs_expr_pi = M_PI;
double const rs_expr_inf = 1.0 / 0.0;
double const rs_expr_nan = 0.0 / 0.0;
#endif /* not NEED_INIT */

/* Forward declarations.  */
static void delete_node (struct node *__node);
static double eval_node (struct node *__node);
static void delete_var (struct var *__var);
static int compare_var (void const *__a, void const *__b);
static struct node *parse_cond (struct expr *__expr, char **__end);
static struct node *parse_sum (struct expr *__expr, char **__end);
static struct node *parse_prod (struct expr *__expr, char **__end);
static struct node *parse_pow (struct expr *__expr, char **__end);
static struct node *parse_fac (struct expr *__expr, char **__end);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   Node handling.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Create a new node.

   If TOK is RS_EXPR_NUM, third argument is a floating-point number.
   If TOK is RS_EXPR_NEG, third argument is an expression.
   If TOK is RS_EXPR_ADD, RS_EXPR_SUB, RS_EXPR_MUL, RS_EXPR_DIV, or
    RS_EXPR_POW, third argument is the left-hand side expression, and
    fourth argument is the right-hand side expression.
   If TOK is RS_EXPR_SYM, third argument is a pointer to a number.
   If TOK is RS_EXPR_SY1, third argument is the address of a function.
   If TOK is RS_EXPR_FUN, third argument is the address of an unary
    function and fourth argument is the function argument (an expression).
   If TOK is RS_EXPR_BIN, third argument is the address of a binary
    function, fourth argument is the first function argument, and fifth
    argument is the second function argument.
   If TOK is RS_EXPR_VAR, third argument is the address of a variable.
   If TOK is RS_EXPR_IF1, third argument is the first operand, fourth
    argument is the second operand, and fifth argument is the third
    operand of a conditional expression.

   Return value is the address of the new node.

   Attention: In case of an error, a null pointer is returned, and
   expression arguments are destroyed!  This behavior avoids extra
   cleanup code in the callers.  */
static struct node *
new_node (struct expr *expr, int tok, ...)
{
  va_list arg_list;
  struct node *node;
  struct node *op;
  int i, c = 0;
  double val;
  void *ref;

  node = calloc (1, sizeof (struct node));
  if (node != NULL)
    {
      node->tok = tok;
      node->num = rs_expr_nan;
      node->ref.sym = NULL;
      node->op[0] = NULL;
      node->op[1] = NULL;
      node->op[2] = NULL;
    }

  va_start (arg_list, tok);
  switch (tok)
    {
    case RS_EXPR_NUM:

      val = va_arg (arg_list, double);
      if (node != NULL)
	node->num = val;

      break;

    case RS_EXPR_NEG:

      c = 1;
      break;

    case RS_EXPR_ADD:
    case RS_EXPR_SUB:
    case RS_EXPR_MUL:
    case RS_EXPR_DIV:
    case RS_EXPR_POW:

      c = 2;
      break;

    case RS_EXPR_SYM:

      ref = va_arg (arg_list, void *);
      if (node != NULL)
	node->ref.sym = ref;

      break;

    case RS_EXPR_SY1:

      ref = va_arg (arg_list, void *);
      if (node != NULL)
	node->ref.sy1 = ref;

      break;

    case RS_EXPR_FUN:

      ref = va_arg (arg_list, void *);
      if (node != NULL)
	node->ref.fun = ref;

      c = 1;
      break;

    case RS_EXPR_BIN:

      ref = va_arg (arg_list, void *);
      if (node != NULL)
	node->ref.bin = ref;

      c = 2;
      break;

    case RS_EXPR_VAR:

      ref = va_arg (arg_list, void *);
      if (node != NULL)
	node->ref.var = ref;

      break;

    case RS_EXPR_IF1:

      c = 3;
      break;

    default:

      /* Should not happen.  */
      abort ();
    }

  for (i = 0; i < c; ++i)
    {
      op = va_arg (arg_list, void *);
      if (node != NULL)
	node->op[i] = op;
      else
	delete_node (op);
    }

  va_end (arg_list);

  if (node == NULL)
    expr->err = 1;

  return node;
}

/* Delete a node (recursively).  */
static void
delete_node (struct node *node)
{
  if (node->op[0] != NULL)
    delete_node (node->op[0]);

  if (node->op[1] != NULL)
    delete_node (node->op[1]);

  if (node->op[2] != NULL)
    delete_node (node->op[2]);

  free (node);
}

/* Evaluate a conditional expression.  */
static double
eval_cond (struct node *cond, struct node *true, struct node *false)
{
  double val;

  val = eval_node (cond);
  if (val == 0.0)
    return eval_node (false);
  if (true != NULL)
    return eval_node (true);

  return val;
}

/* Evaluate a node (recursively).  */
static double
eval_node (struct node *node)
{
  switch (node->tok)
    {
    case RS_EXPR_NUM:

      return node->num;

    case RS_EXPR_NEG:

      return - eval_node (node->op[0]);

    case RS_EXPR_ADD:

      return eval_node (node->op[0]) + eval_node (node->op[1]);

    case RS_EXPR_SUB:

      return eval_node (node->op[0]) - eval_node (node->op[1]);

    case RS_EXPR_MUL:

      return eval_node (node->op[0]) * eval_node (node->op[1]);

    case RS_EXPR_DIV:

      return eval_node (node->op[0]) / eval_node (node->op[1]);

    case RS_EXPR_POW:

      return pow (eval_node (node->op[0]), eval_node (node->op[1]));

    case RS_EXPR_SYM:

      return *node->ref.sym;

    case RS_EXPR_SY1:

      return node->ref.sy1 ();

    case RS_EXPR_FUN:

      return node->ref.fun (eval_node (node->op[0]));

    case RS_EXPR_BIN:

      return node->ref.bin (eval_node (node->op[0]), eval_node (node->op[1]));

    case RS_EXPR_VAR:

      return node->ref.var->val;

    case RS_EXPR_IF1:

      return eval_cond (node->op[0], node->op[1], node->op[2]);

    default:

      /* Should not happen.  */
      abort ();
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   Symbolic variables.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Create a new symbolic variable.

   Return value is the address of the symbolic variable.

   Attention: In case of an error, a null pointer is returned, and the
   destructor for the client data structure is called!  This behavior
   avoids extra cleanup code in the callers.  If the symbolic variable
   already exists in the variable tree of EXPR, the destructor for the
   client data structure is called and the return value is the address
   of the existing symbolic variable.  */
static struct var *
new_var (struct expr *expr, void *data)
{
  struct var *var, **tem;

  var = calloc (1, sizeof (struct var));
  if (var != NULL)
    {
      var->val = rs_expr_nan;
      var->expr = expr;
      var->data = data;

      tem = tsearch (var, &expr->var, compare_var);
      if (tem == NULL)
	{
	  delete_var (var);
	  var = NULL;
	}
      else if (*tem != var)
	{
	  delete_var (var);
	  var = *tem;
	}
    }

  if (var == NULL)
    expr->err = 1;

  return var;
}

/* Delete a symbolic variable.  */
static void
delete_var (struct var *var)
{
  if (var->expr->free != NULL)
    var->expr->free (var->data);

  free (var);
}

/* Comparison function for symbolic variables.  */
static int
compare_var (void const *a, void const *b)
{
  struct var const *left = a;
  struct var const *right = b;

  return left->expr->compar (left->data, right->data);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   The parser.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Skip over whitespace characters.  */
static char *
forward (char const *point)
{
  while (isspace (CHAR_SUBSCRIPT (*point)))
    ++point;

  return (char *) point;
}

/* Convert the character C to a sign token code.  */
static int
tok_sign (struct expr *expr, int c)
{
  if (expr->pos != 0 && expr->pos == c)
    return RS_EXPR_POS;

  if (expr->neg != 0 && expr->neg == c)
    return RS_EXPR_NEG;

  return RS_EXPR_NUL;
}

/* Convert the character C to a conditional token code.  */
static int
tok_cond (struct expr *expr, int c)
{
  if (expr->if1 != 0 && expr->if1 == c)
    return RS_EXPR_IF1;

  if (expr->if2 != 0 && expr->if2 == c)
    return RS_EXPR_IF2;

  return RS_EXPR_NUL;
}

/* Convert the character C to a sum token code.  */
static int
tok_sum (struct expr *expr, int c)
{
  if (expr->add != 0 && expr->add == c)
    return RS_EXPR_ADD;

  if (expr->sub != 0 && expr->sub == c)
    return RS_EXPR_SUB;

  return RS_EXPR_NUL;
}

/* Convert the character C to a product token code.  */
static int
tok_prod (struct expr *expr, int c)
{
  if (expr->mul != 0 && expr->mul == c)
    return RS_EXPR_MUL;

  if (expr->div != 0 && expr->div == c)
    return RS_EXPR_DIV;

  return RS_EXPR_NUL;
}

/* Convert the character C to a power token code.  */
static int
tok_pow (struct expr *expr, int c)
{
  if (expr->pow != 0 && expr->pow == c)
    return RS_EXPR_POW;

  return RS_EXPR_NUL;
}

/* Parse an expression.  */
static struct node *
parse_expr (struct expr *expr, char **end)
{
  return parse_cond (expr, end);
}

/* Parse a conditional.  */
static struct node *
parse_cond (struct expr *expr, char **end)
{
  struct node *cond;
  struct node *true;
  struct node *false;
  char *point, *mark;
  int tok;

  cond = parse_sum (expr, end);
  if (cond == NULL)
    return NULL;

  true = NULL;
  mark = *end;
  point = forward (mark);
  tok = tok_cond (expr, *point);

  while (tok == RS_EXPR_IF1)
    {
      ++point;

      /* The middle operand in a conditional expression may be omitted.
	 Then if the first operand is non-zero, its value is the value
	 of the conditional expression.  */
      true = parse_cond (expr, &point);

      point = forward (point);
      tok = tok_cond (expr, *point);

      if (tok != RS_EXPR_IF2)
	break;

      ++point;

      false = parse_cond (expr, &point);
      if (false == NULL)
	break;

      cond = new_node (expr, RS_EXPR_IF1, cond, true, false);
      if (cond == NULL)
	return NULL;

      true = NULL;
      mark = point;
      point = forward (mark);
      tok = tok_cond (expr, *point);
    }

  if (true != NULL)
    delete_node (true);

  *end = mark;
  return cond;
}

/* Parse a sum.  */
static struct node *
parse_sum (struct expr *expr, char **end)
{
  struct node *left;
  struct node *right;
  char *point, *mark;
  int tok;

  left = parse_prod (expr, end);
  if (left == NULL)
    return NULL;

  mark = *end;
  point = forward (mark);
  tok = tok_sum (expr, *point);

  while (tok != RS_EXPR_NUL)
    {
      ++point;

      right = parse_prod (expr, &point);
      if (right == NULL)
	break;

      left = new_node (expr, tok, left, right);
      if (left == NULL)
	return NULL;

      mark = point;
      point = forward (mark);
      tok = tok_sum (expr, *point);
    }

  *end = mark;
  return left;
}

/* Parse a product.  */
static struct node *
parse_prod (struct expr *expr, char **end)
{
  struct node *left;
  struct node *right;
  char *point, *mark;
  int tok;

  left = parse_pow (expr, end);
  if (left == NULL)
    return NULL;

  mark = *end;
  point = forward (mark);
  tok = tok_prod (expr, *point);

  while (tok != RS_EXPR_NUL)
    {
      ++point;

      right = parse_pow (expr, &point);
      if (right == NULL)
	break;

      left = new_node (expr, tok, left, right);
      if (left == NULL)
	return NULL;

      mark = point;
      point = forward (mark);
      tok = tok_prod (expr, *point);
    }

  *end = mark;
  return left;
}

/* Parse a power.  */
static struct node *
parse_pow (struct expr *expr, char **end)
{
  struct node *left;
  struct node *right;
  char *point, *mark;
  int tok;

  left = parse_fac (expr, end);
  if (left == NULL)
    return NULL;

  mark = *end;
  point = forward (mark);
  tok = tok_pow (expr, *point);

  while (tok != RS_EXPR_NUL)
    {
      ++point;

      /* Right operator associativity!  */
      right = parse_pow (expr, &point);
      if (right == NULL)
	break;

      left = new_node (expr, tok, left, right);
      if (left == NULL)
	return NULL;

      mark = point;
      point = forward (mark);
      tok = tok_pow (expr, *point);
    }

  *end = mark;
  return left;
}

/* Parse a factor.  */
static struct node *
parse_fac (struct expr *expr, char **end)
{
  struct node *node;
  struct var *var;
  char *point;
  int tok;

  point = forward (*end);

  /* Unary plus or minus.  */
  tok = tok_sign (expr, *point);
  if (tok != RS_EXPR_NUL)
    {
      ++point;

      /* Negation has lower precedence than exponentiation.  */
      node = parse_pow (expr, &point);
      if (node == NULL)
	return NULL;

      /* Discard unary plus.  */
      if (tok == RS_EXPR_NEG)
	{
	  node = new_node (expr, tok, node);
	  if (node == NULL)
	    return NULL;
	}

      *end = point;
      return node;
    }

  /* Parenthesized expression.  */
  if (*point == expr->beg)
    {
      ++point;

      node = parse_expr (expr, &point);
      if (node == NULL)
	return NULL;

      point = forward (point);
      if (*point != expr->end)
	{
	  delete_node (node);
	  return NULL;
	}

      ++point;

      *end = point;
      return node;
    }

  /* Numeric word token.

     Cause an abnormal program termination if
     the call-back function is broken.  */
  if (expr->num != NULL)
    {
      char *tem;
      double val;

      tok = RS_EXPR_NUL;
      val = 0.0;

      tem = expr->num (expr, point, &tok, &val);
      if (tem == NULL)
	{
	  expr->err = 1;
	  return NULL;
	}

      if (tem != point)
	{
	  switch (tok)
	    {
	    case RS_EXPR_NUM:

	      node = new_node (expr, tok, val);
	      if (node == NULL)
		return NULL;

	      break;

	    default:

	      abort ();
	    }

	  *end = tem;
	  return node;
	}
    }

  /* Symbolic word token.

     Cause an abnormal program termination if
     the call-back function is broken.  */
  if (expr->sym != NULL)
    {
      struct node *x;
      struct node *y;
      char *tem;
      void *ref;

      tok = RS_EXPR_NUL;
      ref = NULL;

      tem = expr->sym (expr, point, &tok, &ref);
      if (tem == NULL)
	{
	  expr->err = 1;
	  return NULL;
	}

      if (tem != point)
	{
	  if (ref == NULL)
	    abort ();

	  switch (tok)
	    {
	    case RS_EXPR_SYM:
	    case RS_EXPR_SY1:

	      node = new_node (expr, tok, ref);
	      if (node == NULL)
		return NULL;

	      break;

	    case RS_EXPR_FUN:

	      tem = forward (tem);
	      if (*tem != expr->beg)
		return NULL;

	      ++tem;

	      x = parse_expr (expr, &tem);
	      if (x == NULL)
		return NULL;

	      tem = forward (tem);
	      if (*tem != expr->end)
		{
		  delete_node (x);
		  return NULL;
		}

	      ++tem;

	      node = new_node (expr, tok, ref, x);
	      if (node == NULL)
		return NULL;

	      break;

	    case RS_EXPR_BIN:

	      tem = forward (tem);
	      if (*tem != expr->beg)
		return NULL;

	      ++tem;

	      x = parse_expr (expr, &tem);
	      if (x == NULL)
		return NULL;

	      tem = forward (tem);
	      if (*tem != expr->sep)
		{
		  delete_node (x);
		  return NULL;
		}

	      ++tem;

	      y = parse_expr (expr, &tem);
	      if (y == NULL)
		{
		  delete_node (x);
		  return NULL;
		}

	      tem = forward (tem);
	      if (*tem != expr->end)
		{
		  delete_node (x);
		  delete_node (y);

		  return NULL;
		}

	      ++tem;

	      node = new_node (expr, tok, ref, x, y);
	      if (node == NULL)
		return NULL;

	      break;

	    case RS_EXPR_VAR:

	      var = new_var (expr, ref);
	      if (var == NULL)
		return NULL;

	      node = new_node (expr, tok, var);
	      if (node == NULL)
		return NULL;

	      break;

	    default:

	      abort ();
	    }

	  *end = tem;
	  return node;
	}
    }

  return NULL;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   Control requests.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Set the character for the punctuation token TOK to C.  */
static int
set_punct_char (struct expr *expr, va_list argp)
{
  int tok, c;

  tok = va_arg (argp, int);
  c = va_arg (argp, int);

  switch (tok)
    {
    case RS_EXPR_POS:
    case RS_EXPR_NEG:
    case RS_EXPR_ADD:
    case RS_EXPR_SUB:
    case RS_EXPR_MUL:
    case RS_EXPR_DIV:
    case RS_EXPR_POW:
    case RS_EXPR_IF1:
    case RS_EXPR_IF2:

      /* Token can be disabled.  */
      if (c == 0)
	break;

    case RS_EXPR_BEG:
    case RS_EXPR_END:
    case RS_EXPR_SEP:

      if (ispunct (c))
	break;

    default:

      set_errno_and_return_value (EINVAL, -1);
    }

  switch (tok)
    {
    case RS_EXPR_POS: expr->pos = c; break;
    case RS_EXPR_NEG: expr->neg = c; break;
    case RS_EXPR_ADD: expr->add = c; break;
    case RS_EXPR_SUB: expr->sub = c; break;
    case RS_EXPR_MUL: expr->mul = c; break;
    case RS_EXPR_DIV: expr->div = c; break;
    case RS_EXPR_POW: expr->pow = c; break;
    case RS_EXPR_BEG: expr->beg = c; break;
    case RS_EXPR_END: expr->end = c; break;
    case RS_EXPR_SEP: expr->sep = c; break;
    case RS_EXPR_IF1: expr->if1 = c; break;
    case RS_EXPR_IF2: expr->if2 = c; break;
    }

  return 0;
}

/* Set function for scanning numeric word tokens.  */
static int
set_scan_num (struct expr *expr, va_list argp)
{
  expr->num = va_arg (argp, void *);

  return 0;
}

/* Set function for scanning symbolic word tokens.  */
static int
set_scan_sym (struct expr *expr, va_list argp)
{
  expr->sym = va_arg (argp, void *);

  return 0;
}

/* Set comparison function for symbolic variables.  */
static int
set_compar_var (struct expr *expr, va_list argp)
{
  expr->compar = va_arg (argp, void *);

  return 0;
}

/* Set evaluation function for symbolic variables.  */
static int
set_eval_var (struct expr *expr, va_list argp)
{
  expr->eval = va_arg (argp, void *);

  return 0;
}

/* Set destructor function for symbolic variables.  */
static int
set_free_var (struct expr *expr, va_list argp)
{
  expr->free = va_arg (argp, void *);

  return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   Arithmetic expression objects.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize an arithmetic expression.  */
static void
init_expr (struct expr *expr)
{
  expr->root = NULL;

  expr->err = 0;

  expr->pos = '+';
  expr->neg = '-';
  expr->add = '+';
  expr->sub = '-';
  expr->mul = '*';
  expr->div = '/';
  expr->pow = '^';
  expr->beg = '(';
  expr->end = ')';
  expr->sep = ',';
  expr->if1 = '?';
  expr->if2 = ':';

  expr->num = rs_expr_scan_num;
  expr->sym = rs_expr_scan_sym;

  expr->data = NULL;
  expr->var = NULL;
  expr->compar = NULL;
  expr->eval = NULL;
  expr->free = NULL;
  expr->func = NULL;
  expr->arg = NULL;
}

/* Terminate an arithmetic expression.  */
static void
term_expr (struct expr *expr)
{
  if (expr->root != NULL)
    delete_node (expr->root);

  if (expr->var != NULL)
    tdestroy (expr->var, (void (*) (void *)) delete_var);

  expr->root = NULL;
  expr->var = NULL;
}

/* Create an arithmetic expression.  */
void *
rs_expr_new (void)
{
  struct expr *expr;

  expr = calloc (1, sizeof (struct expr));
  if (expr != NULL)
    init_expr (expr);

  return expr;
}

/* Delete an arithmetic expression.  */
void
rs_expr_delete (void *object)
{
  struct expr *expr;

  expr = (struct expr *) object;
  if (expr == NULL)
    return;

  term_expr (expr);
  free (expr);
}

/* Set user data structure.  */
void
rs_expr_set_data (void *object, void *data)
{
  struct expr *expr;

  expr = (struct expr *) object;
  if (expr == NULL)
    set_errno_and_return (EINVAL);

  expr->data = data;
}

/* Get user data structure.  */
void *
rs_expr_get_data (void *object)
{
  struct expr *expr;

  expr = (struct expr *) object;
  if (expr == NULL)
    set_errno_and_return_value (EINVAL, NULL);

  return expr->data;
}

/* Perform an arithmetic expression control request.  */
int
rs_expr_control (void *object, int i, ...)
{
  static int (*f[RS_EXPR_CONTROLS]) (struct expr *, va_list) =
    {
      set_punct_char,
      set_scan_num,
      set_scan_sym,
      set_compar_var,
      set_eval_var,
      set_free_var,
    };

  struct expr *expr;
  va_list argp;
  int ans;

  expr = (struct expr *) object;
  if (expr == NULL)
    set_errno_and_return_value (EINVAL, -1);

  if (i < 0 || i >= RS_EXPR_CONTROLS)
    set_errno_and_return_value (EINVAL, -1);

  va_start (argp, i);
  ans = f[i] (expr, argp);
  va_end (argp);

  return ans;
}

/* Convert the initial part of STRING to an arithmetic expression.  */
void *
rs_expr_scan (void *object, char const *string, char **end)
{
  struct expr *expr;
  char *tail, *nul;

  /* Initialize the tail pointer and set END to the value of STRING.  */
  tail = (char *) string;
  if (end != NULL)
    *end = tail;

  if (string == NULL)
    set_errno_and_return_value (EINVAL, NULL);

  /* Attempt to parse an arithmetic expression.  */
  expr = (struct expr *) object;
  if (expr == NULL)
    {
      expr = rs_expr_new ();
      if (expr == NULL)
	return NULL;
    }
  else
    {
      term_expr (expr);
      expr->err = 0;
    }

  expr->root = parse_expr (expr, &tail);

  /* No arithmetic expression.  */
  if (expr->err == 0 && tail == string)
    expr->err = 1;

  /* No tail pointer, remaining string has to be blank.  */
  if (expr->err == 0 && end == NULL)
    {
      nul = forward (tail);
      if (*nul != 0)
	expr->err = 1;
    }

  if (expr->err != 0)
    {
      if (object == NULL)
	rs_expr_delete (expr);

      return NULL;
    }

  /* Store pointer to tail of the string in *END.  */
  if (end != NULL)
    *end = tail;

  return expr;
}

/* Default scanner for numeric word tokens.  */
char *
rs_expr_scan_num  (void *object, char const *start, int *tok, double *val)
{
  char *end;
  int tem;

  /* Not used.  */
  (void) object;

  if (! isdigit (CHAR_SUBSCRIPT (*start)))
    return (char *) start;

  tem = errno;
  errno = 0;

  *val = strtod (start, &end);
  if (errno != 0 && errno != ERANGE)
    set_errno_and_return_value (tem, (char *) start);

  errno = tem;
  *tok = RS_EXPR_NUM;

  return end;
}

/* Default scanner for symbolic word tokens.  */
#include "rs-expr.gperf.c"

char *
rs_expr_scan_sym (void *object, char const *start, int *tok, void **ref)
{
  struct rs_expr_sym *sym;
  char *end;
  int len;

  object = NULL;

  if (! isalpha (CHAR_SUBSCRIPT (*start)))
    return (char *) start;

  end = (char *) start;
  while (isalnum (CHAR_SUBSCRIPT (*end)))
    ++end;

  len = (end - start);

  sym = (struct rs_expr_sym *) rs_expr_sym1_lookup (start, len);
  if (sym != NULL)
    {
      *tok = sym->e_tok;
      *ref = sym->e_ref;

      return end;
    }

  return (char *) start;
}

/* Evaluate an arithmetic expression.  */
static void
eval_var (void const *a, VISIT order, int level __attribute__ ((unused)))
{
  struct var *var = *((struct var **) a);

  if (order == preorder || order == leaf)
    var->val = var->expr->eval (var->data);
}

double
rs_expr_eval (void *object)
{
  struct expr *expr;

  expr = (struct expr *) object;
  if (expr == NULL || expr->root == NULL)
    set_errno_and_return_value (EINVAL, 0.0);

#if NEED_INIT

  /* Visual C issues an error message if you attempt to divide
     by zero.  */
  if (init == 0)
    {
      double *p, z = 0.0;

      p = (double *) &rs_expr_e; *p = M_E;
      p = (double *) &rs_expr_pi; *p = M_PI;
      p = (double *) &rs_expr_inf; *p = 1.0 / z;
      p = (double *) &rs_expr_nan; *p = 0.0 / z;

      init = 1;
    }

#endif /* NEED_INIT */

  if (expr->var != NULL)
    twalk (expr->var, eval_var);

  return eval_node (expr->root);
}

/* Call FUNC for each known symbolic variable.  */
static void
walk_var_1 (void const *a, VISIT order, int level __attribute__ ((unused)))
{
  struct var *var = *((struct var **) a);

  if (order == preorder || order == leaf)
    ((void (*) (void *)) var->expr->func) (var->data);
}

static void
walk_var_2 (void const *a, VISIT order, int level __attribute__ ((unused)))
{
  struct var *var = *((struct var **) a);

  if (order == preorder || order == leaf)
    ((void (*) (void *, void *)) var->expr->func) (var->data, var->expr->arg);
}

void
rs_expr_walk_var (void *object, void *func, void *arg)
{
  struct expr *expr;

  expr = (struct expr *) object;
  if (expr == NULL || func == NULL)
    set_errno_and_return (EINVAL);

  expr->func = func;
  expr->arg = arg;

  if (arg == NULL)
    twalk (expr->var, walk_var_1);
  else
    twalk (expr->var, walk_var_2);
}
