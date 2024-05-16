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

    test_synchronous(rank, size);

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

    int send_core = (rank + 1) % size;
    int recv_core = (rank - 1 + size) % size;

    start_time = MPI_Wtime();
    for(int i = 0; i < NUM_TEST; i++) {
        sbuf[i] = rank;

        MPI_Send(&sbuf[i], 1, MPI_INT, send_core, 0, MPI_COMM_WORLD);

        if (i % size == rank)
            usleep(WAITING_TIME);

        MPI_Recv(&rbuf[i], 1, MPI_INT, recv_core, 0, MPI_COMM_WORLD, &status);

        assert(rbuf[i] == recv_core);
    }
    run_time = MPI_Wtime() - start_time;

    MPI_Reduce(&run_time, &average_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
    MPI_Reduce(&run_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&run_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    average_time /= size;

    if (rank == 0) {
        printf("Synchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
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

    int recv_core = (rank - 1 + size) % size;
    int send_core = (rank + 1) % size;

    start_time = MPI_Wtime();

    MPI_Request request[NUM_TEST*2];
    for(int i = 0; i < NUM_TEST; i++) {
        sbuf[i] = rank;

        MPI_Isend(&sbuf[i], 1, MPI_INT, send_core, 0, MPI_COMM_WORLD, &request[2*i]);

        if (rank == i)
            usleep(WAITING_TIME);

        MPI_Irecv(&rbuf[i], 1, MPI_INT, recv_core, 0, MPI_COMM_WORLD, &request[2*i+1]);
    }

    MPI_Status status[NUM_TEST*2];
    MPI_Waitall(NUM_TEST*2, request, status);

    run_time = MPI_Wtime() - start_time;

    for(int i = 0; i < NUM_TEST; i++) {
        assert(rbuf[i] == recv_core);
    }

    double total_time = run_time;
    MPI_Reduce(&total_time, &average_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    average_time /= size;

    if (rank == 0) {
        printf("Asynchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }
}