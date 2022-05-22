/* rs-svec.h --- simple vectors

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

#ifndef RS_SVEC_H
#define RS_SVEC_H

#ifndef RS_SVEC_USE_ALIASES
#define RS_SVEC_USE_ALIASES 0
#endif

#include <stddef.h>

#ifdef __cplusplus
#define RS_SVEC_BEGIN_DECL extern "C" {
#define RS_SVEC_END_DECL }
#else /* not __cplusplus */
#define RS_SVEC_BEGIN_DECL
#define RS_SVEC_END_DECL
#endif /* not __cplusplus */

RS_SVEC_BEGIN_DECL

/* Opaque data type for a simple vector.  */
typedef struct rs_svec rs_svec_t;

/* Create a new simple vector.

   First argument LENGTH is the number of vector elements.
    Value has to be greater than or equal to zero.
   Second argument SIZE is the size of a vector element.
    Value has to be greater than zero.

   Return value is a pointer to a simple vector object.  In case
   of an error, the return value is a null pointer and 'errno' is
   set to describe the error.

   Error conditions:

   EINVAL
        Argument LENGTH is less than zero or argument SIZE
        is equal to zero.

   ENOMEM
        The system ran out of memory.  */
extern rs_svec_t *rs_svalloc (int __length, size_t __size);

/* Delete a simple vector.

   Argument VEC is a pointer to a simple vector object.

   It is no error if argument VEC is a null pointer.  */
extern void rs_svfree (rs_svec_t *__vec);

/* Return the number of vector elements.

   Argument VEC is a pointer to a simple vector object.

   Return value is the number of vector elements.  If argument VEC
   is a null pointer, the return value is zero (no error).  */
extern int rs_svlength (rs_svec_t const *__vec);

/* Return the size of a vector element.

   Argument VEC is a pointer to a simple vector object.

   Return value is the size of a vector element.  If argument VEC
   is a null pointer, the return value is zero (no error).  */
extern size_t rs_svsize (rs_svec_t const *__vec);

/* Return a pointer to the first vector element.

   Argument VEC is a pointer to a simple vector object.

   Return value is a pointer to the first vector element.
   If argument VEC is a null pointer, the return value is
   a null pointer (no error).  If the vector is empty, the
   return value is undefined (no error).  */
extern void *rs_svdata (rs_svec_t const *__vec);

/* Return a pointer to a vector element.

   First argument VEC is a pointer to a simple vector object.
   Second argument INDEX is the zero-based element index.  If
    argument INDEX is a negative number, count from the end.
    That means, the relation

        rs_svref (vec, -J) == rs_svref (vec, rs_svlength (vec) - J)

    is true for J from one to the number of vector elements.

   Return value is a pointer to the specified vector element.
   In case of an error, the return value is a null pointer and
   'errno' is set to describe the error.

   Error conditions:

   EINVAL
        Argument VEC is a null pointer.

   EDOM
        Argument INDEX is out of bounds.  */
extern void *rs_svref (rs_svec_t const *__vec, int __index);

/* Find a vector element.

   First argument VEC is a pointer to a simple vector object.
   Second argument KEY is a pointer to a vector element.  It is used
    as an argument to the comparison function to identify the searched
    vector element.
   Third argument COMPAR is the comparison function for the vector
    elements.  This function is called with two pointer arguments and
    should return an integer equal to zero if its first argument is
    considered equal to its second argument.

   It is no error if argument VEC is a null pointer.  If argument KEY
   or COMPAR is a null pointer, the behavior is undefined.

   Return value is a pointer to the matching vector element.  If no
   matching vector element is found, the return value is a null pointer.

   The vector elements must not be sorted.  */
extern void *rs_svfind (rs_svec_t const *__vec, void const *__key, int (*__compar) (void const *, void const *));

/* Search for an element in a sorted vector.

   First argument VEC is a pointer to a simple vector object.
   Second argument KEY is a pointer to a vector element.  It is used
    as an argument to the comparison function to identify the searched
    vector element.
   Third argument COMPAR is the comparison function for the vector
    elements.  This function is called with two pointer arguments and
    should return an integer less than, equal to, or greater than zero
    corresponding to whether its first argument is considered less than,
    equal to, or greater than its second argument.  The elements of the
    simple vector must already be sorted in ascending order according to
    this comparison function.

   It is no error if argument VEC is a null pointer.  If argument KEY
   or COMPAR is a null pointer, the behavior is undefined.

   Return value is a pointer to the matching vector element.  If no
   matching vector element is found, the return value is a null pointer.

   The vector elements must be sorted.  */
extern void *rs_svsearch (rs_svec_t const *__vec, void const *__key, int (*__compar) (void const *, void const *));

/* Sort the vector elements.

   First argument VEC is a pointer to a simple vector object.
   Second argument COMPAR is the comparison function for the vector
    elements.  This function is called with two pointer arguments and
    should return an integer less than, equal to, or greater than zero
    corresponding to whether its first argument is considered less than,
    equal to, or greater than its second argument.

   It is no error if argument VEC is a null pointer.  If argument COMPAR
   is a null pointer, the behavior is undefined.

   Return value is the pointer to the simple vector object.  */
extern rs_svec_t *rs_svsort (rs_svec_t const *__vec, int (*__compar) (void const *, void const *));

/* Call a function for each vector element.

   First argument VEC is a pointer to a simple vector object.
   Second argument FUN is a pointer to a function.  The argument
    of the function is a pointer to a vector element.

   If is no error if argument VEC or argument FUN is a null pointer.

   Return value is the pointer to the simple vector object.  */
extern rs_svec_t *rs_svmap (rs_svec_t const *__vec, void (*__fun) (void *));

/* Call a function for each vector element with one extra argument.

   First argument VEC is a pointer to a simple vector object.
   Second argument FUN is a pointer to a function.  The first
    argument of the function is a pointer to a vector element
    and the second argument is the third argument ARG.
   Third argument ARG is the second argument passed to FUN.

   If is no error if argument VEC or argument FUN is a null pointer.

   Return value is the pointer to the simple vector object.  */
extern rs_svec_t *rs_svmap1 (rs_svec_t const *__vec, void (*__fun) (void *, void *), void *__arg);

#if RS_SVEC_USE_ALIASES
#define svec rs_svec
#define svec_t rs_svec_t
#define svalloc rs_svalloc
#define svfree rs_svfree
#define svlength rs_svlength
#define svsize rs_svsize
#define svdata rs_svdata
#define svref rs_svref
#define svfind rs_svfind
#define svsearch rs_svsearch
#define svsort rs_svsort
#define svmap rs_svmap
#define svmap1 rs_svmap1
#endif /* RS_SVEC_USE_ALIASES */

RS_SVEC_END_DECL

#endif /* not RS_SVEC_H */
