#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 

double rectangle_height(double x) { 
    return 4.0 / (1 + x * x);                   //Retorna el valor de l'alçada del rectangle.
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide the number of steps as a command-line argument.\n");
        return 1;
    }
    int N = atoi(argv[1]);                      //Converteix l'argument de línia de comandes a un enter.
    double start_time = omp_get_wtime();        //Guarda el temps d'inici de l'execució.
    double pi = 0.0; 
    double delta_x = 1.0 / N;
    for (int i = 0; i < N; i++) {               //Bucle que itera N vegades
        double x = (i + 0.5) * delta_x;         //Calcula el valor de x
        pi += rectangle_height(x) * delta_x;    //Incrementa el valor de pi
    }
    double end_time = omp_get_wtime();          //Guarda el temps de finalització de l'execució
        double runtime = end_time - start_time;
    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime); //Imprimeix el valor de pi i el temps d'execució
    return 0;
}
