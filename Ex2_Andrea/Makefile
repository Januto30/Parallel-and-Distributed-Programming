CC=gcc
CFLAGS=-fopenmp
OBJ=sort


all: $(OBJ).c
	$(CC) main.c $(OBJ).c -o $(OBJ) $(CFLAGS)

two: $(OBJ).c
	$(CC) main.c $(OBJ).c -o $(OBJ) $(CFLAGS) -D_NUM_THREADS=2

four: $(OBJ).c
	$(CC) main.c $(OBJ).c -o $(OBJ) $(CFLAGS) -D_NUM_THREADS=4

eight: $(OBJ).c
	$(CC) main.c $(OBJ).c -o $(OBJ) $(CFLAGS) -D_NUM_THREADS=8

sixteen: $(OBJ).c
	$(CC) main.c $(OBJ).c -o $(OBJ) $(CFLAGS) -D_NUM_THREADS=16

clean:
	rm $(OBJ)

