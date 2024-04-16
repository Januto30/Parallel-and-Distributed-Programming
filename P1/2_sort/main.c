#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include "sort.h"


int main(int argc, char* argv[]) {
    int *array;
    int n;
    double start, end;
    double time_seq, time_omp;
    
    if(argc != 2) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s ARRAY_SIZE\n", argv[0]);
        return -1;
    }
    
    n = atoi(argv[1]);

    if (n % _NUM_THREADS != 0){
        fprintf(stderr, "Wrong array size\n%d is not multiple of %d\n",n, _NUM_THREADS);
        return -1;
    }
    
    array = (int*)malloc(n * sizeof(int)); 

    fill_array(array, n);
    
    start=omp_get_wtime();
    sort(array, n);
    end=omp_get_wtime();
    time_seq = end-start;
    printf("Sequential: %f sec\n",time_seq);
    
    fill_array(array, n);
    
    start=omp_get_wtime();
    sort_openmp(array, n);
    end=omp_get_wtime();
    time_omp = end-start;
    printf("OpenMP: %f sec\n",time_omp);
    
    printf("OpenMP was %f times faster\n",time_seq/time_omp);
    
    return 0;
}
