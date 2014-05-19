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

 #define F_x 640
 #define F_y 480

 int vec_sum[F_x];
 int Fi[F_x][F_y];
 int Fj[F_x][F_y];

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
void sendRows() {
  int count = F_x;
  int index;
  int i;
  int w;
  for (i=0;i<F_x;i++) {
        w = nextWorker();
        //printf("Send-index=%d a %d\n",i,w);
    MPI_Send(&i, 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
    MPI_Send(&Fj[i][0], count, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD);
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
  int count = F_x;
  int index = 0;
  int result;
  while (index != -1) {
        MPI_Recv(&index, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD,&status);
        if (index != -1) {
                MPI_Recv(&Fi[index][0], count, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD,&status);
                result = processRow(index);
                MPI_Send(&index, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD);
                MPI_Send(&result, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD);
                //printf("recvRows(task=%d) index=%d result=%d\n", taskId,index,result);
        }
  }
}
int processRow(int index) {
        int i;
        int result = 0;
        for (i=0;i<F_x;i++)
                result = result + Fi[index][i];
        return result;
}

int DoSequencial() {
        for (int i=0;i<F_x;i++) {
                vec_sum[i] = processRow(i);
        }
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
  for (i=0;i<F_x;i++) {
        w = nextWorker();
        //printf("recvResults(%d) waiting data from %d\n", taskId,w);
    MPI_Recv(&index, 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD, &status);
        if (index != -1) {
                MPI_Recv(&vec_sum[index], 1, MPI_INT, w, FROM_MASTER, MPI_COMM_WORLD,&status);
                printf("recvResults(%d) index=%d row-sum=%d\n", taskId,index,vec_sum[index]);
        }
  }
}
void fillMatrix() {
        int i,j;
        int val_i, val_j;
        int prob;
        for (i=0;i<F_x;i++)
                for (j=0;j<F_y;j++) {
                        val_i = rand()%256;
                        val_j = rand()%256;
                        Fi[i][j] = val_i;
                        Fj[i][j] = val_j;
                }
 }
 double start;
 void main(int argc, char **argv) {
        initMPI(argc, argv);
        start = MPI_Wtime();
        if (taskId == MASTER) {
                fillMatrix();
                sendRows();
                recvResults();
                printf("Processing time: %lf\n", MPI_Wtime()-start);
        } else {
                recvRows();
        }
        MPI_Finalize();
 }
