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
        _bfsVisited.resize(grid.rows * grid.cols, 0);
        _timeToFreeCache.resize(grid.rows * grid.cols, -1);
        _bfsGeneration = 0;
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
    
    // 1. אתחול נתונים בסיסיים
    result.reward = 0.0f;
    result.done = false;
    result.won = false;
    result.snakeLen = snake.getSnakeLen();
    
    // 2. עדכון מצב המשחק
    AIInputHandler(action);
    snake.move();
    stepsSinceLastFood++;
    
    auto head = snake.body.front();
    bool hitWall = (head.first < 0 || head.first >= grid.rows || head.second < 0 || head.second >= grid.cols);

    if (!hitWall) {
        grid.update(snake);
    }

    // חישוב starvation
    int baseSteps = (grid.rows + grid.cols) * 2.5; 
    int dynamicMaxSteps = baseSteps + (snake.getSnakeLen() * 3);
    bool starved = stepsSinceLastFood > dynamicMaxSteps;

    // 3. בדיקת Game Over
    if (isGameOver() || starved) {
        result.done = true;
        result.reward = -1000.0f; 
        result.foodEaten = foodEaten; 
        result.snakeLen = snake.getSnakeLen(); 
        fillSafeDeadState(result);
        state = GAMEOVER;
        return result;
    }

    // 4. מילוי מצב ה-AI
    fillAIState(result);
    result.timePressure = (float)stepsSinceLastFood / (float)dynamicMaxSteps;

    // 5. לוגיקת פרסים (Rewards)
    if (isFoodEaten()) {
        snake.grow();
        foodEaten++;
        result.reward = 250.0f;
        stepsSinceLastFood = 0;

        if (isGameWon()) {
            result.done = true;
            result.won = true;
            result.reward = 2000.0f;
            state = GAMEWON;
            return result;
        } else {
            grid.placeFood();
            minDistTOFood = calculateManhattanDistance();
        }
        lastStepRatio = 2.0f; 
    } 
    else {
        // --- תנועה רגילה ---
        result.reward = -0.1f * (1.0f + result.timePressure);
        float currDist = calculateManhattanDistance();
        float delta = minDistTOFood - currDist;
        
        if (delta > 0) {
            result.reward += 1.0f;
            minDistTOFood = currDist;  
        } else if (result.fillPercentage < 0.25f) {
            result.reward -= 1.5f;
        }

        // --- לוגיקת עונש שטח ---
        if (result.accessibleSpace < 0.45f) {
            
            
            if(!canReachTail())
            {
                // 1. מציאת נקודת היציאה האופטימלית ("החוליה החלשה")
                // במקום סתם ללכת לזנב, אנחנו הולכים לחוליה שתוחמת אותנו ותיעלם ראשונה
                std::pair<int, int> targetExit = getExitPoint(); // {y, x}

                auto newHead = snake.body.front(); 
                auto neck = snake.body[1]; 
                
                // חישוב המרחק: האם אנחנו מתקרבים לפתח המילוט?
                // שים לב: targetExit.first זה Y, targetExit.second זה X
                int distNew = abs((int)newHead.second - targetExit.second) + abs((int)newHead.first - targetExit.first);
                int distOld = abs((int)neck.second - targetExit.second) + abs((int)neck.first - targetExit.first);
                
                bool movingTowardsExit = (distNew <= distOld);

                // 2. ספירת שכנים (נשאר אותו דבר)
                int neighbors = getOccupiedNeighbors();

                // 3. מתן בונוס/עונש על זיגזג + כיוון לפתח המילוט
                if (neighbors >= 2) {
                    if (movingTowardsExit) {
                        // זה ה-Holy Grail!
                        // אנחנו גם חוסכים מקום (זיגזג) וגם מתקדמים בדיוק לנקודה שתיפתח בקרוב
                        result.reward += 1.5f; 
                    } 
                } else {
                    // בזבוז שטח
                    result.reward -= 2.0f; 
                }
                
                // 4. חישוב מדדים גיאומטריים (עונש שטח כבד)
                float gridTotalCells = (float)(grid.rows * grid.cols);
                float freeSquares = result.accessibleSpace * gridTotalCells;

                int distEndToExit = abs((int)result.furthestPoint.second - targetExit.second) + abs((int)result.furthestPoint.first - targetExit.first);
                
                // שים לב: actualTailDelay מחושב מול היציאה האמיתית עכשיו
                int actualTailDelay = max(distNew, bodySegmentsInArea(newHead));
                float survivalRatio = freeSquares / (actualTailDelay + 2.0f);
                float continuityFactor = 1.0f / (distEndToExit + 1.0f);

                float spaceGap = 1.0f - result.accessibleSpace;
                float spacePenalty = pow(spaceGap, 3) * 20.0f;

                if (continuityFactor > 0.8f && survivalRatio > 1.1f) {
                    spacePenalty *= 0.1f; 
                } 
                else {
                    if (survivalRatio > 0.9f) {
                        spacePenalty *= (1.5f - continuityFactor); 
                    } else {
                        spacePenalty *= 2.0f; 
                    }
                }

                if (survivalRatio >= lastStepRatio && survivalRatio > 0.7f) {
                    spacePenalty *= 0.6f;
                }

                result.reward -= spacePenalty;
                lastStepRatio = survivalRatio;
            }
        } else {
            lastStepRatio = 2.0f; 
        }
    }

    result.foodEaten = foodEaten;
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
        result.accessibleSpaceN = predictFutureSpace(headX, headY - 1); // North
        result.accessibleSpaceS = predictFutureSpace(headX, headY + 1); // South
        result.accessibleSpaceE = predictFutureSpace(headX + 1, headY); // East
        result.accessibleSpaceW = predictFutureSpace(headX - 1, headY); // West

        //all the sum of space in each dir is equel to the sum in each firaction
        BFSResult bfs = calculateAccessibleSpace();
        result.accessibleSpace = bfs.count;
        result.furthestPoint = bfs.lastPoint; 
    }


BFSResult Game::calculateAccessibleSpace() {
    int startX = snake.body.front().second; 
    int startY = snake.body.front().first;  

    int cols = grid.cols;
    int rows = grid.rows;

    // ניקוי התור והעלאת דור
    while(!_bfsQueue.empty()) _bfsQueue.pop();
    _bfsGeneration++;

    _bfsQueue.push({startX, startY});
    _bfsVisited[startY * cols + startX] = _bfsGeneration;
    
    int reachableCount = 0;
    std::pair<int, int> furthest = {startX, startY}; // ברירת מחדל

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!_bfsQueue.empty()) {
        std::pair<int, int> curr = _bfsQueue.front();
        _bfsQueue.pop();
        
        // עדכון הנקודה הרחוקה ביותר (אופציונלי: אפשר לעשות את זה מדויק יותר לפי מרחק, אבל זה הקירוב הקיים שלך)
        furthest = curr;

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                int index = ny * cols + nx;

                // בדיקת visited לפי הדור הנוכחי
                if (_bfsVisited[index] != _bfsGeneration && grid.cells[ny][nx] != SNAKE) {
                    _bfsVisited[index] = _bfsGeneration;
                    _bfsQueue.push({nx, ny});
                    reachableCount++;
                }
            }
        }
    }

    float totalEmpty = (float)(rows * cols) - snake.getSnakeLen();
    if (totalEmpty <= 0.5f) return {1.0f, furthest};

    return {(float)reachableCount / (float)totalEmpty, furthest};
}


float Game::predictFutureSpace(int nextX, int nextY) {
    // 1. בדיקת גבולות (מהירה)
    if (nextX < 0 || nextX >= grid.cols || nextY < 0 || nextY >= grid.rows) return 0.0f;
    
    // בדיקה האם הצעד הבא הוא אוכל?
    bool isFood = (nextX == grid.foodPosition.second && nextY == grid.foodPosition.first);

    // 2. בדיקת מוות מיידי
    auto tailPos = snake.body.back(); 
    bool tailWillMove = !isFood;

    if (grid.cells[nextY][nextX] == SNAKE) {
        if (!(nextX == tailPos.second && nextY == tailPos.first && tailWillMove)) {
            return 0.0f; 
        }
    }

    // --- אופטימיזציה: שימוש בזיכרון הקיים ---
    while(!_bfsQueue.empty()) _bfsQueue.pop(); // ניקוי שאריות
    _bfsGeneration++; // דור חדש לחיפוש הזה

    _bfsQueue.push({nextX, nextY});
    _bfsVisited[nextY * grid.cols + nextX] = _bfsGeneration; // סימון ביקור
    
    int count = 0;
    
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!_bfsQueue.empty()) {
        std::pair<int, int> curr = _bfsQueue.front();
        _bfsQueue.pop();
        count++;

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            if (nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int index = ny * grid.cols + nx;
                
                // בדיקת ביקור לפי הדור הנוכחי
                if (_bfsVisited[index] != _bfsGeneration) {
                    
                    bool isFree = (grid.cells[ny][nx] != SNAKE);
                    bool isTailAndMoving = (nx == tailPos.second && ny == tailPos.first && tailWillMove);

                    if (isFree || isTailAndMoving) {
                        _bfsVisited[index] = _bfsGeneration; // סימון ביקור
                        _bfsQueue.push({nx, ny});
                    }
                }
            }
        }
    }

    float snakeLen = (float)snake.getSnakeLen();
    if (isFood) snakeLen += 1.0f; 

    float totalEmpty = (float)(grid.rows * grid.cols) - snakeLen;
    if (totalEmpty <= 0.5f) return 1.0f;

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

int Game::bodySegmentsInArea(std::pair<int, int> newHead) {
    int bodySegments = 0;
    int startY = newHead.first;
    int startX = newHead.second;

    // --- אופטימיזציה: שימוש בזיכרון הקיים ---
    while(!_bfsQueue.empty()) _bfsQueue.pop();
    _bfsGeneration++; // דור חדש

    // נסמן את נקודת ההתחלה
    _bfsVisited[startY * grid.cols + startX] = _bfsGeneration;
    _bfsQueue.push({startX, startY});

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while(!_bfsQueue.empty()) {
        auto curr = _bfsQueue.front(); 
        _bfsQueue.pop();
        int cx = curr.first; 
        int cy = curr.second;

        for(int i = 0; i < 4; i++) {
            int nx = cx + dx[i]; 
            int ny = cy + dy[i];

            // בדיקת גבולות
            if(nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int idx = ny * grid.cols + nx;

                // אם לא ביקרנו בסיבוב הנוכחי
                if (_bfsVisited[idx] != _bfsGeneration) {
                    
                    if(grid.cells[ny][nx] == EMPTY) {
                        _bfsVisited[idx] = _bfsGeneration; // סימון ביקור
                        _bfsQueue.push({nx, ny});
                    } 
                    else if(grid.cells[ny][nx] == SNAKE) {
                        _bfsVisited[idx] = _bfsGeneration; // סימון ביקור (כדי לא לספור פעמיים)
                        bodySegments++;
                    }
                }
            }
        }
    }
    return bodySegments;
}

bool Game::canReachTail() {
    // 1. שליפת מיקום הראש והזנב
    int startX = snake.body.front().second; 
    int startY = snake.body.front().first;
    
    int tailX = snake.body.back().second;
    int tailY = snake.body.back().first;

    // בדיקה מהירה: אם הראש ליד הזנב
    if (abs(startX - tailX) + abs(startY - tailY) == 1) return true;

    int cols = grid.cols;
    int rows = grid.rows;

    // --- אופטימיזציה: שימוש בזיכרון הקיים ---
    // ניקוי התור וקידום הדור (כמו שעשינו בפונקציות האחרות)
    while(!_bfsQueue.empty()) _bfsQueue.pop();
    _bfsGeneration++; 

    // דחיפת ההתחלה
    _bfsQueue.push({startX, startY});
    _bfsVisited[startY * cols + startX] = _bfsGeneration; // סימון שביקרנו

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!_bfsQueue.empty()) {
        std::pair<int, int> curr = _bfsQueue.front();
        _bfsQueue.pop();

        // הגענו לזנב? מעולה.
        if (curr.first == tailX && curr.second == tailY) {
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                int index = ny * cols + nx;

                // בדיקה אם ביקרנו בסיבוב הנוכחי (לפי הדור)
                if (_bfsVisited[index] != _bfsGeneration) {
                    
                    bool isTail = (nx == tailX && ny == tailY);
                    bool isEmpty = (grid.cells[ny][nx] != SNAKE);

                    if (isEmpty || isTail) {
                        _bfsVisited[index] = _bfsGeneration; // סימון שביקרנו
                        _bfsQueue.push({nx, ny});
                    }
                }
            }
        }
    }

    return false;
}

int Game::getOccupiedNeighbors()
{
    // חישוב שכנים בלבד
    auto newHead = snake.body.front(); 
    int occupiedNeighbors = 0;
    
    int hx = newHead.second;
    int hy = newHead.first;
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    
    for(int i=0; i<4; i++) {
        int cx = hx + dx[i];
        int cy = hy + dy[i];
        if (cx < 0 || cx >= grid.cols || cy < 0 || cy >= grid.rows || grid.cells[cy][cx] == SNAKE) {
            occupiedNeighbors++;
        }
    }
    return occupiedNeighbors;
}

// מחזירה את הקואורדינטות של החוליה שתפתח לנו את המלכודת הכי מהר
std::pair<int, int> Game::getExitPoint() {
    
    int n = snake.body.size();
    
    // 1. מיפוי הגוף לזמני היעלמות
    // אנחנו כותבים את הערכים החדשים לתוך ה-Cache
    for (int i = 0; i < n; ++i) {
        auto part = snake.body[i]; 
        int time = n - 1 - i; 
        _timeToFreeCache[part.first * grid.cols + part.second] = time;
    }

    // 2. הכנת ה-BFS
    while(!_bfsQueue.empty()) _bfsQueue.pop();
    _bfsGeneration++; 
    
    auto head = snake.body.front();
    _bfsQueue.push({head.second, head.first});
    _bfsVisited[head.first * grid.cols + head.second] = _bfsGeneration;

    int minTimeToFree = 999999;
    std::pair<int, int> bestExit = snake.body.back(); // ברירת מחדל: הזנב

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    // 3. ריצת BFS
    while (!_bfsQueue.empty()) {
        auto curr = _bfsQueue.front();
        _bfsQueue.pop();

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];
            
            if (nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int idx = ny * grid.cols + nx;
                
                if (_bfsVisited[idx] != _bfsGeneration) {
                    
                    if (grid.cells[ny][nx] == EMPTY) {
                        _bfsVisited[idx] = _bfsGeneration; 
                        _bfsQueue.push({nx, ny});
                    } 
                    else if (grid.cells[ny][nx] == SNAKE) {
                        _bfsVisited[idx] = _bfsGeneration; 
                        
                        // קריאה מהירה מה-Cache (בטוחה, כי זה SNAKE ומילאנו אותו למעלה)
                        int t = _timeToFreeCache[idx];
                        
                        if (t != -1 && t < minTimeToFree) {
                            minTimeToFree = t;
                            bestExit = {ny, nx};
                        }
                    }
                }
            }
        }
    }
    
    // 4. ניקוי כירורגי (התוספת החשובה)
    // לפני שיוצאים, אנחנו מנקים *רק* את המשבצות שכתבנו בהן בתחילת הפונקציה.
    // זה מבטיח שבפעם הבאה שהפונקציה תקרא, המערך כולו יהיה -1.
    // זה הרבה יותר מהר מ-std::fill על כל הלוח.
    for (int i = 0; i < n; ++i) {
        auto part = snake.body[i]; 
        _timeToFreeCache[part.first * grid.cols + part.second] = -1;
    }
    
    return bestExit; 
}
    