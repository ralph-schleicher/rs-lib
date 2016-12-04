/* rs-alloc.h --- extended memory allocation.

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

#ifndef RS_ALLOC_H
#define RS_ALLOC_H

#ifdef __cplusplus
#define RS_ALLOC_BEGIN_DECL extern "C" {
#define RS_ALLOC_END_DECL }
#else /* not __cplusplus */
#define RS_ALLOC_BEGIN_DECL
#define RS_ALLOC_END_DECL
#endif /* not __cplusplus */

RS_ALLOC_BEGIN_DECL

extern void (*rs_out_of_memory_hook) (void);

extern void *rs_confirm (void *__p);
extern void *rs_malloc (size_t __size);
extern void *rs_realloc (void *__p, size_t __size);
extern void *rs_calloc (size_t __elem, size_t __size);
extern void *rs_free (void *__p);
extern void *rs_memdup (void const *__p, size_t __size);
extern char *rs_strdup (char const *__s);
extern char *rs_strndup (char const *__s, size_t __n);

RS_ALLOC_END_DECL

#endif /* not RS_ALLOC_H */
