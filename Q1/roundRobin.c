#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    int message, numProcesses, myRank, sendRank, recvRank;

    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &numProcesses);
    MPI_Comm_rank(comm, &myRank);

    //find send and recieve partner
    sendRank = myRank + 1;
    if(sendRank > numProcesses - 1){
        sendRank = 0; //wrap
    }
    
    recvRank = myRank - 1;
    if(recvRank < 0){
        recvRank = numProcesses - 1; //wrap
    }
    
    for(int i = 0; i < atoi(argv[1]); i++){
        if(myRank == 0){
            MPI_Send(&message, 1, MPI_INT, sendRank, 0, comm);
            MPI_Recv(&message, 1, MPI_INT, recvRank, 0, comm, MPI_STATUS_IGNORE);
            message++;
        }
        else{
            MPI_Recv(&message, 1, MPI_INT, recvRank, 0, comm, MPI_STATUS_IGNORE);
            //increment it
            message++;
            MPI_Send(&message, 1, MPI_INT, sendRank, 0, comm);
        }
    }

    MPI_Finalize();
}