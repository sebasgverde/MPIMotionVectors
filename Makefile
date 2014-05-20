
all: vectoresMov run

vectoresMov: vectoresMov.c
	mpicc $< -o $@
	
run:
	mpirun -np 4 vectoresMov

clean:
	rm  -f vectoresMov.o vectoresMov


