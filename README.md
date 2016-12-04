# rs-lib

This is a colorful collection of C functions that proofed to be quite
useful.  All modules are self-contained, that means you only have to
copy the files of a module into your source code directory so that you
can use it.

All symbols should be documented in the associated header file.  If
the documentation is not clear, use the source.


## Modules


### rs-alloc

Extended memory allocation.

This is what you usually know as `xmalloc` and friends.  The only
difference is that the **rs-alloc** library provides an out of memory
hook so that you can customize the error handling if memory allocation
fails.


### rs-array

Multi-dimensional dynamic arrays.

The **rs-array** library provides functions to create
multi-dimensional dynamic arrays so that you can access
array elements with the standard `[]` operator.
For example:

     double **a;

     a = rs_make_array_2d (m, n, sizeof (double), NULL);

     for (i = 0; i < m; ++i)
       for (j = 0; j < n; ++j)
         a[i][j] = 0.0;

     rs_free_array (a);

The keyword is _indirect pointers_.  See file [rs-array.pdf][]
for more details.

[rs-array.pdf]: https://github.com/ralph-schleicher/rs-lib/doc/rs-array.pdf


### rs-backup

GNU style backup files.

Those funny file names like `foo.c~` or `bar.c.~1~`.


### rs-cons

Cons cells and lists.

An implementation of _Greenspun's Tenth Rule of Programming_.


### rs-csv

Read comma-separated values files.

The reader is quite flexible.  You can customize the field delimiter,
quote character, and line comment character.  You can also read
whitespace delimited fields.


### rs-expr

Read and evaluate arithmetic expressions.

The manual is available [here][rs-expr.pdf].

[rs-expr.pdf]: https://github.com/ralph-schleicher/rs-lib/doc/rs-expr.pdf


### rs-lock

Emacs style lock files.

Those funny file names like `.#foo.c`.


### rs-matrix

Selected matrix procedures.

* In-place matrix transposition.


### rs-sort

Utility functions for searching and sorting.


### rs-workshop

A simple but portable thread pool.

There is an implementation for POSIX threads and for the Windows API.
