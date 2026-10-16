// Exercise 2: sum of 1..10,000,000 using MPI (local partial sums + MPI_Reduce)
#include <cstdio>
#include <mpi.h>
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = 10000000;
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long long chunk = N / size;
    long long start = rank * chunk + 1;
    long long end   = (rank == size - 1) ? N : start + chunk - 1; // last rank takes the remainder
    long long local = 0, total = 0;
    for (long long i = start; i <= end; i++) local += i;

    MPI_Reduce(&local, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    if (rank == 0)
        printf("Sum=%lld (expected %lld) procs=%d time=%f s\n", total, N * (N + 1) / 2, size, t1 - t0);
    MPI_Finalize();
    return 0;
}
