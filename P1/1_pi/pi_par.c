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

        int num_threads = omp_get_max_threads();
        double* local_sums = (double*)malloc(num_threads * sizeof(double));

        #pragma omp parallel
        {
                int thread_id = omp_get_thread_num();
                int num_steps_per_thread = (N + num_threads - 1) / num_threads;
                int start_index = thread_id * num_steps_per_thread;
                int end_index = (thread_id + 1) * num_steps_per_thread;
                if (end_index > N) {
                        end_index = N;
                }

                double local_sum = 0.0;
                for (int i = start_index; i < end_index; i++) {
                        double x = (i + 0.5) * delta_x;
                        local_sum += rectangle_height(x) * delta_x;
                }

                local_sums[thread_id] = local_sum;
        }

        for (int i = 0; i < num_threads; i++) {
                pi += local_sums[i];
        }

        double end_time = omp_get_wtime();
        double runtime = end_time - start_time;
        printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);

        free(local_sums);
        return 0;
}