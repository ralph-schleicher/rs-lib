/* rs-string.c --- string functions

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "rs-string.h"

/* Configuration settings.

   RS_STRING_CHAR      Data type ‘char’.
   RS_STRING_WCHAR_T   Data type ‘wchar_t’.  */
#undef CHARP
#ifdef RS_STRING_CHAR
#define CHARP 1
#else
#define CHARP 0
#endif

#undef WCHARP
#ifdef RS_STRING_WCHAR_T
#define WCHARP 1
#else
#define WCHARP 0
#endif

/* Check settings.  */
#if (CHARP && WCHARP)
#error "Configuration is ambiguous."
#endif

#if ! (CHARP || WCHARP)
/* Default to ‘char’.  */
#undef CHARP
#define CHARP 1
#endif

/* Some macros for making most of the code homogeneous.

   CHAR                Data type of a character.
   INT                 Data type of an integer.
   LABEL               Data type identifier for symbols.
   GENPRE              Prefix of a generic string function.
   GENPOST             Suffix of a generic string function.
   LEN(s)              length of the string S.  */
#if CHARP
#define CHAR char
#define INT int
#define LABEL
#define GENPRE str
#define GENPOST
#else /* not CHARP */
#if WCHARP
#define CHAR wchar_t
#define INT wint_t
#define LABEL w
#define GENPRE wcs
#define GENPOST
#else /* not WCHARP */
#error "Fix me!"
#endif /* not WCHARP */
#endif /* not CHARP */
#define STRLEN GENSYM(len)
#define STRCPY GENSYM(cpy)
#define STRCAT GENSYM(cat)
#define STRCHR GENSYM(chr)
#define STRDUP GENSYM(dup)

/* Generic string symbol name.  */
#define GENSYM(tag) GENSYM1(GENPRE,tag,GENPOST)
#define GENSYM1(A,B,C) GENSYM2(A,B,C)
#define GENSYM2(A,B,C) A ## B ## C

/* Internal symbol name.  */
#define INTSYM(tag) INTSYM1(LABEL,tag)
#define INTSYM1(A,B) INTSYM2(A,B)
#define INTSYM2(A,B) A ## B

/* External symbol name.  */
#define EXTSYM(tag) EXTSYM1(LABEL,tag)
#define EXTSYM1(A,B) EXTSYM2(A,B)
#define EXTSYM2(A,B) rs_ ## A ## B

/* Declare or call an internal or external function.  */
#define INTERN(tag,arg) INTSYM(tag) arg
#define EXTERN(tag,arg) EXTSYM(tag) arg

/* The empty string.  */
#define NULSTR INTSYM(nul)

static CHAR const NULSTR[1] = {0};

static CHAR *
INTERN (concatv, (const CHAR *first, va_list argp))
{
  va_list start;
  CHAR *result, *end;
  size_t length;
  CHAR *string;

  /* Save beginning of argument list.  */
#ifdef va_copy
  va_copy (start, argp);
#else
  start = argp;
#endif

  length = STRLEN (first);
  while (1)
    {
      string = va_arg (argp, CHAR *);
      if (string == NULL)
	break;

      length += STRLEN (string);
    }

  result = calloc (length + 1, sizeof (CHAR));
  if (result == NULL)
    return NULL;

  /* Restart at the beginning.  */
#ifdef va_copy
  va_copy (argp, start);
#else
  argp = start;
#endif

  STRCPY (result, first);
  end = STRCHR (result, 0);

  while (1)
    {
      string = va_arg (argp, CHAR *);
      if (string == NULL)
	break;

      STRCPY (end, string);
      end = STRCHR (end, 0);
    }

  return result;
}

/* Create a single string from a variable number of arguments.  */
CHAR *
EXTERN (concat, (CHAR const *first, ...))
{
  va_list argp;
  CHAR *tem;

  if (first == NULL)
    return NULL;

  va_start (argp, first);
  tem = INTERN (concatv, (first, argp));
  va_end (argp);

  return tem;
}

CHAR *
EXTERN (concatx, (CHAR const *first, ...))
{
  va_list argp;
  CHAR *tem;

  if (first == NULL)
    return STRDUP (NULSTR);

  va_start (argp, first);
  tem = INTERN (concatv, (first, argp));
  va_end (argp);

  return tem;
}

/* Like the 'concat' function described above, but use the argument
   pointer ARGP to get optional arguments.  */
CHAR *
EXTERN (concatv, (CHAR const *first, va_list argp))
{
  return (first != NULL ?
	  INTERN (concatv, (first, argp)) :
	  NULL);
}

CHAR *
EXTERN (concatvx, (CHAR const *first, va_list argp))
{
  return (first != NULL ?
	  INTERN (concatv, (first, argp)) :
	  STRDUP (NULSTR));
}

/* Concatenate the two strings FIRST and SECOND.  */
static CHAR *
INTERN (concat2, (CHAR const *first, CHAR const *second))
{
  CHAR *tem;

  if (first == NULL)
    first = NULSTR;

  if (second == NULL)
    second = NULSTR;

  tem = calloc (STRLEN (first) + STRLEN (second) + 1, sizeof (CHAR));
  if (tem == NULL)
    return NULL;

  STRCPY (tem, first);
  STRCAT (tem, second);

  return tem;
}

CHAR *
EXTERN (concat2, (CHAR const *first, CHAR const *second))
{
  return (first != NULL || second != NULL ?
	  INTERN (concat2, (first, second)) :
	  NULL);
}

CHAR *
EXTERN (concat2x, (CHAR const *first, CHAR const *second))
{
  return INTERN (concat2, (first, second));
}

/* Concatenate the three strings FIRST, SECOND, and THIRD.  */
static CHAR *
INTERN (concat3, (CHAR const *first, CHAR const *second, CHAR const *third))
{
  CHAR *tem;

  if (first == NULL)
    first = NULSTR;

  if (second == NULL)
    second = NULSTR;

  if (third == NULL)
    third = NULSTR;

  tem = calloc (STRLEN (first) + STRLEN (second) + STRLEN (third) + 1, sizeof (CHAR));
  if (tem == NULL)
    return NULL;

  STRCPY (tem, first);
  STRCAT (tem, second);
  STRCAT (tem, third);

  return tem;
}

CHAR *
EXTERN (concat3, (CHAR const *first, CHAR const *second, CHAR const *third))
{
  return (first != NULL || second != NULL || third != NULL ?
	  INTERN (concat3, (first, second, third)) :
	  NULL);
}

CHAR *
EXTERN (concat3x, (CHAR const *first, CHAR const *second, CHAR const *third))
{
  return INTERN (concat3, (first, second, third));
}
