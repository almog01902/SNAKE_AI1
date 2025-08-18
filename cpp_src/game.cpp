#include "Game.h"
#include <cmath> // For fabs function



    
    Game::Game(int gridRows, int gridCols, int startX, int startY, int initialLength) 
        :state(MENU),snake(startX, startY, initialLength), grid(gridRows, gridCols),isAI(false) {}

        //---game functions---
    
    void Game::initilizeGrid() {// Initialize the grid and place the snake and food to start the game
        grid.reset();
        grid.placeFood();
        grid.update(snake); // Update the grid with the snake's position and food
        score = 0;
        foodEaten = 0;
        state = PLAYING;
    }

    bool Game::isFoodEaten()// Check if food is eaten
    {
        return snake.body.front() == grid.foodPosition; // Check if the snake's head is at the food position
    }

    bool Game::isGameOver()// Check if the game is over
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

    bool Game::isGameWon()// Check if the game is won
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
float Game::getDistanceForward()// Get distance to danger ahead
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
    float Game::getDistanceLeft()// Get distance to danger on the left
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
    float Game::getDistanceRight()// Get distance to danger on the right
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

    float Game::GetDistanceToFoodX()// Get distance to food in x direction
    {
        return float(snake.body.front().second - grid.foodPosition.second) / float(grid.cols);
    }
    float Game::GetDistanceToFoodY()// Get distance to food in y direction
    {
        return float(snake.body.front().first - grid.foodPosition.first) / float(grid.rows);
    }
    //---player game functions---

    
    void Game::inputHandler() {
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

    void Game::update()
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
        grid.update(snake); // Update the grid with the snake's position and food
    }

    void Game::showStats() {
        cout << "Score: " << score << endl; // Display the score
        cout << "Distance to food (X): " << GetDistanceToFoodX() << endl; // Display distance to food in x direction
        cout << "Distance to food (Y): " << GetDistanceToFoodY() << endl;
        cout << "Distance to danger forward: " << getDistanceForward() << endl; // Display distance to danger ahead
        cout << "Distance to danger left: " << getDistanceLeft() << endl; // Display distance to danger on the left
        cout << "Distance to danger right: " << getDistanceRight() << endl; // Display distance to danger on the right
        cout << "Current direction: " << snake.direction << endl; // Display current direction of the snake
    }


    void Game::playerGame()
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
            showStats(); // Show game stats
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

    
    
    void Game::AIInputHandler(int action)// Handle AI input based on action
    {
        switch(action) {
            case UP: // UP
                snake.changeDirection(UP);
                break;
            case RIGHT: // RIGHT
                snake.changeDirection(RIGHT);
                break;
            case DOWN: // DOWN
                snake.changeDirection(DOWN);
                break;
            case LEFT: // LEFT
                snake.changeDirection(LEFT);
                break;
        }
    }

    

    stepResult Game::step(int action) // Perform a game step based on the action
    {
        stepResult result;
        result.reward = 0.0f; // Initialize reward
        result.done = false; // Initialize done state
        result.direction = snake.direction; // Initialize direction

        
        AIInputHandler(action); // Handle AI input based on action
        snake.move(); // Move the snake
        grid.update(snake); // Update the grid with the snake's position and food
        
        
        
            if(isGameOver()) {
                result.done = true; // Set done state to true if game is over
                result.reward = -100.0f; // Negative reward for game over
                state = GAMEOVER; // Change state to game over
            }
            if(isGameWon()) {
                result.done = true; // Set done state to true if game is won
                result.reward = 1000.0f; // Positive reward for game won
                state = GAMEWON; // Change state to game won
            }
            // Calculate distances and rewards
            float distFoodX = GetDistanceToFoodX(); // Get distance to food in x direction
            float distFoodY = GetDistanceToFoodY(); // Get distance to food in y direction
            result.distFoodX = distFoodX; // Get distance to food in x direction
            result.distFoodY = distFoodY; // Get distance to food in y
            result.distToDangerForward = getDistanceForward();
            result.distToDangerLeft = getDistanceLeft();
            result.distToDangerRight = getDistanceRight();
            if(isFoodEaten()) {
                snake.grow(); // Grow the snake if food is eaten
                score += 10; // Increase score
                foodEaten++;
                grid.placeFood();// Place new food
                result.reward = 10.0f; // Positive reward for eating food
            }
            else
            {
                result.reward = -(fabs(distFoodX) + fabs(distFoodY)); // Negative reward for not eating food
            }
            
            return result; // Return the step result
            
        
    }

    void Game::render()
    {
        system("cls"); // Clear the console
        showStats(); // Show game stats
        Sleep(SNAKE_SPEED); // Sleep for snake speed
        grid.draw(); // Draw the grid
    }
        
    