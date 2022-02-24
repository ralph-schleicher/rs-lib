/* rs-csv.h --- comma-separated values files

   Copyright (C) 2010 Ralph Schleicher

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

#ifndef RS_CSV_H
#define RS_CSV_H

#include <stdio.h>

#ifdef __cplusplus
#define RS_CSV_BEGIN_DECL extern "C" {
#define RS_CSV_END_DECL }
#else /* not __cplusplus */
#define RS_CSV_BEGIN_DECL
#define RS_CSV_END_DECL
#endif /* not __cplusplus */

RS_CSV_BEGIN_DECL

/* Opaque CSV object.  */
typedef struct rs_csv rs_csv_t;

/* Status codes other than zero and ‘EOF’.  */
enum
  {
    RS_CSV_SUCCESS = 0,
    RS_CSV_END_OF_FILE = EOF,
    RS_CSV_SYSTEM_ERROR = '!',
    RS_CSV_PARSE_ERROR = '?',
  };

/* Create a CSV object.

   Return value is a pointer to a CSV object.  In case of an error,
   a null pointer is returned and ‘errno’ is set to describe the
   error.  */
extern rs_csv_t *rs_csv_new (void);

/* Destroy a CSV object.

   Argument OBJ is a pointer to a CSV object.  It is no error if
    argument OBJ is a null pointer.

   Deleting a CSV object means to unconditionally return any allocated
   memory back to the system including the object itself.  After that,
   all references to the CSV object are void.  */
extern void rs_csv_delete (rs_csv_t *__obj);

/* Customize the delimiter characters.

   First argument OBJ is a pointer to a CSV object.
   Second argument SEQ is an array of characters.
   Third argument COUNT is the number of characters.

   If COUNT is zero, fields are delimited by one or more whitespace
   characters.  Since whitespace characters are usually used to align
   field values, whitespace characters at the beginning of a new row
   are ignored.  Thus, a row that contains only whitespace characters
   has exactly one column without a value.  This behavior is in line
   with a non-whitespace delimiter character.

   If there are multiple delimiter characters, the first matching
   delimiter character in a file defines the delimiter character
   for the rest of the file.

   Return value is zero on success.  In case of an error, the return
   value is -1 and ‘errno’ is set to describe the error.

   The default delimiter character is ‘,’, i.e. a comma.  */
extern int rs_csv_set_delimiter (rs_csv_t *__obj, char const *__seq, size_t __count);

/* Customize the quote start characters.

   First argument OBJ is a pointer to a CSV object.
   Second argument SEQ is an array of characters.
   Third argument COUNT is the number of characters.

   If COUNT is zero, field values can not be quoted.  Otherwise, quoted
   field values can contain any character including delimiter characters
   and end of line characters.  A quoted field value ends with the first
   occurrence of the same quote character.  A quote character can be
   escaped by writing two quote characters in a row.

   Return value is zero on success.  In case of an error, the return
   value is -1 and ‘errno’ is set to describe the error.

   The default quote character is ‘"’, i.e. double quote.  */
extern int rs_csv_set_quote_start (rs_csv_t *__obj, char const *__seq, size_t __count);

/* Customize the comment start characters.

   First argument OBJ is a pointer to a CSV object.
   Second argument SEQ is an array of characters.
   Third argument COUNT is the number of characters.

   If COUNT is zero, line comments are disabled.  Otherwise, if
   the first non-whitespace character on a line is a comment start
   character, then all characters up to and including the end of the
   line are ignored.  If line comments are enabled, empty lines are
   ignored, too.

   Return value is zero on success.  In case of an error, the return
   value is -1 and ‘errno’ is set to describe the error.

   Line comments are disabled by default.  */
extern int rs_csv_set_comment_start (rs_csv_t *__obj, char const *__seq, size_t __count);

/* Parse the next field in a CSV file.

   First argument OBJ is a pointer to a CSV object.
   Second argument STREAM is the stream for reading characters.

   The return value is zero on success, ‘EOF’ signals an end of
   file condition, ‘RS_CSV_SYSTEM_ERROR’ signals a system error,
   and ‘RS_CSV_PARSE_ERROR’ signals a parse error.  */
extern int rs_csv_parse (rs_csv_t *__obj, FILE *__stream);

/* Query the status of the parser.

   Argument OBJ is a pointer to a CSV object.

   The return value has the same meaning as the return value of
   the ‘rs_csv_parse’ function.  */
extern int rs_csv_status (rs_csv_t *__obj);

/* Return non-zero if the parser is at the end of a row.  */
extern int rs_csv_endp (rs_csv_t *__obj);

/* Return the current line number (one-based).

   Argument OBJ is a pointer to a CSV object.  */
extern int rs_csv_line (rs_csv_t *__obj);

/* Return the row index (zero-based) of the most recently
   parsed field.

   Argument OBJ is a pointer to a CSV object.  */
extern int rs_csv_row (rs_csv_t *__obj);

/* Return the column index (zero-based) of the most recently
   parsed field.

   Argument OBJ is a pointer to a CSV object.  */
extern int rs_csv_column (rs_csv_t *__obj);

/* Return non-zero if the most recently parsed field is non-null.  */
extern int rs_csv_valuep (rs_csv_t *__obj);

/* Return the address of a character buffer with the contents
   of the most recently parsed field.  The buffer is always
   null-terminated.  */
extern char *rs_csv_value (rs_csv_t *__obj);

/* Return the length of the most recently parsed field.  */
extern size_t rs_csv_length (rs_csv_t *__obj);

RS_CSV_END_DECL

#endif /* not RS_CSV_H */
