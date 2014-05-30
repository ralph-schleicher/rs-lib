/* rs-array.h --- multi-dimensional dynamic arrays.

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


#ifndef RS_ARRAY_H
#define RS_ARRAY_H

#ifdef __cplusplus
#define RS_ARRAY_BEGIN_DECL extern "C" {
#define RS_ARRAY_END_DECL }
#else /* not __cplusplus */
#define RS_ARRAY_BEGIN_DECL
#define RS_ARRAY_END_DECL
#endif /* not __cplusplus */

RS_ARRAY_BEGIN_DECL

/* Make a multi-dimensional array.  */
extern void *rs_make_array (int __rank, int const *__dim, size_t __size, void const *__elem);

/* Make a two-dimensional array.  */
extern void *rs_make_array_2d (int __y, int __x, size_t __size, void const *__elem);

/* Make a three-dimensional array.  */
extern void *rs_make_array_3d (int __z, int __y, int __x, size_t __size, void const *__elem);

/* Make a four-dimensional array.  */
extern void *rs_make_array_4d (int __u, int __z, int __y, int __x, size_t __size, void const *__elem);

/* Make a five-dimensional array.  */
extern void *rs_make_array_5d (int __v, int __u, int __z, int __y, int __x, size_t __size, void const *__elem);

/* Make a six-dimensional array.  */
extern void *rs_make_array_6d (int __w, int __v, int __u, int __z, int __y, int __x, size_t __size, void const *__elem);

/* Free a multi-dimensional array.  */
extern void rs_free_array (void *__a);

/* Return a pointer to the elements of a multi-dimensional array.  */
extern void *rs_array_elements (void const *__a);

RS_ARRAY_END_DECL

#endif /* not RS_ARRAY_H */
