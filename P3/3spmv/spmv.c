#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

#define N 1024    // number of matrix rows will be N*N
#define ROWSIZE 9 // number of nonzero cols of sparse matrix

void spmv_cpu(int m, int r, double* vals, int* cols, double* x, double* y)
{
    for(int i = 0; i < m; i++) {
        y[i] = 0.0;   
        for(int j = 0; j < r; j++){
            y[i] += vals[j + i*r]*x[cols[j + i*r]]; // (j + i*r) calcula l'index del element (si no s'enten fer a paper per veure que si funciona)
        }
    }
}

void spmv_gpu(int m, int r, double* vals, int* cols, double* x, double* y){
    #pragma acc parallel loop present(vals[0:m*r], cols[0:m*r], x[0:m], y[0:m])
    for(int i = 0; i < m; i++) {
        double sum = 0.0;
        #pragma acc loop seq reduction(+:sum)
        for(int j = 0; j < r; j++){
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
    int     vec_size = N*N;
    double  time_start, time_end, time_cpu, time_gpu;

    double* x     = (double*) malloc (vec_size*sizeof(double));
    double* y_cpu = (double*) malloc (vec_size*sizeof(double));
    double* y_gpu = (double*) malloc (vec_size*sizeof(double));

    double* Avals = (double*) malloc (ROWSIZE*vec_size*sizeof(double));
    int* Acols = (int*) malloc (ROWSIZE*vec_size*sizeof(int));

    // fill vector with sinusoidal for testing the code
    for(int i = 0; i < vec_size; i++)
    {
        x[i] = sin(i*0.01);
        y_cpu[i] = 0.0;
        y_gpu[i] = 0.0;
    }

    fill_matrix(Avals, Acols);
    
    //CPU-VERSION----------------------------------------------------------------------------------------------
    time_start = omp_get_wtime();
    for(int i = 0; i < 100; i++)
        spmv_cpu(vec_size, ROWSIZE, Avals, Acols, x, y_cpu);
    time_end = omp_get_wtime();
    time_cpu = time_end - time_start;
    //---------------------------------------------------------------------------------------------------------

    //GPU-VERSION----------------------------------------------------------------------------------------------
    #pragma acc enter data copyin(x[0:vec_size], Avals[0:vec_size*ROWSIZE], Acols[0:vec_size*ROWSIZE], y_gpu[0:vec_size]) 
                                                            //x[0:vec_size]             -> copia vec_size elements de x a la GPU
                                                            //Avals[0:vec_size*ROWSIZE] -> copia vec_size*ROWSIZE elements de Avals a la GPU
                                                            //Acols[0:vec_size*ROWSIZE] -> copia vec_size*ROWSIZE elements de Acols a la GPU
                                                            //y_gpu[0:vec_size]         -> copia vec_size elements de y_gpu a la GPU
    time_start = omp_get_wtime();
    for(int i = 0; i < 100; i++)
        spmv_gpu(vec_size, ROWSIZE, Avals, Acols, x, y_gpu);
    time_end = omp_get_wtime();
    #pragma acc exit data copyout(y_gpu[0:vec_size])
    time_gpu = time_end - time_start;
    //---------------------------------------------------------------------------------------------------------

    // compare gpu and cpu results
    double norm2 = 0.0;
    for(int i = 0; i < vec_size; i++)
        norm2 += (y_cpu[i] - y_gpu[i])*(y_cpu[i] - y_gpu[i]);
    norm2 = sqrt(norm2);

    printf("spmv comparison cpu vs gpu error: %e, size %d\n",norm2, vec_size);

    printf("CPU Time: %lf\n", time_cpu);
    printf("GPU Time: %lf\n", time_gpu);

    // free allocated memory
    free(x);
    free(y_cpu);
    free(y_gpu);
    free(Avals);
    free(Acols);
}