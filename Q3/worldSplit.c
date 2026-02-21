/*Kayla Humphrey
02/20/2026
MPI Comm Splitting practice*/
#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv){
    //initalize mpi
    MPI_Init(&argc, &argv);
    int worldRank, PROCESSES;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &PROCESSES);

    int Q = 4;
    int P = 2;
    //meaning that world size needs to be 8

    //split 1, color is based on rank / Q
    int color = worldRank / Q;
    int split1Rank;
    MPI_Comm split1Comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, worldRank, &split1Comm);
    MPI_Comm_rank(split1Comm, &split1Rank);

    //split 2, color is based on rank % Q
    color = worldRank % Q;
    int split2Rank;
    MPI_Comm split2Comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, worldRank, &split2Comm);
    MPI_Comm_rank(split2Comm, &split2Rank);

    printf("World rank: %d, Split1 rank: %d, Split2 rank: %d\n", worldRank, split1Rank, split2Rank);
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
