CFLAGS=-O3 -lm

EXE=.c
RM=rm

EXES = indef_integral_seq indef_integral_par

all: seq par

seq: CC=gcc
seq: indef_integral_seq
	$(CC) indef_integral_seq.c -o indef_integral_seq $(CFLAGS)

par: CC=mpicc
par: indef_integral_par
	$(CC) indef_integral_par.c -o indef_integral_par $(CFLAGS)

clean:
	$(RM) $(EXES)

