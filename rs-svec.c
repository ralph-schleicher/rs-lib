/* rs-svec.c --- simple vectors

   Copyright (C) 2014 Ralph Schleicher

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
#include <errno.h>
#include <search.h>

#include "rs-svec.h"

/* Data type for a simple vector.  */
struct rs_svec
  {
    /* Number of elements.  Value is greater than or equal to zero.  */
    int length;

    /* Size of an element.  Value is greater than zero.  */
    size_t size;

    /* The elements.  */
    void *data;
  };

static void
run_hook (rs_svec_t const *vec, void (*f) (void *))
{
  int n;
  size_t s;
  char *p;

  n = vec->length;
  s = vec->size;
  p = vec->data;

  for (; n > 0; --n)
    {
      f (p);
      p += s;
    }
}

static void
run_hook_with_arg (rs_svec_t const *vec, void (*f) (void *, void *), void *arg)
{
  int n;
  size_t s;
  char *p;

  n = vec->length;
  s = vec->size;
  p = vec->data;

  for (; n > 0; --n)
    {
      f (p, arg);
      p += s;
    }
}

/* Create a new simple vector.  */
rs_svec_t *
rs_svalloc (int n, size_t s)
{
  rs_svec_t *vec;

  if (n < 0 || s == 0)
    {
      errno = EINVAL;
      return NULL;
    }

  vec = calloc (1, sizeof (rs_svec_t));
  if (vec != NULL)
    {
      vec->length = n;
      vec->size = s;
      vec->data = NULL;

      if (n > 0)
	{
	  vec->data = calloc (n, s);
	  if (vec->data == NULL)
	    {
	      free (vec);
	      vec = NULL;
	    }
	}
    }

  return vec;
}

/* Delete a simple vector.  */
void
rs_svfree (rs_svec_t *vec)
{
  if (vec != NULL)
    {
      vec->length = 0;
      vec->size = 0;

      if (vec->data != NULL)
	{
	  free (vec->data);
	  vec->data = NULL;
	}

      free (vec);
      vec = NULL;
    }
}

/* Return the number of vector elements.  */
int
rs_svlength (rs_svec_t const *vec)
{
  return (vec != NULL ? vec->length : 0);
}

/* Return the size of a vector element.  */
size_t
rs_svsize (rs_svec_t const *vec)
{
  return (vec != NULL ? vec->size : 0);
}

/* Return a pointer to the first vector element.  */
void *
rs_svdata (rs_svec_t const *vec)
{
  return (vec != NULL ? vec->data : NULL);
}

/* Return a pointer to a vector element.  */
void *
rs_svref (rs_svec_t const *vec, int j)
{
  int n;
  size_t s;
  char *p;

  if (vec == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  n = vec->length;
  s = vec->size;
  p = vec->data;

  if (j < 0)
    j += n;

  if (j < 0 || j >= n)
    {
      errno = EDOM;
      return NULL;
    }

  return p + j * s;
}

/* Find a vector element.  */
void *
rs_svfind (rs_svec_t const *vec, void const *key, int (*compar) (void const *, void const *))
{
  size_t n = (size_t) vec->length;

  return (vec != NULL && vec->length > 0 ?
	  lfind (key, vec->data, &n, vec->size, compar) :
	  NULL);
}

/* Search for an element in a sorted vector.  */
void *
rs_svsearch (rs_svec_t const *vec, void const *key, int (*compar) (void const *, void const *))
{
  size_t n = (size_t) vec->length;

  return (vec != NULL && vec->length > 0 ?
	  bsearch (key, vec->data, n, vec->size, compar) :
	  NULL);
}

/* Sort the vector elements.  */
rs_svec_t *
rs_svsort (rs_svec_t const *vec, int (*compar) (void const *, void const *))
{
  if (vec != NULL && vec->length > 1)
    qsort (vec->data, vec->length, vec->size, compar);

  return (rs_svec_t *) vec;
}

/* Call a function for each vector element.  */
rs_svec_t *
rs_svmap (rs_svec_t const *vec, void (*fun) (void *))
{
  if (vec != NULL && fun != NULL)
    run_hook (vec, fun);

  return (rs_svec_t *) vec;
}

/* Call a function for each vector element with one extra argument.  */
rs_svec_t *
rs_svmap1 (rs_svec_t const *vec, void (*fun) (void *, void *), void *arg)
{
  if (vec != NULL && fun != NULL)
    run_hook_with_arg (vec, fun, arg);

  return (rs_svec_t *) vec;
}
