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

    /// TODO

    //printf("Integral 2D: Rank %d of %d\n", rank, size);

    /// Check parameters
    if(argc != 4) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s filename Nx Ny\n", argv[0]);
        return -1;
    }
    char* filename = argv[1];
    int Nx = (int)strtol(argv[2], NULL, 10);
    int Ny = (int)strtol(argv[3], NULL, 10);

    const double deltaX = 2.0*X_MAX/(double) Nx;
    const double deltaY = 2.0*Y_MAX/(double) Ny;

    /// TODO compute rank ranges

    /// TODO


    /// TODO Read data
    double* data = NULL;
    
    /// TODO

    double start_time = omp_get_wtime();
    /// TODO Compute integral
    
    /// TODO

    double run_time = omp_get_wtime() - start_time;

    ///  Print results
    if(rank == 0)
    {
        printf("Result with N=%d M=%d size: %d threads: %d is %.12lf in %lf seconds\n", Nx, Ny,
			size, omp_get_max_threads(), result, run_time);
    }

    MPI_Finalize();
    return 0;
}
