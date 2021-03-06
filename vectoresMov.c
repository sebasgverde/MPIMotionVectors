/*
 *  mpi-example2.c
 *
 *
 *  Created by Edwin Montoya on 10/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

 #include <stdio.h>
 #include <math.h>

#include "mpi.h"

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

#define TAMANIO 100
#define TAMMACROBLOQUE 10

 #define F_x 640
 #define F_y 480

 int vec_sum[F_x];
 int Fi[TAMANIO];
 int Fj[TAMANIO];
 double referencia[TAMANIO - TAMMACROBLOQUE][2];

int     taskId,
        numTasks,
        numWorkers,
        sourceId,
        destId,
        currentWorker=0;

 MPI_Status status;

void initMPI(int argc, char **argv) {
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
        MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
        numWorkers = numTasks-1;
}
int valorAbs(int a)
{
	if(a < 0)
		return a * -1;
	else
		return a;
}

double similitud(int parte[], int dest[], int tamParte)
{
	/*ascii maximo menos ascii minimo por el numero de partes, esta
	seria la maxima diferencia, es decir, 1*/
	double constanteMaximaAscii = (255-0)*tamParte;

	int acum = 0, i;
	for(i = 0; i < tamParte; i++)
	{
		int o = parte[i] - dest[i];
		acum += valorAbs(parte[i] - dest[i]);
	}

	return (acum/constanteMaximaAscii);//por regla de 3
}


int processRow(int index)
{
	//clock_t start, startt1, end, endt1;	
	int m = TAMMACROBLOQUE;//leerArchivos(p, "p.txt");
	int n = TAMANIO;//leerArchivos(d, "d.txt");

	int i;
	
	double temp;
	int ind = -1;
	double match = 100;
		
	for(i = 0; i < n-m; i++)
	{		
		temp = similitud(Fi+index, Fj+i, m);
		//printf("%i %i %f \n",index, i, temp);

		if(temp == 0)
		{
			ind = i;
			match = 0;
			//printf("%i %i %f \n",index, ind, match);
			referencia[index][0] = (double)ind;
			referencia[index][1] = (double)match;
			return;
		}
		else if (temp < match)
		{
			ind = i;
			match = temp;
		}
	}
	//printf("%i %i %f \n",index, ind, match);
	referencia[index][0] = (double)ind;
	referencia[index][1] = (double)match;
}
void sendRows() {
  int count = TAMANIO;
  int index;
  int i;
  int w;
  //mando todo el destino a cada trabajador
  for(i=0; i < numWorkers; i++)
  {
	w = nextWorker();
	
	MPI_Send(&Fj[0], count, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
	printf("enviado destino a %d\n", w);
  }
  //empiezo a rotar los patrones con sus indices
  count = TAMMACROBLOQUE;
  for (i=0;i<TAMANIO-TAMMACROBLOQUE;i++) {
    w = nextWorker();
    MPI_Send(&i, 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
	        //printf("Send-index=%d a %d\n",i,w);
    MPI_Send(&Fi[i]/*[0]*/, count, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
  }
  //printf("finalizando...\n");
  int fin=-1;
  for (i=1;i<=numWorkers;i++) {
    w = nextWorker();
        MPI_Send(&fin, 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
        //printf("finalizando el worker %d\n", w);
  }
}

void recvRows() {
  int count = TAMANIO;
  int index = 0;
  int result;
  
  MPI_Recv(&Fj[0], count, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD,&status);
  printf("recivido destino en %d\n", taskId);
  while (index != -1) {
        MPI_Recv(&index, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD,&status);
		count = TAMMACROBLOQUE;
        if (index != -1) {
                MPI_Recv(&Fi[index], count, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD,&status);
				//printf("recivido indice %d en %d\n", index, taskId);
                processRow(index);
				//printf("termino procesamiento en %d\n", taskId);
                MPI_Send(&index, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD);
                MPI_Send(&referencia[index][0], 1, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD);
				MPI_Send(&referencia[index][1], 1, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD);				
                //printf("enviados los resultados (task=%d) index=%d ind=%f match=%f\n\n", taskId,index, referencia[index][0], referencia[index][1]);
        }
  }
}

void DoSequencial()
{
	//clock_t start, startt1, end, endt1;	
	int m = TAMMACROBLOQUE;//leerArchivos(p, "p.txt");
	int n = TAMANIO;//leerArchivos(d, "d.txt");

	int i, j;
	for(i = 0; i < n-m; i++)
	{
		double temp;
		int ind = -1;
		double match = 100;
		int band = 1;
	
		for(j = 0; j < n-m && band == 1; j++)
		{
			temp = similitud(Fi+i, Fj+j, m);		

			if(temp == 0)
			{
				ind = i;
				match = temp;
				band = 0;
			}
			else if (temp < match)
			{
				ind = i;
				match = temp;
			}
		}
		//printf("%i %i %f \n",i, ind, match);
		referencia[i][0] = (double)ind;
		referencia[i][1] = (double)match;
	}
	return;
}

int nextWorker() {
        if (currentWorker >= numWorkers)
                currentWorker = 0;
        currentWorker++;
        return currentWorker;
}

void recvResults() {
  int count = F_x;
  int i, index,w;
  currentWorker=0;
  for (i=0;i<TAMANIO-TAMMACROBLOQUE;i++) {
        w = nextWorker();
        //printf("recvResults(%d) waiting data from %d\n", taskId,w);
    MPI_Recv(&index, 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD, &status);
        if (index != -1) {
                MPI_Recv(&referencia[index][0], 1, MPI_DOUBLE, w, FROM_MASTER, MPI_COMM_WORLD,&status);
				MPI_Recv(&referencia[index][1], 1, MPI_DOUBLE, w, FROM_MASTER, MPI_COMM_WORLD,&status);
                //printf("recvResults(%d) index=%d ind=%f match=%f\n", taskId,index,referencia[index][0], referencia[index][1]);
        }
  }
}
void fillMatrix() {
        int j;
        int val_i, val_j;
		for (j=0;j<TAMANIO;j++) {
				//val_i = rand()%256;
				val_j = rand()%256;
				Fi[j] = val_j;
				Fj[j] = val_j + rand()%50;//((rand()%1)*100-50);
		}
		
 }
 
 void imprimirMatrices()
 {
	int j;
	for (j=0;j<TAMANIO;j++) {
		printf("%d %d\n", Fi[j], Fj[j]);
	}
 }
 
 void imprimirReferencias()
 {
	int j;
 	for (j=0;j<TAMANIO-TAMMACROBLOQUE;j++) {
		printf("%f %f\n", referencia[j][0], referencia[j][1]);
	}
 }
 
 double start;
 void main(int argc, char **argv) {
        initMPI(argc, argv);
		
		/*
		start = MPI_Wtime();
		if (taskId == MASTER) 
		{
			fillMatrix();
			//imprimirMatrices();
			DoSequencial();
			//imprimirReferencias();
			printf("Tiempo de procesamiento secuencial: %lf\n", MPI_Wtime()-start);
        }
		*/

		
        start = MPI_Wtime();
        if (taskId == MASTER) {
                fillMatrix();
				imprimirMatrices();
                sendRows();
                recvResults();
				imprimirReferencias();
                printf("Tiempo de procesamiento paralelo: %lf\n", MPI_Wtime()-start);
        } else {
                recvRows();
        }
        MPI_Finalize();
 }
