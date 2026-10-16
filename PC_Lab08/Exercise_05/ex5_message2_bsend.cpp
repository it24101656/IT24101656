// Exercise 5.2: message2.cc rewritten with a Buffered Send (MPI_Bsend).
// The send variable x is not modified. Run with at least 4 processes.
#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(void)
{
    int rank;
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(name, &len);
    int x, y;
    x = 30;
    if (rank == 1) {
      int bsize = sizeof(int) + MPI_BSEND_OVERHEAD;
      char *buf = (char *)malloc(bsize);
      MPI_Buffer_attach(buf, bsize);
      printf("Sending (Bsend) message to rank 3 from rank 1\n");
      MPI_Bsend(&x, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);   // returns immediately: data copied to buffer
      MPI_Buffer_detach(&buf, &bsize);                   // waits until buffered message is delivered
      free(buf);
    }
    else if (rank == 3) {
      MPI_Recv(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
      printf("in rank 3 the value of y is %d\n", y);
    }
    else
      printf("Just a normal process From rank %d machine %s\n", rank, name);
    MPI_Finalize();
}
