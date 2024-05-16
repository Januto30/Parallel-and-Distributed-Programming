CC=nvc
OBJ=cg
FLAGS=-Minfo:accel

all: cpu gpu

cpu: $(OBJ)_cpu.c
	$(CC) -acc:host $(FLAGS) -o $(OBJ)_cpu.x $<

gpu: $(OBJ)_gpu.c
	$(CC) -acc:gpu $(FLAGS) -o $(OBJ)_gpu.x $<

clean:
	$(RM) $(OBJ)_{g,c}pu.x

cleanall:
	$(RM) $(OBJ)_{g,c}pu.x *.err *.out
