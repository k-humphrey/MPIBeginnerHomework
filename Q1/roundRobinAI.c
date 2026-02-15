/*Prompt used:
do this in c 
Write an MPI program (by hand, no LLMs) that passes a message of one integer around in a logical ring of processes with MPI WORLD.
The integer should start at 0 in process 0 and be incremented each time it passes around the ring,
and you should be able to have the message go around the ring N times, where N is specified at compile time.
AI: Chatgpt 
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef N
#define N 5   /* Number of full rotations around the ring */
#endif

int main(int argc, char *argv[])
{
    int rank, size;
    int value;
    int next, prev;
    int tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Determine neighbors in the logical ring */
    next = (rank + 1) % size;
    prev = (rank - 1 + size) % size;

    if (rank == 0) {
        value = 0;

        for (int i = 0; i < N; i++) {
            /* Send to next process */
            MPI_Send(&value, 1, MPI_INT, next, tag, MPI_COMM_WORLD);

            /* Receive from previous process */
            MPI_Recv(&value, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            /* Increment after full rotation */
            value++;

            printf("Process 0 completed rotation %d, value = %d\n", i + 1, value);
        }
    } else {
        for (int i = 0; i < N; i++) {
            /* Receive from previous */
            MPI_Recv(&value, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            /* Pass to next */
            MPI_Send(&value, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
