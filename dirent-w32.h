/* dirent-w32.h --- basic ‘dirent’ for Windows

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

#ifndef DIRENT_W32_H
#define DIRENT_W32_H

/* For _MAX_PATH.  */
#include <stdlib.h>

#ifdef __cplusplus
#define DIRENT_W32_BEGIN_DECL extern "C" {
#define DIRENT_W32_END_DECL }
#else /* not __cplusplus */
#define DIRENT_W32_BEGIN_DECL
#define DIRENT_W32_END_DECL
#endif /* not __cplusplus */

DIRENT_W32_BEGIN_DECL

/* Opaque directory stream object.  */
typedef struct _dirent_w32_DIR DIR;

/* Data structure for a directory entry.  */
#define _DIRENT_HAVE_D_FILENO
#define _DIRENT_HAVE_D_NAMLEN
#define _DIRENT_HAVE_D_RECLEN

struct dirent
  {
    /* File name component.  */
    char d_name[_MAX_PATH];

    /* Length of the file name.  */
    size_t d_namlen, d_reclen;

    /* File serial number.  */
    long int d_ino, d_fileno;
  };

/* Functions.  */
extern DIR *opendir (char const *__dirname);
extern struct dirent *readdir (DIR *__dir);
extern int readdir_r (DIR *__dir, struct dirent *__entry, struct dirent **__result);
extern int closedir (DIR *__dir);

DIRENT_W32_END_DECL

#endif /* not DIRENT_W32_H */
