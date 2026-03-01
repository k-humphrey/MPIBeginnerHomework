#ifndef PARTITIONEDGRID_H
#define PARTITIONEDGRID_H
    #include <fstream>
    #include <iostream>
    #include <sstream>

    using namespace std;

    class PartitionedGrid{
        private:
            char ** global_array;
            int grid_rows;
            int grid_columns;
        public:
            PartitionedGrid();
            ~PartitionedGrid();
            void printGrid();
            int getRows();
            int getColumns();
    };


    /*PartitionedGrid constructor
    reads in file of world seed and creates the array*/
    PartitionedGrid::PartitionedGrid(){
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
        global_array = new char*[grid_rows];
        for (int i = 0; i < grid_rows; i++) {
            global_array[i] = new char[grid_columns];
        }

        //traverse array, while reading file into it's locations
        for(int curRow = 0; curRow < grid_rows; curRow++){
            getline(file, line);
            for(int curCol = 0; curCol < grid_columns; curCol++){
                global_array[curRow][curCol] = line[curCol];
            }
        }  
    }

    void PartitionedGrid::printGrid(){
         for(int curRow = 0; curRow < grid_rows; curRow++){
            for(int curCol = 0; curCol < grid_columns; curCol++){
                cout << global_array[curRow][curCol] << " ";
            } 
            cout << endl;
        }
    }

    PartitionedGrid::~PartitionedGrid(){
        //delete all allocated memory
         for (int i = 0; i < grid_rows; i++) {
            delete global_array[i];
        }
        delete global_array;
    }

    int PartitionedGrid::getRows(){
        return grid_rows;
    }
    int PartitionedGrid::getColumns(){
        return grid_columns;
    }
    
#endif