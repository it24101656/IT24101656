// Exercise 6: Pi (Monte Carlo) with explicit Send/Recv.
// Workers send their hit count to rank 0. Rank 0 receives with MPI_ANY_SOURCE
// (results are accepted in whatever order they arrive).
// Compile with -DFIXED_SOURCE to receive from rank i in fixed order (the "original" to compare with):
//   CC ex6_pi_any_source.cpp -o ex6_any
//   CC -DFIXED_SOURCE ex6_pi_any_source.cpp -o ex6_fixed
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
    if (rank == size - 1) n = N - (N / size) * (size - 1);
    srand(12345 + rank);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long long hits = 0;
    for (long long i = 0; i < n; i++) {
        double x = (double)rand() / RAND_MAX, y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) hits++;
    }

    if (rank != 0) {
        MPI_Send(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        long long total = hits, part;
        MPI_Status status;
        for (int i = 1; i < size; i++) {
#ifdef FIXED_SOURCE
            MPI_Recv(&part, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, &status);
#else
            MPI_Recv(&part, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
#endif
            total += part;
            // status.MPI_SOURCE tells which rank this result came from
        }
        double t1 = MPI_Wtime();
#ifdef FIXED_SOURCE
        const char *mode = "fixed-source";
#else
        const char *mode = "any-source";
#endif
        printf("mode=%s pi=%f samples=%lld procs=%d time=%f s\n", mode, 4.0 * total / N, N, size, t1 - t0);
    }
    MPI_Finalize();
    return 0;
}
