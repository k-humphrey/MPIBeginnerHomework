#ifndef PARTITIONEDGRID_H
#define PARTITIONEDGRID_H
    #include <fstream>
    #include <iostream>
    #include <sstream>

    using namespace std;

    class PartitionedGrid{
        private:
        public:
            void printGrid(char*, int, int);
            void stripHalo(char*, char*,  int, int);
    };

    void PartitionedGrid::printGrid(char* array, int grid_rows, int grid_columns){
         for(int curRow = 0; curRow < grid_rows; curRow++){
            for(int curCol = 0; curCol < grid_columns; curCol++){
                cout << array[curRow * grid_columns + curCol] << " ";
            } 
            cout << endl;
        }
    }
    void PartitionedGrid::stripHalo(char* tempArray, char* new_world, int rows, int columns){
        int curEntry = 0;
        for(int curRow = 1; curRow < rows - 1; curRow++){
            for(int curCol = 1; curCol < columns - 1; curCol++){
                tempArray[curEntry] = new_world[curRow * columns + curCol];
                curEntry++;
            }
        }
    }

  

#endif