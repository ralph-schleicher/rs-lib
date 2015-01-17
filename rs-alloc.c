/* rs-alloc.c --- extended memory allocation.

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
#include <errno.h>
#include <stdio.h>

#include "rs-alloc.h"

/* Procedure to call if memory allocation fails.  */
void (*rs_out_of_memory_hook) (void) = NULL;

/* Cause an abnormal program termination.  */
static void
xabort (void)
{
  if (rs_out_of_memory_hook != NULL)
    rs_out_of_memory_hook ();

  abort ();
}

/* Validate a pointer.  */
void *
rs_confirm (void *p)
{
  if (p == NULL)
    xabort ();

  return p;
}

/* Return the address of a memory block SIZE bytes long.  */
void *
rs_malloc (size_t size)
{
  void *tem;

  tem = malloc (size);
  if (tem == NULL)
    xabort ();

  return tem;
}

/* Change the size of the memory block whose address is P to be SIZE.
   If the argument P is a null pointer, a memory block SIZE bytes long
   will be allocated.

   Return value is the address of the memory block.  */
void *
rs_realloc (void *p, size_t size)
{
  void *tem;

  tem = (p == NULL) ? malloc (size) : realloc (p, size);
  if (tem == NULL)
    xabort ();

  return tem;
}

/* Return the address of a memory block big enough to store a vector of
   ELEM elements, each of size SIZE.  */
void *
rs_calloc (size_t elem, size_t size)
{
  void *tem;

  tem = calloc (elem, size);
  if (tem == NULL)
    xabort ();

  return tem;
}

/* Deallocate the memory block whose address is P.
   It is no error if the argument P is a null pointer.

   The return value is alsways a null pointer.  */
void *
rs_free (void *p)
{
  if (p != NULL)
    free (p);

  return NULL;
}

/* Duplicate a memory region.  */
void *
rs_memdup (void const *p, size_t size)
{
  void *tem;

  tem = malloc (size);
  if (tem == NULL)
    xabort ();

  return memcpy (tem, p, size);
}

/* Duplicate the string S.  */
char *
rs_strdup (char const *s)
{
  char *tem;

  tem = strdup (s);
  if (tem == NULL)
    xabort ();

  return tem;
}

/* Duplicate the string S but copy not more than N characters.
   The result is always null-terminated.  */
char *
rs_strndup (char const *s, size_t n)
{
#if HAVE_STRNDUP

  char *tem;

  tem = strndup (s, n);
  if (tem == NULL)
    xabort ();

  return tem;

#else /* not HAVE_STRNDUP */

  size_t len;
  char *tem;

  /* Do not copy more than the hole string.  */
  len = strlen (s);
  if (n > len)
    n = len;

  tem = calloc (n + 1, sizeof (char));
  if (tem == NULL)
    xabort ();

  /* Terminate the new string.  */
  tem[n] = 0;

  return memcpy (tem, s, n * sizeof (char));

#endif /* not HAVE_STRNDUP */
}
