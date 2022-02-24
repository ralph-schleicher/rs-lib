/* rs-try.c --- throw/catch control flow pattern

   Copyright (C) 2017 Ralph Schleicher

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following err
   are met:

      * Redistributions of source code must retain the above copyright
        notice, this list of err and the following disclaimer.

      * Redistributions in binary form must reproduce the above copyright
        notice, this list of err and the following disclaimer in
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

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "rs-try.h"

#ifdef __GNUC__
#define static_inline static inline
#else /* not __GNUC__ */
#ifdef _MSC_VER
#define static_inline static __inline
#else /* not _MSC_VER */
#define static_inline static
#endif /* not _MSC_VER */
#endif /* not __GNUC__ */

#if RS_TRY_STACK_SIZE > 0
#define USE_HEAP 0
#define USE_STACK 1
#define STACK_SIZE RS_TRY_STACK_SIZE
#else /* RS_TRY_STACK_SIZE <= 0 */
#define USE_HEAP 1
#define USE_STACK 0
#define STACK_SIZE 0
#endif /* RS_TRY_STACK_SIZE <= 0 */

/* Symbolic names for the execution steps.  */
enum
  {
    TRY = 1,
    CATCH,
    FINALLY,
    DONE
  };

/* Built-in catch tags.  */
rs_try_define_tag (RS_TRY_CONDITION, NULL);
rs_try_define_tag (RS_TRY_WARNING, RS_TRY_CONDITION);
rs_try_define_tag (RS_TRY_ERROR, RS_TRY_CONDITION);

/* The catch tag used to mark the end of a list of catch tags.  */
rs_try_tag_t const _rs_try_end[1];

/* The most recently established throw/catch control flow environment.  */
_rs_try_thread_local rs_try_env_t *_rs_try_env = NULL;

/* Function to be called if a control error occurs.  */
void (*rs_try_control_error_hook) (void) = NULL;

/* Signal a control error.  */
static_inline void
control_error (void)
{
  if (rs_try_control_error_hook != NULL)
    rs_try_control_error_hook ();

  abort ();
}

#if USE_STACK

/* Pre-allocate throw/catch control flow environments.  */
static _rs_try_thread_local rs_try_env_t stack[STACK_SIZE];

/* Return the current stack depth, i.e. the number of active
   throw/catch control flow environments.  */
static_inline int
stack_depth (void)
{
  return (_rs_try_env == NULL ? 0 : _rs_try_env - stack + 1);
}

#endif /* USE_STACK */

static_inline void
transfer_control (int up)
{
  if (up != 0)
    {
      rs_try_env_t *env;

      env = _rs_try_env->_rs_up;
      if (env == NULL)
	{
	  errno = EFAULT;
	  control_error ();
	}

      /* Move catch tag and associated data.  */
      env->_rs_tag = _rs_try_env->_rs_tag;
      env->_rs_super = _rs_try_env->_rs_super;
      env->_rs_file = _rs_try_env->_rs_file;
      env->_rs_line = _rs_try_env->_rs_line;
      env->_rs_errno = _rs_try_env->_rs_errno;

#if USE_HEAP
      free (_rs_try_env);
#endif
      _rs_try_env = env;
    }

  _rs_try_env->_rs_thrown = 1;
  _rs_try_env->_rs_caught = 0;

  _rs_try_longjmp (_rs_try_env->_rs_state, 1);
}

static_inline int
subtypep (rs_try_tag_t const *tag, rs_try_tag_t const *super)
{
  /* ‘NULL’ is the mother of all tags.  */
  if (super == NULL)
    return 1;

  for (; tag != NULL; tag = tag->_rs_super)
    {
      if (tag == super)
	return 1;
    }

  return 0;
}

int
_rs_try_init (void)
{
  rs_try_env_t *env;

#if USE_HEAP

  env = calloc (1, sizeof (rs_try_env_t));
  if (env == NULL)
    control_error ();

#else /* not USE_HEAP */
#if USE_STACK

  if (_rs_try_env == NULL)
    env = stack;
  else if (stack_depth () < RS_TRY_STACK_SIZE)
    env = _rs_try_env + 1;
  else
    {
      errno = ENOMEM;
      control_error ();
    }

#else /* not USE_STACK*/
#error "Fix me"
#endif /* not USE_STACK */
#endif /* not USE_HEAP */

  /* Initialization.  */
  env->_rs_step = 0;
  env->_rs_thrown = 0;
  env->_rs_caught = 0;
  env->_rs_tag = NULL;
  env->_rs_super = NULL;
  env->_rs_file = NULL;
  env->_rs_line = 0;
  env->_rs_errno = 0;
  env->_rs_up = _rs_try_env;

  _rs_try_env = env;

  return 0;
}

int
_rs_try_test (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  /* Next step.  */
  ++_rs_try_env->_rs_step;

  if (_rs_try_env->_rs_step == DONE)
    {
      rs_try_env_t *env;

      if (_rs_try_env->_rs_thrown && ! _rs_try_env->_rs_caught)
	transfer_control (1);

      env = _rs_try_env->_rs_up;

#if USE_HEAP
      free (_rs_try_env);
#endif
      _rs_try_env = env;

      return 0;
    }

  return 1;
}

int
_rs_try (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  return (_rs_try_env->_rs_step == TRY);
}

int
_rs_catch (void *ignored, ...)
{
  va_list argp;
  rs_try_tag_t const *tag;

  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_step != CATCH || _rs_try_env->_rs_thrown == 0 || _rs_try_env->_rs_caught != 0)
    return 0;

  va_start (argp, ignored);

  while (1)
    {
      tag = va_arg (argp, rs_try_tag_t const *);
      if (tag == _rs_try_end)
	break;

      if (subtypep (_rs_try_env->_rs_tag, tag))
	{
	  _rs_try_env->_rs_caught = 1;
	  _rs_try_env->_rs_super = tag;

	  break;
	}
    }

  va_end (argp);

  return _rs_try_env->_rs_caught;
}

int
_rs_finally (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  return (_rs_try_env->_rs_step == FINALLY);
}

void
_rs_throw (rs_try_tag_t const *tag, char const *file, int line)
{
  if (_rs_try_env == NULL)
    {
      errno = EFAULT;
      control_error ();
    }

  _rs_try_env->_rs_tag = tag;
  _rs_try_env->_rs_file = file;
  _rs_try_env->_rs_line = line;
  _rs_try_env->_rs_errno = errno;

  transfer_control (_rs_try_env->_rs_step == TRY ? 0 : 1);
}

/* Return true if TAG1 is a sub-type of TAG2.  */
int
rs_try_subtypep (rs_try_tag_t const *tag1, rs_try_tag_t const *tag2)
{
  return subtypep (tag1, tag2);
}

/* Return the super-type of a catch tag.  */
rs_try_tag_t const *
rs_try_super_tag (rs_try_tag_t const *tag)
{
  return (tag == NULL ? NULL : tag->_rs_super);
}

/* Return the symbol name of a catch tag as a string constant.  */
char const *
rs_try_tag_name (rs_try_tag_t const *tag)
{
  return (tag == NULL ? "NULL" : tag->_rs_name);
}

/* Return the tag argument of the throw.  */
rs_try_tag_t const *
rs_try_throw_tag (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_thrown == 0)
    errno = EAGAIN;

  return _rs_try_env->_rs_tag;
}

/* Return the super-type matching the thrown tag.  */
rs_try_tag_t const *
rs_try_catch_tag (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_thrown == 0)
    errno = EAGAIN;

  return _rs_try_env->_rs_super;
}

/* Return the source code location where the throw occurs.  */
char const *
rs_try_file_name (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_thrown == 0)
    errno = EAGAIN;

  return _rs_try_env->_rs_file;
}

int
rs_try_line_number (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_thrown == 0)
    errno = EAGAIN;

  return _rs_try_env->_rs_line;
}

/* Return the value of ‘errno’ when the throw occurs.  */
int
rs_try_error_number (void)
{
  if (_rs_try_env == NULL)
    {
      errno = EINVAL;
      control_error ();
    }

  if (_rs_try_env->_rs_thrown == 0)
    errno = EAGAIN;

  return _rs_try_env->_rs_errno;
}
