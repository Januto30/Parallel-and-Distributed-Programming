#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include <omp.h>

double rectangle_height(double x) { 
    return 4.0 / (1 + x * x);                   // Returns the height of the rectangle.
}

void compute_pi(int start, int end, double delta_x, double* pi) {
    double local_pi = 0.0;
    for (int i = start; i < end; i++) {
        double x = (i + 0.5) * delta_x;
        local_pi += rectangle_height(x) * delta_x;
    }
    #pragma omp atomic
    *pi += local_pi;
}

void parallel_compute_pi(int N, int M, double delta_x, double* pi) {
    if (N <= M) {
        compute_pi(0, N, delta_x, pi);
    } else {
        int mid = N / 2;
        #pragma omp task
        parallel_compute_pi(mid, M, delta_x, pi);
        #pragma omp task
        parallel_compute_pi(N - mid, M, delta_x, pi);
        #pragma omp taskwait
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please provide the number of steps and the minimum number of steps in a task as command-line arguments.\n");
        return 1;
    }
    int N = atoi(argv[1]);                      // Convert the number of steps to an integer.
    int M = atoi(argv[2]);                      // Convert the minimum number of steps in a task to an integer.
    double start_time = omp_get_wtime();        // Save the start time of the execution.
    double pi = 0.0; 
    double delta_x = 1.0 / N;
    #pragma omp parallel
    {
        #pragma omp single
        parallel_compute_pi(N, M, delta_x, &pi);
    }
    double end_time = omp_get_wtime();          // Save the end time of the execution.
    double runtime = end_time - start_time;
    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime); // Print the value of pi and the execution time
    return 0;
}
