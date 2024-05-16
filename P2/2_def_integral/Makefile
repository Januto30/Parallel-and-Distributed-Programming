CFLAGS=-O3 -lm

EXE=.c
RM=rm

EXES = def_integral_seq def_integral_par

all: seq par

seq: CC=gcc
seq: def_integral_seq
	$(CC) def_integral_seq.c -o def_integral_seq $(CFLAGS)

par: CC=mpicc
par: def_integral_par
	$(CC) def_integral_par.c -o def_integral_par $(CFLAGS)

clean:
	$(RM) $(EXES) 

