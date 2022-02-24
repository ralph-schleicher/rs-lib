/* rs-sort.h --- searching and sorting

   Copyright (C) 1993 Ralph Schleicher

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

#include "rs-sort.h"

int
rs_compare_int (void const *left, void const *right)
{
  int a = *((int const *) left);
  int b = *((int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_unsigned_int (void const *left, void const *right)
{
  unsigned int a = *((unsigned int const *) left);
  unsigned int b = *((unsigned int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_long (void const *left, void const *right)
{
  long int a = *((long int const *) left);
  long int b = *((long int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_unsigned_long (void const *left, void const *right)
{
  unsigned long int a = *((unsigned long int const *) left);
  unsigned long int b = *((unsigned long int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_short (void const *left, void const *right)
{
  short int a = *((short int const *) left);
  short int b = *((short int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_unsigned_short (void const *left, void const *right)
{
  unsigned short int a = *((unsigned short int const *) left);
  unsigned short int b = *((unsigned short int const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_char (void const *left, void const *right)
{
  char a = *((char const *) left);
  char b = *((char const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_unsigned_char (void const *left, void const *right)
{
  unsigned char a = *((unsigned char const *) left);
  unsigned char b = *((unsigned char const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_signed_char (void const *left, void const *right)
{
  signed char a = *((signed char const *) left);
  signed char b = *((signed char const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_float (void const *left, void const *right)
{
  float a = *((float const *) left);
  float b = *((float const *) right);

  return (a > b) - (a < b);
}

int
rs_compare_double (void const *left, void const *right)
{
  double a = *((double const *) left);
  double b = *((double const *) right);

  return (a > b) - (a < b);
}
