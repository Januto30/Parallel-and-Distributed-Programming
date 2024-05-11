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

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3) {
        if(rank == 0) {
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N;
    double* integral = (double *) calloc(N+1, sizeof(double));

    int start = rank * N / size;
    int end = (rank+1) * N / size;

    clock_t start_time = clock();

    for (int i = start+1; i <= end; i++){
        double x = (i-0.5)*deltaX;
        integral[i] = integral[i-1] + deltaX*function(x);
    }

    double local_result = integral[end];
    double global_result;

    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        double exact = exact_integral(X_MAX);
        double error = fabs(global_result - exact);
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, global_result, exact, error, run_time);
    }

    /// Print integral data
    MPI_File file;
    MPI_Offset offset = start * sizeof(double);
    MPI_File_open(MPI_COMM_WORLD, "indef_integral_par.dat", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
    MPI_File_write_at(file, offset, integral+start, end-start+1, MPI_DOUBLE, MPI_STATUS_IGNORE);
    MPI_File_close(&file);

    /// Print integral info
    char filename_info[100];
    sprintf(filename_info, "indef_integral_par.info");
    FILE* file_info = fopen(filename_info, "w");
    if (file_info != NULL) {
        fprintf(file_info, "# %s\n", "Indefinite integral version parallel");
        fprintf(file_info, "data %s\n", "indef_integral_par.dat");
        fprintf(file_info, "N %d\n", N + 1);
        fprintf(file_info, "XMAX %lf\n", X_MAX);
        fprintf(file_info, "size %d\n", size);
        fclose(file_info);
    }

    free(integral);
    MPI_Finalize();
    return 0;
}


/*
for (int i = start+1; i <= end; i++){
    double x = (i-0.5)*deltaX;
    integral[i] = integral[i-1] + deltaX*function(x);
}

if (rank > 0) {
    double offset_value;
    MPI_Recv(&offset_value, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    integral[start] += offset_value;
}

if (rank < size-1) {
    double offset_value = integral[end];
    MPI_Send(&offset_value, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
}
*/