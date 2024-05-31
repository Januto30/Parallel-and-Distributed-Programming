#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cuda.h"

#define N 1024
#define ROWSIZE 9
#define THREADS_PER_BLOCK 128


__global__ void cuspmv(int m, int r, double* dvals, int *dcols, double* dx, double *dy)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < m; i += stride)
    {
        double sum = 0.0;
        for (int j = 0; j < r; j++)
        {
            sum += dvals[j + i * r] * dx[dcols[j + i * r]];
        }
        dy[i] = sum;
    }
}


void spmv_cpu(int m, int r, double* vals, int* cols, double* x, double* y)
{
    for(int i = 0; i < m; i++)
    {
        double sum = 0.0;
        for(int j = 0; j < r; j++)
        {
            sum += vals[j + i*r]*x[cols[j + i*r]];
        }
        y[i] = sum;
    }
}


void fill_matrix(double* vals, int* cols)
{

    int indx[ROWSIZE];
    int row_count = 0;
    for(int j = 0; j < N ; j++){
        for(int i = 0; i < N; i++){

            indx[0] = i     + (j - 2)*N;
            indx[1] = i     + (j - 1)*N;
            indx[2] = i - 2 +  j     *N;
            indx[3] = i - 1 +  j     *N;
            indx[4] = i     +  j     *N;
            indx[5] = i + 1 +  j     *N;
            indx[6] = i + 2 +  j     *N;
            indx[7] = i     + (j + 1)*N;
            indx[8] = i     + (j + 2)*N;

            for(int row = 0; row < ROWSIZE; row++)
            {
                if(indx[row] < 0 || indx[row] >= N*N)
                {
                    cols[row + row_count*ROWSIZE] = i + j*N;
                    vals[row + row_count*ROWSIZE] = 0.0;
                }
                else
                {
                    cols[row + row_count*ROWSIZE] = indx[row];
                    if(row == 4)
                    {
                        vals[row + row_count*ROWSIZE] = 0.95;
                    }
                    else
                    {
                        vals[row + row_count*ROWSIZE] = -0.95/(ROWSIZE - 1);
                    }
                }
            }
            row_count++;
        }
    }

    vals[4 + (N*N/2 + N/2)*ROWSIZE] =  1.001*vals[4 + (N*N/2 + N/2)*ROWSIZE];
}

int main()
{
    int vec_size = N*N;

    float time_cpu, time_gpu;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    double* x     = (double*) malloc (vec_size*sizeof(double));
    double* y_cpu = (double*) malloc (vec_size*sizeof(double));
    double* y_gpu = (double*) malloc (vec_size*sizeof(double));

    double* Avals = (double*) malloc (ROWSIZE*vec_size*sizeof(double));
    int*    Acols = (int   *) malloc (ROWSIZE*vec_size*sizeof(int));


    // fill vector with sinusoidal for testing the code
    for(int i = 0; i < vec_size; i++)
    {
        x[i] = sin(i*0.01);
        y_cpu[i] = 0.0;
    }

    fill_matrix(Avals, Acols);

    // measure time of CPU implementation
    cudaEventRecord(start);

    for (int i = 0; i < 100; ++i)
        spmv_cpu(vec_size, ROWSIZE, Avals, Acols, x, y_cpu);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time_cpu, start, stop);


    // complete here your cuda code
    double* dx;
    double* dy_gpu;

    double* dAvals;
    int*    dAcols;


    // allocate arrays in GPU
    cudaMalloc((void**)&dx, vec_size*sizeof(double));
    cudaMalloc((void**)&dy_gpu, vec_size*sizeof(double));
    cudaMalloc((void**)&dAvals, ROWSIZE*vec_size*sizeof(double));
    cudaMalloc((void**)&dAcols, ROWSIZE*vec_size*sizeof(int));

    // transfer data to GPU
    cudaMemcpy(dx, x, vec_size*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(dAvals, Avals, ROWSIZE*vec_size*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(dAcols, Acols, ROWSIZE*vec_size*sizeof(int), cudaMemcpyHostToDevice);

    // calculate threads and blocks
    int numBlocks = (vec_size + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    dim3 gridBlock(numBlocks, 1, 1);
    dim3 threadBlock(THREADS_PER_BLOCK, 1, 1);


    for( int i=0; i<100; i++){
        // call your GPU kernel here
        cuspmv<<<gridBlock, threadBlock>>>(vec_size, ROWSIZE, dAvals, dAcols, dx, dy_gpu);
    }

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time_gpu, start, stop);

    // transfer result to CPU RAM
    cudaMemcpy(y_gpu, dy_gpu, vec_size*sizeof(double), cudaMemcpyDeviceToHost);
    
    // free arrays in GPU
    cudaFree(dx);
    cudaFree(dy_gpu);
    cudaFree(dAvals);
    cudaFree(dAcols);


    // comparison between gpu and cpu results
    double norm2 = 0.0;
    for(int i = 0; i < vec_size; i++)
        norm2 += (y_cpu[i] - y_gpu[i])*(y_cpu[i] - y_gpu[i]);

    norm2 = sqrt(norm2);

    printf("spmv comparison cpu vs gpu error: %e, size %d\n", norm2, vec_size);

    printf("CPU Time: %lf\n", time_cpu/1000);
    printf("GPU Time: %lf\n", time_gpu/1000);

    // free CPU arrays
    free(x);
    free(y_cpu);
    free(y_gpu);
    free(Acols);
    free(Avals);
}
