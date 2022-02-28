## GNUmakefile --- make file for my library.

# Copyright (C) 1993 Ralph Schleicher

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in
#      the documentation and/or other materials provided with the
#      distribution.
#
#    * Neither the name of the copyright holder nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

## Code:

# GNU C.
GCC = gcc -Wall -W -O3

# MinGW compiler.
#
#! make 'GCC=$(MINGW)'
MINGW = x86_64-w64-mingw32-gcc -Wall -W -O3

# Microsoft Visual C.
#
# -TC	Treat all files named on the command line as C source files.
# -Ox	Uses maximum optimization.
# -Z7	Produces an .obj file containing full symbolic debugging information for use with the debugger.
# -Za	Emit an error for language constructs that are not compatible with either ANSI C or ANSI C++.
# -Wall	Enables all warnings, including those disabled by default.
# -W3	Displays all level 2 warnings and all other warnings recommended for production purposes.
#
#! make OBJ=.obj
CL = cl -nologo -TC -W3 -D_CRT_SECURE_NO_WARNINGS=1

# Object file extension.
OBJ = .o

librs_OBJECTS = $(patsubst %.c,%$(OBJ),$(filter %.c,$(librs_SOURCES)))
librs_SOURCES = \
rs-alloc.h rs-alloc.c \
rs-array.h rs-array.c \
rs-backup.h rs-backup.c \
rs-blas.h rs-blas-real-double.c rs-blas-real-float.c rs-blas-complex-double.c rs-blas-complex-float.c \
rs-cons.h rs-cons.c \
rs-csv.h rs-csv.c \
rs-expr.h rs-expr.c \
rs-lock.h rs-lock.c \
rs-matrix.h rs-matrix-transpose.c \
rs-sort.h rs-sort.c \
rs-string.h rs-string-char.c rs-string-wchar_t.c \
rs-try.h rs-try.c \
$(nil)
librs_BUILT_SOURCES = \
rs-expr.gperf.c \
$(nil)
librs_DOC = \
README.html \
$(nil)

## Rules:

%.o: %.c
	$(GCC) $(CFLAGS) -c $<

%.obj: %.c
	$(CL) $(CFLAGS) -c $<

%.gperf.c: %.gperf
	gperf $< > $@~ && mv -f $@~ $@

%.html: %.md
	markdown $< > $@~ && mv -f $@~ $@

.PHONY: all
all: $(librs_BUILT_SOURCES) $(librs_OBJECTS) $(librs_DOC)

.PHONY: clean
clean:
	rm -f *.o *.obj

.PHONY: sync
sync: all
	~/src/github/github.sh rs-lib

## Dependencies:

rs-alloc$(OBJ): rs-alloc.h rs-alloc.c
rs-array$(OBJ): rs-array.h rs-array.c
rs-backup$(OBJ): rs-backup.h rs-backup.c
rs-blas-real-double$(OBJ): rs-blas.h rs-blas.c rs-blas-real-double.c
rs-blas-real-float$(OBJ): rs-blas.h rs-blas.c rs-blas-real-float.c
rs-blas-complex-double$(OBJ): rs-blas.h rs-blas.c rs-blas-complex-double.c
rs-blas-complex-float$(OBJ): rs-blas.h rs-blas.c rs-blas-complex-float.c
rs-cons$(OBJ): rs-cons.h rs-cons.c
rs-csv$(OBJ): rs-csv.h rs-csv.c
rs-expr$(OBJ): rs-expr.h rs-expr.c rs-expr.gperf.c
rs-lock$(OBJ): rs-lock.h rs-lock.c
rs-matrix-transpose$(OBJ): rs-matrix.h rs-matrix-transpose.c
rs-sort$(OBJ): rs-sort.h rs-sort.c
rs-string-char$(OBJ): rs-string.h rs-string.c rs-string-char.c
rs-string-wchar_t$(OBJ): rs-string.h rs-string.c rs-string-wchar_t.c
rs-try$(OBJ): rs-try.h rs-try.c
rs-workshop$(OBJ): rs-workshop.h rs-workshop.c rs-workshop-pthread.c rs-workshop-w32.c

# local variables:
# compile-command: "make -r "
# end:

## GNUmakefile ends here
