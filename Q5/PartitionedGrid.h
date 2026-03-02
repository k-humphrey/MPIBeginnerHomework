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
    };

    void PartitionedGrid::printGrid(char* array, int grid_rows, int grid_columns){
         for(int curRow = 0; curRow < grid_rows; curRow++){
            for(int curCol = 0; curCol < grid_columns; curCol++){
                cout << array[curRow * grid_columns + curCol] << " ";
            } 
            cout << endl;
        }
    }

  

#endif