/* rs-csv.c --- comma-separated values files.

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "rs-csv.h"

#ifdef __GNUC__
#define static_inline static inline
#else /* not __GNUC__ */
#ifdef _MSC_VER
#define static_inline static __inline
#else /* not _MSC_VER */
#define static_inline static
#endif /* not _MSC_VER */
#endif /* not __GNUC__ */

/* Set ‘errno’ and return VALUE back to the caller.  */
#define set_errno_and_return_value(e,value)				\
do									\
  {									\
    errno = (e);							\
    return value;							\
  }									\
while (0)

/* Parser states other than status codes.  */
enum
  {
    /* New line.  */
    NL = '\n',

    /* Field separator.  */
    FS = '\t',
  };

/* Data structure for a CSV object.  */
struct rs_csv
  {
    /* Physical line number (one based).  */
    int line;

    /* Row and column index (zero based).  */
    int row, col;

    /* Parser state.  */
    int busy;
    int state;
    int err;

    /* Buffer for caching the field value (a string).  */
    size_t val_size;
    char *val_buf;

    /* Buffer position for adding characters.  If VAL_END is a null
       pointer, the field value is null, too.  Otherwise, VAL_END
       points to the end of the buffer.  */
    size_t val_len;
    char *val_end;

    /* The actual delimiter character.  A value of ‘EOF’ means that
       the delimiter is undefined.  */
    int delim;

    /* Number of delimiter characters.  A value of zero means that
       fields are delimited by one or more whitespace characters.  */
    size_t delim_count;

    /* Sequence of valid delimiter characters.  */
    char *delim_seq;
    char delim_buf[8];

    /* Number of quote characters.  A value of zero means that field
       values can not be quoted.  */
    size_t quote_count;

    /* Sequence of start characters for a quoted field.  */
    char *quote_seq;
    char quote_buf[8];

    /* Number of characters beginning a line comment.  A value of zero
       means that line comments are disabled.  */
    size_t comment_count;

    /* Sequence of start characters for a line comment.  */
    char *comment_seq;
    char comment_buf[8];
  };

/* Create a CSV object.  */
rs_csv_t *
rs_csv_new (void)
{
  rs_csv_t *obj;

  obj = calloc (1, sizeof (rs_csv_t));
  if (obj != NULL)
    {
      obj->line = 1;
      obj->row = -1;
      obj->col = -1;
      obj->busy = 0;
      obj->state = NL;
      obj->err = 0;
      obj->val_size = 0;
      obj->val_buf = NULL;
      obj->val_len = 0;
      obj->val_end = NULL;
      obj->delim = EOF;
      obj->delim_count = 1;
      obj->delim_seq = memcpy (obj->delim_buf, ",", obj->delim_count);
      obj->quote_count = 1;
      obj->quote_seq = memcpy (obj->quote_buf, "\"", obj->quote_count);
      obj->comment_count = 0;
      obj->comment_seq = memcpy (obj->comment_buf, "", obj->comment_count);
    }

  return obj;
}

/* Destroy a CSV object.  */
void
rs_csv_delete (rs_csv_t *obj)
{
  if (obj == NULL)
    return;

  if (obj->val_buf != NULL)
    free (obj->val_buf);

  if (obj->delim_seq != obj->delim_buf)
    free (obj->delim_seq);

  if (obj->quote_seq != obj->quote_buf)
    free (obj->quote_seq);

  if (obj->comment_seq != obj->comment_buf)
    free (obj->comment_seq);

  free (obj);
}

/* Customize the delimiter characters.  */
int
rs_csv_set_delimiter (rs_csv_t *obj, char const *seq, size_t count)
{
  size_t k;

  if (obj->busy != 0)
    set_errno_and_return_value (EBUSY, -1);

  if (seq == NULL)
    {
      if (count != 0)
	set_errno_and_return_value (EINVAL, -1);

      seq = "";
    }

  for (k = 0; k < count; ++k)
    {
      if (! isascii (seq[k]))
	set_errno_and_return_value (EINVAL, -1);
    }

  if (count > sizeof (obj->delim_buf))
    set_errno_and_return_value (EINVAL, -1);

  /* Copy the delimiter characters.  */
  memcpy (obj->delim_seq, seq, count);

  /* Save number of characters.  */
  obj->delim_count = count;

  /* If there is only one delimiter character, fix it.  */
  obj->delim = (obj->delim_count == 1 ?
		obj->delim_seq[0] :
		EOF);

  return 0;
}

/* Customize the quote start characters.  */
int
rs_csv_set_quote_start (rs_csv_t *obj, char const *seq, size_t count)
{
  size_t k;

  if (obj->busy)
    set_errno_and_return_value (EBUSY, -1);

  if (seq == NULL)
    {
      if (count != 0)
	set_errno_and_return_value (EINVAL, -1);

      seq = "";
    }

  for (k = 0; k < count; ++k)
    {
      if (! isascii (seq[k]))
	set_errno_and_return_value (EINVAL, -1);
    }

  if (count > sizeof (obj->quote_buf))
    set_errno_and_return_value (EINVAL, -1);

  /* Copy the quote start characters.  */
  strncpy (obj->quote_seq, seq, count);

  /* Save number of characters.  */
  obj->quote_count = count;

  return 0;
}

/* Customize the comment start characters.  */
int
rs_csv_set_comment_start (rs_csv_t *obj, char const *seq, size_t count)
{
  size_t k;

  if (obj->busy)
    set_errno_and_return_value (EBUSY, -1);

  if (seq == NULL)
    {
      if (count != 0)
	set_errno_and_return_value (EINVAL, -1);

      seq = "";
    }

  for (k = 0; k < count; ++k)
    {
      if (! isascii (seq[k]))
	set_errno_and_return_value (EINVAL, -1);
    }

  if (count > sizeof (obj->comment_buf))
    set_errno_and_return_value (EINVAL, -1);

  /* Copy the comment start characters.  */
  strncpy (obj->comment_seq, seq, count);

  /* Save number of characters.  */
  obj->comment_count = count;

  return 0;
}

/*
 * The Parser
 */

static_inline int
get_char (rs_csv_t *obj, FILE *stream)
{
  int c;

  while (1)
    {
      c = getc (stream);
      if (c != EOF)
	return c;

      if (errno == EINTR)
	{
	  errno = 0;
	  continue;
	}

      if (errno != 0 && obj->err == 0)
	obj->err = errno;

      return EOF;
    }
}

/* Return true if C is a delimiter character.

   Argument C should not be an end of line character!  */
static_inline int
delimiterp (rs_csv_t *obj, int c)
{
  if (obj->delim_count == 0 && isspace (c))
    return 1;

  /* If there are multiple delimiter characters, the first matching
     delimiter character in a file defines the delimiter character
     for the rest of the file.  */
  if (obj->delim == EOF)
    {
      char *p;

      p = memchr (obj->delim_seq, c, obj->delim_count);
      if (p == NULL)
	return 0;

      obj->delim = *p;
    }

  return (c == obj->delim);
}

/* Return true if C is a quote start character.  */
static_inline int
quote_start_p (rs_csv_t *obj, int c)
{
  return (obj->quote_count > 0
	  && memchr (obj->quote_seq, c, obj->quote_count) != NULL);
}

/* Return true if C is a comment start character.  */
static_inline int
comment_start_p (rs_csv_t *obj, int c)
{
  return (obj->comment_count > 0
	  && memchr (obj->comment_seq, c, obj->comment_count) != NULL);
}

/* Return true if C is an end of line character.  */
static_inline int
end_of_line_p (int c)
{
  return (c == EOF || c == '\n' || c == '\r');
}

static_inline void
begin_value (rs_csv_t *obj)
{
  if (obj->val_buf != NULL)
    memset (obj->val_buf, 0, obj->val_size);

  obj->val_len = 0;
  obj->val_end = NULL;
}

static_inline int
add_char (rs_csv_t *obj, int c)
{
  /* Check for room.  Reserve space for the terminating null
     character.  */
  if (obj->val_len + 1 >= obj->val_size)
    {
      size_t n;
      char *p;

      /* New buffer size.  */
      n = obj->val_size + BUFSIZ;

      p = (obj->val_buf == NULL ? malloc (n) : realloc (obj->val_buf, n));
      if (p == NULL)
	{
	  if (obj->err == 0)
	    obj->err = errno;

	  return RS_CSV_SYSTEM_ERROR;
	}

      obj->val_size = n;
      obj->val_buf = p;

      /* Relocate buffer position.  */
      obj->val_end = obj->val_buf + obj->val_len;

      /* Clear newly allocated area.  */
      memset (obj->val_end, 0, obj->val_size - obj->val_len);
    }
  else if (obj->val_end == NULL)
    obj->val_end = obj->val_buf + obj->val_len;

  /* Add the character.  */
  *obj->val_end = c;

  /* Adjust field length.  */
  ++obj->val_len;
  ++obj->val_end;

  return 0;
}

static_inline void
end_value (rs_csv_t *obj, int trim)
{
  /* Terminate field value.  */
  if (obj->val_end != NULL)
    *obj->val_end = 0;

  /* Trim trailing spaces.  */
  if (trim != 0)
    {
      while (obj->val_len > 0 && isspace (obj->val_end[-1]))
	{
	  --obj->val_len;
	  --obj->val_end;

	  /* Clear trimmed area.  */
	  *obj->val_end = 0;
	}
   }
}

static_inline int
parse_end_of_line (rs_csv_t *obj, FILE *stream, int c)
{
  switch (c)
    {
    case '\r':

      /* Mac or DOS style end of line character.
	 Fall through!  */
      c = get_char (obj, stream);
      if (c != '\n')
	ungetc (c, stream);

    case '\n':

      /* Unix style end of line character.  */
      ++obj->line;

      /* Success.  */
      return 0;
    }

  /* Failure.  */
  return EOF;
}

/* Parse a simple, i.e. unquoted, field value.

   Argument C is the first character.

   Return value is zero on success.  */
static_inline int
parse_simple (rs_csv_t *obj, FILE *stream, int c)
{
  while (1)
    {
      if (add_char (obj, c) != 0)
	return RS_CSV_SYSTEM_ERROR;

      c = get_char (obj, stream);

      if (end_of_line_p (c) || delimiterp (obj, c))
	{
	  ungetc (c, stream);
	  end_value (obj, 1);

	  return 0;
	}
    }
}

/* Parse a quoted field value.

   Argument Q is the quote character.

   Return value is zero on success.  */
static_inline int
parse_quoted (rs_csv_t *obj, FILE *stream, int q)
{
  int c;

  while (1)
    {
      c = get_char (obj, stream);
      if (c == EOF)
	return RS_CSV_PARSE_ERROR;

      if (c == q)
	{
	  c = get_char (obj, stream);
	  if (c != q)
	    {
	      ungetc (c, stream);
	      end_value (obj, 0);

	      return 0;
	    }
	}
      else if (parse_end_of_line (obj, stream, c) == 0)
	{
	  /* Add Unix style end of line character.  */
	  c = '\n';
	}

      if (add_char (obj, c) != 0)
	return RS_CSV_SYSTEM_ERROR;
    }
}

static_inline int
status_from_state (rs_csv_t *obj)
{
  return ((obj->state == NL || obj->state == FS) ? RS_CSV_SUCCESS : obj->state);
}

int
rs_csv_parse (rs_csv_t *obj, FILE *stream)
{
  int e, c, op, val;

  /* Disable customization.  */
  obj->busy = 1;

  /* Clear error flags.  */
  obj->err = 0;

  e = errno;
  errno = 0;

  /* Clear field value.  */
  begin_value (obj);

  /* Evaluate parser state.  */
  switch (obj->state)
    {
    case NL:
    new_line:

      /* If an end of file condition is signaled at the very beginning
	 of a line, treat the previous end of line character as a line
	 terminator instead of a line separator.  */
      c = get_char (obj, stream);
      if (c == EOF)
	{
	end_of_file:

	  /* Change parser state.  */
	  obj->state = (obj->err == 0 ? EOF : RS_CSV_SYSTEM_ERROR);

	  /* We are done.  */
	  goto done;
	}

      /* Otherwise, ignore leading whitespace characters.  */
      while (! end_of_line_p (c) && isspace (c))
	c = get_char (obj, stream);

      /* If line comments are enabled, ignore comments and empty
	 lines.  */
      if (c != EOF && obj->comment_count > 0)
	{
	  if (comment_start_p (obj, c))
	    {
	      /* Skip comment start character.  */
	      c = get_char (obj, stream);

	      /* Skip rest of the line.  */
	      while (! end_of_line_p (c))
		c = get_char (obj, stream);

	      if (c == EOF)
		goto end_of_file;
	    }

	  if (parse_end_of_line (obj, stream, c) == 0)
	    goto new_line;
	}

      ungetc (c, stream);

      /* Next row.  */
      ++obj->row;

      /* First column.  */
      obj->col = 0;

      break;

    case FS:

      /* Next column.  */
      ++obj->col;

      break;

    default:

      /* Already done.  */
      goto done;
    }

  /* Op-code for the parser state.  */
  op = 0;

  /* Non-zero means that a field value has been read.  */
  val = 0;

  while (1)
    {
      c = get_char (obj, stream);
      if (c == EOF)
	{
	  op = EOF;
	  break;
	}

      if (parse_end_of_line (obj, stream, c) == 0)
	{
	  op = NL;
	  break;
	}

      if (delimiterp (obj, c))
	{
	  if (obj->delim_count == 0)
	    {
	      /* Fields are delimited by one or more whitespace
		 characters.  */
	      while (1)
		{
		  c = get_char (obj, stream);

		  if (end_of_line_p (c) || ! isspace (c))
		    {
		      ungetc (c, stream);
		      break;
		    }
		}
	    }

	  op = FS;
	  break;
	}

      if (isspace (c))
	continue;

      /* Field value.  */
      if (val != 0)
	{
	  op = RS_CSV_PARSE_ERROR;
	  break;
	}

      val = 1;

      op = (quote_start_p (obj, c) ?
	    parse_quoted (obj, stream, c) :
	    parse_simple (obj, stream, c));
      if (op != 0)
	break;
    }

  /* Save parser state.  */
  obj->state = (obj->err == 0 ? op : RS_CSV_SYSTEM_ERROR);

  /* Restore ‘errno’.  */
  errno = (obj->err || e);

  return ((obj->state == EOF ||
	   obj->state == NL ||
	   obj->state == FS) ?
	  RS_CSV_SUCCESS :
	  obj->state);

 done:

  /* Restore ‘errno’.  */
  errno = (obj->err || e);

  return obj->state;
}

/* Query the status of the parser.  */
int
rs_csv_status (rs_csv_t *obj)
{
  return (obj->state == NL || obj->state == FS ) ? 0 : obj->state;
}

/* Return non-zero if the parser is at the end of a row.  */
int
rs_csv_endp (rs_csv_t *obj)
{
  return (obj->state == NL && obj->col >= 0);
}

/* Return the current line number (one-based).  */
int
rs_csv_line (rs_csv_t *obj)
{
  return obj->line;
}

/* Return the row index (zero-based) of the most recently
   parsed field.  */
int
rs_csv_row (rs_csv_t *obj)
{
  return obj->row;
}

/* Return the column index (zero-based) of the most recently
   parsed field.  */
int
rs_csv_column (rs_csv_t *obj)
{
  return obj->col;
}

/* Return non-zero if the most recently parsed field is non-null.  */
int
rs_csv_valuep (rs_csv_t *obj)
{
  return (obj->val_end != NULL);
}

/* Return the address of a character buffer with the contents
   of the most recently parsed field.  The buffer is always
   null-terminated.  */
char *
rs_csv_value (rs_csv_t *obj)
{
  return obj->val_buf;
}

/* Return the length of the most recently parsed field.  */
size_t
rs_csv_length (rs_csv_t *obj)
{
  return obj->val_len;
}
