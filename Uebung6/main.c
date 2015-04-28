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

const int THREADS = 4;
const int zylinder_length = 25; // in cm
const int segments_per_cm = 1;
const float outer_temp = 30;
const float inner_temp = 22;
const float ice_temp = 0;

int init_full_hoehe = 2.0;
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
            fprintf(res,"%f ",mat[c + d*size]); 
        }
        fprintf(res,"\n");
    }
}
void test(int size)
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
    srand( (unsigned) time(NULL) ) ;
    int size = zylinder_length * segments_per_cm + 2;
    int old_full_hoehe = -1;
    int z;
    int toggle_counter = 2;
    
    float prev_sum = 0;
    float prev_hoehe = 0;
    for (z = 0; z < 115; z++)
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
            for (i = 0; i < THREADS; i++)
            {
                pthread_create(&threads[i], NULL, (void*) test,(void*) (intptr_t)size);
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
            char str[100];
            snprintf(str,100,"%d.txt",k);
            if ( init_full_hoehe == 19  ) 
            {
                writeResults(copyMat,size,str);
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
        float average = sum/(counter);
        printf("temp: %f\n",average);
        printf("fullhoehe: %d\n\n\n",init_full_hoehe);
      


        if (average < 10)
            init_full_hoehe--;
        else init_full_hoehe++;
        toggle_counter--;
        if (toggle_counter == 0)
        {
            if (old_full_hoehe == init_full_hoehe)
            {
                printf("\nAbbruch!\n");
                if (abs(10 - sum) > abs(10 - prev_sum))
                {
                    init_full_hoehe = prev_hoehe;
                } 
                break;
            }
            toggle_counter = 2;
            old_full_hoehe = init_full_hoehe;
        }
        prev_hoehe = init_full_hoehe;
        prev_sum = average;
    }
    printMat(mat,size);
    printf("\ngewuenschte Fullhoehe [cm]: %d\n",init_full_hoehe);
    printf("gewuenschte Fullhoehe [Prozent]: %f\n",(float)init_full_hoehe/((float)zylinder_length));
    return 0;
}

