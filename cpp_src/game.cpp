#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include "StructsAndConstants.h" // Include the header file for constants and structs
using namespace std;







class Snake {
    public:
    vector<pair<int,int>> body; // Snake body represented as a list of coordinates
    int direction;// Current direction of the snake
    int length;// Length of the snake
    Snake(int startX,int startY,int initialLength) : direction(RIGHT), length(initialLength)
    {
        for(int i=0; i< initialLength;i++)
        {
            body.push_back(make_pair(startX, startY - i)); // Initialize snake vertically
        }
    } 
    void move()// Move the snake in the current direction
    {
        pair<int,int> head = body.front();
        switch(direction) {
            case UP:
                head.first--;
                break;
            case RIGHT:
                head.second++;
                break;
            case DOWN:
                head.first++;
                break;
            case LEFT:
                head.second--;
                break;
        }
        for(int i = body.size() - 1; i > 0; i--) {
            body[i] = body[i - 1]; // Move the body segments
        }
        body[0] = head; // Update the head position
    }
    void changeDirection(int newDirection) {
        // Prevent the snake from reversing direction
        if ((direction == UP && newDirection != DOWN) ||
            (direction == DOWN && newDirection != UP) ||
            (direction == LEFT && newDirection != RIGHT) ||
            (direction == RIGHT && newDirection != LEFT)) {
            direction = newDirection;
        }
    }
    void grow() {
        length++;
        body.push_back(body.back()); // Add a new segment at the tail
    }

};

class Grid
{
    public:
    int rows;
    int cols;// Number of rows and columns in the grid
    pair<int,int> foodPosition;// Position of the food
    vector<vector<int>> cells;
    Grid(int rows, int cols) : rows(rows), cols(cols)
    {
        cells = vector<vector<int>>(rows, vector<int>(cols, 0));
    }
    void draw() {// Draw the grid
        for(int i = 0; i <rows;i++)
        {
            for(int j =0; j< cols;j++)
            {
                if(cells[i][j] == EMPTY)
                    cout << ".";
                else if(cells[i][j] == SNAKE)
                    cout << "S"; // Snake
                else if(cells[i][j] == FOOD)
                    cout << "F"; // Food
                else
                    cout << "X"; // Obstacle
            }
            cout << endl;
        }
    }
    void reset()// Reset the grid to empty state
    {
        for(int i = 0; i <rows;i++)
        {
            for(int j =0; j< cols;j++)
            {
                cells[i][j] = 0;
            }
        }
    }
    void placeSnake(const Snake& snake) {// Place the snake on the grid
        for (const auto& segment : snake.body) {
            cells[segment.first][segment.second] = SNAKE;
        }
    }
    void placeFood()// Place food in a random empty cell
    {
        int x = rand() % rows;
        int y = rand() % cols;
        while (cells[x][y] != EMPTY) { // Ensure food is placed in an empty cell
            x = rand() % rows;
            y = rand() % cols;
        }
        foodPosition = make_pair(x, y);
        cells[x][y] = FOOD; // Place food

    }
    void restoreFood()// Restore food position after clearing the grid
    {
        cells[foodPosition.first][foodPosition.second] = FOOD; // Restore food
    }
};



class Game
{
    public:
    int score;
    int state; // 0: menu, 1: game, 2: game over
    int foodEaten;
    bool render;
    bool isAI;
    Snake snake;
    Grid grid;
    Game(int gridRows, int gridCols, int startX, int startY, int initialLength) 
        :state(MENU),snake(startX, startY, initialLength), grid(gridRows, gridCols),render(false),isAI(false) {}

        //---game functions---
    
    void initilizeGrid() {// Initialize the grid and place the snake and food to start the game
        grid.reset();
        grid.placeSnake(snake);
        grid.placeFood();
        grid.draw();
        score = 0;
        foodEaten = 0;
        state = PLAYING;
    }

    
    void toggleRender() {// Toggle rendering on or off
        if(_kbhit()){
           char input = _getch();
        if (input == 'r' || input == 'R') // Check if 'r' or 'R' is pressed 
        render = !render; // Toggle rendering
        }
    }
    bool isFoodEaten()// Check if food is eaten
    {
        return grid.cells[snake.body.front().first][snake.body.front().second] == FOOD;
    }

    bool isGameOver()// Check if the game is over
    {
        pair<int,int> head = snake.body.front();
        // Check if the snake collides with itself or the walls
        if (head.first < 0 || head.first >= grid.rows || head.second < 0 || head.second >= grid.cols) {
            return true; // Collision with walls
        }
        for (size_t i = 1; i < snake.body.size(); i++) {
            if (snake.body[i] == head) {
                return true; // Collision with itself
            }
        }
        return false; // No collision
    }

    bool isGameWon()// Check if the game is won
    {
        for(int i = 0; i < grid.rows; i++) {
            for(int j = 0; j < grid.cols; j++) {
                if(grid.cells[i][j] != SNAKE) {
                    return false; // If there's any empty cell, the game is not won
                }
            }
        }
        return true; // All cells are occupied by the snake
    }

    //--template functions for player and AI game functions--
float getDistanceForward()// Get distance to danger ahead
    {
        pair<int,int> head = snake.body.front();
        int dr=0, dc=0;
        switch(snake.direction) {// Check the direction of the snake
            case UP:
                dr = -1; // Move up
                break;
            case RIGHT:
                dc = 1; // Move right
                break;
            case DOWN:
                dr = 1; // Move down
                break;
            case LEFT:
                dc = -1; // Move left
                break;
        }
        float dist = 1.0f;
        int r = head.first + dr; // Calculate new row
        int c = head.second + dc; // Calculate new column
        while(r>=0 && r < grid.rows && c >= 0 && c < grid.cols) {
            if(grid.cells[r][c] == OBSTACLE || grid.cells[r][c] == SNAKE) {
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return dist / float((dr!= 0) ? grid.rows : grid.cols); // Normalize distance
    }
    float getDistanceLeft()// Get distance to danger on the left
    {
        pair<int,int> head = snake.body.front();
        int dr=0, dc=0;
        switch(snake.direction) {
            case UP:
                dc = -1; // Move left
                break;
            case RIGHT:
                dr = -1; // Move down
                break;
            case DOWN:
                dc = 1; // Move right
                break;
            case LEFT:
                dr = 1; // Move up
                break;
        }
        float dist = 1.0f;
        int r = head.first + dr; // Calculate new row
        int c = head.second + dc; // Calculate new column
        while(r>=0 && r < grid.rows && c >= 0 && c < grid.cols) {
            if(grid.cells[r][c] == OBSTACLE || grid.cells[r][c] == SNAKE) {
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return dist / float((dr!= 0) ? grid.rows : grid.cols); // Normalize distance
    }
    float getDistanceRight()// Get distance to danger on the right
    {
        pair<int,int> head = snake.body.front();
        int dr=0, dc=0;
        switch(snake.direction) {
            case UP:
                dc = 1; // Move right
                break;
            case RIGHT:
                dr = 1; // Move down
                break;
            case DOWN:
                dc = -1; // Move left
                break;
            case LEFT:
                dr = -1; // Move  up
                break;
        }
        float dist = 1.0f;
        int r = head.first + dr; // Calculate new row
        int c = head.second + dc; // Calculate new column
        while(r>=0 && r < grid.rows && c >= 0 && c < grid.cols) {
            if(grid.cells[r][c] == OBSTACLE || grid.cells[r][c] == SNAKE) {
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return dist / float((dr!= 0) ? grid.rows : grid.cols); // Normalize distance
    }

    float GetDistanceToFoodX()// Get distance to food in x direction
    {
        return float(snake.body.front().first - grid.foodPosition.first) / float(grid.cols);
    }
    float GetDistanceToFoodY()// Get distance to food in y direction
    {
        return float(snake.body.front().second - grid.foodPosition.second) / float(grid.rows);
    }
    //---player game functions---

    
    void inputHandler() {
        if(_kbhit()){
            char input = _getch(); // Get user input
            switch(input) {
                case 'w':
                    snake.changeDirection(UP);
                    break;
                case 'd':
                    snake.changeDirection(RIGHT);
                    break;
                case 's':
                    snake.changeDirection(DOWN);
                    break;
                case 'a':
                    snake.changeDirection(LEFT);
                    break;
            }
        }
    }

    void update()
    {

        snake.move(); // Move the snake

        if (isGameOver()) {
            state = GAMEOVER; // Change state to game over
            return; // Exit update function
        }
        if(isGameWon()) {
            state = GAMEWON; // Change state to game won
            return; // Exit update function
        }

        if (isFoodEaten()) { // Check if food is eaten
            snake.grow(); // Grow the snake
            score += 10; // Increase score
            foodEaten++;
            grid.placeFood(); // Place new food
        }

        
        grid.reset(); // Reset the grid
        grid.restoreFood(); // Restore food position
        grid.placeSnake(snake); // Place the snake on the grid
    }


    void playerGame()
    {
        if(state == MENU)
        {
            cout << "Welcome to Snake Game!" << endl;
            cout << "Press any key to start..." << endl;
            _getch(); // Wait for user input
            state = PLAYING; // Change state to playing
        }
        initilizeGrid();// Initialize the grid and place the snake and food to start the game
        while(state == PLAYING)
        {
            system("cls"); // Clear the console
            cout << "Score: " << score << endl;// Display the score
            cout<< "Distance to food (X): " << GetDistanceToFoodX() << endl; // Display distance to food in x direction
            cout<< "Distance to food (Y): " << GetDistanceToFoodY() << endl;
            cout << "Distance to danger forward: " << getDistanceForward() << endl; // Display distance to danger ahead
            cout << "Distance to danger left: " << getDistanceLeft() << endl; // Display distance to danger on the left
            cout << "Distance to danger right: " << getDistanceRight() << endl; //
            
            inputHandler();// Handle user input for snake direction
            update();// Update the game state

            grid.draw(); // Draw the grid
            
            Sleep(SNAKE_SPEED); // Sleep for 200 milliseconds

        }
        if(state == GAMEOVER)
        {
            
            cout << "Game Over! Your score: " << score << endl;
            cout << "Press any key to exit..." << endl;
            _getch(); // Wait for user input
            exit(0); // Exit the game
        }
        else if(state == GAMEWON)
        {
            cout << "Congratulations! You won the game!" << endl;
                cout << "Press any key to exit..." << endl;
                _getch(); // Wait for user input
                exit(0); // Exit the game
        }
    }
    //---AI game functions---

    
    
    void AIInputHandler(int action)// Handle AI input based on action
    {
        switch(action) {
            case 0: // UP
                snake.changeDirection(UP);
                break;
            case 1: // RIGHT
                snake.changeDirection(RIGHT);
                break;
            case 2: // DOWN
                snake.changeDirection(DOWN);
                break;
            case 3: // LEFT
                snake.changeDirection(LEFT);
                break;
        }
    }

    

    stepResult step(int action)
    {
        stepResult result;
        result.reward = 0.0f; // Initialize reward
        result.done = false; // Initialize done state
        result.direction = snake.direction; // Initialize direction

        grid.reset(); // Reset the grid
        grid.placeSnake(snake); // Place the snake on the grid
        AIInputHandler(action); // Handle AI input based on action
        snake.move(); // Move the snake
        
        
            if(isGameOver()) {
                result.done = true; // Set done state to true if game is over
                result.reward += -100.0f; // Negative reward for game over
                state = GAMEOVER; // Change state to game over
            }
            if(isGameWon()) {
                result.done = true; // Set done state to true if game is won
                result.reward += 1000.0f; // Positive reward for game won
                state = GAMEWON; // Change state to game won
            }
            result.distFoodX = GetDistanceToFoodX(); // Get distance to food in x direction
            result.distFoodY = GetDistanceToFoodY(); // Get distance to food in y
            result.distToDangerForward = getDistanceForward();
            result.distToDangerLeft = getDistanceLeft();
            result.distToDangerRight = getDistanceRight();
            if(isFoodEaten()) {
                snake.grow(); // Grow the snake if food is eaten
                score += 10; // Increase score
                foodEaten++;
                grid.placeFood(); // Place new food
                result.reward += 10.0f; // Positive reward for eating food
            }
            else
            {
                result.reward += -0.01f; // Negative reward for not eating food
            }
            
            grid.reset(); // Reset the grid
            grid.restoreFood(); // Restore food position
            grid.placeSnake(snake); // Place the snake on the grid
            
            if(render) {
                system("cls"); // Clear the console
                cout << "Score: " << score << endl; // Display the score
                cout << "Distance to food (X): " << result.distFoodX << endl; // Display distance to food in x direction
                cout << "Distance to food (Y): " << result.distFoodY << endl;
                cout << "Distance to danger forward: " << result.distToDangerForward << endl; // Display distance to danger ahead
                cout << "Distance to danger left: " << result.distToDangerLeft << endl;
                cout << "Distance to danger right: " << result.distToDangerRight << endl; // Display distance to danger on the right
                cout << "Current direction: " << result.direction << endl; // Display current direction of the snake
                Sleep(SNAKE_SPEED); // Sleep for snake speed
                grid.draw(); // Draw the grid
                
            }
            return result; // Return the step result
            
        
    }
    
    void run()
    {
        
        playerGame(); // Start the player game
        
    }
        
    
};



int main()
{
    Game game(GRID_SIZE, GRID_SIZE, GRID_SIZE / 2, GRID_SIZE / 2, INITIAL_SNAKE_LENGTH);
    game.initilizeGrid(); // Start the game (remove later we dont need to draw the grid at the start for AI)

    //make option for player or ai after the model is finished
    game.state = PLAYING; // Set game state to playing
    int count =0;
    while (game.state == PLAYING)
    {
        game.toggleRender(); // Toggle rendering on or off
        count++;

        int action = count % 4; // Example action for AI (0: UP, 1: RIGHT, 2: DOWN, 3: LEFT)
        stepResult result = game.step(action); // Get step result


        //return result to model
    }

    return 0; // Exit the program

}