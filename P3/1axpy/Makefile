CC=nvc
OBJ=axpy
FLAGS=-acc=gpu -Minfo=accel
all: $(OBJ).c
	$(CC) $(FLAGS)  -o $(OBJ).x $<
clean:
	$(RM) $(OBJ).x
cleanall:
	$(RM) $(OBJ).x *.err *.out
