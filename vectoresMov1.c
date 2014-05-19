
#include <stdio.h>

#include "mpi.h"
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

#define TAMANIO 20
#define TAMMACROBLOQUE 5

int p[TAMANIO];// = {'a','b','c'};
int d[TAMANIO];//= {'3','4','d','e','f','s','b','3','a','5','6'};

double t0, t1, t2;

int leerArchivos(int arre[], char* archivo)
{
	FILE *file = fopen(archivo, "r");

	int i=0;
	char num;
	while(fscanf(file, "%c", &num) > 0) {
	    arre[i] = num;
	    i++;
	}
	fclose(file);
	return i;
}

 void fillMatrix() {
        int i,j;
        int val_i, val_j;
        int prob;
		for (i=0;i<TAMANIO;i++) {
				val_i = rand()%256;
				//val_j = rand()%256;
				p[i] = val_i;
				d[i] = val_i;
		}
 }

int valorAbs(int a)
{
	if(a < 0)
		return a * -1;
	else
		return a;
}

//calcula la similitud de 2 vectores por suma de diferencias
double similitud(int parte[], int dest[], int tamParte)
{
	/*ascii maximo menos ascii minimo por el numero de partes, esta
	seria la maxima diferencia, es decir, 1*/
	double constanteMaximaAscii = (126-32)*tamParte;

	int acum = 0, i;
	for(i = 0; i < tamParte; i++)
	{
		int o = parte[i] - dest[i];
		acum += valorAbs(parte[i] - dest[i]);
	}

	return (acum/constanteMaximaAscii);//por regla de 3
}

void vectorMov()
{
	//clock_t start, startt1, end, endt1;
	double temp;
	int ind = -1;
	double match = 100;
	
	double startt1 = MPI_Wtime();
	fillMatrix();
	int m = 5;//leerArchivos(p, "p.txt");
	int n = 20;//leerArchivos(d, "d.txt");
  	t1 = MPI_Wtime() - startt1;


	//int n = 38;//sizeof((int) * d);
	//int m = 5;//sizeof((int) * p);
	int i;
	for(i = 0; i < n-m; i++)
	{
		double start = MPI_Wtime();
		temp = similitud(p, d+i, m);		
		t0= MPI_Wtime() - start;

		if(temp == 0)
		{
			printf("%i %d\n", i, 0);
			return;
		}
		else if (temp < match)
		{
			ind = i;
			match = temp;
		}
	}
	printf("%i %f \n", ind, match);
	return;
}

void initMPI(int argc, char **argv) {
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
        MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
        numWorkers = numTasks-1;
}

void main(int argc, char **argv)
{
	//clock_t startt2, endt2;
	initMPI(argc, argv);
	double startt2 = MPI_Wtime();
	vectorMov();
    t2 = MPI_Wtime()-startt2;

    printf("t0 = %ld\n", t0);
	printf("t1 = %d\n", t1);
    printf("t2 = %d\n", t2);
	
	MPI_Finalize();
}