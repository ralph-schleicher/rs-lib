/* rs-lock.h --- Emacs style lock files.

   Copyright (C) 1999 Ralph Schleicher

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

#ifndef RS_LOCK_H
#define RS_LOCK_H

#ifdef __cplusplus
#define RS_LOCK_BEGIN_DECL extern "C" {
#define RS_LOCK_END_DECL }
#else /* not __cplusplus */
#define RS_LOCK_BEGIN_DECL
#define RS_LOCK_END_DECL
#endif /* not __cplusplus */

RS_LOCK_BEGIN_DECL

/* Attempt to lock the file FILE_NAME.  */
extern int rs_lock_file (char const *__file_name, int __force);

/* Unlock the file FILE_NAME.  */
extern int rs_unlock_file (char const *__file_name);

/* Test if file FILE_NAME is locked.  */
extern int rs_file_locked_p (char const *__file_name);

RS_LOCK_END_DECL

#endif /* not RS_LOCK_H  */
