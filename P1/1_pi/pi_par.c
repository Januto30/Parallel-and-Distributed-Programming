#include <stdio.h> 
#include <stdlib.h> 
#include <omp.h> /

double rectangle_height(double x) { 
        return 4.0 / (1 + x * x); //Càlcul de l'alçada del rectangle.
}

int main(int argc, char *argv[]) {
        if (argc < 2) { //Comprova si s'ha proporcionat el nombre d'arguments de línia de comandes.
                printf("Please provide the number of steps as a command-line argument.\n"); 
                return 1;
        }

        int N = atoi(argv[1]); //Converteix l'argument de línia de comandes a un enter.
        double start_time = omp_get_wtime(); //Obté el temps d'inici de l'execució.
        double pi = 0.0; 
        double delta_x = 1.0 / N;

        int num_threads = omp_get_max_threads(); //Obté el nombre màxim de fils d'execució.
        double* local_sums = (double*)malloc(num_threads * sizeof(double)); //Crea un array per emmagatzemar les sumes locals.

        #pragma omp parallel //Inicia una regió paral·lela.
        {
                int thread_id = omp_get_thread_num(); //Obté l'identificador del fil d'execució actual.
                int num_steps_per_thread = (N + num_threads - 1) / num_threads; //Calcula el nombre de passos per fil d'execució
                int start_index = thread_id * num_steps_per_thread; //Calcula l'índex d'inici per al fil d'execució actual
                int end_index = (thread_id + 1) * num_steps_per_thread; //Calcula l'índex de fi per al fil d'execució actual
                if (end_index > N) { //Comprova si l'índex de fi és més gran que el nombre total de passos
                        end_index = N; //Si és així, ajusta l'índex de fi al nombre total de passos
                }

                double local_sum = 0.0;
                for (int i = start_index; i < end_index; i++) {
                        double x = (i + 0.5) * delta_x; //Calcula el valor de x per a cada pas.
                        local_sum += rectangle_height(x) * delta_x; //Calcula la suma local.
                }

                local_sums[thread_id] = local_sum; //Emmagatzema la suma local a l'array de sumes locals.
        }

        for (int i = 0; i < num_threads; i++) {
                pi += local_sums[i]; //Suma les sumes locals a la variable pi
        }

        double end_time = omp_get_wtime(); //Obté el temps de finalització de l'execució
        double runtime = end_time - start_time; //Calcula el temps d'execució
        printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
        free(local_sums); // Allibera la memòria de l'array de sumes locals.
        return 0;
}