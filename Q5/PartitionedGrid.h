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
            int topRowHalo;
            int bottomRowHalo;
            int leftColHalo;
            int rightColHalo;
            int topRowInner;
            int bottomRowInner;
            int leftColInner;
            int rightColInner;
            int topLeftCorner;
            int topRightCorner;
            int bottomLeftCorner;
            int bottomRightCorner;
            int topLeftInnerCorner;
            int topRightInnerCorner;
            int bottomLeftInnerCorner;
            int bottomRightInnerCorner;
            void printGrid(char*, int, int);
            void stripHalo(char*, char*,  int, int);
            void setNeighbors(int, int, int, int, int);
            void setIndexes(int, int);
            void doUpdates(char*, char*, int, int);
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
        toCol = (col_coord + 1 + process_columns) % process_columns;
        upRight = toRow * process_columns + toCol;
        
        //up left
        toCol = (col_coord - 1 + process_columns) % process_columns;
        upLeft = toRow * process_columns + toCol;

        //down
        toRow = (row_coord + 1 + process_rows) % process_rows;
        toCol = col_coord;
        down = toRow * process_columns + toCol;

        //down right (again, using downs row coord)
        toCol = (col_coord + 1 + process_columns) % process_columns;
        downRight = toRow * process_columns + toCol;

        //down left
        toCol = (col_coord - 1 + process_columns) % process_columns;
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

    void PartitionedGrid::setIndexes(int row, int column){
        //row * columns + column 
        int lastRow = row - 1;
        int lastCol = column - 1;
        topRowHalo = 0 * column + 1;
        bottomRowHalo = lastRow * column + 1;
        leftColHalo  = 1 * column + 0;
        rightColHalo = 1 * column + lastCol;
        topRowInner = 1 * column + 1;
        bottomRowInner = (row - 2) * column + 1;
        leftColInner  = 1 * column + 1;
        rightColInner = 1 * column + (column - 2);
        topLeftCorner = 0 * column + 0;
        topRightCorner = 0 * column + lastCol;
        bottomLeftCorner = lastRow * column + 0;
        bottomRightCorner = lastRow * column + lastCol;
        topLeftInnerCorner = 1 * column + 1;
        topRightInnerCorner = 1 * column + (column - 2);
        bottomLeftInnerCorner  = (row - 2) * column + 1;
        bottomRightInnerCorner = (row - 2) * column + (column - 2);
    }

    void PartitionedGrid::doUpdates(char* old_world, char* new_world, int rows, int columns){
        //go through inner indexes and check if the cell is alive or dead :)
        for(int curRow = 1; curRow <= rows - 2; curRow++){
            for(int curCol = 1; curCol <= columns - 2; curCol++){
                //new world needs to be set to this value
                int myIndex = curRow * columns + curCol;

                int aliveNeighbors =
                (old_world[(curRow - 1) * columns + curCol] == '*') + //Up
                (old_world[curRow * columns + (curCol + 1)] == '*') + //right
                (old_world[curRow * columns + (curCol - 1)] == '*') + //left
                (old_world[(curRow + 1) * columns + curCol] == '*') + //down
                (old_world[(curRow - 1) * columns + (curCol + 1)] == '*') + //up right
                (old_world[(curRow - 1) * columns + (curCol - 1)] == '*') + //up left
                (old_world[(curRow + 1) * columns + (curCol + 1)] == '*') + //down right
                (old_world[(curRow + 1) * columns + (curCol - 1)] == '*'); //down left

                if(old_world[myIndex] == '*' && aliveNeighbors > 1 && aliveNeighbors < 4){
                    new_world[myIndex] = '*';
                }
                else if(old_world[myIndex] == '.' && aliveNeighbors == 3){
                    new_world[myIndex] = '*';
                }
                else{
                    new_world[myIndex] = '.';
                }
             
            }
        }
    }

  

#endif