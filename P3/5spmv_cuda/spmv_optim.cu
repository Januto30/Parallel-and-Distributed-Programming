#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cuda.h"

#define N 1024
#define ROWSIZE 9
#define THREADS_PER_BLOCK 128



__global__ void cuspmv(int m, double* dvals, int *dcols, double* dx, double *dy)
{
    int row = blockIdx.x * blockDim.x + threadIdx.x; // Identificar l'index de la fila

    __shared__ double shared_dx[THREADS_PER_BLOCK * ROWSIZE]; //memoria compartida per dx

    if (row < m) //Entrem si la fila es troba dins del rang de m, es a dir, si la fila es troba dins de la matriu.
    {
        double dot = 0.0;

    
        for (int j = 0; j < ROWSIZE; j++) { //Carreguem els elements necessaris de dx a la memoria compartida
            int col_idx = dcols[row * ROWSIZE + j]; 
            if (col_idx < N) { //Si el valor es troba dins de la matriu
                shared_dx[threadIdx.x * ROWSIZE + j] = dx[col_idx]; //Carreguem el valor de dx a la memoria compartida
            }
        }

        //Syncronitzem els threads per assegurar que tots els threads han carregat els valors a la memoria compartida
        __syncthreads(); 

        for (int j = 0; j < ROWSIZE; j++) { // Calculem el producte escalar
            dot += dvals[row * ROWSIZE + j] * shared_dx[threadIdx.x * ROWSIZE + j]; 
        }
        dy[row] = dot; //Guardem a la memoria global el resultat
    }
}




void spmv_cpu(int m, int r, double* vals, int* cols, double* x, double* y)
{
    for(int i = 0; i < m; i++) {
        y[i] = 0.0;   
        for(int j = 0; j < r; j++){
            y[i] += vals[j + i*r]*x[cols[j + i*r]]; // (j + i*r) calcula l'index del element (si no s'enten fer a paper per veure que si funciona)
        }
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
    cudaMalloc(&dAvals, ROWSIZE*vec_size*sizeof(double));
    cudaMalloc(&dAcols, ROWSIZE*vec_size*sizeof(int));
    cudaMalloc(&dx, vec_size*sizeof(double));
    cudaMalloc(&dy_gpu, vec_size*sizeof(double));

    // transfer data to GPU
    cudaMemcpy(dAvals, Avals, vec_size * ROWSIZE * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(dAcols, Acols, vec_size* ROWSIZE * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dx, x, vec_size * sizeof(double), cudaMemcpyHostToDevice);

    // calculate threads and blocks
    int threadsPerBlock = THREADS_PER_BLOCK;

    // create the gridBlock
    int blocksPerGrid = (vec_size + threadsPerBlock - 1) / threadsPerBlock;

    cudaEventRecord(start);

    for( int i=0; i<100; i++){
        // call your GPU kernel here
        cuspmv<<<blocksPerGrid, threadsPerBlock>>>(vec_size, dAvals, dAcols, dx, dy_gpu); 
    }

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time_gpu, start, stop);

    // transfer result to CPU RAM
    cudaMemcpy(y_gpu, dy_gpu, vec_size * sizeof(double), cudaMemcpyDeviceToHost);

    // free arrays in GPU
    cudaFree(dAvals);
    cudaFree(dAcols);
    cudaFree(dx);
    cudaFree(dy_gpu);

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
