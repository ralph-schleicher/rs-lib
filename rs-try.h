/* rs-try.h --- throw/catch control flow pattern.

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

#ifndef RS_TRY_H
#define RS_TRY_H

#ifndef RS_TRY_USE_SIGJMP
#define RS_TRY_USE_SIGJMP 0
#endif
#ifndef RS_TRY_USE_THREADS
#define RS_TRY_USE_THREADS 1
#endif
#ifndef RS_TRY_USE_KEYWORDS
#define RS_TRY_USE_KEYWORDS 0
#endif
#ifndef RS_TRY_STACK_SIZE
#define RS_TRY_STACK_SIZE 0
#endif

#include <setjmp.h>

#ifdef __cplusplus
#define RS_TRY_BEGIN_DECL extern "C" {
#define RS_TRY_END_DECL }
#else /* not __cplusplus */
#define RS_TRY_BEGIN_DECL
#define RS_TRY_END_DECL
#endif /* not __cplusplus */

RS_TRY_BEGIN_DECL

#if RS_TRY_USE_SIGJMP
#define _rs_try_jmp_buf sigjmp_buf
#define _rs_try_setjmp(__state) sigsetjmp ((__state), 1)
#define _rs_try_longjmp(__state,__value) siglongjmp ((__state), (__value))
#else /* not RS_TRY_USE_SIGJMP */
#define _rs_try_jmp_buf jmp_buf
#define _rs_try_setjmp(__state) setjmp (__state)
#define _rs_try_longjmp(__state,__value) longjmp ((__state), (__value))
#endif /* not RS_TRY_USE_SIGJMP */

#if RS_TRY_USE_THREADS
#ifdef _MSC_VER
#define _rs_try_thread_local __declspec(thread)
#else /* not _MSC_VER */
#define _rs_try_thread_local __thread
#endif /* not _MSC_VER */
#else /* not RS_TRY_USE_THREADS */
#define _rs_try_thread_local
#endif /* not RS_TRY_USE_THREADS */

/* Catch tag object.

   A null pointer is a valid catch tag, too.  */
typedef struct _rs_try_tag rs_try_tag_t;

struct _rs_try_tag
  {
    /* Super-type.  */
    rs_try_tag_t const *_rs_super;

    /* Symbol name.  */
    char const *_rs_name;
  };

/* Define a catch tag.  */
#define rs_try_define_tag(__name,__super) \
rs_try_tag_t const __name[1] = {{__super, #__name}}

/* Declare a catch tag.  */
#define rs_try_declare_tag(__name) \
rs_try_tag_t const __name[]

/* Built-in catch tags.  */
extern rs_try_declare_tag (RS_TRY_CONDITION);
extern rs_try_declare_tag (RS_TRY_WARNING);
extern rs_try_declare_tag (RS_TRY_ERROR);

/* The catch tag used to mark the end of a list of catch tags.  */
extern rs_try_declare_tag (_rs_try_end);

/* Return true if TAG1 is a sub-type of TAG2.  */
extern int rs_try_subtypep (rs_try_tag_t const *__tag1, rs_try_tag_t const *__tag2);

/* Return the super-type of a catch tag.  */
extern rs_try_tag_t const *rs_try_super_tag (rs_try_tag_t const *__tag);

/* Return the symbol name of a catch tag as a string constant.  */
extern char const *rs_try_tag_name (rs_try_tag_t const *__tag);

/* Throw/catch control flow environment.  */
typedef struct _rs_try_env rs_try_env_t;

struct _rs_try_env
  {
    /* Execution step.  Value is either ‘try’, ‘catch’, ‘finally’,
       or ‘done’.  */
    int _rs_step;

    /* Whether or not a tag has been thrown/caught.  */
    int _rs_thrown:1;
    int _rs_caught:1;

    /* The tag argument of the throw.  */
    rs_try_tag_t const *_rs_tag;

    /* The super-type matching the thrown tag.  */
    rs_try_tag_t const *_rs_super;

    /* Source code location where the throw occurs.  */
    char const *_rs_file;
    int _rs_line;

    /* Value of ‘errno’ when the throw occurs.  */
    int _rs_errno;

    /* Up-level control flow environment.  */
    rs_try_env_t *_rs_up;

    /* Program state.  */
    _rs_try_jmp_buf _rs_state;
 };

/* The most recently established throw/catch control flow environment.  */
extern _rs_try_thread_local rs_try_env_t *_rs_try_env;

/* Return the tag argument of the throw.  */
extern rs_try_tag_t const *rs_try_throw_tag (void);

/* Return the super-type matching the thrown tag.  */
extern rs_try_tag_t const *rs_try_catch_tag (void);

/* Return the source code location where the throw occurs.  */
extern char const *rs_try_file_name (void);
extern int rs_try_line_number (void);

/* Return the value of ‘errno’ when the throw occurs.  */
extern int rs_try_error_number (void);

/* Function to be called if a control error occurs.  */
extern void (*rs_try_control_error_hook) (void);

/* Control flow.  */
extern int _rs_try_init (void);
extern int _rs_try_test (void);

extern int _rs_try (void);
extern int _rs_catch (void *__ignored, ...);
extern int _rs_finally (void);

extern void _rs_throw (rs_try_tag_t const *__tag, char const *__file, int __line);

#define rs_try \
for (_rs_try_init (), _rs_try_setjmp (_rs_try_env->_rs_state); _rs_try_test (); ) if (_rs_try ())

#define rs_catch(...) \
else if (_rs_catch (NULL, __VA_ARGS__, _rs_try_end))

#define rs_finally \
else if (_rs_finally ())

#define rs_throw(__tag) \
_rs_throw ((__tag), __FILE__, __LINE__)

#if RS_TRY_USE_KEYWORDS
#ifdef try
#undef try
#endif
#ifdef catch
#undef catch
#endif
#ifdef finally
#undef finally
#endif
#ifdef throw
#undef throw
#endif
#define try rs_try
#define catch rs_catch
#define finally rs_finally
#define throw rs_throw
#endif /* RS_TRY_USE_KEYWORDS */

RS_TRY_END_DECL

#endif /* not RS_TRY_H */
