#ifndef PARTITIONEDGRID_H
#define PARTITIONEDGRID_H
    #include <fstream>
    #include <iostream>
    #include <sstream>

    using namespace std;

    class PartitionedGrid{
        private:
            char * global_array;
            char * old_world;
            char * new_world;
            int grid_rows;
            int grid_columns;
            int local_rows;
            int local_columns;
            bool has_local;
            bool has_global;
        public:
            PartitionedGrid();
            PartitionedGrid(int, int);
            ~PartitionedGrid();
            void printGrid();
            void printLocalGrid();
            int getRows();
            int getColumns();
    };


    /*PartitionedGrid constructor
    reads in file of world seed and creates the array*/
    PartitionedGrid::PartitionedGrid(){
        //initalize values
        global_array = nullptr;
        old_world    = nullptr;
        new_world    = nullptr;
        has_global = false;
        has_local  = false;

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
        ss >> grid_rows >> grid_columns; 

        //allocate memory for 2d char array
        global_array = new char[grid_rows * grid_columns];

        //traverse array, while reading file into it's locations
        for(int curRow = 0; curRow < grid_rows; curRow++){
            getline(file, line);
            for(int curCol = 0; curCol < grid_columns; curCol++){
                global_array[curRow * grid_columns + curCol] = line[curCol];
            }
        }  
        has_global = true;
    }

    void PartitionedGrid::printGrid(){
         for(int curRow = 0; curRow < grid_rows; curRow++){
            for(int curCol = 0; curCol < grid_columns; curCol++){
                cout << global_array[curRow * grid_columns + curCol] << " ";
            } 
            cout << endl;
        }
    }

    void PartitionedGrid::printLocalGrid(){
         for(int curRow = 0; curRow < local_rows; curRow++){
            for(int curCol = 0; curCol < local_columns; curCol++){
                cout << old_world[curRow * local_columns + curCol] << " ";
            } 
            cout << endl;
        }
    }

    PartitionedGrid::~PartitionedGrid(){
        //delete all allocated memory (hopefully)
        if(has_global){
            delete[] global_array;
        }
        if(has_local){
            delete[] old_world;
            delete[] new_world;
        }
        
    }

    int PartitionedGrid::getRows(){
        return grid_rows;
    }
    int PartitionedGrid::getColumns(){
        return grid_columns;
    }

    PartitionedGrid::PartitionedGrid(int rows, int columns){
        //initalize values
        global_array = nullptr;
        old_world    = nullptr;
        new_world    = nullptr;
        has_global = false;
        has_local  = false;

        //add halos
        local_rows = rows + 2;
        local_columns = columns + 2;

        old_world = new char[local_rows * local_columns];
        new_world = new char[local_rows * local_columns];

        //initialize values
        for(int curRow = 0; curRow < local_rows; curRow++){
            for(int curCol = 0; curCol < local_columns; curCol++){
                old_world[curRow * local_columns + curCol] = '@';
                new_world[curRow * local_columns + curCol] = '#';
            }
        }
        has_local = true;
    }

#endif