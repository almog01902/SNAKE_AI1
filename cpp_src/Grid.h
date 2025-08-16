#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <vector>
#include <utility> // For std::pair
#include "Snake.h" // Include the snake class header
#include "StructsAndConstants.h" // Include the header file for constants and structs
using namespace std;

class Grid
{
    public:
    int rows;
    int cols;// Number of rows and columns in the grid
    pair<int,int> foodPosition;// Position of the food
    vector<vector<int>> cells;
    Grid(int rows,int cols);
    void draw(); // Draw the grid
    void reset(); // Reset the grid to empty state
    void update(const Snake& snake); // Reset the grid to empty state
    void placeSnake(const Snake& snake); // Place the snake on the grid
    void placeFood(); // Place food in a random empty cell

};



#endif // "GRID_H"