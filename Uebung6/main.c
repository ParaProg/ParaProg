//-------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
//#include <cblas.h>
#include <string.h>
//#include <x86intrin.h>
#include "timing.h"
//-------------------------------------//
#define FALSE   0
#define TRUE    1
//-------------------------------------//

const int THREADS = 28;
const int zylinder_length = 155; // in cm
const int segments_per_cm = 1;
const float outer_temp = 30;
const float inner_temp = 15;
const float ice_temp = 0;

int init_full_hoehe = 20.0;
double *copyMat;
double *mat;

void initMat(double *mat, int size)
{
    int i,j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {   
            if ( j > zylinder_length*segments_per_cm - init_full_hoehe*segments_per_cm &&
                (i == 0 || i == size - 1 || j == 0 || j == size - 1))
                mat[i+j*size] = ice_temp;
            else if (i == 0 || i == size - 1 || j == 0 || j == size - 1)
                mat[i+j*size] = outer_temp;
            else 
                mat[i+j*size] = inner_temp;
        }
    }
}

void printMat(double *mat, int size)
{
    int i,j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            printf("%.1f ",mat[j+i*size]);
            if (mat[j+i*size] < 10)
                printf(" ");
        }
        printf("\n");
    }
}

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
void step(int size)
{
    int k;
    for (k = 0; k < size; k++)
    {
        int x,y;
        x = rand() % (size-2)+1;
        y = rand() % (size-2)+1;
        int z1,z2,z3,z4,z5;
        z1 = copyMat[x - 1 + y * size];
        z2 = copyMat[x + 1 + y * size];
        z3 = copyMat[x + (y - 1) * size];
        z4 = copyMat[x + (y + 1) * size];
        z5 = copyMat[x + y * size];
        copyMat[x + y*size] = (z1+z2+z3+z4+z5)/5.0;
    }
}

int main()
{
    timing_t stime, etime;
    get_time(&stime);
    srand( (unsigned) time(NULL) ) ;
    int size = zylinder_length * segments_per_cm + 2;
    int old_full_hoehe = -1;
    int z;
    int toggle_counter = 2;
    
    float prev_sum = 0;
    float prev_hoehe = 0;
    float average = 999;
    while (abs(average - 10) > 0.5 )
    {
        mat = (double *) malloc( size * size * sizeof(double));
        initMat(mat,size);
        //printMat(mat,size); 
        pthread_t threads[THREADS];
        int i,j,k;
        for (k = 0; k < 200; k++)
        {
            copyMat = (double *) malloc( size * size * sizeof(double));
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    copyMat[i+j*size] = mat[i+j*size];
                }
            }
    /*
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {   
            if ( j > zylinder_length*segments_per_cm - init_full_hoehe*segments_per_cm &&
                (i == 0 || i == size - 1 || j == 0 || j == size - 1))
                copyMat[i+j*size] = ice_temp;
            else if (i == 0 || i == size - 1 || j == 0 || j == size - 1)
                copyMat[i+j*size] = outer_temp;
            else 
            {
            }
                //copyMat[i+j*size] = inner_temp;
        }
    }
    */
            for (i = 0; i < THREADS; i++)
            {
                pthread_create(&threads[i], NULL, (void*) step,(void*) (intptr_t)size);
            }
            for (i = 0; i < THREADS; i++)
            {
                pthread_join(threads[i], NULL);
            }
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    mat[i+j*size] = copyMat[i+j*size];
                }
            }
        }
        float sum = 0;
        int counter = 0;
        for (i = 1; i < size-1; i++)
        {
            for (j = 1; j < size-1; j++)
            {
                sum += copyMat[i+j*size];
                counter++;
            }
        }
        average = sum/(counter);

        if (average < 10) init_full_hoehe--;
        else init_full_hoehe++;

        printf("average temp: %f\n",average);
        printf("fullhoehe: %d\n\n\n",init_full_hoehe);
    }
    // printMat(mat,size);
    printf("\ngewuenschte Fullhoehe [cm]: %d\n",init_full_hoehe);
    printf("gewuenschte Fullhoehe [Prozent]: %f\n",(float)init_full_hoehe/((float)zylinder_length));
    get_time(&etime);
    long double dif = timespec_diff(stime,etime);
    printf("laufzeit: %LF\n",dif);
    return 0;
}

