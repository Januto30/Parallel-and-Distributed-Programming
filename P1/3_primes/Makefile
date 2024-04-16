CC=gcc
CFLAGS=-fopenmp
OBJ=primes


all: $(OBJ).c
	$(CC) $(OBJ).c -o $(OBJ) $(CFLAGS)

primes: $(OBJ).c
	$(CC) $(OBJ).c -o $(OBJ) $(CFLAGS)

clean:
	rm $(OBJ)

