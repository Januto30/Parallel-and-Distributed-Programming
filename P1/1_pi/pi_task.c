#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double rectangle_height(double x) {
    return 4.0 / (1 + x * x);
}

void compute_pi(int start, int end, double delta_x, double* pi) {
    for (int i = start; i < end; i++) {
        double x = (i + 0.5) * delta_x;
        *pi += rectangle_height(x) * delta_x;
    }
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    double start_time = omp_get_wtime();
    double pi = 0.0;
    double delta_x = 1.0 / N;

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < N; i += M) {
                int start = i;
                int end = i + M;
                if (end > N) {
                    end = N;
                }
                #pragma omp task
                {
                    double local_pi = 0.0;
                    compute_pi(start, end, delta_x, &local_pi);
                    #pragma omp critical
                    {
                        pi += local_pi;
                    }
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    double runtime = end_time - start_time;
    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
    return 0;
}