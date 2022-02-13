/* rs-string.h --- string functions

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

#ifndef RS_STRING_H
#define RS_STRING_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
#define RS_STRING_BEGIN_DECL extern "C" {
#define RS_STRING_END_DECL }
#else /* not __cplusplus */
#define RS_STRING_BEGIN_DECL
#define RS_STRING_END_DECL
#endif /* not __cplusplus */

RS_STRING_BEGIN_DECL

/* Create a single string from a variable number of arguments.
   The first null pointer argument terminates the argument list.

   If the first argument is not a null pointer, return value is
   a string.  If the first argument is a null pointers, return
   value is a null pointer (no error).  If an error occurs, the
   return value is a null pointer and ‘errno’ is set to describe
   the error.  */
extern char *rs_concat (char const *__first, ...);
extern wchar_t *rs_wconcat (wchar_t const *__first, ...);

/* Create a single string from a variable number of arguments.
   The first null pointer argument terminates the argument list.

   Return value is a string.  If an error occurs, the return
   value is a null pointer and ‘errno’ is set to describe the
   error.  */
extern char *rs_concatx (char const *__first, ...);
extern wchar_t *rs_wconcatx (wchar_t const *__first, ...);

/* Like the ‘rs_concat’ function described above, but use the
   argument pointer ARGP to get the optional arguments.  */
extern char *rs_concatv (char const *__first, va_list __argp);
extern wchar_t *rs_wconcatv (wchar_t const *__first, va_list __argp);

/* Like the ‘rs_concatx’ function described above, but use the
   argument pointer ARGP to get the optional arguments.  */
extern char *rs_concatvx (char const *__first, va_list __argp);
extern wchar_t *rs_wconcatvx (wchar_t const *__first, va_list __argp);

/* Concatenate the two strings FIRST and SECOND.

   If any argument is not a null pointer, return value is
   a string.  If all arguments are null pointers, return
   value is a null pointer (no error).  If an error occurs,
   the return value is a null pointer and ‘errno’ is set to
   describe the error.  */
extern char *rs_concat2 (char const *__first, char const *__second);
extern wchar_t *rs_wconcat2 (wchar_t const *__first, wchar_t const *__second);

/* Concatenate the two strings FIRST and SECOND.

   Any null pointer argument is treated like an empty string.

   Return value is a string.  If an error occurs, the return
   value is a null pointer and ‘errno’ is set to describe the
   error.  */
extern char *rs_concat2x (char const *__first, char const *__second);
extern wchar_t *rs_wconcat2x (wchar_t const *__first, wchar_t const *__second);

/* Concatenate the three strings FIRST, SECOND, and THIRD.

   If any argument is not a null pointer, return value is
   a string.  If all arguments are null pointers, return
   value is a null pointer (no error).  If an error occurs,
   the return value is a null pointer and ‘errno’ is set to
   describe the error.  */
extern char *rs_concat3 (char const *__first, char const *__second, char const *__third);
extern wchar_t *rs_wconcat3 (wchar_t const *__first, wchar_t const *__second, wchar_t const *__third);

/* Concatenate the three strings FIRST, SECOND, and THIRD.

   Any null pointer argument is treated like an empty string.

   Return value is a string.  If an error occurs, the return
   value is a null pointer and ‘errno’ is set to describe the
   error.   */
extern char *rs_concat3x (char const *__first, char const *__second, char const *__third);
extern wchar_t *rs_wconcat3x (wchar_t const *__first, wchar_t const *__second, wchar_t const *__third);

RS_STRING_END_DECL

#endif /* not RS_STRING_H */
