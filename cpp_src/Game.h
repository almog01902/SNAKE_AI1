#ifndef Game_H
#define Game_H

#include <iostream>
#include <vector>
#include <utility> // For std::pair
#include <conio.h> // For _getch()
#include <windows.h> // For Sleep()
#include "StructsAndConstants.h" // Include the header file for constants and structs
#include "Snake.h" // Include the snake class header
#include "Grid.h" // Include the grid class header
using namespace std;

class Game
{
    public:
    int score;
    int state; // 0: menu, 1: game, 2: game over
    int foodEaten;
    bool isAI;
    Snake snake;
    Grid grid;

    Game(int gridRows, int gridCols, int startX, int startY, int initialLength);

        //---game functions---
    
    void initilizeGrid(); // Initialize the grid and place the snake and food to start the game
    bool isFoodEaten(); // Check if food is eaten
    bool isGameOver(); // Check if the game is over
    bool isGameWon(); // Check if the game is won
    

        //---player game functions---
    
    void inputHandler(); // Handle user input for snake direction
    void playerGame(); // Start player game mode
    void update(); // Update the game state


        //---AI game functions---
    float GetDistanceToFoodX(); // Get distance to food in x direction
    float GetDistanceToFoodY(); // Get distance to food in y direction
    float getDistanceForward(); // Get distance to danger ahead
    float getDistanceLeft(); // Get distance to danger on the left
    float getDistanceRight(); // Get distance to danger on the right
    void AIInputHandler(int action); // Handle AI input based on action
    stepResult step(int action); // Perform a game step based on the action
    int testRandom();

    //---debug functions---
    void showStats(); // Show game stats (score, distances, etc.)
    void render();
};

#endif // Game_H