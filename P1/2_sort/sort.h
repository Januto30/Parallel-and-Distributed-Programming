#ifndef SORT_H
#define SORT_H

#ifndef _NUM_THREADS
    #define _NUM_THREADS 2
#endif

int sort(int *array, int n);
int sort_openmp(int *array, int n);
void fill_array(int *array, int n);

#endif
