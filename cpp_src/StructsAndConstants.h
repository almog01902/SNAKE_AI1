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
    float reward; // Reward for the current step
    bool done; // Whether the game is over
    float distFoodX;// Distance to food in x direction
    float distFoodY; // Distance to food in y direction
    float distToDangerForward; // Whether there is danger ahead
    float distToDangerLeft;// Whether there is danger to the left
    float distToDangerRight; // Whether there is danger to the left or right
    int direction; // Current direction of the snake
    int foodEaten; //number of food eaten
    bool won;
    int snakeLen;

};


#endif // STRUCTS_AND_CONSTANTS_H