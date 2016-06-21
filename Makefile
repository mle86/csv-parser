CC=gcc
CFLAGS=-O1 -fomit-frame-pointer -I. -Wall -Ilib
BIN=csv
SRC=csv.c


default:
	$(CC) $(CFLAGS) -o $(BIN) -DPROGNAME='"$(BIN)"' $(SRC)

install:
	strip $(BIN)

clean:
	rm -f $(BIN) *.so *.o *.a *~ *.bak
