#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double rectangle_height(double x) {
    return 4.0 / (1 + x * x);
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    double start_time = clock();
    double pi = 0.0;
    double delta_x = 1.0 / N;
    for (int i = 0; i < N; i++) {
        double x = (i + 0.5) * delta_x;
        pi += rectangle_height(x) * delta_x;
    }
    double end_time = clock();
    double runtime = (end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
    return 0;
}