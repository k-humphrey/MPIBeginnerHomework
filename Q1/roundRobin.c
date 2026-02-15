/*Kayla Humphrey
roundRobin.c
2/15/2026
MPI program for HW1 Parallel computing. A program that passes messages in a logical ring. Revised with suggestions from AI.*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

//revised from AI
#ifndef N
#define N 2
#endif

int main(int argc, char* argv[]){
    //variables for mpi
    int message, numProcesses, myRank, sendRank, recvRank;
    MPI_Comm comm = MPI_COMM_WORLD;

    //init mpi and get number of processes + everyones rank
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &numProcesses);
    MPI_Comm_rank(comm, &myRank);

    //find send and recieve partner (revised from AI)
    sendRank = (myRank + 1) % numProcesses;
    recvRank = (myRank - 1 + numProcesses) % numProcesses;
    
    //loop through the ring N times, incrementing each message passed
    for(int i = 0; i < N; i++){
        if(myRank == 0){
            MPI_Send(&message, 1, MPI_INT, sendRank, 0, comm);
            MPI_Recv(&message, 1, MPI_INT, recvRank, 0, comm, MPI_STATUS_IGNORE);
            message++;
        }
        else{
            MPI_Recv(&message, 1, MPI_INT, recvRank, 0, comm, MPI_STATUS_IGNORE);
            message++;
            MPI_Send(&message, 1, MPI_INT, sendRank, 0, comm);
        }
    }

    //print out summary info
    if(myRank == 0){
        printf("Ending amount of messages passed: %d\n", message);
    }

    //must finalize mpi
    MPI_Finalize();

    return 0;
}