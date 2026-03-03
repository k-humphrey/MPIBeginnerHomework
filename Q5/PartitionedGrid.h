#ifndef PARTITIONEDGRID_H
#define PARTITIONEDGRID_H
    #include <fstream>
    #include <iostream>
    #include <sstream>

    using namespace std;

    class PartitionedGrid{
        private:
        public:
            int up;
            int down;
            int right;
            int left;
            int upLeft;
            int upRight;
            int downLeft;
            int downRight;
            void printGrid(char*, int, int);
            void stripHalo(char*, char*,  int, int);
            void setNeighbors(int, int, int, int, int);
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

    void PartitionedGrid::setNeighbors(int process_rows, int process_columns, int row_coord, int col_coord, int my_rank){
        int toRow = 0, toCol = 0;

        //we are calculating the rank of the column in that direction relative to us (with wrapping)
        //up 
        toRow = (row_coord - 1 + process_rows) % process_rows;
        toCol = col_coord;
        up = toRow * process_columns + toCol;

        //up right (because we found up, now we just find right of that)
        toCol = (col_coord - 1 + process_columns) % process_columns;
        upRight = toRow * process_columns + toCol;
        
        //up left
        toCol = (col_coord + 1 + process_columns) % process_columns;
        upLeft = toRow * process_columns + toCol;

        //down
        toRow = (row_coord + 1 + process_rows) % process_rows;
        toCol = col_coord;
        down = toRow * process_columns + toCol;

        //down right (again, using downs row coord)
        toCol = (col_coord - 1 + process_columns) % process_columns;
        downRight = toRow * process_columns + toCol;

        //down left
        toCol = (col_coord + 1 + process_columns) % process_columns;
        downLeft = toRow * process_columns + toCol;

        //left
        toRow = row_coord;
        toCol = (col_coord + 1 + process_columns) % process_columns;
        left = toRow * process_columns + toCol;

        //right
        toRow = row_coord;
        toCol = (col_coord - 1 + process_columns) % process_columns;
        right = toRow * process_columns + toCol;
        
    }

  

#endif