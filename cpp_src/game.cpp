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
        foodEaten =0;
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

    //--template functions for player and AI game functions--
pair<float,int> Game::getDistanceForward()// Get distance to danger ahead
    {
        int obstacle = EMPTY;
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
                obstacle = grid.cells[r][c];
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return make_pair( dist / float((dr!= 0) ? grid.rows : grid.cols),obstacle); // Normalize distance
    }
    pair<float,int> Game::getDistanceLeft()// Get distance to danger on the left
    {
        int obstacle = EMPTY;
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
                obstacle = grid.cells[r][c];
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return make_pair(dist / float((dr!= 0) ? grid.rows : grid.cols),obstacle); // Normalize distance
    }
    pair<float,int> Game::getDistanceRight()// Get distance to danger on the right
    {
        int obstacle = EMPTY;
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
                obstacle = grid.cells[r][c];
                break; // Stop if there's an obstacle or snake
            }
            dist++; // Increase distance
            r += dr; // Move in the direction of the snake
            c += dc; // Move in the direction of the snake
        }
        return make_pair( dist / float((dr!= 0) ? grid.rows : grid.cols),obstacle); // Normalize distance
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
        cout << "Distance to danger forward: " << getDistanceForward().first << endl; // Display distance to danger ahead
        cout << "Distance to danger left: " << getDistanceLeft().first << endl; // Display distance to danger on the left
        cout << "Distance to danger right: " << getDistanceRight().first << endl; // Display distance to danger on the right
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

    

    stepResult Game::step(int action)
    {
        float lastDangerForward = 0.0f;
        stepResult result;
        result.reward = 0.0f;
        result.done = false;
        result.won = false;
        int snakeLen = snake.getSnakeLen();
        result.snakeLen = snakeLen;
        

        AIInputHandler(action);//get the ai imput
        result.direction = snake.direction;
        snake.move();
        


        if(isGameOver()) {
            result.done = true;
            result.reward = -5000.0f;
            state = GAMEOVER;
            result.foodEaten = foodEaten;
            return result; 
        }
        if(isGameWon()) {
            result.done = true;
            result.won = true;
            result.foodEaten = foodEaten;
            state = GAMEWON;
            return result;  
        }

        //get food distance
        float distFoodX = GetDistanceToFoodX();
        float distFoodY = GetDistanceToFoodY();
        float currDist = fabs(distFoodX) + fabs(distFoodY);

        result.distFoodX = distFoodX;
        result.distFoodY = distFoodY;

        //get danger dist
        auto [distanceToDangerForward,obsForward] = getDistanceForward();
        auto [distanceToDangerLeft,obsLeft] = getDistanceLeft();
        auto [distanceToDangerRight,obsRight] = getDistanceRight();
        result.distToDangerForward = distanceToDangerForward;
        result.distToDangerLeft = distanceToDangerLeft;
        result.distToDangerRight = distanceToDangerRight;

        if(isFoodEaten()) {
            snake.grow();        
            foodEaten++;     
            grid.placeFood();  
            result.reward = 150.0f;  
            minDistTOFood = fabs(GetDistanceToFoodX()) + fabs(GetDistanceToFoodY());
        }
        else 
        {
            float delta = minDistTOFood - currDist;


            bool correctDir = false;
            switch(snake.direction) {
                case RIGHT: correctDir = (distFoodX < 0); break; // צריך להיות שלילי כי foodX > snakeX
                case LEFT:  correctDir = (distFoodX > 0); break; // צריך להיות חיובי
                case DOWN:  correctDir = (distFoodY < 0); break; // foodY > snakeY -> למטה
                case UP:    correctDir = (distFoodY > 0); break; // foodY < snakeY -> למעלה
            }

            // חישוב reward
            if (delta > 0) {//snake get closer
                
                result.reward = correctDir ? delta * 5.0f : delta * 3.0f;
                // עדכון המרחק המינימלי
                minDistTOFood = currDist;
            } 
            else if (delta < 0) { //if sanke dosent get closer to food
                
                result.reward = -fabs(delta) * 5.0f;
            } 
            else {
                //zig-zag
                result.reward = -1.0f;
            }

            
        }

        /*
        if(getDistanceForward().second == SNAKE && 
        distanceToDangerLeft == 0.05 && obsLeft == SNAKE
        && distanceToDangerRight == 0.05 &&  obsRight == SNAKE 
        && snakeLen <=100)
        {
            result.reward -=10.0f;//dead end
        }
        */
        grid.update(snake);//update only after chacking if game over 

        return result;
    }
    

    void Game::render()
    {
        system("cls"); // Clear the console
        showStats(); // Show game stats
        Sleep(SNAKE_SPEED); // Sleep for snake speed
        grid.draw(); // Draw the grid
    }
        
    