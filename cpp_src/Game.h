#ifndef Game_H
#define Game_H

#include <iostream>
#include <vector>
#include <utility> // For std::pair
#include <conio.h> // For _getch()
#include <windows.h> // For Sleep()
#include <tuple>
#include <queue>
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
    float minDistTOFood;
    int stepsSinceLastFood =0;
    float lastStepRatio = 1.0f;
    int startX;
    int startY;
    int initialLength;
    std::vector<int> _bfsVisited;      // במקום bool, נשתמש ב-int לדורות
    std::vector<int> _timeToFreeCache; // עבור getExitPoint
    std::queue<std::pair<int, int>> _bfsQueue;
    
    int _bfsGeneration; // מונה שרץ קדימה, חוסך איפוס מערך
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
    void AIInputHandler(int action); // Handle AI input based on action
    stepResult step(int action); // Perform a game step based on the action
    pair<float,int> getDistanceInDirection(int dr,int dc);
    void fillRadar(stepResult& result);//function that fill the reader of the ai
    float calculateManhattanDistance();
    void fillAIState(stepResult& result);//function that fill all the ai state 
    float predictFutureSpace(int startX, int startY);//function that show how much space we have to move in chosen dir
    BFSResult calculateAccessibleSpace();//function that show hoe mucw space we have to move in all directions
    void getTailDir(float& outX,float& outY);//get the distance between the head and the tail
    void getAcssesibleSpace(stepResult& result);//fill the data result for acessible (N S W E) space in result
    void fillSafeDeadState(stepResult& result);
    int bodySegmentsInArea(pair<int,int> head);
    bool canReachTail();
    int getOccupiedNeighbors();
    ExitInfo getExitPoint();


    //---debug functions---
    void showStats(); // Show game stats (score, distances, etc.)
    void render();

    //get functions
    vector<vector<int>> getGrid();
};



#endif // Game_H