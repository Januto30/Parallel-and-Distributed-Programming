#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.h"

int sort(int *array, int n) {
    int i, j, tmp;

    for (i=1;i<n;i++) {  
        tmp = array[i];  
        for(j=i-1;j >= 0 && array[j] > tmp;j--) {  
            array[j+1] = array[j];  
        }  
        array[j+1] = tmp;  
    }
}

int sort_openmp(int *array, int n) {
    // TODO
}

void fill_array(int *array, int n) {
    int i;
    
    srand(time(NULL));
    for(i=0; i < n; i++) {
        array[i] = rand()%n;
    }
}
