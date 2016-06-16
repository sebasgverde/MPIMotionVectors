
all: vectoresMov

vectoresMov: vectoresMov.c
	mpicc $< -o $@
	
runsec:
	mpirun -np 2 vectoresMov

run:
	mpirun -np 4 vectoresMov > result.txt

clean:
	rm  -f vectoresMov.o vectoresMov


