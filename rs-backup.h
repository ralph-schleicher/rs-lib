/* rs-backup.h --- GNU style backup files.

   Copyright (C) 2006 Ralph Schleicher

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

#ifndef RS_BACKUP_H
#define RS_BACKUP_H

#ifdef __cplusplus
#define RS_BACKUP_BEGIN_DECL extern "C" {
#define RS_BACKUP_END_DECL }
#else /* not __cplusplus */
#define RS_BACKUP_BEGIN_DECL
#define RS_BACKUP_END_DECL
#endif /* not __cplusplus */

RS_BACKUP_BEGIN_DECL

/* Backup methods.  */
enum
  {
    /* Evaluate 'VERSION_CONTROL' environment variable.
       If it is not set, fall back to 'existing'.  */
    RS_BACKUP_DEFAULT = 0,

    /* Always make simple backups.  */
    RS_BACKUP_SIMPLE = 1,

    /* Always make numbered backups.  */
    RS_BACKUP_NUMBERED = 2,

    /* Make numbered backups for files that have numbered backups.
       Otherwise, make simple backups.  */
    RS_BACKUP_EXISTING = 3,

    /* Never make a backup file.  */
    RS_BACKUP_NONE = -1
  };

/* Backup a file.

   First argument FILE_NAME is the file to be saved.
   Second argument METHOD is the backup method.

   Return value is 0 on success, that means FILE_NAME is out of the way.
   In case of an error, a value of -1 is returned and 'errno' is set to
   describe the error.  */
extern int rs_backup_file (char const *__file_name, int __method);

RS_BACKUP_END_DECL

#endif /* not RS_BACKUP_H  */
