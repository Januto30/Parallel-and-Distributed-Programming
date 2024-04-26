#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include "sort.h"
#include <limits.h>

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

int sort_openmp(int* array, int n) {
    
    int *tmp_array = (int *)malloc(n * sizeof(int)); //array temporal (reservo memoria)

    int chunk_size = (n + _NUM_THREADS - 1) / _NUM_THREADS;//calcular mida de cada segment(chunk)

    // PART PARAL·LELA
    #pragma omp parallel num_threads(_NUM_THREADS)
    {
        int tid = omp_get_thread_num(); //tid (ID) del thread actual
        int start = tid * chunk_size; //inici del segment
        int end = (tid == _NUM_THREADS - 1) ? n : (tid + 1) * chunk_size;

        sort(array + start, end - start);//ordenem cada segment asignat a cada thread
        
        memcpy(tmp_array + start, array + start, (end - start) * sizeof(int));//copiem els segments ordenats al array temporal
    }

    // PART SECUENCIAL
    int head_i[_NUM_THREADS] = {0}; //aqui guardem la posicio del valor mes petit de cada segment

    for (int i = 0; i < n; i++) {

        int min_value = INT_MAX;
        int min_index = -1;

        for (int j = 0; j < _NUM_THREADS; j++) { //Trobar el valor mes petit de cada segment en l'array temporal
            int end = (j == _NUM_THREADS - 1) ? n : (j + 1) * chunk_size;
            if (head_i[j] < end && tmp_array[head_i[j]] < min_value) {
                min_value = tmp_array[head_i[j]]; //guardem el valor mes petit
                min_index = j;
            }
        }

        array[i] = min_value; //copiem el valor mes petit al array original
        head_i[min_index]++; //avancem el punter del segment on hem copiat el valor
    }

    free(tmp_array); //alliberem memoria (array temporal)
}


void fill_array(int *array, int n) {
    int i;
    
    srand(time(NULL));
    for(i=0; i < n; i++) {
        array[i] = rand()%n;
    }
}

//FUNCIO CREADA PER SABER SI SORT_OPENMP FUNCIONA CORRECTAMENT

int is_sorted(int* array, int n) {
    for(int i = 0; i < n - 1; i++) {
        if(array[i] > array[i + 1]) {
            return 0;  // Array no esta ordenat
        }
    }
    return 1;  // Array SI esta ordenat
}



