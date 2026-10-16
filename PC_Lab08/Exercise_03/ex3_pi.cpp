// Exercise 3: Monte Carlo estimate of Pi, 10,000,000 samples split across ranks
#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = 10000000;
    long long n = N / size;
    if (rank == size - 1) n = N - (N / size) * (size - 1);   // remainder to last rank
    srand(12345 + rank);   // different seed per rank, otherwise every rank gives the same points

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long long hits = 0, total = 0;
    for (long long i = 0; i < n; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) hits++;
    }
    MPI_Reduce(&hits, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    if (rank == 0)
        printf("pi=%f samples=%lld procs=%d time=%f s\n", 4.0 * total / N, N, size, t1 - t0);
    MPI_Finalize();
    return 0;
}
