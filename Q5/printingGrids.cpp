/*Kayla Humphrey
02/22/26
A method of printing grids of cells for the game of life*/

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

void printGrid(int row, int column, vector<char> charVector, int generation);

int main(int argc, char** argv){
    int row = 20;
    int column = 20;
    int generation = 0;
    vector<char> charVector(row * column, '*');
    printGrid(row, column, charVector, generation);
}

void printGrid(int row, int column, vector<char> charVector, int generation){
    std::ofstream out("generation.txt");
    out << "Results of generation: " << generation << endl;
    for(int r = 0; r < row; r++){
        for(int c = 0; c < column; c++){
            out << charVector[r * column + c];
        }
        out << endl;
    }
    out.close();
}