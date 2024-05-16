CC=mpicc
CFLAGS=-O1 -g -ggdb

EXES=mpi_comms

mpi_comms: mpi_comms.c
	$(CC) mpi_comms.c -o mpi_comms $(CFLAGS)

clean:
	$(RM) $(EXES) *.$(OBJ)
