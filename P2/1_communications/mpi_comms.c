#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>

#define NUM_TEST 50
#define WAITING_TIME 1e6

void test_synchronous(int rank, int n);
void test_asynchronous(int rank, int n);

int main(int argc, char** argv)
{
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(size <= 2) {
        if (rank == 0)
            printf("Error, this program needs to be executed with more than 2 processors.\n");
        return 1;
    }

    /// FIRST TEST - SYNCHRONOUS COMMUNICATION
    test_synchronous(rank, size);

    /// FIRST TEST - ASYNCHRONOUS COMMUNICATION
    test_asynchronous(rank, size);

    MPI_Finalize();
    return 0;
}

void test_synchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST];
    int rbuf[NUM_TEST];
    MPI_Status status;

    /// TODO compute send/receive cores
    int send_core = -1;
    int recv_core = -1;
    /// TODO

    start_time = MPI_Wtime();
    for(int i = 0; i < NUM_TEST; i++) {
        sbuf[i] = rank;

        /// TODO send
        
        /// TODO

        if (i % size == rank)
            usleep(WAITING_TIME);

        /// TODO recv

        /// TODO

        assert(rbuf[i] == recv_core);
    }
    run_time = MPI_Wtime() - start_time;

    /// TODO compute average, max and min times

    /// TODO compute average, max and min times

    if (rank == 0) {
        printf("Asynchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

    MPI_Barrier(MPI_COMM_WORLD);

}

void test_asynchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST];
    int rbuf[NUM_TEST];
    /// TODO compute send/receive cores
    int recv_core = -1;
    int send_core = -1;
    /// TODO

    start_time = MPI_Wtime();

    for(int i = 0; i < NUM_TEST; i++) {
        sbuf[i] = rank;

        /// TODO async send

        /// TODO

        if (rank == i)
            usleep(WAITING_TIME);

        /// TODO async send

        /// TODO
    }



    /// TODO wait for async send/receive

    /// TODO

    run_time = MPI_Wtime() - start_time;

    for(int i = 0; i < NUM_TEST; i++) {
        assert(rbuf[i] == recv_core);
    }

    /// TODO compute average, max and min times

    /// TODO

    if (rank == 0) {
        printf("Asynchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

}
