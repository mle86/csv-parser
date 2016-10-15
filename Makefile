#!/usr/bin/make -f

BIN=csv
DEST=/usr/local/bin/$(BIN)
CHOWN=root:root

OBJS =csv.o
OBJS+=aux.o
OBJS+=input.o
OBJS+=output.o

DEP=*.h

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

install: $(BIN)
	strip $(BIN)
	cp $(BIN) $(DEST)
	chown $(CHOWN) $(DEST)

test:
	git submodule update --init test/framework/
	test/run-all-tests.sh

clean:
	rm -f $(BIN) *.o a.out *~

