/*Kayla Humphrey
02/20/2026
MPI program that demonstrates many collective communications*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    int myRank, real_max, PROCESSES;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &PROCESSES);
    char myChar = 65 + myRank; //Letters starting from A
    char *charArray = malloc(sizeof(char) * PROCESSES);

    int option = atoi(argv[1]);
    if(option == 1){
        //Bcast and reduce = allreduce
        if(0 == myRank){
            printf("BCast with reduce\n");
        }
        MPI_Reduce(&myRank, &real_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Bcast(&real_max, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Rank: %d, real_max: %d\n", myRank, real_max);
    }
    if(option == 2){
        //allreduce
        if(0 == myRank){
            printf("All reduce\n");
        }
        MPI_Allreduce(&myRank, &real_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        printf("Rank: %d, real_max: %d\n", myRank, real_max);

    }
    if(option == 3){
        //mpigather then mpit bcast = mpiallgather
        if(0 == myRank){
            printf("gather and bcast\n");
        }
        MPI_Gather(&myChar, 1, MPI_CHAR, charArray, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(charArray, PROCESSES, MPI_CHAR, 0, MPI_COMM_WORLD);
        for(int i = 0; i < PROCESSES; i++){
            printf("%c, ", charArray[i]);
        }
        printf("\n");
    }
    if(option == 4){
        //mpiallgather
        if(0 == myRank){
            printf("All gather\n");
        }

        MPI_Allgather(&myChar, 1, MPI_CHAR, charArray, 1, MPI_CHAR, MPI_COMM_WORLD);
        for(int i = 0; i < PROCESSES; i++){
            printf("%c, ", charArray[i]);
        }
        printf("\n");
    }
    if(option == 5){
        //build send buffers (personal messages)
        int sendBuffer[PROCESSES];
        int recvBuffer[PROCESSES];
        for(int i = 0; i < PROCESSES; i++){
            sendBuffer[i] = myRank + i;
        }

        //mpiall to all 
        if(0 == myRank){
            printf("All to all\n");
        }
        MPI_Alltoall(sendBuffer, 1, MPI_INT, recvBuffer, 1, MPI_INT, MPI_COMM_WORLD);
        for(int i = 0; i < PROCESSES; i++){
            printf("%d, ", recvBuffer[i]);
        }
        printf("\n");
    }
    

    //free allocated memory
    free(charArray);
    MPI_Finalize();
    return 0;
}