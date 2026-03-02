#include <iostream>
#include <mpi.h>
#include "PartitionedGrid.h"

using namespace std;


void partitionSize(int parts, int total, int part_coord, int & return_entries);

int main(int argc, char** argv){
    PartitionedGrid grid;
    //initialize mpi
    int total_processes = 0, my_rank = 0, iterations = 0, process_rows = 0, process_columns = 0, total_rows = 0, total_columns = 0;
    int share_vector[5];
    char * global_array = nullptr;
    char * old_world = nullptr;
    char * new_world = nullptr;
    char * tempArray = nullptr;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(0 == my_rank){
        int dims[2] = {0, 0}; //2D
        MPI_Dims_create(total_processes, 2, dims); //let mpi pick the process grid
        process_rows = dims[0];
        process_columns = dims[1];
        cout << "Process rows, columns, " << process_rows << " " << process_columns << endl;
        //read from file
        string filename;
        cout << "Enter a filename that contains your starting seed: ";
        cin >> filename;
        ifstream file(filename);
        if(!file.is_open()){
            cout << "Error opening file";
        }
        //file is open now, read contents into global array

        string line = " "; 
        getline(file, line); //read first line that contains grid rows and columns
        stringstream ss(line);
        ss >> total_rows >> total_columns; 

        //allocate memory for 2d char array
        global_array = new char[total_rows * total_columns];

        //traverse array, while reading file into it's locations
        for(int curRow = 0; curRow < total_rows; curRow++){
            getline(file, line);
            for(int curCol = 0; curCol < total_columns; curCol++){
                global_array[curRow * total_columns + curCol] = line[curCol];
            }
        }

        //get iterations
        cout << "How many iterations?: ";
        cin >> iterations;    

        //share this to every rank so they can partiton things too
        share_vector[0] = total_rows;
        share_vector[1] = total_columns;
        share_vector[2] = process_rows;
        share_vector[3] = process_columns;
        share_vector[4] = iterations;
 
    }
    //share to everyone
    MPI_Bcast(share_vector, 5, MPI_INT, 0, MPI_COMM_WORLD);
    //everyone sets variables accordingly
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

    //call partition size to find out how many rows and how many columns each process needs to have of the global grid
    int local_rows = 0, local_columns = 0;
    partitionSize(process_rows, total_rows, my_row_coord, local_rows);
    partitionSize(process_columns, total_columns, my_col_coord, local_columns);
    int entries = local_rows * local_columns;

    //allocate local grid (add halos :-) )
    local_rows += 2;
    local_columns += 2;

    old_world = new char[local_rows * local_columns];
    new_world = new char[local_rows * local_columns];
    tempArray = new char[(local_rows - 2) * (local_columns - 2)];

    //initialize values
    for(int curRow = 0; curRow < local_rows; curRow++){
        for(int curCol = 0; curCol < local_columns; curCol++){
            old_world[curRow * local_columns + curCol] = '@';
            new_world[curRow * local_columns + curCol] = '#';
        }
    }

    //gather entries at process 0 (for counts)
    int recv_buf[total_processes];
    MPI_Gather(&entries, 1, MPI_INT, recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);


    //process 0 has to scatter the global array according to displacement and counts that it can calculate
    int counts[total_processes];
    int displacements[total_processes];
    if(0 == my_rank){
        //how many entries does each process get? what index does that start from?
        counts[0] = recv_buf[0];
        displacements[0] = 0;
        for(int i = 1; i < total_processes; i++){
            counts[i] = recv_buf[i];
            displacements[i] = displacements[i-1] + counts[i-1];
        }
    }

    //scatterv here, put into newworld so that I can format it properly into old world (target destination)
    MPI_Scatterv(global_array, counts, displacements, MPI_CHAR, new_world, entries , MPI_CHAR, 0, MPI_COMM_WORLD);

    //load new world into old world
    //we must skip row 1, and last row, col 1 and last column :) those are halos
    int curEntry = 0;
    for(int curRow = 1; curRow < local_rows - 1; curRow++){
        for(int curCol = 1; curCol < local_columns - 1; curCol++){
            old_world[curRow * local_columns + curCol] = new_world[curEntry];
            curEntry++;
        }
    }

    //set up loop
    //find my neighbors
    grid.setNeighbors(process_rows, process_columns, my_row_coord, my_col_coord, my_rank);
    for(int i = 0; i < iterations; i++){
        //post recieves from
        //up (recieve a row from my upper neighbor)

        //down
        //left
        //right
        //up left 
        //up right
        //down left
        //down right
        
        //send to
        //up (take my row 1 (-edges) and send to bottom halo of upper neighbor)
        //down (take my last row - 1 without edges) and send to top halo of lower neighbor)
        //left (strip my left column out and send to my left neighbors halo)
        //right (stip my right column out and send to my right neighbors halo)
        //up left (take my upper left inner corner, send to bottom right corner halo of upper left neighbor)
        //up right (take my upper right inner corner, send to bottom left corner halo of upper right neighbor)
        //down left (take my bottom left inner corner, send to upper right corner halo of bottom left neighbor)
        //down right (take my bottom right inner corner, send to the upper left corner halo of my bottom right neighbor)

        //do updates (every cell)

        //swap old and new

        //strip halos off to gather
        grid.stripHalo(tempArray, old_world, local_rows, local_columns); //eventually change to new_world
        //gather
        MPI_Gatherv(tempArray, entries, MPI_CHAR, global_array, counts, displacements, MPI_CHAR, 0, MPI_COMM_WORLD);
        
        //let rank 0 print out the global array :)
        if(0 == my_rank){
            cout << grid.upLeft << grid.up << grid.upRight << endl;
            cout << grid.left << my_rank << grid.right << endl;
            cout << grid.downLeft << grid.down << grid.downRight << endl; 
            grid.printGrid(global_array, total_rows, total_columns);
        }
        

    }
    
   
    
    //finalize mpi and clean memory
    delete[] old_world;
    delete[] new_world;
    delete[] tempArray;
    if(0 == my_rank){
        delete[] global_array;
    }
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
