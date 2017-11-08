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
rs-cons.h rs-cons.c \
rs-csv.h rs-csv.c \
rs-expr.h rs-expr.c \
rs-lock.h rs-lock.c \
rs-matrix.h rs-matrix-transpose.c \
rs-sort.h rs-sort.c \
rs-workshop.h rs-workshop.c \
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

rs-alloc.o: rs-alloc.h rs-alloc.c
rs-array.o: rs-array.h rs-array.c
rs-backup.o: rs-backup.h rs-backup.c
rs-cons.o: rs-cons.h rs-cons.c
rs-csv.o: rs-csv.h rs-csv.c
rs-expr.o: rs-expr.h rs-expr.gperf.c
rs-lock.o: rs-lock.h rs-lock.c
rs-matrix-transpose.o: rs-matrix.h rs-matrix-transpose.c
rs-sort.o: rs-sort.h rs-sort.c
rs-workshop.o: rs-workshop.h rs-workshop.c rs-workshop-pthread.c rs-workshop-w32.c
rs-try.o: rs-try.h rs-try.c

# local variables:
# compile-command: "make -r "
# end:

## GNUmakefile ends here
