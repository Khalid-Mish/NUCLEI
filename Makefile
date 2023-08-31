# In *my* code, I had the interpreter code added to the parser code.
# This additional code is protected by #ifdef INTERP / #endif clauses.
# This code is only "seen" by the compiler if you have #defined INTERP
# This #define would be annoying to change inside nuclei.c/.h, so instead it is
# set in the gcc/clang statement using -DINTERP
# In this way ./parse & ./interp can both be built from the same source file.

CC      := gcc
DEBUG   := -g3 
OPTIM   := -O3
CFLAGS  := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror
RELEASE := $(CFLAGS) $(OPTIM)
COV 	:= -fPIC -fprofile-arcs -ftest-coverage -fprofile-generate
SANI    := $(CFLAGS) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALG    := $(CFLAGS)  $(DEBUG)
NCLS    := $(wildcard *.ncl)
PRES := $(NCLS:.ncl=.pres)
IRES := $(NCLS:.ncl=.ires)
LIBS    := -lm



parse: nuclei.c general.c nuclei.h lisp.h
	$(CC) nuclei.c general.c linked.c $(RELEASE) -o parse $(LIBS) ${if $(REP), $(COV)}

parse_s: nuclei.c general.c nuclei.h lisp.h
	$(CC) nuclei.c general.c linked.c $(SANI) -o parse_s $(LIBS)

parse_v: nuclei.c general.c nuclei.h lisp.h
	$(CC) nuclei.c general.c linked.c $(VALG) -o parse_v $(LIBS)

all: parse parse_s parse_v interp interp_s interp_v

interp: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(RELEASE) -DINTERP -o interp $(LIBS)

interp_s: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(SANI) -DINTERP -o interp_s $(LIBS)

interp_v: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(VALG) -DINTERP -o interp_v $(LIBS)


# Run the test program and output the result to a file.
# run with "make report REP=1"
report: clean parse
	./parse testprog.ncl
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory out


# For all .ncl files, run them and output result to a .pres (prase result) 
# or .ires (interpretted result) file.
runall : ./parse_s ./interp_s $(PRES) $(IRES)

%.pres:
	-./parse_s  $*.ncl > $*.pres 2>&1
%.ires:
	-./interp_s $*.ncl > $*.ires 2>&1

clean:
	rm -f parse parse_s parse_v interp interp_s interp_v $(PRES) $(IRES) *.gcda *.gcno *.gcov *.info *.profraw