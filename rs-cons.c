/* rs-cons.c --- cons cells and lists.

   Copyright (C) 1998 Ralph Schleicher

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in
        the documentation and/or other materials provided with the
        distribution.

      * The name of the author may not be used to endorse or promote
        products derived from this software without specific prior
        written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
   IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.  */

/* Commentary:

   Any sufficiently complicated C or Fortran program contains an
   ad hoc, informally-specified, bug-ridden, slow implementation
   of half of Common Lisp.

                         (Greenspun's Tenth Rule of Programming)  */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "rs-cons.h"

/* Cons cell object data structure.  */
struct cons
  {
    void *car;
    void *cdr;
  };

#define xcons(obj) ((struct cons *) (obj))
#define xcar(c) (xcons (c)->car)
#define xcdr(c) (xcons (c)->cdr)
#define xsetcar(c,v) (xcar (c) = (void *) (v))
#define xsetcdr(c,v) (xcdr (c) = (void *) (v))

static int
comp (void const *a, void const *b)
{
  return (a > b) - (a < b);
}

static void *
cons (void const *obj1, void const *obj2)
{
  void *cell;

  cell = calloc (1, sizeof (struct cons));
  if (cell != NULL)
    {
      xsetcar (cell, obj1);
      xsetcdr (cell, obj2);
    }

  return cell;
}

static void *
car_safe (void const *cell)
{
  return (cell != NULL ? xcar (cell) : NULL);
}

static void *
cdr_safe (void const *cell)
{
  return (cell != NULL ? xcdr (cell) : NULL);
}

static int
length (void const *list)
{
  void *tail;
  int n = 0;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    ++n;

  return n;
}

static void
destroy (void *list)
{
  void *tem;

  while (list != NULL)
    {
      tem = xcdr (list);

      free (list);
      list = tem;
    }
}

/* Argument N has to be a non-negative number.  */
static void *
nthcdr (int n, void const *list)
{
  void *tail = (void *) list;

  while (n-- > 0 && tail != NULL)
    tail = xcdr (tail);

  return tail;
}

/* Argument LIST has to be non-empty and N has to be a positive number.  */
static void *
last (void const *list, int n)
{
  void *tail = (void *) list;

  if (n == 1)
    {
      void *tem;

      while (1)
	{
	  tem = xcdr (tail);
	  if (tem == NULL)
	    return tail;

	  tail = tem;
	}
    }

  n = length (list) - n;
  if (n < 0)
    return tail;

  return nthcdr (n, list);
}

static void *
conc (void *list, void const *tail)
{
  void *tem;

  if (list == NULL)
    return (void *) tail;

  if (tail != NULL)
    {
      tem = last (list, 1);
      xsetcdr (tem, tail);
    }

  return list;
}

static void *
reconc (void *list, void const *tail)
{
  void *work, *tem;

  work = list;
  list = (void *) tail;

  while (work != NULL)
    {
      tem = xcdr (work);
      xsetcdr (work, list);

      list = work;
      work = tem;
    }

  return list;
}

static void *
merge (void *l1, void *l2, int (*compar) (void const *, void const *))
{
  struct cons tem[1];
  void *list;

  list = tem;

  while (l1 != NULL && l2 != NULL)
    {
      if (compar (xcar (l1), xcar (l2)) <= 0)
        {
	  xsetcdr (list, l1);
          list = xcdr (list);
          l1 = xcdr (l1);
        }
      else
        {
	  xsetcdr (list, l2);
          list = xcdr (list);
          l2 = xcdr (l2);
        }
    }

  xsetcdr (list, (l1 != NULL ? l1 : l2));

  return xcdr (tem);
}

static void *
sort (void *list, int (*compar) (void const *, void const *))
{
  void *l1, *l2;

  l1 = list;
  if (l1 == NULL)
    return list;

  l2 = xcdr (list);
  if (l2 == NULL)
    return list;

  while (1)
    {
      l2 = xcdr (l2);
      if (l2 == NULL)
	break;

      l2 = xcdr (l2);
      if (l2 == NULL)
	break;

      list = xcdr (list);
    }

  l2 = xcdr (list);
  xsetcdr (list, NULL);

  return merge (sort (l1, compar), sort (l2, compar), compar);
}

static void *
assoc (void const *key, void const *list, int (*f) (void const *, void const *))
{
  void *tail, *pair;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    {
      pair = xcar (tail);
      if (pair == NULL)
	continue;

      if (f (xcar (pair), key) == 0)
	return pair;
    }

  return NULL;
}

static void *
assoc_if (int (*p) (void const *), void const *list)
{
  void *tail, *pair;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    {
      pair = xcar (tail);
      if (pair == NULL)
	continue;

      if (p (xcar (pair)))
	return pair;
    }

  return NULL;
}

static void *
assoc_if_not (int (*p) (void const *), void const *list)
{
  void *tail, *pair;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    {
      pair = xcar (tail);
      if (pair == NULL)
	continue;

      if (! p (xcar (pair)))
	return pair;
    }

  return NULL;
}

/* Create a new cons cell.

   The car of the cons cell is set to OBJ1 and the cdr of the cons cell
   is set to OBJ2.  */
void *
rs_cons (void const *obj1, void const *obj2)
{
  return cons (obj1, obj2);
}

/* Return the car of CELL.  */
void *
rs_car (void const *cell)
{
  return xcar (cell);
}

/* Return the cdr of CELL.  */
void *
rs_cdr (void const *cell)
{
  return xcdr (cell);
}

/* More accessor functions.  */
void *
rs_caar (void const *cell)
{
  return xcar (xcar (cell));
}

void *
rs_cadr (void const *cell)
{
  return xcar (xcdr (cell));
}

void *
rs_cdar (void const *cell)
{
  return xcdr (xcar (cell));
}

void *
rs_cddr (void const *cell)
{
  return xcdr (xcdr (cell));
}

void *
rs_caaar (void const *cell)
{
  return xcar (xcar (xcar (cell)));
}

void *
rs_caadr (void const *cell)
{
  return xcar (xcar (xcdr (cell)));
}

void *
rs_cadar (void const *cell)
{
  return xcar (xcdr (xcar (cell)));
}

void *
rs_caddr (void const *cell)
{
  return xcar (xcdr (xcdr (cell)));
}

void *
rs_cdaar (void const *cell)
{
  return xcdr (xcar (xcar (cell)));
}

void *
rs_cdadr (void const *cell)
{
  return xcdr (xcar (xcdr (cell)));
}

void *
rs_cddar (void const *cell)
{
  return xcdr (xcdr (xcar (cell)));
}

void *
rs_cdddr (void const *cell)
{
  return xcdr (xcdr (xcdr (cell)));
}

void *
rs_caaaar (void const *cell)
{
  return xcar (xcar (xcar (xcar (cell))));
}

void *
rs_caaadr (void const *cell)
{
  return xcar (xcar (xcar (xcdr (cell))));
}

void *
rs_caadar (void const *cell)
{
  return xcar (xcar (xcdr (xcar (cell))));
}

void *
rs_caaddr (void const *cell)
{
  return xcar (xcar (xcdr (xcdr (cell))));
}

void *
rs_cadaar (void const *cell)
{
  return xcar (xcdr (xcar (xcar (cell))));
}

void *
rs_cadadr (void const *cell)
{
  return xcar (xcdr (xcar (xcdr (cell))));
}

void *
rs_caddar (void const *cell)
{
  return xcar (xcdr (xcdr (xcar (cell))));
}

void *
rs_cadddr (void const *cell)
{
  return xcar (xcdr (xcdr (xcdr (cell))));
}

void *
rs_cdaaar (void const *cell)
{
  return xcdr (xcar (xcar (xcar (cell))));
}

void *
rs_cdaadr (void const *cell)
{
  return xcdr (xcar (xcar (xcdr (cell))));
}

void *
rs_cdadar (void const *cell)
{
  return xcdr (xcar (xcdr (xcar (cell))));
}

void *
rs_cdaddr (void const *cell)
{
  return xcdr (xcar (xcdr (xcdr (cell))));
}

void *
rs_cddaar (void const *cell)
{
  return xcdr (xcdr (xcar (xcar (cell))));
}

void *
rs_cddadr (void const *cell)
{
  return xcdr (xcdr (xcar (xcdr (cell))));
}

void *
rs_cdddar (void const *cell)
{
  return xcdr (xcdr (xcdr (xcar (cell))));
}

void *
rs_cddddr (void const *cell)
{
  return xcdr (xcdr (xcdr (xcdr (cell))));
}

/* Set the car of CELL to OBJ and return CELL.  */
void *
rs_rplaca (void *cell, void const *obj)
{
  xsetcar (cell, obj);

  return cell;
}

/* Set the cdr of CELL to OBJ and return CELL.  */
void *
rs_rplacd (void *cell, void const *obj)
{
  xsetcdr (cell, obj);

  return cell;
}

/* Set the car of CELL to OBJ and return OBJ.  */
void *
rs_setcar (void *cell, void const *obj)
{
  xsetcar (cell, obj);

  return (void *) obj;
}

/* Set the cdr of CELL to OBJ and return OBJ.  */
void *
rs_setcdr (void *cell, void const *obj)
{
  xsetcdr (cell, obj);

  return (void *) obj;
}

/* Create a new list with N elements.  Each list element is set to OBJ.
   Return value is the first cons cell of the list.  If argument N is
   zero, the return value is a null pointer (no error).  */
void *
rs_make_list (int n, void const *obj)
{
  void *list, *tem;

  if (n < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  for (list = NULL; n > 0; --n)
    {
      tem = cons (obj, list);
      if (tem == NULL)
	{
	  destroy (list);
	  return NULL;
	}

      list = tem;
    }

  return list;
}

/* Destroy the cons cells of a proper list.  Second argument F is
   the destructor call-back function for a list element.  */
void
rs_free_list (void *list, void (*f) (void *))
{
  if (f != NULL)
    {
      void *tem;

      while (list != NULL)
	{
	  tem = xcar (list);
	  if (tem != NULL)
	    f (tem);

	  tem = xcdr (list);

	  free (list);
	  list = tem;
	}
    }
  else
    destroy (list);
}

/* Create a new list with N elements.  List elements are set to the
   supplied arguments.  If argument N is zero, the return value is a
   null pointer (no error).  */
void *
rs_list (int n, ...)
{
  void *list, *tem;
  void const *obj;
  va_list ap;

  if (n < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  va_start (ap, n);

  for (list = NULL; n > 0; --n)
    {
      obj = va_arg (ap, void const *);

      tem = cons (obj, list);
      if (tem == NULL)
	{
	  destroy (list);
	  va_end (ap);
	  return NULL;
	}

      list = tem;
    }

  va_end (ap);

  return reconc (list, NULL);
}

/* Return the number of list elements.  */
int
rs_list_length (void const *list)
{
  return length (list);
}

/* Return the first list element.  */
void *
rs_list_first (void const *list)
{
  return xcar (list);
}

void *
rs_list_second (void const *list)
{
  return xcar (xcdr (list));
}

void *
rs_list_third (void const *list)
{
  return xcar (xcdr (xcdr (list)));
}

void *
rs_list_fourth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (list))));
}

void *
rs_list_fifth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (list)))));
}

void *
rs_list_sixth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (xcdr (list))))));
}

void *
rs_list_seventh (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (list)))))));
}

void *
rs_list_eighth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (list))))))));
}

void *
rs_list_ninth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (list)))))))));
}

void *
rs_list_tenth (void const *list)
{
  return xcar (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (xcdr (list))))))))));
}

/* Return the Nth list element.  Argument N is zero based, that means
   if N is zero, the first list element is returned.  If there are less
   elements in the list, the return value is NULL (no error).  */
void *
rs_list_nth (int n, void const *list)
{
  if (n < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  return car_safe (nthcdr (n, list));
}

/* Return the rest of a list.  */
void *
rs_list_rest (void const *list)
{
  return xcdr (list);
}

/* Return the Nth cdr of a list.  Argument N is zero based, that means
   if N is zero, the return value is LIST.  If there are less elements
   in the list, the return value is NULL (no error).  */
void *
rs_list_nthcdr (int n, void const *list)
{
  if (n < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  return nthcdr (n, list);
}

/* Return the last N elements of a list.  If there are less elements at
   the end of the list, the return value is LIST.  If N is zero, the
   return value is NULL (no error).  */
void *
rs_list_last (void const *list, int n)
{
  if (n < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  return (list != NULL && n != 0 ? last (list, n) : NULL);
}

/* Add OBJ as the first element to LIST.  */
void *
__rs_list_push (void const *obj, void **addr)
{
  void *list;
  void *tail = *addr;

  list = cons (obj, tail);
  if (list != NULL)
    *addr = list;

  return list;
}

/* Remove the first element from LIST and return it.  */
void *
__rs_list_pop (void **addr)
{
  void *list = *addr;
  void *tail;
  void *obj;

  if (list != NULL)
    {
      obj = xcar (list);
      tail = xcdr (list);

      free (list);
      *addr = tail;
    }
  else
    obj = NULL;

  return obj;
}

/* Move the first element from list FROM to list TO .
   Therefore,

        rs_list_restack (from, to);

   is syntactically equal to

        rs_list_push (rs_list_pop (from), to);

   except that it is more efficient as well as fail-save.  */
void *
__rs_list_restack (void **addr1, void **addr2)
{
  void *from = *addr1;
  void *to = *addr2;
  void *tail;

  if (from != NULL)
    {
      tail = xcdr (from);
      xsetcdr (from, to);

      *addr1 = tail;
      *addr2 = from;
    }

  return from;
}

/* Remove the elements designated by OFFSET and LENGTH from LIST, and
   replace them with SUBST (a list).  Return value is the list of
   removed elements.  If OFFSET is negative, count from the end of the
   list (see 'last').  If OFFSET is arbitrarily large, operate at the
   end of LIST.  Likewise, if OFFSET is arbitrarily small, operate at
   the beginning of LIST.  If LENGTH is positive, remove up to LENGTH
   elements from OFFSET onwards.  If LENGTH is negative, remove the
   elements from OFFSET onwards except for the last -LENGTH elements
   of LIST.  In that case, if the positions specified by OFFSET and
   LENGTH overlap, the return value is the empty list and 'errno' is
   set to 'ERANGE'.  */
void *
__rs_list_splice (void **addr, int offs, int len, void const *subst)
{
  void *list = *addr;
  void *prev, *next;
  void *start, *end;

  /* Normalize arguments.  */
  if (offs < 0 || len < 0)
    {
      int n = length (list);

      if (offs < 0)
	{
	  offs += n;
	  if (offs < 0)
	    offs = 0;
	}
      else
	{
	  if (offs > n)
	    offs = n;
	}

      if (len < 0)
	{
	  len += n - offs;
	  if (len < 0)
	    {
	      errno = ERANGE;
	      return NULL;
	    }
	}
    }

  if (list == NULL)
    {
      if (subst != NULL)
	*addr = (void *) subst;

      return NULL;
    }

  /* Locate beginning of splice.  */
  prev = NULL;
  start = list;

  if (offs > 0)
    {
      do
	{
	  prev = start;
	  start = xcdr (start);
	}
      while (start != NULL && --offs > 0);
    }

  /* Locate end of splice.  */
  end = prev;
  next = start;

  if (len > 0 && next != NULL)
    {
      do
	{
	  end = next;
	  next = xcdr (next);
	}
      while (next != NULL && --len > 0);

      xsetcdr (end, NULL);
    }
  else
    {
      /* There are no elements to delete.
	 Return value is the empty list.  */
      start = NULL;
    }

  /* Insert new elements.  */
  if (subst != NULL)
    {
      if (next != NULL)
	xsetcdr (last (subst, 1), next);

      next = (void *) subst;
    }

  if (prev != NULL)
    xsetcdr (prev, next);
  else
    *addr = next;

  return start;
}

/* Concatenate two lists.  */
void *
rs_list_conc (void *list, void const *tail)
{
  return conc (list, tail);
}

void *
rs_list_reconc (void *list, void const *tail)
{
  return reconc (list, tail);
}

/* Reverse a list.  */
void *
rs_list_reverse (void *list)
{
  return reconc (list, NULL);
}

/* Sort a list.  */
void *
rs_list_sort (void *list, int (*compar) (void const *, void const *))
{
  return sort (list, compar);
}

void *
rs_list_acons (void const *key, void const *value, void const *list)
{
  void *pair, *tem;

  pair = cons (key, value);
  if (pair != NULL)
    {
      tem = cons (pair, list);
      if (tem != NULL)
	return tem;

      free (pair);
    }

  return NULL;
}

void *
rs_list_assoc (void const *key, void const *list, int (*f) (void const *, void const *))
{
  if (f == NULL)
    f = comp;

  return assoc (key, list, f);
}

void *
rs_list_assoc_if (int (*p) (void const *), void *list)
{
  if (p == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  return assoc_if (p, list);
}

void *
rs_list_assoc_if_not (int (*p) (void const *), void *list)
{
  if (p == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  return assoc_if_not (p, list);
}

/* Apply a function to each element of a list.  */
void *
rs_list_mapc (void (*f) (void const *), void const *list)
{
  void *tail;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    f (xcar (tail));

  return (void *) list;
}

void *
rs_list_mapc_data (void (*f) (void const *, void const *), void const *list, void const *data)
{
  void *tail;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    f (xcar (tail), data);

  return (void *) list;
}

void *
rs_list_mapl (void (*f) (void const *), void const *list)
{
  void *tail;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    f (tail);

  return (void *) list;
}

void *
rs_list_mapl_data (void (*f) (void const *, void const *), void const *list, void const *data)
{
  void *tail;

  for (tail = (void *) list; tail != NULL; tail = xcdr (tail))
    f (tail, data);

  return (void *) list;
}
