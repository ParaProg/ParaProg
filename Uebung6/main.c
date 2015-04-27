//-------------------------------------//
#include <stdio.h>
#include <stdlib.h>
//#include <cblas.h>
#include <string.h>
//#include <x86intrin.h>
#include "timing.h"
//-------------------------------------//
#define FALSE   0
#define TRUE    1
//-------------------------------------//

const int zylinder_durchmesser = 10; // in cm
const int segments_per_cm = 1;
const float outer_temp = 30;
const float inner_temp = 20;
const float ice_temp = 0;
const float full_hoehe = 4;

void initMat(double *mat, int size)
{
    int i,j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {   
            if ( j > zylinder_durchmesser - full_hoehe &&
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
            if (mat[j+i*size] == 0)
                printf(" ");
        }
        printf("\n");
    }
}

int main()
{
    int size = zylinder_durchmesser * segments_per_cm + 2;
    double *mat = (double *) malloc( size * size * sizeof(double));
    initMat(mat,size);


    printMat(mat,size);
    return 0;
}

