CFLAGS=-O3 -fopenmp

EXE=.c
RM=rm

EXES = integral2D_par

all: par

par: CC=mpicc
par: integral2D_par
	$(CC) integral2D_par.c -o integral2D_par $(CFLAGS)

clean:
	$(RM) $(EXES)

