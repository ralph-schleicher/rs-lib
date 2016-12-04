/* rs-cons.h --- cons cells and lists.

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

#ifndef RS_CONS_H
#define RS_CONS_H

#ifdef __cplusplus
#define RS_CONS_BEGIN_DECL extern "C" {
#define RS_CONS_END_DECL }
#else /* not __cplusplus */
#define RS_CONS_BEGIN_DECL
#define RS_CONS_END_DECL
#endif /* not __cplusplus */

RS_CONS_BEGIN_DECL

/* Creating cons cells.  */
extern void *rs_cons (void const *__obj1, void const *__obj2);

/* Accessing cons cells.  */
extern void *rs_car (void const *__cell);
extern void *rs_cdr (void const *__cell);
extern void *rs_caar (void const *__cell);
extern void *rs_cadr (void const *__cell);
extern void *rs_cdar (void const *__cell);
extern void *rs_cddr (void const *__cell);
extern void *rs_caaar (void const *__cell);
extern void *rs_caadr (void const *__cell);
extern void *rs_cadar (void const *__cell);
extern void *rs_caddr (void const *__cell);
extern void *rs_cdaar (void const *__cell);
extern void *rs_cdadr (void const *__cell);
extern void *rs_cddar (void const *__cell);
extern void *rs_cdddr (void const *__cell);
extern void *rs_caaaar (void const *__cell);
extern void *rs_caaadr (void const *__cell);
extern void *rs_caadar (void const *__cell);
extern void *rs_caaddr (void const *__cell);
extern void *rs_cadaar (void const *__cell);
extern void *rs_cadadr (void const *__cell);
extern void *rs_caddar (void const *__cell);
extern void *rs_cadddr (void const *__cell);
extern void *rs_cdaaar (void const *__cell);
extern void *rs_cdaadr (void const *__cell);
extern void *rs_cdadar (void const *__cell);
extern void *rs_cdaddr (void const *__cell);
extern void *rs_cddaar (void const *__cell);
extern void *rs_cddadr (void const *__cell);
extern void *rs_cdddar (void const *__cell);
extern void *rs_cddddr (void const *__cell);

/* Modifying cons cells.  */
extern void *rs_rplaca (void *__cell, void const *__obj);
extern void *rs_rplacd (void *__cell, void const *__obj);
extern void *rs_setcar (void *__cell, void const *__obj);
extern void *rs_setcdr (void *__cell, void const *__obj);

/* Building lists.  */
extern void *rs_make_list (int __n, void const *__obj);
extern void rs_free_list (void *__list, void (*__f) (void *));

extern void *rs_list (int __n, ...);

/* List properties.  */
extern int rs_list_length (void const *__list);

/* Accessing list elements.  */
extern void *rs_list_first (void const *__list);
extern void *rs_list_second (void const *__list);
extern void *rs_list_third (void const *__list);
extern void *rs_list_fourth (void const *__list);
extern void *rs_list_fifth (void const *__list);
extern void *rs_list_sixth (void const *__list);
extern void *rs_list_seventh (void const *__list);
extern void *rs_list_eighth (void const *__list);
extern void *rs_list_ninth (void const *__list);
extern void *rs_list_tenth (void const *__list);

extern void *rs_list_nth (int __n, void const *__list);

/* Accessing sub-lists.  */
extern void *rs_list_rest (void const *__list);

extern void *rs_list_nthcdr (int __n, void const *__list);
extern void *rs_list_last (void const *__list, int __n);

/* Modifying list variables.  */
#define rs_list_push(__obj,__list) __rs_list_push ((__obj), &(__list))
extern void *__rs_list_push (void const *__obj, void **__addr);
#define rs_list_pop(__list) __rs_list_pop (&(__list))
extern void *__rs_list_pop (void **__addr);
#define rs_list_restack(__from,__to) __rs_list_restack (&(__from), &(__to))
extern void *__rs_list_restack (void **__addr1, void **__addr2);
#define rs_list_splice(__list,__offset,__length,__subst) __rs_list_splice (&(__list), (__offset), (__length), (__subst))
extern void *__rs_list_splice (void **__addr, int __offset, int __length, void const *__subst);

/* Rearranging lists.  */
extern void *rs_list_conc (void *__list, void const *__tail);
extern void *rs_list_reconc (void *__list, void const *__tail);
extern void *rs_list_reverse (void *__list);
extern void *rs_list_sort (void *__list, int (*__compar) (void const *, void const *));

/* Associated lists.  */
extern void *rs_list_acons (void const *__key, void const *__value, void const *__list);
extern void *rs_list_assoc (void const *__key, void const *__list, int (*__f) (void const *, void const *));
extern void *rs_list_assoc_if (int (*__p) (void const *), void *__list);
extern void *rs_list_assoc_if_not (int (*__p) (void const *), void *__list);

/* Iterating lists.  */
void *rs_list_mapc (void (*__f) (void const *), void const *__list);
void *rs_list_mapl (void (*__f) (void const *), void const *__list);

void *rs_list_mapc_data (void (*__f) (void const *, void const *), void const *__list, void const *__data);
void *rs_list_mapl_data (void (*__f) (void const *, void const *), void const *__list, void const *__data);

RS_CONS_END_DECL

#endif /* not RS_CONS_H */
