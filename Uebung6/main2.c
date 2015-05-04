//-------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
//#include <cblas.h>
#include <string.h>
#include <unistd.h>
//#include <x86intrin.h>
#include "timing.h"
//-------------------------------------//
#define FALSE   0
#define TRUE    1
//-------------------------------------//

const int SIZE = 50;
const int THREADS = 300;
const int WRITE_RESULTS = TRUE;

double* readMat;
double* writeMat;

double INNER_TEMP = 15;
double WARM_TEMP = 30;
double COLD_TEMP = 0;
int hoehe = 10;

int finished = FALSE;


void writeResults(double *mat,int size, char* filename)
{
    FILE *res;
    res = fopen(filename,"w");

    int c,d;
    for (c = 0; c < size; c++)
    {
        for (d = 0; d < size; d++)
        {
            fprintf(res,"%f ",mat[d + c*size]); 
        }
        fprintf(res,"\n");
    }
}

void initMat(double *mat)
{
    int c,d;
    for (c = 0; c < SIZE; c++)
    {
        for (d = 0; d < SIZE; d++)
        {
            if (isPointWarm(c,d))
            {
                readMat[c + d * SIZE] = WARM_TEMP;
            } else if (isPointCold(c,d))
            {
                readMat[c + d * SIZE] = COLD_TEMP;
            } else {
                readMat[c + d * SIZE] = INNER_TEMP;
            }
        }
    }   
}

int isPointWarm(int x, int y)
{
    if ( (x == 0 || x == SIZE - 1 || y == 0 || y == SIZE - 1)  && y <= hoehe )
    {
       return TRUE;
    }
    return FALSE;
}

int isPointCold(int x, int y)
{
    if ( (x == 0 || x == SIZE - 1 || y == 0 || y == SIZE - 1)  && y > hoehe )
    {
        return TRUE;
    }
    return FALSE;
}

void step()
{   
    int size = SIZE;
    int i;
    for ( i = 0; i < 200; i++)
    {
        int c,d;
        c = rand() % (size-2)+1;
        d = rand() % (size-2)+1;
        if (isPointWarm(c,d)) 
        {
            writeMat[c + d * SIZE] =  WARM_TEMP;
        } else if(isPointCold(c,d))
        {
            writeMat[c + d * SIZE] = COLD_TEMP;
        }
        else {
            double z1 = readMat[c + 1 + d * size];
            double z2 = readMat[c - 1 + d * size];
            double z3 = readMat[c + (d - 1) * size];
            double z4 = readMat[c + (d + 1) * size];
            double z5 = readMat[c + d * size];
            double z6 = readMat[c + 1 + (d - 1) * size];
            double z7 = readMat[c - 1 + (d + 1) * size];
            double z8 = readMat[c - 1 + (d - 1) * size];
            double z9 = readMat[c + 1 + (d + 1) * size];
            writeMat[c + d * SIZE] = (z1 + z2 + z3 + z4 + z5 + z6 + z7 + z8 + z9) / 9.0;
        }
    }
}

void copyMats()
{
    int c,d;
    for (c = 1; c < SIZE-1; c++)
    {
        for (d = 1; d < SIZE-1; d++)
        {
            readMat[c + d * SIZE] = writeMat[c + d * SIZE];
        }
    }
}

int main()
{
    timing_t stime, etime;
    get_time(&stime);
    printf("init\n");
    srand( (unsigned) time(NULL) ) ;
    int i,j;
    pthread_t threads[THREADS];
    readMat = malloc( SIZE * SIZE * sizeof(double));
    writeMat = malloc( SIZE * SIZE * sizeof(double));   
    initMat(readMat);
    writeResults(readMat,SIZE,"0.txt");
    int c,d;
    for (c = 1; c < SIZE-1; c++)
    {
        for (d = 1; d < SIZE-1; d++)
        {
            writeMat[c + d * SIZE] = readMat[c + d * SIZE];
        }
    }

    float average = 200;
    int steps = 0;
    while (abs(10 - average) > 0.5)
    {
        int e; // waint long enough
        for (e = 0; e < 500; e++)
        {
            copyMats();
            for (i = 0; i < THREADS; i++)
            {
                pthread_create(&threads[i], NULL, (void*) step, NULL);
            }
            /*
            int e = 0;
            while (timespec_diff(stime,etime) < 2)
            {
                long double dif = timespec_diff(stime,etime);
                printf("%LF  \n",dif);
                get_time(&etime);
                //e++;
                //printf("\nwoop: %d  ",e);
            }
            */
            for (i = 0; i < THREADS; i++)
            {
                pthread_join(threads[i], NULL);
            }
        }
        if (WRITE_RESULTS)
        {
            char str[100];
            snprintf(str,100,"%d.txt",steps);
            writeResults(readMat,SIZE,str);
        }
        steps++;
        // unsigned int us2 = 50000;
        // usleep(us2);

        float sum = 0;
        int counter = 0;
        int x,y;
        for (x = 1; x < SIZE-1; x++)
        {
            for (y = 1; y < SIZE-1; y++)
            {
                sum += readMat[x+y*SIZE];
                counter++;
            }
        }
        average = sum/(counter);
        if (average < 10)
        {
            hoehe--;
        } 
        else 
        {
            hoehe++;
        }
        printf("temp: %f\t hoehe: %d\n",average,hoehe);
    }

    return 0;
}

