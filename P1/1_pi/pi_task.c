#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 

double rectangle_height(double x) {
    return 4.0 / (1 + x * x); //Calcula l'altura del rectangle per a un valor de x donat
}

void compute_pi(int start, int end, double delta_x, double* pi) {
    double local_sum = 0.0;                 //Variable local per a emmagatzemar la suma parcial.
    for (int i = start; i < end; i++) {
        double x = (i + 0.5) * delta_x;     //Calcula el valor de x per a cada iteració.
        local_sum += rectangle_height(x) * delta_x; //Calcula l'àrea del rectangle i l'acumula a la suma parcial.
    }
    #pragma omp atomic                      //Assegura l'operació atòmica 
                                            //  per a actualitzar la variable compartida pi.
    *pi += local_sum;                       //Actualitza el valor de pi amb la suma parcial.
}

void divide_and_conquer(int start, int end, double delta_x, double* pi, int M) {
    if (end - start <= (M > 1024 ? M : 1024)) { //Comprova si la tasca és prou petita per a ser resolta directament.
        compute_pi(start, end, delta_x, pi);
    } else {
        int mid = (start + end) / 2;        //Calcula el punt mig de la tasca actual.
        #pragma omp task                    //Crea una tasca paral·lela.
        divide_and_conquer(start, mid, delta_x, pi, M); //Divideix la tasca en dues parts i les processa de forma paral·lela
        divide_and_conquer(mid, end, delta_x, pi, M);   //Divideix la tasca en dues parts i les processa de forma paral·lela
        #pragma omp taskwait                //Espera a que totes les tasques paral·leles finalitzin
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please provide the number of steps and the minimum number of steps in a task as command-line arguments.\n"); // Mostra un missatge d'error si no es proporcionen els arguments esperats
        return 1;
    }

    int N = atoi(argv[1]); 
    int M = atoi(argv[2]);
    double start_time = omp_get_wtime();            //Obté el temps d'inici de l'execució
    double pi = 0.0; 
    double delta_x = 1.0 / N;

    #pragma omp parallel                            //Inicia una regió paral·lela
    {
        #pragma omp single                          //Indica que només un thread d'execució ha de realitzar aquesta secció
        divide_and_conquer(0, N, delta_x, &pi, M);  //"Divide and conquere" per a calcular pi de forma paral·lela
    }

    double end_time = omp_get_wtime();              //Obté el temps de finalització de l'execució.
    double runtime = end_time - start_time;         //Calcula el temps d'execució.
    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime); //Imprimim el valor de pi i el temps d'execució
    return 0;
}