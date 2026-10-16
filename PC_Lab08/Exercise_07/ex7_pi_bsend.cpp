// Exercise 7: Exercise 6 with Buffered Send (MPI_Bsend) in the workers.
// Rank 0 still receives with MPI_ANY_SOURCE.
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
        int bsize = sizeof(long long) + MPI_BSEND_OVERHEAD;
        char *buf = (char *)malloc(bsize);
        MPI_Buffer_attach(buf, bsize);
        MPI_Bsend(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        MPI_Buffer_detach(&buf, &bsize);
        free(buf);
    } else {
        long long total = hits, part;
        MPI_Status status;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&part, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total += part;
        }
        double t1 = MPI_Wtime();
        printf("mode=bsend pi=%f samples=%lld procs=%d time=%f s\n", 4.0 * total / N, N, size, t1 - t0);
    }
    MPI_Finalize();
    return 0;
}
