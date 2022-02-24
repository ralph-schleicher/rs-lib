/* rs-lock.c --- Emacs style lock files

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif
#ifndef HAVE_PROCESS_H
#define HAVE_PROCESS_H 1
#endif
#ifndef ssize_t
#define ssize_t int
#endif
#ifndef pid_t
#define pid_t int
#endif
#else /* not _MSC_VER */
#ifdef __LCC__
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif
#ifndef HAVE_PROCESS_H
#define HAVE_PROCESS_H 1
#endif
#ifndef ssize_t
#define ssize_t int
#endif
#ifndef pid_t
#define pid_t int
#endif
#else /* not __LCC__ */
#ifdef __MINGW32__
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif
#ifndef HAVE_PROCESS_H
#define HAVE_PROCESS_H 1
#endif
#else /* not __MINGW32__ */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif
#ifndef HAVE_SYMLINK
#define HAVE_SYMLINK 1
#endif
#ifndef HAVE_READLINK
#define HAVE_READLINK 1
#endif
#ifndef HAVE_KILL
#define HAVE_KILL 1
#endif
#endif /* not __MINGW32__ */
#endif /* not __LCC__ */
#endif /* not _MSC_VER */

#ifdef __GNUC__
#ifndef alloca
#define alloca __builtin_alloca
#endif /* not alloca */
#else /* not __GNUC__ */
#ifdef _MSC_VER
#include <malloc.h>
#define alloca _alloca
#else /* not _MSC_VER */
#if HAVE_ALLOCA_H
#include <alloca.h>
#else /* not HAVE_ALLOCA_H */
#ifdef _AIX
#pragma alloca
#else /* not _AIX */
#ifndef alloca
#include <stddef.h>
/* Allocate N bytes of memory on the stack of the calling function.  */
extern void *alloca (size_t __n);
#endif /* not alloca */
#endif /* not _AIX */
#endif /* not HAVE_ALLOCA_H */
#endif /* not _MSC_VER */
#endif /* not __GNUC__ */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#if HAVE_PROCESS_H
#include <process.h>
#ifdef _MSC_VER
#define getpid _getpid
#endif /* _MSC_VER */
#endif /* HAVE_PROCESS_H */
#ifdef _MSC_VER
#include <io.h>
#define open _open
#define close _close
#define read _read
#define write _write
#define unlink _unlink
#endif /* _MSC_VER */
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include "rs-lock.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* Switch statement fall though marker.  */
#if defined (__GNUC__) && (__GNUC__ >= 7)
#define fall_through __attribute__ ((fallthrough))
#else /* not __GNUC__ */
#define fall_through /* FALLTHROUGH */ (void) 0
#endif /* not __GNUC__ */

/* Buffer size in bytes (including the terminating null character) to
   store the lock file name associated with FILE_NAME.  */
#define LOCK_FILE_NAME_SIZE(file_name) ((strlen (file_name) + 2 + 1) * sizeof (char))

/* Number of characters required for storing the printed representation
   of the largest possible process identifier (including the terminating
   null character).  */
#define PID_LENGTH ((4 * sizeof (long int)) + 1)

/* Create a symbolic link.  */
static int
do_symlink (char const *buffer, char const *lock_file_name)
{
#if HAVE_SYMLINK

  return symlink (buffer, lock_file_name);

#else /* not HAVE_SYMLINK */

  int h;
  size_t s;
  ssize_t n;

  h = open (lock_file_name, O_WRONLY | O_BINARY | O_CREAT | O_EXCL, S_IREAD | S_IWRITE);
  if (h == -1)
    return -1;

  s = strlen (buffer) * sizeof (char);
  n = write (h, buffer, s);

  if (close (h) == -1)
    return -1;

  return (n == (ssize_t) (s)) ? 0 : -1;

#endif /* not HAVE_SYMLINK */
}

/* Read contents of symbolic link.  */
static int
do_readlink (char const *lock_file_name, char *buffer, size_t size)
{
#if HAVE_READLINK

  return readlink (lock_file_name, buffer, size);

#else /* not HAVE_READLINK */

  int h;
  size_t s;
  ssize_t n;

  h = open (lock_file_name, O_RDONLY | O_BINARY);
  if (h == -1)
    return -1;

  s = size * sizeof (char);
  n = read (h, buffer, s);

  if (close (h) == -1)
    return -1;

  if (n < 0 || (n % sizeof (char)) != 0)
    return -1;

  return n / sizeof (char);

#endif /* not HAVE_READLINK */
}

/* Check if PID is a valid process.  */
static int
do_kill (pid_t pid)
{
#if HAVE_KILL

  return kill (pid, 0);

#else /* not HAVE_KILL */
#ifdef _WIN32

  HANDLE h;

  h = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);
  if (h != NULL)
    {
      CloseHandle (h);

      return 0;
    }

  if (GetLastError () == ERROR_ACCESS_DENIED)
    errno = EPERM;
  else
    errno = ESRCH;

  return -1;

#endif /* _WIN32 */

  errno = EPERM;

  return -1;

#endif /* not HAVE_KILL */
}

/* Return true if CH is a file name separator character.  */
static int
file_name_separator_p (int ch)
{
#ifdef _WIN32

  return (ch == '\\' || ch == '/');

#else /* not _WIN32 */

  return (ch == '/');

#endif /* not _WIN32 */
}

/* Put the lock file name associated with FILE_NAME into
   LOCK_FILE_NAME.  */
static void
make_lock_file_name (char *lock_file_name, char const *file_name)
{
  char *point, *mark;

  strcpy (lock_file_name, file_name);

  /* Shift the non-directory part of the file name (including the
     terminating null character) right two characters.  */
  for (point = mark = lock_file_name; *point != 0; ++point)
    {
      if (file_name_separator_p (*point))
	mark = point + 1;
    }

  memmove (mark + 2, mark, (strlen (mark) + 1) * sizeof (char));

  /* Insert the '.#'.  */
  mark[0] = '.';
  mark[1] = '#';
}

/* Determine the owner of a lock.  Return value is 0 if the lock file
   does not exist or if it is obsolete, 1 if another process owns it,
   2 if the current process owns it, and -1 if an error occurs.  */
static int
lock_owner (char const *lock_file_name)
{
  char buffer[PID_LENGTH];
  int length;
  long int owner;

  /* Read contents of lock file.  */
  length = do_readlink (lock_file_name, buffer, PID_LENGTH);
  if (length < 0 || length >= (int) PID_LENGTH)
    return -1;

  /* Terminate BUFFER.  */
  buffer[length] = 0;

  /* Attempt to parse the owner's process id.  */
  if (sscanf (buffer, "%ld", &owner) != 1)
    owner = -1;

  /* Assume that the lock is obsolete if it contains an invalid
     process id.  */
  if (owner > 0 && owner == (pid_t) owner)
    {
      /* Check if the current process owns the lock.  */
      if (owner == getpid ())
	return 2;

      /* Check if the owner of the lock is still alive.  */
      if (do_kill (owner) == 0 || errno == EPERM)
	return 1;
    }

  return 0;
}

/* Attempt to lock the file FILE_NAME.  */
int
rs_lock_file (char const *file_name, int force)
{
  char *lock_file_name;
  char buffer[PID_LENGTH];
  int result;

  /* Create the name of the lock file.  */
  lock_file_name = alloca (LOCK_FILE_NAME_SIZE (file_name));
  if (lock_file_name == NULL)
    return -1;

  make_lock_file_name (lock_file_name, file_name);

  /* Create the lock file contents.  */
  sprintf (buffer, "%ld", (long int) getpid ());

  /* Try to lock the file.  */
  result = do_symlink (buffer, lock_file_name);
  if (result == 0)
    return 0;

  if (errno == EEXIST)
    {
      switch (lock_owner (lock_file_name))
	{
	case 2:

	  return 0;

	case 1:

	  if (force == 0)
	    return 1;

	  fall_through;

	case 0:

	  /* Try to break the lock.  */
	  if (unlink (lock_file_name) == 0)
	    return do_symlink (buffer, lock_file_name);
	}
    }

  return -1;
}

/* Unlock the file FILE_NAME.  */
int
rs_unlock_file (char const *file_name)
{
  char *lock_file_name;
  int owner;

  /* Create the name of the lock file.  */
  lock_file_name = alloca (LOCK_FILE_NAME_SIZE (file_name));
  if (lock_file_name == NULL)
    return -1;

  make_lock_file_name (lock_file_name, file_name);

  /* Determine owner of the lock.  */
  owner = lock_owner (lock_file_name);
  if (0 && owner != 2)
    return owner;

  return unlink (lock_file_name);
}

/* Test if file FILE_NAME is locked.  */
int
rs_file_locked_p (char const *file_name)
{
  char *lock_file_name;

  /* Create the name of the lock file.  */
  lock_file_name = alloca (LOCK_FILE_NAME_SIZE (file_name));
  if (lock_file_name == NULL)
    return -1;

  make_lock_file_name (lock_file_name, file_name);

  return lock_owner (lock_file_name);
}
