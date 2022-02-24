/* rs-workshop-pthread.c --- a simple but portable thread pool

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

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "rs-workshop.h"

/* Forward declarations.  */
typedef struct rs_task rs_task_t;

static int init (rs_workshop_t *workshop);
static void destroy (rs_workshop_t *workshop);
static void *worker_thread (rs_workshop_t *workshop);
static int push_task (rs_task_t *task, rs_workshop_t *workshop);
static rs_task_t *pop_task (rs_workshop_t *workshop);
static void finish (rs_workshop_t *workshop, int shutdown);
static void shutdown (rs_workshop_t *workshop);

/* Thread pool.  */
struct rs_workshop
  {
    /* Synchronize access to the pool data.  */
    pthread_mutex_t pool_lock[1];

    /* Boss/worker synchronization.  */
    pthread_cond_t work_cond[1];
    pthread_cond_t idle_cond[1];

    /* Initialization level.  */
    int rollback;

    /* List of workers.  */
    pthread_t *worker;

    /* Total number of workers.  */
    int workers;

    /* Number of idle workers.  */
    int idle;

    /* List of work orders, a FIFO queue.  */
    rs_task_t *first;
    rs_task_t *last;

    /* Non-zero means that ‘rs_workshop_wait’ is executing.  */
    unsigned int waiting:1;

    /* Non-zero means that ‘rs_workshop_close’ is executing,
       that means the pool is going to be destroyed.  */
    unsigned int closing:1;
  };

/* Envelope for a work order.  */
struct rs_task
  {
    /* A linked list.  */
    rs_task_t *link;

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

  if (init (workshop) != 0)
    goto failure;

  if (workers > 0)
    {
      int k;

      workshop->worker = calloc (workers, sizeof (pthread_t));
      if (workshop->worker == NULL)
	goto failure;

      /* Create the worker threads.  */
      for (k = 0; k < workers; ++k)
	{
	  if (pthread_create (workshop->worker + k, NULL, (void *) worker_thread, workshop) != 0)
	    {
	      /* If we have already created one or more threads,
		 continue with that many threads.  */
	      if (k > 0 && errno == EAGAIN)
		break;

	      goto failure;
	    }

	  /* Increase number of workers.  */
	  ++workshop->workers;
	}
    }

  return workshop;

 failure:

  destroy (workshop);
  free (workshop);

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

  if (workshop->workers == 0)
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

  if (push_task (task, workshop) != 0)
    {
      free (task);
      return -1;
    }

  return 0;
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

  if (workshop->workers > 0)
    finish (workshop, 0);

  return 0;
}

/* Close a workshop.  */
int
rs_workshop_close (rs_workshop_t *workshop)
{
  if (workshop != NULL)
    {
      destroy (workshop);
      free (workshop);
    }

  return 0;
}

/* Initialize a thread pool object.  */
static int
init (rs_workshop_t *workshop)
{
  /* Static initialization.  */
  workshop->rollback = 0;
  workshop->worker = NULL;
  workshop->workers = 0;
  workshop->idle = 0;
  workshop->first = NULL;
  workshop->last = NULL;
  workshop->waiting = 0;
  workshop->closing = 0;

  /* Dynamic initialization.  */
  if (pthread_mutex_init (workshop->pool_lock, NULL) != 0)
    return -1;

  ++workshop->rollback; /* 1 */

  if (pthread_cond_init (workshop->work_cond, NULL) != 0)
    return -1;

  ++workshop->rollback; /* 2 */

  if (pthread_cond_init (workshop->idle_cond, NULL) != 0)
    return -1;

  ++workshop->rollback; /* 3 */

  return 0;
}

/* Terminate a thread pool object.  */
static void
destroy (rs_workshop_t *workshop)
{
  if (workshop->workers > 0)
    shutdown (workshop);

  if (workshop->worker != NULL)
    free (workshop->worker);

  if (workshop->rollback >= 3)
    assert (pthread_cond_destroy (workshop->idle_cond) == 0);

  if (workshop->rollback >= 2)
    assert (pthread_cond_destroy (workshop->work_cond) == 0);

  if (workshop->rollback >= 1)
    assert (pthread_mutex_destroy (workshop->pool_lock) == 0);
}

/* Start function for a worker.  */
static void *
worker_thread (rs_workshop_t *workshop)
{
  rs_task_t *task;
  rs_task_t job[1];

  while (1)
    {
      task = pop_task (workshop);
      if (task == NULL)
	break;

      /* Unpack work order.  */
      memcpy (job, task, sizeof (rs_task_t));

      /* Throw away the envelope.  */
      free (task);

      /* Start working.  */
      job->fun (job->arg);
    }

  return NULL;
}

/* Add a work order to the queue.  */
static int
push_task (rs_task_t *task, rs_workshop_t *workshop)
{
  int retval = 0;

  assert (pthread_mutex_lock (workshop->pool_lock) == 0);

  if (workshop->waiting)
    {
      /* Reject work order.  */
      errno = EBUSY;
      retval = -1;
    }
  else
    {
      /* Append work order to the queue.  */
      if (workshop->first == NULL)
	{
	  workshop->first = task;
	  workshop->last = task;

	  /* Circular list.  */
	  task->link = task;
	}
      else
	{
	  workshop->last->link = task;
	  task->link = workshop->first;
	  workshop->last = task;
	}

      /* Activate idle workers.  */
      if (workshop->idle > 0)
	assert (pthread_cond_signal (workshop->work_cond) == 0);
    }

  assert (pthread_mutex_unlock (workshop->pool_lock) == 0);

  return retval;
}

/* Get a work order from the queue.  */
static rs_task_t *
pop_task (rs_workshop_t *workshop)
{
  rs_task_t *task = NULL;

  assert (pthread_mutex_lock (workshop->pool_lock) == 0);

  /* Looking for a job.  */
  ++workshop->idle;

  if (workshop->waiting)
    assert (pthread_cond_signal (workshop->idle_cond) == 0);

  while (workshop->first == NULL && ! workshop->closing)
    assert (pthread_cond_wait (workshop->work_cond, workshop->pool_lock) == 0);

  /* There is something to do.  */
  --workshop->idle;

  if (! workshop->closing)
    {
      task = workshop->first;
      if (task != NULL)
	{
	  if (workshop->first == workshop->last)
	    {
	      /* This is the last task in the queue.  */
	      workshop->first = NULL;
	      workshop->last = NULL;
	    }
	  else
	    {
	      /* Two or more tasks.  */
	      workshop->last->link = workshop->first = task->link;
	    }
	}
    }

  assert (pthread_mutex_unlock (workshop->pool_lock) == 0);

  return task;
}

/* Finish all pending work orders.  */
static void
finish (rs_workshop_t *workshop, int shutdown)
{
  assert (pthread_mutex_lock (workshop->pool_lock) == 0);

  if (! (workshop->waiting || workshop->closing))
    {
      /* Close the queue.  */
      workshop->waiting = 1;

      /* Wait until all workers take a break.  */
      while (workshop->first != NULL || workshop->workers != workshop->idle)
	assert (pthread_cond_wait (workshop->idle_cond, workshop->pool_lock) == 0);

      if (! shutdown)
	{
	  /* Reopen the queue.  */
	  workshop->waiting = 0;
	}
      else
	{
	  /* Close the shop.  */
	  workshop->closing = 1;
	}
    }

  assert (pthread_mutex_unlock (workshop->pool_lock) == 0);
}

/* Like ‘finish’ but terminate all worker threads, too.  */
static void
shutdown (rs_workshop_t *workshop)
{
  int k;

  finish (workshop, 1);

  /* Tell all workers that it is time to quit.  */
  assert (pthread_cond_broadcast (workshop->work_cond) == 0);

  /* Wait for workers to leave the shop.  */
  for (k = 0; k < workshop->workers; ++k)
    assert (pthread_join (workshop->worker[k], NULL) == 0);

  if (workshop->worker != NULL)
    free (workshop->worker);

  workshop->worker = NULL;
  workshop->workers = 0;
  workshop->idle = 0;
}

/*
 * local variables:
 * compile-command: "gcc -g -Wall -W -o rs-workshop-t1 rs-workshop-t1.c rs-workshop-pthread.c -pthread "
 * end:
 */
