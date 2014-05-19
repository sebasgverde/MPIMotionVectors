
all: vectoresMov

vectoresMov: vectoresMov.o
	$(CC) -o $@ $<


vectoresMov.o: vectoresMov.c
	$(CC) -g -c $< 

clean:
	rm  -f vectoresMov.o vectoresMov


