/* rs-workshop-w32.c --- a simple but portable thread pool.

   Copyright (C) 2016 Ralph Schleicher

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

/* Commentary:

   Microsoft Windows 7 Enterprise (Windows NT 6.1.7601 Service Pack 1)
   Microsoft C/C++ Optimizing Compiler Version 16.00.30319.01 for x64 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
/* Use the new thread pool API.  */
#if defined (_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#undef _WIN32_WINNT
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>

/* Forward declarations.  */
typedef struct rs_task rs_task_t;

static void worker (TP_CALLBACK_INSTANCE *instance, void *context);

/* Thread pool.  */
struct rs_workshop
  {
    TP_POOL *pool;
    TP_CLEANUP_GROUP *group;
    TP_CALLBACK_ENVIRON env[1];
  };

/* Envelope for a work order.  */
struct rs_task
  {
    /* The call-back function.  */
    void (*fun) (void *);

    /* Function argument.  */
    void *arg;
  };

/* Open a workshop.  */
rs_workshop_t *
rs_workshop_open (int workers)
{
  rs_workshop_t *workshop;

  if (workers < 0)
    {
      errno = EINVAL;
      return NULL;
    }

  workshop = calloc (1, sizeof (rs_workshop_t));
  if (workshop == NULL)
    return NULL;

  workshop->pool = NULL;
  workshop->group = NULL;

  InitializeThreadpoolEnvironment (workshop->env);

  if (workers > 0)
    {
      workshop->pool = CreateThreadpool (NULL);
      if (workshop->pool == NULL)
	{
	  errno = ENOMEM;
	  goto failure;
	}

      if (SetThreadpoolThreadMinimum (workshop->pool, 1) == FALSE)
	{
	  errno = EAGAIN;
	  goto failure;
	}

      SetThreadpoolThreadMaximum (workshop->pool, workers);

      workshop->group = CreateThreadpoolCleanupGroup ();
      if (workshop->group == NULL)
	{
	  errno = ENOMEM;
	  goto failure;
	}

      /* Define the thread pool to be used when creating a call-back.  */
      SetThreadpoolCallbackPool (workshop->env, workshop->pool);

      /* Define the cleanup group to be used when creating a call-back.  */
      SetThreadpoolCallbackCleanupGroup (workshop->env, workshop->group, NULL);
    }

  return workshop;

 failure:

  if (workshop->group != NULL)
    CloseThreadpoolCleanupGroup (workshop->group);

  if (workshop->pool != NULL)
    CloseThreadpool (workshop->pool);

  DestroyThreadpoolEnvironment (workshop->env);

  return NULL;
}

/* Place a work order.  */
int
rs_workshop_order (rs_workshop_t *workshop, void (*fun) (void *), void *arg)
{
  rs_task_t *task;

  if (workshop == NULL || fun == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  if (workshop->pool == NULL)
    {
      fun (arg);
      return 0;
    }

  /* Put work order in an envelope.  */
  task = calloc (1, sizeof (rs_task_t));
  if (task == NULL)
    return -1;

  task->fun = fun;
  task->arg = arg;

  if (TrySubmitThreadpoolCallback (worker, task, workshop->env) == TRUE)
    return 0;

  /* Cleanup.  */
  free (task);

  errno = ENOMEM;
  return -1;
}

/* Wait until all work orders are processed.  */
int
rs_workshop_wait (rs_workshop_t *workshop)
{
  if (workshop == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  if (workshop->pool != NULL)
    CloseThreadpoolCleanupGroupMembers (workshop->group, FALSE, NULL);

  return 0;
}

/* Close a workshop.  */
int
rs_workshop_close (rs_workshop_t *workshop)
{
  if (workshop != NULL)
    {
      if (workshop->pool != NULL)
	{
	  CloseThreadpoolCleanupGroupMembers (workshop->group, FALSE, NULL);
	  CloseThreadpoolCleanupGroup (workshop->group);
	  CloseThreadpool (workshop->pool);
	}

      free (workshop);
    }

  return 0;
}

/* Process a work order.  */
static void
worker (TP_CALLBACK_INSTANCE *instance, void *task)
{
  rs_task_t job[1];

  /* Not used.  */
  (void) instance;

  /* Unpack work order.  */
  memcpy (job, task, sizeof (rs_task_t));

  /* Throw away the envelope.  */
  free (task);

  /* Start working.  */
  job->fun (job->arg);
}

/*
 * local variables:
 * compile-command: "cl -Fors-workshop-t1 rs-workshop-t1.c rs-workshop.c "
 * end:
 */
