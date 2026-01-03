#include "Game.h"
#include <cmath> // For fabs function
#include <queue>



    
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
        return (snake.getSnakeLen() >= grid.rows * grid.cols);
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
    float max_dim = (float)max(grid.rows-1, grid.cols-1);
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
    

    stepResult Game::step(int action) 
    {
        stepResult result;
        
        //1. initilize stats
        result.reward = 0.0f;
        result.done = false;
        result.won = false;
        result.snakeLen = snake.getSnakeLen();
        auto oldHead = snake.body.front();
        int oldHeadY = oldHead.first;
        int oldHeadX = oldHead.second;
 
        // 2. update game state
        AIInputHandler(action);
        snake.move();
        stepsSinceLastFood++;

        //calculate the max steps for a snake in its length(we want the snake to bo optimal and will not make cirecles around the apple)
        int baseSteps = (grid.rows + grid.cols) * 2.5; 
        int dynamicMaxSteps = baseSteps + (snake.getSnakeLen() * 3);

        bool starved = stepsSinceLastFood>dynamicMaxSteps;
        // 3. chack the game state
        if (isGameOver()|| starved) {
            result.done = true;
            result.reward = -1000.0f; 
            result.foodEaten = foodEaten; 
            
            result.snakeLen = snake.getSnakeLen(); 
            fillSafeDeadState(result);
            state = GAMEOVER;
            return result;
        }

        //4. give the ai the state
        fillAIState(result);

        float timePressure = (float)stepsSinceLastFood / (float)dynamicMaxSteps;

        result.timePressure = timePressure;

        // 5. rewards
        if (isFoodEaten()) {
            snake.grow();
            foodEaten++;
            result.reward = 250.0f;
            stepsSinceLastFood =0;
            //calaculate min dist
            

                if (isGameWon()) {
                result.done = true;
                result.won = true;
                result.reward = 2000.0f;
                result.foodEaten = foodEaten; 
                result.snakeLen = snake.getSnakeLen();
                state = GAMEWON;
                return result;
                }
                else
                {
                    grid.placeFood();
                    minDistTOFood = calculateManhattanDistance();
                }
        } 
        else {

            //1.punishment for each time the snake doesnt eat
            result.reward = -0.1f * (1.0f + timePressure);
            //2. reaward on getting close to the apple
            float currDist = calculateManhattanDistance();
            float delta = minDistTOFood - currDist;
            
            if (delta > 0){

                result.reward+=1.0f;
                minDistTOFood = currDist;  
            } 

            float spaceGap = 1.0f - result.accessibleSpace;
            float spacePenalty = 0.0f;

            // נעניש רק אם השטח הנגיש באמת נמוך (מתחת ל-40%)
            // שימוש בחזקה שלישית (pow 3) הופך את העונש לסלחני מאוד בחצי לוח וקטלני בסוף
            if (result.accessibleSpace < 0.45f) {

                auto newHead = snake.body.front();
                auto tail = snake.body.back(); // המיקום הנוכחי של הזנב

                // חישוב מרחקים (Manhattan Distance)
                int oldDistToTail = abs(oldHeadX - (int)tail.second) + abs(oldHeadY - (int)tail.first);
                int newDistToTail = abs((int)newHead.second - (int)tail.second) + abs((int)newHead.first - (int)tail.first);
                // הוספת הבוליאן החסר: האם הצעד קירב אותנו לזנב?
                bool movingToTail = (newDistToTail < oldDistToTail);

                float gridTotalCells = (float)(grid.rows * grid.cols);
                float freeSquares = result.accessibleSpace * gridTotalCells;
                
                // בדיקת "מרחב הישרדות": האם יש מספיק מקום לזגזג עד שהזנב יתפנה?
                // (distToTail הוא כמות הצעדים המינימלית שהזנב צריך לעשות)
                bool hasSurvivalSpace = (freeSquares > (newDistToTail + 2));

                float spaceGap = 1.0f - result.accessibleSpace;
                float spacePenalty = pow(spaceGap, 3) * 20.0f;

                if (movingToTail && hasSurvivalSpace) {
                    // המצב האידיאלי: הולך לזנב ויש לו "חמצן" לזגזג
                    spacePenalty *= 0.05f; 
                } 
                else if (!movingToTail && hasSurvivalSpace) {
                    // הולך לזנב אבל החלל נהיה מסוכן מדי (חור קטן)
                    spacePenalty *= 0.5f; 
                }
                else if(movingToTail && !hasSurvivalSpace)
                {
                    //we keep this the same way for now becuse he will probably wont survive
                }
                else
                {
                    //worst situation
                    spacePenalty*=1.2f;
                }

                result.reward -= spacePenalty;
            }

        }

        result.foodEaten = foodEaten;
        // 6. update the grid and return result
        grid.update(snake);
        return result;
    }
    
    void Game::fillAIState(stepResult& result) {
    // 1. normalized place for the head
    result.headX_norm = (float)snake.body.front().second / (float)(grid.cols - 1);
    result.headY_norm = (float)snake.body.front().first / (float)(grid.rows - 1);

    // 2. distance to food
    result.distFoodX = (GetDistanceToFoodX()+1.0f)/2.0f;
    result.distFoodY = (GetDistanceToFoodY()+1.0f)/2.0f;

    // 3. fill the rader
    fillRadar(result);

    //4. one hot direction
    result.isUp    = (snake.direction == UP)    ? 1.0f : 0.0f;
    result.isDown  = (snake.direction == DOWN)  ? 1.0f : 0.0f;
    result.isLeft  = (snake.direction == LEFT)  ? 1.0f : 0.0f;
    result.isRight = (snake.direction == RIGHT) ? 1.0f : 0.0f;

    // 5. fill percantege
    result.fillPercentage = (float)snake.getSnakeLen() / (float)(grid.rows * grid.cols);
    // 6. tails dir feom player
    getTailDir(result.diffX,result.diffy);
    // 7. acssesible space
    getAcssesibleSpace(result);
}

    void Game::getAcssesibleSpace(stepResult& result)
    {

        int headX = snake.body.front().second;
        int headY = snake.body.front().first;
        //fill N S E W
        result.accessibleSpaceN = getAccesibleSpaceInDir(headX, headY - 1); // North
        result.accessibleSpaceS = getAccesibleSpaceInDir(headX, headY + 1); // South
        result.accessibleSpaceE = getAccesibleSpaceInDir(headX + 1, headY); // East
        result.accessibleSpaceW = getAccesibleSpaceInDir(headX - 1, headY); // West

        //all the sum of space in each dir is equel to the sum in each firaction
        result.accessibleSpace = calculateAccessibleSpace();
    }


    float Game::calculateAccessibleSpace() {

    //  y הוא rows, x הוא cols

    int startX = snake.body.front().second; // Column
    int startY = snake.body.front().first;  // Row

   

    int cols = grid.cols;
    int rows = grid.rows;



    std::vector<bool> visited(rows * cols, false);
    std::queue<std::pair<int, int>> q;

    q.push({startX, startY});
    visited[startY * cols + startX] = true;
    int reachableCount = 0;
    while (!q.empty()) {

        std::pair<int, int> curr = q.front();
        q.pop();

        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        for (int i = 0; i < 4; i++) {

        
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            //chacking borders

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                int index = ny * cols + nx;

                //cells[row][col] -> [ny][nx]
                if (!visited[index] && grid.cells[ny][nx] != SNAKE) {
                    visited[index] = true;
                    q.push({nx, ny});
                    reachableCount++;
                }

            }

        }

    }

    // calculate all empty spaces
    float totalEmpty = (float)(grid.rows * grid.cols) - snake.getSnakeLen() -1;//-1 for apple
    if (totalEmpty == 0) return 1.0f;

    //normalized return
    return (float)reachableCount / (float)totalEmpty;

}

    float Game::getAccesibleSpaceInDir(int startX, int startY) {

    if (startX < 0 || startX >= grid.cols || startY < 0 || startY >= grid.rows) return 0;
    

    if (grid.cells[startY][startX] == SNAKE) return 0;

    std::vector<bool> visited(grid.rows * grid.cols, false);
    std::queue<std::pair<int, int>> q;

    q.push({startX, startY});
    visited[startY * grid.cols + startX] = true;
    int count = 0;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!q.empty()) {
        std::pair<int, int> curr = q.front();
        q.pop();
        count++;

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            if (nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int index = ny * grid.cols + nx;
                if (!visited[index] && grid.cells[ny][nx] != SNAKE) {
                    visited[index] = true;
                    q.push({nx, ny});
                }
            }
        }
    }

    // calculate all empty spaces
    float totalEmpty = (float)(grid.rows * grid.cols) - snake.getSnakeLen() -1;//-1 for apple
    
    return (float)count / totalEmpty;
}


void Game::getTailDir(float& outX, float& outY) {

    auto head = snake.body.front();
    auto tail = snake.body.back();

    //distance
    float diffY = (float)tail.first - (float)head.first;
    float diffX = (float)tail.second - (float)head.second;

    
    outY = diffY / (float)grid.rows;
    outX = diffX / (float)grid.cols;
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
    

    void Game::fillSafeDeadState(stepResult& result) {
    // איפוס מיקומי ראש ומרחק אוכל
    result.headX_norm = 0.0f;
    result.headY_norm = 0.0f;
    result.distFoodX  = 0.0f;
    result.distFoodY  = 0.0f;

    // איפוס הראדאר (8 כיוונים)
    result.distN = 0.0f; result.distS = 0.0f; result.distE = 0.0f; result.distW = 0.0f;
    result.distNW = 0.0f; result.distNE = 0.0f; result.distSW = 0.0f; result.distSE = 0.0f;

    // איפוס כיוון נוכחי
    result.isUp = 0.0f; result.isDown = 0.0f; result.isLeft = 0.0f; result.isRight = 0.0f;

    // איפוס מדדי שטח (הכי חשוב כדי למנוע קריסה של Flood Fill)
    result.accessibleSpace = 0.0f;
    result.accessibleSpaceN = 0.0f;
    result.accessibleSpaceS = 0.0f;
    result.accessibleSpaceE = 0.0f;
    result.accessibleSpaceW = 0.0f;

    // איפוס נתונים נוספים
    result.fillPercentage = 0.0f;
    result.timePressure = 1.0f; // במוות מרעב הלחץ הוא מקסימלי
    result.diffX = 0.0f;
    result.diffy = 0.0f;
}
    