/* rs-workshop.h --- a simple but portable thread pool

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

#ifndef RS_WORKSHOP_H
#define RS_WORKSHOP_H

#ifdef __cplusplus
#define RS_WORKSHOP_BEGIN_DECL extern "C" {
#define RS_WORKSHOP_END_DECL }
#else /* not __cplusplus */
#define RS_WORKSHOP_BEGIN_DECL
#define RS_WORKSHOP_END_DECL
#endif /* not __cplusplus */

RS_WORKSHOP_BEGIN_DECL

/* Opaque thread pool object.  */
typedef struct rs_workshop rs_workshop_t;

/* Open a workshop, i.e. create a new thread pool object.

   Argument WORKERS is the maximum number of co-workers.  A value of
    zero means to not queue up work orders, i.e. the client will be
    blocked until the current work order is processed.

   Return value is a pointer to a thread pool object.  In case of an
   error, a null pointer is returned and ‘errno’ is set to describe
   the error.

   The following error conditions are defined for this function:

   EINVAL
        Argument WORKERS is less than zero.

   ENOMEM
        The system ran out of memory.

   EAGAIN
        The system lacked the necessary resources (other than memory)
        to initialize the thread pool.  */
extern rs_workshop_t *rs_workshop_open (int __workers);

/* Place a work order.

   First argument WORKSHOP is a pointer to a thread pool object.
   Second argument FUN is the address of a function to be called
    when the work order is processed by a worker.
   Third argument ARG is the argument for the call-back function.

   Return value is zero on success.  In case of an error, -1 is
   returned and ‘errno’ is set to describe the error.

   The following error conditions are defined for this function:

   EINVAL
        One of the following is true.

           * Argument WORKSHOP is a null pointer.
           * Argument FUN is a null pointer.

   ENOMEM
        The system ran out of memory.

   EBUSY
        The work order is rejected because ‘rs_workshop_wait’ is
        executing.  */
extern int rs_workshop_order (rs_workshop_t *__workshop, void (*__fun) (void *), void *__arg);

/* Wait until all work orders are processed.

   Argument WORKSHOP is a pointer to a thread pool object.

   Return value is zero on success.  In case of an error, -1 is
   returned and ‘errno’ is set to describe the error.

   The following error conditions are defined for this function:

   EINVAL
        Argument WORKSHOP is a null pointer.  */
extern int rs_workshop_wait (rs_workshop_t *__workshop);

/* Close a workshop.

   Argument WORKSHOP is a pointer to a thread pool object.  It is
    no error if argument WORKSHOP is a null pointer.

   Return value is zero on success.  In case of an error, -1 is
   returned and ‘errno’ is set to describe the error.

   No error conditions are defined for this function.  */
extern int rs_workshop_close (rs_workshop_t *__workshop);

RS_WORKSHOP_END_DECL

#endif /* not RS_WORKSHOP_H */
