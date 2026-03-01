#include <iostream>
#include <mpi.h>
#include "PartitionedGrid.h"

using namespace std;


void partitionSize(int parts, int total, int part_coord, int & return_entries);

int main(int argc, char** argv){
    //initialize mpi
    int total_processes = 0, my_rank = 0, iterations = 0, process_rows = 0, process_columns = 0, total_rows = 0, total_columns = 0;
    int share_vector[5];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    //everyone needs to know process rows and columns so they can partition
    if(0 == my_rank){
        int dims[2] = {0, 0}; //2D
        MPI_Dims_create(total_processes, 2, dims); //let mpi pick the process grid
        process_rows = dims[0];
        process_columns = dims[1];
        cout << "Rows: " << process_rows << " columns: " << process_columns << endl;

        //use constructor from class object to create grid
        PartitionedGrid global_world;
        global_world.printGrid();

        cout << "How many iterations?: ";
        cin >> iterations;    

        total_rows = global_world.getRows();
        total_columns = global_world.getColumns();
        //share this to every rank so they can partiton things too
        share_vector[0] = total_rows;
        share_vector[1] = total_columns;
        share_vector[2] = process_rows;
        share_vector[3] = process_columns;
        share_vector[4] = iterations;
 
    }
    //share to everyone
    MPI_Bcast(share_vector, 5, MPI_INT, 0, MPI_COMM_WORLD);
    if (0 != my_rank){
        total_rows = share_vector[0];
        total_columns = share_vector[1];
        process_rows = share_vector[2];
        process_columns = share_vector[3];
        iterations = share_vector[4];

    }
    //figure out each processes coordinates on the grid
    int my_row_coord = my_rank / process_columns;
    int my_col_coord = my_rank % process_columns;

    //call partition size to find out how many rows, and how many columns each process needs to have of the global grid
    int local_rows = 0, local_columns = 0;
    partitionSize(process_rows, total_rows, my_row_coord, local_rows);
    partitionSize(process_columns, total_columns, my_col_coord, local_columns);

    //allocate local grid (add halos :-) )
    
    //everyone calculates their partition of the rows or columns
    //process 0 has to scatter the global array according to displacement and counts that it can calculate
    //set up loop
    //do updates (each process sends left halo, right halo, top halo, bottom halo, and corners)
    //they recieve the halos too from neighbors
    //they calculate what cells die or live, and update locally
    //gather
    //print out world
    //loop again if we need to
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
