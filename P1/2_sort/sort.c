#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
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

    int i, j, temp;
    int chunk_size = (n + _NUM_THREADS - 1) / _NUM_THREADS; //tamany de chunk que cada thread ordenara
    int* array_temp = (int*)malloc(n * sizeof(int)); //array temporal on es guardaran els chunks ordenats
    int* array_final = (int*)malloc(n * sizeof(int)); //array final on es guardara el resultat final
    int* head_i = (int*)malloc(_NUM_THREADS * sizeof(int)); //index de cada thread per saber on esta en el seu chunk

    // Copiar array original al array temporal
    memcpy(array_temp, array, n * sizeof(int));

    // SECCIO PARALELA

    #pragma omp parallel num_threads(_NUM_THREADS) shared(array_temp, array_final, head_i, chunk_size, n) private(i, j, temp)
    //EVITAR RACE CONDITIONS PER AIXO FEM PRIVATE I SHARED

    {
        int id = omp_get_thread_num(); //id de cada thread
        int start = id * chunk_size; //inici del chunk
        int end = (id == _NUM_THREADS - 1) ? n : start + chunk_size; //final del chunk (fent start + chunk_size, donava error)
        //SI EL THREAD ES L'ULTIM, EL FINAL DEL CHUNK ES N, SINO, EL FINAL ES EL INICI + CHUNK_SIZE

        // Cada thread ordena el seu chunk
        for (i = start + 1; i < end; i++) {
            temp = array_temp[i];
            j = i - 1;
            while (j >= start && array_temp[j] > temp) {
                array_temp[j + 1] = array_temp[j];
                j--;
            }
            array_temp[j + 1] = temp;
        }

        // Copiar chunk ordenat al array final
        head_i[id] = start;
    }

    //SECCIO SEQUENCIAL

    // Merge de tots els chunks (ja ordenats)
    int index = 0;
    while (index < n) {
        int min_value = INT_MAX; //int_max llibreria limits.h
        int min_index = -1;

        // Busquem el valor mes petit de tots els threads
        for (int k = 0; k < _NUM_THREADS; k++) {
            int thread_index = head_i[k];
            if (thread_index < (k == _NUM_THREADS - 1 ? n : (k + 1) * chunk_size) && array_temp[thread_index] < min_value) {
                // Si el valor es mes petit que el minim actual, el guardem
                min_value = array_temp[thread_index];
                min_index = k;
            }
        }

        
        array_final[index++] = min_value; // Copiem el valor mes petit al array final

        // Avancem el punter del thread on hem copiat el valor
        head_i[min_index]++;
    }

    // Copiar resultat final al array original
    memcpy(array, array_final, n * sizeof(int));


    free(array_temp);
    free(array_final);
    free(head_i);
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