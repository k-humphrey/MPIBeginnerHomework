#include <iostream>
#include <mpi.h>
#include "PartitionedGrid.h"

using namespace std;


void partitionSize(int parts, int total, int part_coord, int & return_entries);

int main(int argc, char** argv){
    PartitionedGrid grid;
    //initialize mpi
    int total_processes = 0, my_rank = 0, iterations = 0, process_rows = 0, process_columns = 0, total_rows = 0, total_columns = 0;
    int share_vector[6];
    string filename;
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

    //gather at process 0
    int recv_rows[total_processes];
    int my_rows_no_halo = local_rows - 2;
    MPI_Gather(&my_rows_no_halo, 1, MPI_INT, recv_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int recv_cols[total_processes];
    int my_cols_no_halo = local_columns - 2;
    MPI_Gather(&my_cols_no_halo, 1, MPI_INT, recv_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //process 0 has to scatter the global array according to displacement and counts that it can calculate
    int counts[total_processes];
    int displacements[total_processes];
    MPI_Datatype subarray_types[total_processes];

    //everyone post an irecieve for their array
    MPI_Request req[total_processes + 1];
    int numreq = 1;
    MPI_Irecv(new_world, entries, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &req[0]);
    if(0 == my_rank){
        //how many entries does each process get? easy
        counts[0] = recv_rows[0] * recv_cols[0];
        displacements[0] = 0;
        for(int i = 1; i < total_processes; i++){
            counts[i] = recv_rows[i] * recv_cols[i];
        }
        //but where does it start? worse.
        int rowsBeforeMe = 0;
        int colBeforeMe = 0;
        int processIndex = 0;
        int sizes[2] = {total_rows, total_columns};
        int subsizes[2];
        int starts[2];

        for(int curProcRow = 0; curProcRow < process_rows; curProcRow++){
            colBeforeMe = 0; //resets each row
            //for each process row,
            for(int curProcCol = 0; curProcCol < process_columns; curProcCol++){
                subsizes[0] = recv_rows[processIndex];
                subsizes[1] = recv_cols[processIndex];
                starts[0] = rowsBeforeMe;
                starts[1] = colBeforeMe;
                MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_CHAR, &subarray_types[processIndex]);
                MPI_Type_commit(&subarray_types[processIndex]);
                displacements[processIndex] = rowsBeforeMe * total_columns + colBeforeMe;
                colBeforeMe += recv_cols[processIndex];
                processIndex++;
            }
            rowsBeforeMe += recv_rows[processIndex - 1];
        }

        //okay... now we have a different subarray type to describe each subarray..
        //lets send the pieces manually. scatterv wont work :( 
        numreq = total_processes + 1;
        for(int i = 0; i < total_processes; i++){
            MPI_Isend(global_array, 1, subarray_types[i], i, 0, MPI_COMM_WORLD, &req[i+1]);
        }
    }
    //waitall
    MPI_Waitall(numreq, req, MPI_STATUSES_IGNORE);
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
    //find my neighbors, and get indexing out of the way
    grid.setNeighbors(process_rows, process_columns, my_row_coord, my_col_coord, my_rank);
    grid.setIndexes(local_rows, local_columns);
    int innerRowSize = local_columns - 2;
    MPI_Request req2[16];
    MPI_Datatype column_type;
    MPI_Type_vector(local_rows - 2, 1, local_columns, MPI_CHAR, &column_type);
    MPI_Type_commit(&column_type);
    for(int i = 0; i < iterations; i++){
        //post recieves from
        //from up 
        MPI_Irecv(&old_world[grid.topRowHalo], innerRowSize , MPI_CHAR, grid.up, 0, MPI_COMM_WORLD, &req2[0]);
        //from down
        MPI_Irecv(&old_world[grid.bottomRowHalo], innerRowSize, MPI_CHAR, grid.down, 1, MPI_COMM_WORLD, &req2[2]);
        //left
        MPI_Irecv(&old_world[grid.leftColHalo], 1, column_type, grid.left, 2, MPI_COMM_WORLD, &req2[4]);
        //right
        MPI_Irecv(&old_world[grid.rightColHalo], 1, column_type, grid.right, 3, MPI_COMM_WORLD, &req2[6]);
        //up left 
        MPI_Irecv(&old_world[grid.topLeftCorner], 1, MPI_CHAR, grid.upLeft, 4, MPI_COMM_WORLD, &req2[8]);
        //up right
        MPI_Irecv(&old_world[grid.topRightCorner], 1, MPI_CHAR, grid.upRight, 5, MPI_COMM_WORLD, &req2[10]);
        //down left
        MPI_Irecv(&old_world[grid.bottomLeftCorner], 1, MPI_CHAR, grid.downLeft, 6, MPI_COMM_WORLD, &req2[12]);
        //down right
        MPI_Irecv(&old_world[grid.bottomRightCorner], 1, MPI_CHAR, grid.downRight, 7, MPI_COMM_WORLD, &req2[14]);

        
        //send to
        //up
        MPI_Isend(&old_world[grid.topRowInner], innerRowSize, MPI_CHAR, grid.up, 1, MPI_COMM_WORLD, &req2[3]);
        //down 
        MPI_Isend(&old_world[grid.bottomRowInner], innerRowSize, MPI_CHAR, grid.down, 0, MPI_COMM_WORLD, &req2[1]);
        //left 
        MPI_Isend(&old_world[grid.leftColInner], 1, column_type, grid.left, 3, MPI_COMM_WORLD, &req2[7]);
        //right 
        MPI_Isend(&old_world[grid.rightColInner], 1 , column_type, grid.right, 2, MPI_COMM_WORLD, &req2[5]);
        //up left 
        MPI_Isend(&old_world[grid.topLeftInnerCorner], 1, MPI_CHAR, grid.upLeft, 7,  MPI_COMM_WORLD, &req2[13]);
        //up right 
        MPI_Isend(&old_world[grid.topRightInnerCorner], 1, MPI_CHAR, grid.upRight, 6,  MPI_COMM_WORLD, &req2[15]);
        //down left 
        MPI_Isend(&old_world[grid.bottomLeftInnerCorner], 1, MPI_CHAR, grid.downLeft, 5,  MPI_COMM_WORLD, &req2[11]);
        //down right (take my bottom right inner corner, send to the upper left corner halo of my bottom right neighbor)
        MPI_Isend(&old_world[grid.bottomRightInnerCorner], 1, MPI_CHAR, grid.downRight, 4,  MPI_COMM_WORLD, &req2[9]);

        MPI_Waitall(16, req2, MPI_STATUSES_IGNORE);

        //do updates (every cell)
        grid.doUpdates(old_world, new_world, local_rows, local_columns);
        //swap old and new
        char *tmp = old_world;
        old_world = new_world;
        new_world = tmp;

        //strip halos off to gather
        grid.stripHalo(tempArray, new_world, local_rows, local_columns); 
        //gather doesnt work anymore.
        

        
        //let rank 0 print out the global array :)
        if(1 == my_rank){
            cout << "Iteration " << i << endl << endl; 
            grid.printGrid(new_world ,local_rows, local_columns);
            //grid.printGrid(global_array, total_rows, total_columns);
        }
        MPI_Barrier(MPI_COMM_WORLD);
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
