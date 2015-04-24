//-------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <string.h>
#include <x86intrin.h>
#include "timing.h"
//-------------------------------------//
#define FALSE   0
#define TRUE    1
//-------------------------------------//

const int sizes[] = {16,32,64,128,256,512,1024,2048}; // ,1024,2048};
const int writeRes = FALSE; // warning, needs lots of disk space.
const int writeAna = TRUE;

int stepcounter = 0;
int maxsteps = 0;

long double* simpleTimes;
long double* blockedTimes;
long double* blasTimes;
long double* blockedSimdTimes;
long double* simpleSimdTimes;

double* simpleFlops;
double* blockedFlops;
double* blasFlops;
double* blockedSimdFlops;
double* simpleSimdFlops;

void initMatrices(double *matrice1, double *matrice2)
{
    int c,d;
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            matrice1[c + d*sizes[stepcounter]] = d+1;
        }
    }

    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            matrice2[c + d*sizes[stepcounter]] = d+1;
        }
    }
}

void writeResults(double *res1, double *res2, double *res3, double *res4,double *res5, char* filename)
{
    FILE *res;
    res = fopen(filename,"w");

    int c,d;
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            fprintf(res,"%f ",res1[c + d*sizes[stepcounter]]); 
        }
        fprintf(res,"\n");
    }
    
    fprintf(res,"\n\n");
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            fprintf(res,"%f ",res2[c + d*sizes[stepcounter]]); 
        }
        fprintf(res,"\n");
    }
    
    fprintf(res,"\n\n");
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            fprintf(res,"%f ",res3[c + d*sizes[stepcounter]]); 
        }
        fprintf(res,"\n");
    }
    
    fprintf(res,"\n\n");
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            fprintf(res,"%f ",res4[c + d*sizes[stepcounter]]); 
        }
        fprintf(res,"\n");
    }
    
    fprintf(res,"\n\n");
    for (c = 0; c < sizes[stepcounter]; c++)
    {
        for (d = 0; d < sizes[stepcounter]; d++)
        {
            fprintf(res,"%f ",res5[c + d*sizes[stepcounter]]); 
        }
        fprintf(res,"\n");
    }
}

void writeAnalysis()
{
    int i;
    FILE *fp1;
    
    fp1 = fopen("./data/meta.txt","w");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%d ",sizes[i]);
    }

    fp1 = fopen("./data/dataSimple.txt","w");
    fprintf(fp1,"Simple Multiplication\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%Lf ",simpleTimes[i]);    
    }
    
    fprintf(fp1,"\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%f ",simpleFlops[i]);    
    }
    fprintf(fp1,"\n");

    fp1 = fopen("./data/dataBlocked.txt","w");
    fprintf(fp1,"Blocked Multiplication\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%Lf ",blockedTimes[i]);    
    }
    
    fprintf(fp1,"\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%f ",blockedFlops[i]);    
    }
    fprintf(fp1,"\n");

    fp1 = fopen("./data/dataBlas.txt","w");
    fprintf(fp1,"Blas Multiplication\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%Lf ",blasTimes[i]);    
    }
    
    fprintf(fp1,"\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%f ",blasFlops[i]);    
    }
    fprintf(fp1,"\n");

    fp1 = fopen("./data/dataBlockedSimd.txt","w");
    fprintf(fp1,"Blocked SIMD Multiplication\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%Lf ",blockedSimdTimes[i]);    
    }
    
    fprintf(fp1,"\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%f ",blockedSimdFlops[i]);    
    }
    fprintf(fp1,"\n");

    fp1 = fopen("./data/dataSimpleSimd.txt","w");
    fprintf(fp1,"Simple SIMD Multiplication\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%Lf ",simpleSimdTimes[i]);    
    }
    
    fprintf(fp1,"\n");
    for (i = 0; i < maxsteps; i++)
    {
        fprintf(fp1,"%f ",simpleSimdFlops[i]);    
    }
    fprintf(fp1,"\n");
}

void doSimpleMultiplication(double *in1, double *in2, double *out)
{
    int sum = 0;
    int c,d,k;
    for (c = 0; c < sizes[stepcounter]; c++) 
    {
        for (d = 0; d < sizes[stepcounter]; d++) 
        {
            for (k = 0; k < sizes[stepcounter]; k++) 
            {
                sum += in1[c+k*sizes[stepcounter]] * in2[k+d*sizes[stepcounter]];
            }
        out[c+d*sizes[stepcounter]] = sum;
        sum = 0;
        }
    }
}

/*  This didnt work!
void doBlockedMultiplication(double* in1,double* in2,double* out)
{
    double *mat1 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    double *mat2 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    int i, jj, j, kk;
    int block_size = 4;

    for (jj = 0; jj <= sizes[stepcounter]; jj += block_size) 
    {
        for (kk = 0; kk <= sizes[stepcounter]; kk += block_size) 
        {
    int sum = 0;
    int c,d,k;
    for (c = jj; c < jj+block_size; c++) 
    {
        for (d = kk; d < kk+block_size; d++) 
        {
            for (k = jj; k < jj+block_size; k++) 
            {
                sum += in1[c+k*sizes[stepcounter]] * in2[k+d*sizes[stepcounter]];
            }
        out[c+d*sizes[stepcounter]] = sum;
        sum = 0;
        }
    }
        }
    }
}
*/

void doBlockedMultiplication(double* in1, double* in2, double* out)
{
    int block_size = 16;
    int i0,j0,k0,i,j,k;
    int n = sizes[stepcounter];
    for (i0 = 0; i0 < n; i0 += block_size)
    {
        for (j0 = 0; j0 < n; j0 += block_size)
        {
            for (k0 = 0; k0 < n; k0 += block_size)
            {
                for (i = i0; i < i0 + block_size; i += 1)
                {
                    for (j = j0; j < j0 + block_size; j += 1)
                    {
                        double sum = 0;
                        for (k = k0; k < k0 + block_size; k += 1)
                        {
                            sum += in1[i+k*sizes[stepcounter]] * in2[k+j*sizes[stepcounter]];
                        }
                        out[i+j*sizes[stepcounter]] += sum;
                    }
                }
            }
        }
    }
}

// this is not needed
void doSimpleSmidMultiplication(double* in1,double* in2,double* out)
{
    __m128d a1, a2, b1, c1, c2;
    int size = sizes[stepcounter],i,j,k;
    for (i = 0; i < size / 2; i++) 
    {
        for (j = 0; j < size / 2; j++) 
        {
            c1 = _mm_load_pd(out + (2 * j * size) + (i * 2)); 
            c2 = _mm_load_pd(out + size + (2 * j * size) + (i * 2)); 
            for (k = 0; k < size; k++) 
            {
                a1 = _mm_load1_pd(in1 + k + (2 * j * size));
                a2 = _mm_load1_pd(in1 + size + k + (2 * j * size));
                b1 = _mm_load_pd(in2 + (k * size) + (i * 2));
                c1 = _mm_add_pd(c1, _mm_mul_pd(a1, b1));
                c2 = _mm_add_pd(c2, _mm_mul_pd(a2, b1));
            }
            _mm_store_pd(out + (2 * j * size) + (i * 2), c1);
            _mm_store_pd(out + size + (2 * j * size) + (i * 2), c2);
        }
    }
}

void doBlockedSmidMultiplication(double* in1, double* in2, double* out) 
{
    __m128d a00, a01, a10, a11,b0,b1,c0,c1;
    unsigned int block_size = 4;
    int size = sizes[stepcounter],i,j,k;
    unsigned int blocks = (size / block_size);
    for (i = 0; i < blocks; i++) 
    {
        for (j = 0; j < blocks; j++) 
        {
            c0 = _mm_load_pd(&out[size * (i * block_size)+ (j) * block_size]);
            c1 = _mm_load_pd(&out[size * (i * block_size + 1)+ (j) * block_size]);
            for (k = 0; k < blocks; k++) 
            {
                a00 = _mm_load1_pd(&in1[size * (i * block_size)+ (k) * block_size]); 
                a01 = _mm_load1_pd(&in1[size * (i * block_size)+ (k) * block_size + 1]); 
                a10 = _mm_load1_pd(&in1[size * (i * block_size + 1)+ (k) * block_size]); 
                a11 = _mm_load1_pd(&in1[size * (i * block_size + 1)+ (k) * block_size + 1]); 
                b0 = _mm_load_pd(&in2[size * (k * block_size)+ (j) * block_size]); 
                b1 = _mm_load_pd(&in2[size * (k * block_size + 1)+ (j) * block_size]); 
                c0 = _mm_add_pd(c0, _mm_mul_pd(a00, b0)); 
                c0 = _mm_add_pd(c0, _mm_mul_pd(a01, b1)); 
                c1 = _mm_add_pd(c1, _mm_mul_pd(a10, b0)); 
                c1 = _mm_add_pd(c1, _mm_mul_pd(a11, b1)); 
            }
            _mm_store_pd(&out[size * (i * block_size)+ (j) * block_size], c0);
            _mm_store_pd(&out[size * (i * block_size + 1)+ (j) * block_size], c1);
        }
    }
}


void doBLASMultiplication(double* in1,double* in2,double* out)
{
    cblas_dgemm(101, 111, 111, sizes[stepcounter], sizes[stepcounter], sizes[stepcounter], 1, 
                in1, sizes[stepcounter], in2, sizes[stepcounter], 1, out, sizes[stepcounter]);
}

void doStep()
{
    // declare some stuff
    double *mat1, *mat2, *res1, *res2, *res3, *res4, *res5;
    mat1 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    mat2 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    res1 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    res2 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    res3 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    res4 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    res5 = (double *) malloc(sizes[stepcounter]*sizes[stepcounter] * sizeof(double));
    timing_t stime, etime;

    // fill matrices with some numbers.
    initMatrices(mat1,mat2);

    // only print readable matrices
    if (sizes[stepcounter] < 10)
    {
        int c,d;
        for (c = 0; c < sizes[stepcounter]; c++)
        {
            for (d = 0; d < sizes[stepcounter]; d++)
            {
                printf("%f ",mat2[c + d*sizes[stepcounter]]); 
            }
            printf("\n");
        }
    }


    get_time(&stime); // start Time
    doSimpleMultiplication(mat1,mat2,res1);
    get_time(&etime); // end Time
    simpleTimes[stepcounter] = timespec_diff(stime, etime);
    simpleFlops[stepcounter] = (sizes[stepcounter] + 2*sizes[stepcounter]*sizes[stepcounter] + 
            sizes[stepcounter]*sizes[stepcounter]*sizes[stepcounter]) * 1.e-6 / simpleTimes[stepcounter];

    get_time(&stime);
    doBlockedMultiplication(mat1,mat2,res2);
    get_time(&etime);
    blockedTimes[stepcounter] = timespec_diff(stime, etime);
    blockedFlops[stepcounter] = 0; // calc flops for algorithm here :P
    
    get_time(&stime);
    doBLASMultiplication(mat1,mat2,res3);
    get_time(&etime);
    blasTimes[stepcounter] = timespec_diff(stime, etime);
    blasFlops[stepcounter] = 0; 
    
    get_time(&stime);
    doBlockedSmidMultiplication(mat1,mat2,res4);
    get_time(&etime);
    blockedSimdTimes[stepcounter] = timespec_diff(stime, etime);
    blockedSimdFlops[stepcounter] = 0;
    
    get_time(&stime);
    doSimpleSmidMultiplication(mat1,mat2,res5);
    get_time(&etime);
    simpleSimdTimes[stepcounter] = timespec_diff(stime, etime);
    simpleSimdFlops[stepcounter] = 0;

    if (writeRes)
    {
        char number[100];
        sprintf(number, "./solutions/solution%d.txt", stepcounter);
        printf("saving result to: %s\n",number);
        writeResults(res1,res2,res3,res4,res5,number);
    }
    
    free(mat1);
    free(mat2);
    free(res1);
    free(res2);
    free(res3);
    free(res4);
}

void run()
{
    maxsteps = sizeof(sizes) / sizeof(int);
    
    simpleTimes = (long double *) malloc(maxsteps * maxsteps * sizeof(long double));
    blockedTimes = (long double *) malloc(maxsteps * maxsteps * sizeof(long double));
    blasTimes = (long double *) malloc(maxsteps * maxsteps * sizeof(long double));
    blockedSimdTimes = (long double *) malloc(maxsteps * maxsteps * sizeof(long double));
    simpleSimdTimes = (long double *) malloc(maxsteps * maxsteps * sizeof(long double));

    simpleFlops = (double *) malloc(maxsteps * maxsteps * sizeof(double));
    blockedFlops = (double *) malloc(maxsteps * maxsteps * sizeof(double));
    blasFlops = (double *) malloc(maxsteps * maxsteps * sizeof(double));
    blockedSimdFlops = (double *) malloc(maxsteps * maxsteps * sizeof(double));
    simpleSimdFlops = (double *) malloc(maxsteps * maxsteps * sizeof(double));

    for (stepcounter = 0; stepcounter < maxsteps; stepcounter++)
    {
        printf("Step: %d of %d, size of matrix: %dx%d\n",stepcounter+1,maxsteps,sizes[stepcounter],sizes[stepcounter]);
        doStep();
    }
    if (writeAna)
    {
        printf("saving analysis to <./data/...>. Use plotter script to visualize.\n");
        writeAnalysis();
    }
}

int main()
{
    printf("Starting MatriceMultiplicationTest\n");
    run();
    printf("Finished MatriceMultiplicationTest\n");
    return 0;
}

