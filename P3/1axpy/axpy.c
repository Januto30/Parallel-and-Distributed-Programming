#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

void axpy_cpu(int n, double alpha, double* x, double* y)
{
    for (int i = 0; i < n; i++)
    {
        y[i] = alpha * x[i] + y[i];
    }
}

void axpy_gpu(int n, double alpha, double* x, double* y)
{
    #pragma acc parallel loop present(x[0:n], y[0:n])
    for (int i = 0; i < n; i++)
    {
        y[i] = alpha * x[i] + y[i];
    }
}


int main(int argc, char **argv)
{
    int vec_size = atoi(argv[1]);

    double  time_start, time_end, time_cpu, time_gpu;
    double  alpha = 0.5;

    double *x     = (double*) malloc (vec_size*sizeof(double));
    double *y_cpu = (double*) malloc (vec_size*sizeof(double));
    double *y_gpu = (double*) malloc (vec_size*sizeof(double));


    // fill vectors with sinusoidals for testing the code
    for(int i = 0; i < vec_size; i++)
    {
        x[i] = sin(i*0.01);
        y_cpu[i] = cos(i*0.01);
        y_gpu[i] = cos(i*0.01);
    }


    time_start = omp_get_wtime();

    for(int i = 0; i < 100; i++)
        axpy_cpu(vec_size, alpha, x, y_cpu);

    time_end = omp_get_wtime();
    time_cpu = time_end - time_start;

    //time_start = omp_get_wtime();

    #pragma acc enter data copyin(x[0:vec_size], y_gpu[0:vec_size])
    time_start = omp_get_wtime();
    for(int i = 0; i < 100; i++){
        axpy_gpu(vec_size, alpha, x, y_gpu);
    }
    time_end = omp_get_wtime();

    #pragma acc exit data copyout(y_gpu[0:vec_size])

    //time_end = omp_get_wtime();
    time_gpu = time_end - time_start;



    // compare gpu and cpu results
    double norm2 = 0.0;
    for(int i = 0; i < vec_size; i++)
        norm2 += (y_cpu[i] - y_gpu[i])*(y_cpu[i] - y_gpu[i]);

    norm2 = sqrt(norm2);

    printf("axpy comparison cpu vs gpu error: %e, size %d\n",
           norm2, vec_size);

    double speed_up = 1; // TODO
    printf("CPU Time: %lf - GPU Time: %lf - speed-up = %lf\n", time_cpu, time_gpu, speed_up);

    // free allocated memory
    free(x);
    free(y_cpu);
    free(y_gpu);
}
