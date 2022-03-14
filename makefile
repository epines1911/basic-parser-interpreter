CC=gcc
COMMON= -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O2
LDLIBS = -lm
CFLAGS=$(COMMON)

NLBS := $(wildcard *.nlb)
RESULTS := $(NLBS:.nlb=.result)

all : parse parse_s parse_v interp interp_s interp_v


interp: nlab.c nlab.h
	$(CC) nlab.c $(CFLAGS) -O2 -DINTERP -o interp $(LDLIBS)

interp_s: nlab.c nlab.h
	$(CC) nlab.c $(CFLAGS) $(SANITIZE) -g3 -DINTERP -o interp_s $(LDLIBS)

interp_v: nlab.c nlab.h
	$(CC) nlab.c $(CFLAGS) -g3 -DINTERP -o interp_v $(LDLIBS)


parse: nlab.c nlab.h 
	$(CC) nlab.c $(CFLAGS) -O2 -o parse $(LDLIBS)

parse_s: nlab.c nlab.h
	$(CC) nlab.c $(CFLAGS) $(SANITIZE) -g3 -o parse_s $(LDLIBS)

parse_v: nlab.c nlab.h
	$(CC) nlab.c $(CFLAGS) -g3 -o parse_v $(LDLIBS)


extension: nlab.c nlab.h 
	$(CC) nlab.c $(CFLAGS) -O2 -DINTERP -DEXT -o extension $(LDLIBS)

# -CC=gcc = sets the compiler
# -CFALGS=... = sets the flags to compile with

# COMMON:
# -Wall = Switch on all warnings
# -Wextra = Even more warnings!
# -Werror = Don't let me compile the program if there are any warnings
# -pedantic = issues warnings about certain forbidden behaviours
# -std=c99 = sets the standard of C you are working with

# DEBUG:
# -g3 = provide the maximum amount of debugging information

# SANITIZE:
# -fsanitize=undefined = warn me about undefined parts of C
# -fsanitize=address = warn me when I'm handling memory incorrectly

# PRODUCTION:
# -O3 = Speed up the code as much as possible

# LDLIBS
# -lm = maths - lets you use pow() etc but you still have to #include <math.h>

runall: $(RESULTS)

%.result:
	./interp $*.nlb > $*.results

clean:
	rm -f parse parse_s parse_v interp interp_s interp_v $(RESULTS)

