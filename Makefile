#!/usr/bin/make -f

BIN=csv
DEST=/usr/local/bin/$(BIN)
CHOWN=root:root

OBJS =csv.o
OBJS+=aux.o
OBJS+=input.o
OBJS+=output.o
OBJS+=escape.o
OBJS+=nstr.o

DEP=*.h def/*.def

CC=gcc
LD=gcc
#CFEAT =-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS=-O3 -std=c99 -pedantic -Wall -Wextra -DPROGNAME='"$(BIN)"' -D_FORTIFY_SOURCE=1 -fPIE

.PHONY : all install test clean


all: $(BIN)

# $@ is the target
# $^ is the list of all prerequisites
# $* is the stem (everything which matched %, including path)
# $< is the first prerequisite

$(BIN): $(OBJS)
# Link all objects to final binary.
# All needed objects are prerequisites here,
# so they will be compiled.
	$(LD) -o $@  $(CFLAGS) $(LDFLAGS) $^

%.o: %.c $(DEP)
# This rule compiles one source file into one object file.
# All existing header files are prerequisites.
	$(CC) -c -o ${*}.o $(CFLAGS) $<

README.md: man/csv.1 man/*.md
	git submodule update --init man/man-to-md/
	perl man/man-to-md.pl --word CSV --formatted-code --comment --paste-section-after DESCRIPTION:'Installation.md' --paste-section-after DESCRIPTION:'Code Standard.md' --paste-after HEADLINE:'Badges.md' <man/csv.1 >$@

install: $(BIN)
	strip $(BIN)
	cp $(BIN) $(DEST)
	chown $(CHOWN) $(DEST)
	
	mkdir -p /usr/local/share/man/man1/
	cp man/csv.1 /usr/local/share/man/man1/
	chmod 0644 /usr/local/share/man/man1/csv.1
	gzip -f /usr/local/share/man/man1/csv.1

test:
	git submodule update --init test/framework/
	test/run-all-tests.sh

clean:
	rm -f $(BIN) *.o a.out *~

