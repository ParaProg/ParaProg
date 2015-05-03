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

const int SIZE = 30;
const int THREADS = 1;

double* readMat;
double* writeMat;

double INNER_TEMP = 10;
double WARM_TEMP = 30;
double COLD_TEMP = 0;
double hoehe = 25;

int finished = FALSE;

typedef struct data2 data2;
struct data2
{
    int size;
    int x;
    int y;
};

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

void step(data2* data)
{   
    int size = data->size;
    while(finished == FALSE)
    {
        int c,d;
        for (c = data->x + 1; c < data->x + size-1; c++)
        {
            for (d = data->y + 1; d < data->y + size-1; d++)
            {
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
    printf("init\n");
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

    int size = SIZE/THREADS;
    j = 0;
    for (i = 0; i < THREADS; i++)
    {
        if ( i > SIZE/(size) ) 
        {
            i == 0;
            j++;
        }
        data2 dat2 = {size,i,j};
        data2 *dat = &dat2;
        printf("creating thread\n");
        pthread_create(&threads[i], NULL, (void*) step,(void*) dat);
    }

    for (i = 1; i < 200; i++)
    {
        copyMats();
        
        char str[100];
        snprintf(str,100,"%d.txt",i);
        writeResults(readMat,SIZE,str);
        
        unsigned int us = 50000;
        usleep(us);
    }
    finished = TRUE;

    return 0;
}

