
#include <stdio.h>
#include <sys/time.h>

#define TAMANIO 1024

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
	
	struct timeval startt1 ; gettimeofday(&startt1, NULL);
	int m = leerArchivos(p, "p.txt");
	int n = leerArchivos(d, "d.txt");
	struct timeval endt1 ; gettimeofday(&endt1, NULL);
  	t1 = (double)(endt1.tv_usec - startt1.tv_usec)*1000;


	//int n = 38;//sizeof((int) * d);
	//int m = 5;//sizeof((int) * p);
	int i;
	for(i = 0; i < n-m; i++)
	{
		struct timeval start ;gettimeofday(&start, NULL);
		temp = similitud(p, d+i, m);
		struct timeval end; gettimeofday(&end, NULL);		
		t0= (double)(end.tv_usec - start.tv_usec)*1000.0;

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

void main()
{
	//clock_t startt2, endt2;

	struct timeval startt2 ; gettimeofday(&startt2, NULL);
	vectorMov();
	struct timeval endt2 ; gettimeofday(&endt2, NULL);
    t2 = (double)(endt2.tv_usec - startt2.tv_usec)*1000.0;

    printf("t0 = %ld\n", t0);
	printf("t1 = %d\n", t1);
    printf("t2 = %d\n", t2);
}