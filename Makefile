
all: vectoresMov run

vectoresMov: vectoresMov.c
	mpicc $< -o $@
	
runsec:
	mpirun -np 1 vectoresMov

run:
	mpirun -np 4 vectoresMov

clean:
	rm  -f vectoresMov.o vectoresMov


