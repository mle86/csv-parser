#!/usr/bin/make -f

BIN=csv
DEST=/usr/local/bin/$(BIN)
CHOWN=root:root

OBJS =csv.o
OBJS+=aux.o
OBJS+=input.o
OBJS+=output.o
OBJS+=escape.o

DEP=*.h def/*.def

CC=gcc
LD=gcc
CFEAT =-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS=-O3 -std=c99 -pedantic -Wall -Wextra -DPROGNAME='"$(BIN)"' $(CFEAT)

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

README.md: man/*
	perl man/to-readme.pl --comment --paste-after DESCRIPTION:'Installation.md' <man/csv.1 >README.md

install: $(BIN)
	strip $(BIN)
	cp $(BIN) $(DEST)
	chown $(CHOWN) $(DEST)
	
	cp man/csv.1 /usr/local/share/man/man1/
	chmod 0644 /usr/local/share/man/man1/csv.1
	gzip -f /usr/local/share/man/man1/csv.1

test:
	git submodule update --init test/framework/
	test/run-all-tests.sh

clean:
	rm -f $(BIN) *.o a.out *~

