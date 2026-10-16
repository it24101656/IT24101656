// Exercise 5.1: source/destination mismatch. Based on message2.cc.
// Rank 1 sends to rank 3, but rank 3 waits for a message from rank 2 (nobody sends it).
// Expected: MPI_Recv never matches, so rank 3 blocks forever; MPI_Ssend on rank 1 also
// blocks because the receive is never posted for it -> the program HANGS (deadlock).
// Run with a short time limit (e.g. timeout 10 mpirun -n 4 ./ex5_mismatch  or #SBATCH --time=00:00:30)
#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(void)
{
    int rank;
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int x = 30, y = 0;
    if (rank == 1) {
      printf("Sending message to rank 3 from rank 1\n"); fflush(stdout);
      MPI_Ssend(&x, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
      printf("Rank 1: send completed\n");
    }
    else if (rank == 3) {
      printf("Rank 3: waiting for a message from rank 2 (never sent)\n"); fflush(stdout);
      MPI_Recv(&y, 1, MPI_INT, 2 /* WRONG source: should be 1 */, 0, MPI_COMM_WORLD, &status);
      printf("Rank 3: y = %d\n", y);
    }
    MPI_Finalize();
}
