#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double rectangle_height(double x) {
                return 4.0 / (1 + x * x);
}

int main(int argc, char *argv[]) {
        if (argc < 2) {
                printf("Please provide the number of steps as a command-line argument.\n");
        return 1;
                                                                }

        int N = atoi(argv[1]);
        double start_time = omp_get_wtime();
        double pi = 0.0;
        double delta_x = 1.0 / N;

        #pragma omp parallel
        {
                double local_sum = 0.0;
                #pragma omp for
                for (int i = 0; i < N; i++) {
                        double x = (i + 0.5) * delta_x;
                        local_sum += rectangle_height(x) * delta_x;
                }
                #pragma omp critical
                {
                        pi += local_sum;
                }
        }
        double end_time = omp_get_wtime();
        double runtime = end_time - start_time;
        printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
        return 0;
}