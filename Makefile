
all: vectoresMov

vectoresMov: vectoresMov.c
	mpicc $< -o $@
	
run:
	mpirun -np 1 vectoresMov

clean:
	rm  -f vectoresMov.o vectoresMov


