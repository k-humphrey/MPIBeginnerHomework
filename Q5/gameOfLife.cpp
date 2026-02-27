#include <iostream>
#include <mpi.h>

using namespace std;


void partitionSize(int parts, int total, int part_coord, int & return_entries);

int main(int argc, char** argv){
    //initialize mpi
    int total_processes = 0, my_rank = 0; 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //everyone needs to know process rows and columns so they can partition
    if(0 == my_rank){
        int process_rows = 0, process_columns = 0;
        int dims[2] = {0, 0}; //2D
        MPI_Dims_create(total_processes, 2, dims); //let mpi pick the process grid
        process_rows = dims[0];
        process_columns = dims[1];
        cout << "Rows: " << process_rows << " columns: " << process_columns << endl;
        //read from file, create global array, get iterations
    }

    //scatter info to all processes (process rows, columns, m, n, iterations)
    //set up loop
    //do updates
    //gather
    //print out world
    //finalize mpi and clean memory
    MPI_Finalize();
}

//gotta call this twice
//one tells how many rows, other is columns
//usage ex: parts = process rows, total = total rows, part_coord = index of process row(3 rows means 0, 1, or 2), 
void partitionSize(int parts, int total, int part_coord, int & return_entries){
    int base = total / parts; //integer division to get a base partition
    int remainder = total % parts; //figure out the remainder to be distributed

    //distribute remainder
    if (part_coord < remainder)
        return_entries = base + 1;
    else
        return_entries = base;
    
}
