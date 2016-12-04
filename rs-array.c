/* rs-array.c --- multi-dimensional dynamic arrays.

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
#include <limits.h>
#include <errno.h>

#include "rs-array.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif

#ifdef _MSC_VER
/* Microsoft Visual C.  */
#if _MSC_VER >= 1600
#include <stdint.h>
#else /* _MSC_VER < 1600 */
#include <stddef.h>
#ifndef _UINTPTR_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#else /* not _WIN64 */
typedef unsigned long int uintptr_t;
#endif /* not _WIN64 */
#endif /* _UINTPTR_T_DEFINED */
#endif /* _MSC_VER < 1600 */
#else /* not _MSC_VER */
#include <stdint.h>
#endif /* not _MSC_VER */

#define marker ((uintptr_t) 1)
#define is_shared(p) (((uintptr_t) (p)) & marker)
#define make_shared(p) ((void *) (((uintptr_t) (p)) | marker))
#define clear_shared(p) ((void *) (((uintptr_t) (p)) & ~marker))

/* Make a multi-dimensional array.

   First argument RANK is the array rank, that is the number of array
    dimensions.  Value has to be greater than or equal to 2.
   Second argument DIM are the array dimensions (a vector of positive
    numbers).  Dimensions are in increasing order, that is the first
    vector element of DIM defines the number of array elements in the
    first dimension.
   Third argument SIZE is the size of a single array element.  Value
    has to be a positive number.
   Fourth argument ELEM is a pointer to the array elements.  If ELEM
    is non-null, make a shared array.  Otherwise, allocate a memory
    region for the array elements.

   Return value is the multi-dimensional array.  If an error occurs,
   the return value is a null pointer and 'errno' is set to describe
   the error.  */
static void *
make_array (int rank, int const *dim, size_t size, void const *elem)
{
  char *a;
  void **ip;
  size_t n;
  int i;

  /* Check arguments.  */
  if (rank < 2 || dim == NULL || size == 0)
    {
      errno = EINVAL;
      return NULL;
    }

  /* Count number of array elements.  */
  n = 1;

  for (i = 0; i < rank; ++i)
    {
      if (dim[i] < 1)
	{
	  errno = EINVAL;
	  return NULL;
	}

      /* Check for overflow.  */
      if (n > SIZE_MAX / dim[i])
	{
	  errno = ERANGE;
	  return NULL;
	}

      n *= dim[i];
    }

  /* Whether or not to make a shared array.  */
  if (elem == NULL)
    {
      /* Allocate memory for the array elements.  */
      a = (char *) calloc (n, size);
      if (a == NULL)
	return NULL;
    }
  else
    {
      /* Make a shared array.  */
      a = (char *) elem;
    }

  /* Count number of indirect pointers.  */
  n = 0;

  for (i = 1; i < rank; ++i)
    n = dim[i] * (1 + n);

  /* Allocate memory for the indirect pointers.  */
  ip = calloc (1 + n, sizeof (void *));
  if (ip == NULL)
    {
      if (elem == NULL)
	free (a);
    }
  else
    {
      void **ap, **bp;
      size_t c, incr;

      /* The first indirect pointer is reserved for private use.
	 It contains the address of the array elements and an optional
	 flag for marking shared arrays.  */
      *ip++ = (elem == NULL) ? a : make_shared (a);

      /* Number of indirect pointers for the highest dimension.  */
      n = dim[rank - 1];

      /* Setup indirect pointers.  */
      ap = ip;
      bp = ap + n;

      /* Link intermediate dimensions.  */
      for (i = rank - 2; i > 0; --i)
	{
	  for (c = n; c > 0; --c)
	    {
	      *ap++ = (void *) bp;
	      bp += dim[i];
	    }

	  n *= dim[i];
	}

      /* Link first dimension.  */
      incr = dim[0] * size;

      for (c = n; c > 0; --c)
	{
	  *ap++ = a;
	  a += incr;
	}
    }

  return (void *) ip;
}

void *
rs_make_array (int rank, int const *dim, size_t size, void const *elem)
{
  return make_array (rank, dim, size, elem);
}

/* Free a multi-dimensional array.

   Argument A is a multi-dimensional array.

   If A is a shared array, the memory region for the array elements is
   not freed.  It is no error if A is a null pointer.  */
void
rs_free_array (void *a)
{
  void **ip;

  if (a == NULL)
    return;

  ip = (void **) a;
  --ip;

  if (! is_shared (*ip))
    free (*ip);

  free (ip);
}

/* Return a pointer to the elements of a multi-dimensional array.

   Argument A is a multi-dimensional array.

   Return value is the address of the first array element of A.  If an
   error occurs, the return value is a null pointer and 'errno' is set
   to describe the error.  */
void *
rs_array_elements (void const *a)
{
  void **ip;

  if (a == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  ip = (void **) a;
  --ip;

  return clear_shared (*ip);
}

/* Make a two-dimensional array.  */
void *
rs_make_array_2d (int y, int x, size_t size, void const *elem)
{
  int dim[2];

  dim[0] = x;
  dim[1] = y;

  return make_array (2, dim, size, elem);
}

/* Make a three-dimensional array.  */
void *
rs_make_array_3d (int z, int y, int x, size_t size, void const *elem)
{
  int dim[3];

  dim[0] = x;
  dim[1] = y;
  dim[2] = z;

  return make_array (3, dim, size, elem);
}

/* Make a four-dimensional array.  */
void *
rs_make_array_4d (int u, int z, int y, int x, size_t size, void const *elem)
{
  int dim[4];

  dim[0] = x;
  dim[1] = y;
  dim[2] = z;
  dim[3] = u;

  return make_array (4, dim, size, elem);
}

/* Make a five-dimensional array.  */
void *
rs_make_array_5d (int v, int u, int z, int y, int x, size_t size, void const *elem)
{
  int dim[5];

  dim[0] = x;
  dim[1] = y;
  dim[2] = z;
  dim[3] = u;
  dim[4] = v;

  return make_array (5, dim, size, elem);
}

/* Make a six-dimensional array.  */
void *
rs_make_array_6d (int w, int v, int u, int z, int y, int x, size_t size, void const *elem)
{
  int dim[6];

  dim[0] = x;
  dim[1] = y;
  dim[2] = z;
  dim[3] = u;
  dim[4] = v;
  dim[5] = w;

  return make_array (6, dim, size, elem);
}
