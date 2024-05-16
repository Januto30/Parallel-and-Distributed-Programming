#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define X_MAX 1.0
#define Y_MAX 1.0

int main(int argc, char* argv[])
{
    /// TODO Init MPI
    int rank, size;
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    /// TODO
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //ID del proces
    MPI_Comm_size(MPI_COMM_WORLD, &size);//Mida total (num de processos)

    //printf("Integral 2D: Rank %d of %d\n", rank, size);

    /// Check parameters
    if(argc != 4) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s filename Nx Ny\n", argv[0]);
        return -1;
    }
    char* filename = argv[1];
    int Nx = (int)strtol(argv[2], NULL, 10);
    int Ny = (int)strtol(argv[3], NULL, 10);

    //calculem la mida dels intervals/cada pas
    const double deltaX = 2.0*X_MAX/(double) Nx;
    const double deltaY = 2.0*Y_MAX/(double) Ny;

    /// TODO compute rank ranges

    int rows_process = Ny / size; //files per proces
    int first_row = rank * rows_process; //primera fila del proces
    if (rank == size - 1) { //si es l'ultim proces
        rows_process += Ny % size;
    }


    /// TODO Read data
    double* data = (double*)malloc(rows_process * Nx * sizeof(double)); //matriu 
    if(data == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return -1;
    }

    /// TODO
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);//obrim el fitxer
    MPI_Offset offset = first_row * Nx * sizeof(double); //offset per cada proces
    MPI_File_read_at(file, offset, data, rows_process * Nx, MPI_DOUBLE, MPI_STATUS_IGNORE);
    MPI_File_close(&file);


    double start_time = omp_get_wtime();

    /// TODO Compute integral
    double result = 0.0;
    #pragma omp parallel for reduction(+:result)
    for (int i = 0; i < rows_process; i++) {//per cada fila
        for (int j = 0; j < Nx; j++) {//per cada columna
            double x_mid = -1.0 + (0.5 + j) * deltaX;//calculem la posicio de la cel·la x
            double y_mid = -1.0 + (0.5 + first_row + i) * deltaY;//calculem la posicio de la cel·la y
            result += data[i * Nx + j] * deltaX * deltaY;//calculem la integral
        }
    }

    /// TODO
    double total_result;
    //sumem els resultats de tots els processos
    MPI_Reduce(&result, &total_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double run_time = omp_get_wtime() - start_time;

    ///  Print results
    if(rank == 0)
    {
        printf("Result with N=%d M=%d size: %d threads: %d is %.12lf in %lf seconds\n", Nx, Ny,
                        size, omp_get_max_threads(), total_result, run_time);
    }

    free(data);

    MPI_Finalize();
    return 0;
}

