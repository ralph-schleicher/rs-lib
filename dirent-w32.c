/* dirent-w32.c --- basic ‘dirent’ for Windows.

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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <io.h>

#include "dirent-w32.h"

/* Directory stream object.  */
struct _dirent_w32_DIR
  {
    intptr_t handle;
    long int pos;
    int err;

    struct dirent ent[1];
    struct _finddata_t data[1];
  };

/* Open a directory stream for reading.  */
DIR *
opendir (char const *dirname)
{
  DIR *dir;
  char *spec;
  char *point;
  int old_err;

  if (dirname == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  /* Create directory stream object.  */
  dir = calloc (1, sizeof (DIR));
  if (dir == NULL)
    return NULL;

  dir->handle = -1;
  dir->pos = 0;
  dir->err = 0;

  /* Build file name pattern.  */
  spec = calloc (strlen (dirname) + 3, sizeof (char));
  if (spec == NULL)
    goto failure;

  for (point = strcpy (spec, dirname); *point != 0; ++point)
    {
      if (*point == '/')
	*point = '\\';
    }

  if (point != spec && point[-1] == '\\')
    --point;

  strcpy (point, "\\*");

  /* Open directory stream.  */
  old_err = errno;

  dir->handle = _findfirst (spec, dir->data);
  if (dir->handle == -1)
    {
      if (errno != ENOENT)
	goto failure;

      dir->err = errno;
      errno = old_err;
    }

  free (spec);
  return dir;

 failure:

  if (spec != NULL)
    free (spec);

  if (dir != NULL)
    free (dir);

  return NULL;
}

/* Read the next entry from the directory.  */
static struct dirent *
dirent_w32_readdir (DIR *dir, struct dirent *entry)
{
  struct _finddata_t data[1];

  if (dir->pos == 0)
    {
      if (dir->err != 0)
	{
	  errno = dir->err;
	  return NULL;
	}

      memcpy (data, dir->data, sizeof (struct _finddata_t));
    }
  else
    {
      if (_findnext (dir->handle, data) != 0)
	return NULL;
    }

  /* Prepare directory entry.  */
  strcpy (entry->d_name, data->name);
  entry->d_reclen = entry->d_namlen = strlen (entry->d_name);
  entry->d_fileno = entry->d_ino = dir->pos;

  /* Next entry.  */
  ++dir->pos;

  return entry;
}

struct dirent *
readdir (DIR *dir)
{
  if (dir == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  return dirent_w32_readdir (dir, dir->ent);
}

int
readdir_r (DIR *dir, struct dirent *entry, struct dirent **_result)
{
  struct dirent *result;

  if (dir == NULL || entry == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  /* TODO: Use a mutex lock.  */
  result = dirent_w32_readdir (dir, entry);

  if (_result != NULL)
    *_result = result;

  return (result != NULL ? 0 : -1);
}

/* Close a directory stream.  */
int
closedir (DIR *dir)
{
  if (dir == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  if (dir->handle != -1)
    {
      if (_findclose (dir->handle) != 0)
	return -1;
    }

  free (dir);
  return 0;
}
