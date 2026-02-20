/*Kayla Humphrey
Vector * Vector dot product
2/19/2026*/

#include <iostream>
#include "mpi.h"

using namespace std;

int main(int argc, char **argv){
    int SIZE = atoi(argv[1]);
    //two vectors of size SIZE initialized to be all ones.
    vector<int> vector1(SIZE, 1);
    vector<int> vector2(SIZE, 1);
    //cout << "Expected output: " << SIZE << endl;

    //mpi init
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int myRank, PROCESSES;
    MPI_Comm_rank(comm, &myRank);
    MPI_Comm_size(comm, &PROCESSES);

    /*This part is STRAIGHT from the textbook.
    I think its really clever!*/
    int ibegin = SIZE * (myRank) / PROCESSES;
    int iend = SIZE * (myRank + 1) / PROCESSES;
    int nsize = iend - ibegin;

    cout << "Ibegin : " << ibegin << " Iend: " << iend << " Nsize: " << nsize << endl;
    MPI_Finalize(); 



}
