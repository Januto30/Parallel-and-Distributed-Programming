#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double function(double x)
{
    return x*sin(x);
}

double exact_quadrature(double x)
{
    return sin(x) - x*cos(x);
}

int main(int argc, char* argv[])
{

    /// Check parameters
    if(argc != 3) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        return -1;
    }
    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N;


    clock_t start_time = clock();

    /// Compute integral

    int i;
    double result = 0;
    for (i = 1; i <= N; i++) {
        double x_middle = (i - 0.5) * deltaX;
        result = result + function(x_middle) * deltaX;
    }

    /// Compute error and print results
    double exact = exact_quadrature(X_MAX);
    double error = fabs(result - exact);
    double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, result, exact, error, run_time);

    return 0;
}