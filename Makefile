#!/usr/bin/make -f

BIN=csv
DEST=/usr/local/bin/$(BIN)
CHOWN=root:root

OBJS =csv.o
OBJS+=str.o
OBJS+=aux.o

DEP=*.h

CC=gcc
LD=gcc
CFLAGS =-O3 -Wall -I. -DPROGNAME='"$(BIN)"'
LDFLAGS=-Wl,--allow-multiple-definition

.PHONY : all install clean


all: $(BIN)

# $@ is the target
# $^ is the list of all prerequisites
# $* is the stem (everything which matched %, including path)
# $< is the first prerequisite

$(BIN): $(OBJS)
test: test.o $(OBJS)
# Link all objects to final binary.
# All needed objects are prerequisites here,
# so they will be compiled.
	$(LD) -o $@  $(CFLAGS) $(LDFLAGS) $^

%.o: %.c $(DEP)
# This rule compiles one source file into one object file.
# All existing header files are prerequisites.
	$(CC) -c -o ${*}.o $(CFLAGS) $<

install:
	strip $(BIN)
	cp $(BIN) $(DEST)
	chown $(CHOWN) $(DEST)

clean:
	rm -f $(BIN) *.o a.out *~ test 
