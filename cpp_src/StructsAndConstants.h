#ifndef STRUCTS_AND_CONSTANTS_H
#define STRUCTS_AND_CONSTANTS_H
// Constants for directions and game states and grid elements
enum Direction { UP, RIGHT, DOWN, LEFT };// Directions for the snake movement
enum GameState { MENU, PLAYING, GAMEOVER, GAMEWON };// Game states for the game
enum cellType { EMPTY, SNAKE, FOOD, OBSTACLE };// Cell types for the grid
const int GRID_SIZE = 20; // Size of the grid
const int  INITIAL_SNAKE_LENGTH =3; // Initial length of the snake
const int SNAKE_SPEED = 100; // Speed of the snake in milliseconds


struct stepResult//for ai agent
{
    //where the head is on the board(normalized)
    float headX_norm;
    float headY_norm;

    //distance to food (normalized)
    float distFoodX;
    float distFoodY;
    
    //rader (the distance to to obstacle in all 8 direction(normalized))
    float distN,  distS,  distE,  distW;   
    float distNE, distNW, distSE, distSW;  

    float fillPercentage;//how much the board filled

    float isUp , isDown,isLeft,isRight; //current direction of the snake

    float accessibleSpace;//how much space can the snake move through

    float diffX , diffy; //vector from the head to tail normalized

    float timePressure;

    float accessibleSpaceN, accessibleSpaceS,accessibleSpaceE,accessibleSpaceW;

    

    //Metadata
    float reward; // Reward for the current step
    bool done; // Whether the game is over
    bool won;//whether the game won
    int snakeLen;//current snake Length
    int foodEaten; //number of food eaten
};


#endif // STRUCTS_AND_CONSTANTS_H