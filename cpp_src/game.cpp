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
        minDistTOFood = fabs(GetDistanceToFoodX()) + fabs(GetDistanceToFoodY());
    }

    vector<vector<int>> Game::getGrid()
    {
        return grid.cells;
    }

    bool Game::isFoodEaten()// Check if food is eaten
    {
        return snake.body.front() == grid.foodPosition; // Check if the snake's head is at the food position
    }

    bool Game::isGameOver()// Check if the game is over
    {
        pair<int,int> head = snake.body.front();
        // Check if the snake collides with itself or the walls
        if (head.first < 0 || head.first >= grid.cols || head.second < 0 || head.second >= grid.rows) {
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

    pair<float, int> Game::getDistanceInDirection(int dr, int dc) {
    pair<int, int> head = snake.body.front();
    float dist = 0.0f;
    int r = head.first + dr;
    int c = head.second + dc;
    int obstacle = EMPTY;

    //chack where when we hit an obstacle or the body
    while (r >= 0 && r < grid.rows && c >= 0 && c < grid.cols) {
        if (grid.cells[r][c] == SNAKE || grid.cells[r][c] == OBSTACLE) {
            obstacle = grid.cells[r][c];
            break;
        }
        dist += 1.0f;
        r += dr;
        c += dc;
    }

    // normalized
    float max_dim = (float)max(grid.rows, grid.cols);
    return make_pair(dist / max_dim, obstacle);
}

    void Game::fillRadar(stepResult& result) {//function that fill the reader of the ai

    int dr[] = {-1,  1,  0,  0, -1, -1,  1,  1}; //the dirctions N S E W NW NE SW SE
    int dc[] = { 0,  0,  1, -1, -1,  1, -1,  1};
    
    float distances[8];

    for (int i = 0; i < 8; i++) {
        
        auto res = getDistanceInDirection(dr[i], dc[i]);
        distances[i] = res.first; 
    }

    //add the direction to the result directions
    result.distN  = distances[0];
    result.distS  = distances[1];
    result.distE  = distances[2];
    result.distW  = distances[3];
    result.distNW = distances[4];
    result.distNE = distances[5];
    result.distSW = distances[6];
    result.distSE = distances[7];
}
    

    stepResult Game::step(int action) {
        stepResult result;
        
        //1. initilize stats
        result.reward = 0.0f;
        result.done = false;
        result.won = false;
        result.snakeLen = snake.getSnakeLen();

        // 2. update game state
        AIInputHandler(action);
        result.direction = snake.direction;
        snake.move();

        // 3. chack the game state
        if (isGameOver()) {
            result.done = true;
            result.reward = -50.0f; 
            result.foodEaten = foodEaten; 
            result.snakeLen = snake.getSnakeLen(); 
            state = GAMEOVER;
            return result;
        }
        
        if (isGameWon()) {
            result.done = true;
            result.won = true;
            result.reward = 500.0f;
            result.foodEaten = foodEaten; 
            result.snakeLen = snake.getSnakeLen();
            state = GAMEWON;
            return result;
        }

        //4. give the ai the state
        fillAIState(result);

        // 5. rewards
        if (isFoodEaten()) {
            snake.grow();
            foodEaten++;
            grid.placeFood();
            result.reward = 150.0f;
            //calaculate min dist
            minDistTOFood = calculateManhattanDistance();
        } 
        else {
            // reaward or punish based on the ai disicisions  
            float currDist = calculateManhattanDistance();
            float delta = minDistTOFood - currDist;
            
            if (delta > 0) result.reward = 1.0f;  //getting close to food
            else if (delta < 0) result.reward = -1.5f; // getting away from food
            else result.reward = -0.1f; // just moving around
            
            minDistTOFood = currDist;
        }

        result.foodEaten = foodEaten;
        // 6. update the grid and return result
        grid.update(snake);
        return result;
    }
    
    void Game::fillAIState(stepResult& result) {
    // 1. normalized place for the head
    result.headX_norm = (float)snake.body.front().second / (float)grid.cols;
    result.headY_norm = (float)snake.body.front().first / (float)grid.rows;

    // 2. distance to food
    result.distFoodX = GetDistanceToFoodX();
    result.distFoodY = GetDistanceToFoodY();

    // 3. fill the rader
    fillRadar(result);


    // 4. fill percantege
    result.fillPercentage = (float)snake.getSnakeLen() / (float)(grid.rows * grid.cols);
}

float Game::calculateManhattanDistance() {
    return fabs(GetDistanceToFoodX()) + fabs(GetDistanceToFoodY());
}

    void Game::render()
    {
        system("cls"); // Clear the console
        showStats(); // Show game stats
        Sleep(SNAKE_SPEED); // Sleep for snake speed
        grid.draw(); // Draw the grid
    }
        
    