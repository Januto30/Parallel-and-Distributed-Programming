CC=nvcc
OBJ=spmv
FLAGS=-O3

all: $(OBJ).cu
	$(CC) $(FLAGS)  -o $(OBJ).x $<

optim: $(OBJ)_optim.cu
	$(CC) $(FLAGS)  -o $(OBJ)_optim.x $<

clean:
	$(RM) $(OBJ).x $(OBJ)_optim.x

cleanall:
	$(RM) $(OBJ).x $(OBJ)_optim.x *.err *.out
