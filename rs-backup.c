/* rs-backup.c --- GNU style backup files.

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER
#ifndef HAVE_RENAME
#define HAVE_RENAME 1
#endif
#else /* not _MSC_VER */
#ifdef __LCC__
#ifndef HAVE_RENAME
#define HAVE_RENAME 1
#endif
#else /* not __LCC__ */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif
#ifndef HAVE_RENAME
#define HAVE_RENAME 1
#endif
#endif /* not __LCC__ */
#endif /* not _MSC_VER */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef _MSC_VER
#include "dirent-w32.h"
#else /* not _MSC_VER  */
#include <dirent.h>
#endif /* not _MSC_VER  */

#include "rs-backup.h"

#ifdef _WIN32
#ifdef _MSC_VER
#define xstrncmp strnicmp
#else /* not _MSC_VER */
#define xstrncmp strncasecmp
#endif /* not _MSC_VER */
#else /* not _WIN32 */
#define xstrncmp strncmp
#endif /* not _WIN32 */
#ifdef _MSC_VER
#define unlink _unlink
#define strnicmp _strnicmp
#endif

/* Forward declarations.  */
static char *backup_file_name (char const *__file_name, int __method);
static char *simple_backup_file_name (char const *__file_name);
static char *numbered_backup_file_name (char const *__file_name, int __force);
static int digitp (int __c);

/* Backup a file.  */
int
rs_backup_file (char const *file_name, int method)
{
  char *backup;
  int olderr, retval;

  backup = backup_file_name (file_name, method);
  if (backup == NULL)
    return -1;

  if (backup == file_name)
    return unlink (backup);

  olderr = errno;
  retval = -1;

#if HAVE_RENAME

  if (rename (file_name, backup) == -1 && errno != ENOENT)
    {
      /* On Windows, 'rename' fails if a file with the new name exists.  */
      if (errno == EEXIST)
	{
	  if (unlink (backup) == 0 && rename (file_name, backup) == 0)
	    goto success;
	}

      goto failure;
    }

#else /* not HAVE_RENAME */

  if (unlink (backup) == -1 && errno != ENOENT)
    goto failure;

  if (link (file_name, backup) == -1 && errno == ENOENT)
    goto success;

  if (unlink (file_name) == -1)
    goto failure;

#endif /* not HAVE_RENAME */

 success:

  errno = olderr;
  retval = 0;

 failure:

  free (backup);
  return retval;
}

/* Return the backup file name for FILE_NAME.  */
static char *
backup_file_name (char const *file_name, int method)
{
  if (method == RS_BACKUP_DEFAULT)
    {
      char *env = getenv ("VERSION_CONTROL");

      method = (env == NULL ?
		RS_BACKUP_EXISTING :
		((strcmp (env, "simple") == 0
		  || strcmp (env, "never") == 0) ?
		 RS_BACKUP_SIMPLE :
		 ((strcmp (env, "numbered") == 0
		   || strcmp (env, "t") == 0) ?
		  RS_BACKUP_NUMBERED :
		  ((strcmp (env, "existing") == 0
		    || strcmp (env, "nil") == 0) ?
		   RS_BACKUP_EXISTING :
		   ((strcmp (env, "none") == 0
		     || strcmp (env, "off") == 0) ?
		    RS_BACKUP_NONE :
		    /* Any other value.  */
		    RS_BACKUP_EXISTING)))));
    }

  switch (method)
    {
    case RS_BACKUP_SIMPLE:
      return simple_backup_file_name (file_name);
    case RS_BACKUP_NUMBERED:
      return numbered_backup_file_name (file_name, 1);
    case RS_BACKUP_EXISTING:
      return numbered_backup_file_name (file_name, 0);
    case RS_BACKUP_NONE:
      return (char *) file_name;
    }

  errno = EINVAL;
  return NULL;
}

static char *
simple_backup_file_name (char const *file_name)
{
  char *backup;

  backup = calloc (strlen (file_name) + 2, sizeof (char));
  if (backup != NULL)
    {
      strcpy (backup, file_name);
      strcat (backup, "~");
    }

  return backup;
}

static char *
numbered_backup_file_name (char const *file_name, int force)
{
  DIR *dir;
  char *backup;
  size_t len, minlen;
  long int vernum, num;
  char *start, *tail;
  char *point;
  int tem;

  backup = calloc (strlen (file_name) + 4 * sizeof (long int) + 4, sizeof (char));
  if (backup == NULL)
    return NULL;

  /* Replace backslashes with slashes.  */
  point = strcpy (backup, file_name);

  while (1)
    {
      point = strchr (point, '\\');
      if (point == NULL)
	break;

      *point++ = '/';
    }

  /* Put START at the beginning of the file name part
     and open the parent directory for reading.  */
  start = strrchr (backup, '/');
  if (start != NULL)
    {
      start += 1;
      tem = *start;
      *start = 0;

      dir = opendir (backup);

      *start = tem;
    }
  else
    {
      start = backup;

      dir = opendir (".");
    }

  if (dir == NULL)
    goto failure;

  /* Minimum file name length of a numbered backup file name.  */
  len = strlen (start);
  minlen = len + 4;

  /* Highest version number.  */
  vernum = force ? 0 : -1;

  while (1)
    {
      struct dirent *ent;

      ent = readdir (dir);
      if (ent == NULL)
	break;

      tail = ent->d_name + len;

      if (strlen (ent->d_name) < minlen
	  || xstrncmp (ent->d_name, start, len) != 0
	  || tail[0] != '.' || tail[1] != '~' || ! digitp (tail[2]))
	continue;

      for (point = tail + 3; digitp (*point); ++point)
	(void) 0;

      if (point[0] != '~' || point[1] != 0)
	continue;

      /* Found a numbered backup file.  */
      tem = errno;
      errno = 0;

      num = strtol (tail + 2, NULL, 10);
      if (errno == 0 && num < LONG_MAX)
	{
	  if (num > vernum)
	    vernum = num;
	}

      errno = tem;
    }

  if (closedir (dir) == -1)
    goto failure;

  dir = NULL;

  /* Make numbered backups for files that have numbered backups.
     Otherwise, make single backups.  */
  tail = start + len;

  if (vernum < 0)
    strcpy (tail, "~");
  else
    sprintf (tail, ".~%ld~", vernum + 1);

  point = realloc (backup, strlen (backup) + 1);
  if (point == NULL)
    goto failure;

  backup = point;
  return backup;

 failure:

  if (dir != NULL)
    closedir (dir);

  if (backup != NULL)
    free (backup);

  return NULL;
}

static int
digitp (int c)
{
  switch (c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':

      return 1;

    default:

      return 0;
    }
}
