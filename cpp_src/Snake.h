#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <utility> // For std::pair
#include "StructsAndConstants.h" // Include the header file for constants and structs
using namespace std;

class Snake {
    public:
    vector<pair<int,int>> body; // Snake body represented as a list of coordinates
    int direction; // Current direction of the snake
    int length; // Length of the snake
    Snake(int startX, int startY, int initialLength);
    void move(); // Move the snake in the current direction
    void changeDirection(int newDirection); // Change the direction of the snake
    void grow(); // Grow the snake by adding a new segment at the tail
};

#endif // SNAKE_H