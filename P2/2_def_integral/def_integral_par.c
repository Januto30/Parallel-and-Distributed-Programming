#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

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

    MPI_Init(&argc, &argv);

    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /// Check parameters
    if(argc != 3) {
        if(rank==0){
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N;
    clock_t start_time = clock();

    /// Compute integral
    int i;
    double result = 0.0;
    int start = rank * (N / size);  //n rectangulos entre los procesos
    int end;
    if (rank == size - 1) {
        end = N;  // El último proceso toma todos los rectángulos restantes
    } else {
        end = (rank + 1) * (N / size);
    }

    //Cada procés calcula una part de la integral
    for (i = start; i < end; i+=size) { //rank pk aixi i comença des de 0
        double x_middle = (i + 0.5) * deltaX;
        result = result + function(x_middle) * deltaX;
    }

    //Hem de sumar els resultats de tots els processos
    double total_result;
    MPI_Reduce(&result, &total_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    //Hem de calcular l'error i imprimir els resultats
    if(rank==0){
        double exact = exact_quadrature(X_MAX);
        double error = fabs(total_result - exact);
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, result, exact, error, run_time);
    }

    MPI_Finalize();
    return 0;
}
