CC=gcc
CFLAGS=-O2 -I. -Wall
BIN=csv
SRC=csv.c


default:
	$(CC) $(CFLAGS) -o $(BIN) -DPROGNAME='"$(BIN)"' $(SRC)

install:
	strip $(BIN)

clean:
	rm -f $(BIN) *.so *.o *.a *~ *.bak
