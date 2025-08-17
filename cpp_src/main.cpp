#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include "StructsAndConstants.h" // Include the header file for constants and structs
#include "Snake.h" // Include the snake class header
#include "Grid.h" // Include the grid class header
#include "Game.h" // Include the game class header
using namespace std;

// Main function to start the game
void startGameForAI() {
    Game game(GRID_SIZE, GRID_SIZE, GRID_SIZE / 2, GRID_SIZE / 2, INITIAL_SNAKE_LENGTH);
    game.initilizeGrid(); // Start the game (remove later we dont need to draw the grid at the start for AI)

    //make option for player or ai after the model is finished
    game.state = PLAYING; // Set game state to playing
    int count = 0; // Initialize count for AI actions
    while (game.state == PLAYING) {
        game.render();
        count++;
        int action = count %4; // Example action for AI (0: UP, 1: RIGHT, 2: DOWN, 3: LEFT)
        stepResult result = game.step(action); // Get step result
    }
}

void startGameForPlayer() {
    Game game(GRID_SIZE, GRID_SIZE, GRID_SIZE / 2, GRID_SIZE / 2, INITIAL_SNAKE_LENGTH);
    game.playerGame(); // Start player game mode
}


int main()
{
    startGameForPlayer();

    return 0; // Exit the program

}

//g++ .\cpp_src\main.cpp .\cpp_src\snake.cpp .\cpp_src\game.cpp .\cpp_src\grid.cpp -o game.exe
// Compile command for Windows