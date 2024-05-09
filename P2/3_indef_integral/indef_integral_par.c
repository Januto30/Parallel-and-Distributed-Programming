#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

double function(double x)
{
    return x*sin(x);
}

double exact_integral(double x)
{
    return sin(x) - x*cos(x);
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N;
    double* integral = (double *) calloc(N+1, sizeof(double));

    int local_N = N / size;
    double local_X_MAX = X_MAX;
    if (rank == size - 1) {
        local_N = N - (size - 1) * local_N;
        local_X_MAX = X_MAX - (size - 1) * local_N * deltaX;
    }

    double* local_integral = (double *) calloc(local_N+1, sizeof(double));
    double local_def_result = 0.0;
    
    for (int i = 1; i <= local_N; i++){
        double x = (rank * local_N + i - 0.5) * deltaX;
        local_integral[i] = local_integral[i-1] + deltaX * function(x);
    }
    local_def_result = local_integral[local_N];

    double* def_results = NULL;
    if (rank == 0) {
        def_results = (double *) calloc(size, sizeof(double));
    }
    MPI_Gather(&local_def_result, 1, MPI_DOUBLE, def_results, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            def_results[i] += def_results[i-1];
        }
    }

    MPI_Bcast(def_results, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double exact = exact_integral(X_MAX);

    double error = fabs(def_results[size-1] - exact);
    double run_time = 0.0;
    if (rank == 0) {
        run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    }

    if (rank == 0) {
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, def_results[size-1], exact, error, run_time);

        char filename_dat[100];
        sprintf(filename_dat, "indef_integral_mpi.dat");
        FILE* file_dat = fopen(filename_dat, "wb");
        if (file_dat != NULL) {
            fwrite(def_results, sizeof(double), size, file_dat);
            fclose(file_dat);
        }
        
        char filename_info[100];
        sprintf(filename_info, "indef_integral_mpi.info");
        FILE* file_info = fopen(filename_info, "w");
        if (file_dat != NULL) {
            fprintf(file_info, "# %s\n", "Indefinite integral version MPI");
            fprintf(file_info, "data %s\n", filename_dat);
            fprintf(file_info, "N %d\n", N + 1);
            fprintf(file_info, "XMAX %lf\n", X_MAX);
            fprintf(file_info, "size %d\n", size);
            fclose(file_info);
        }

        free(def_results);
    }

    free(integral);
    free(local_integral);

    MPI_Finalize();

    return 0;
}