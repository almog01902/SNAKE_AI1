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
    
    // 1. אתחול נתונים בסיסיים
    result.reward = 0.0f;
    result.done = false;
    result.won = false;
    result.snakeLen = snake.getSnakeLen();
    
    // 2. עדכון מצב המשחק
    AIInputHandler(action);
    snake.move();
    stepsSinceLastFood++;

    // חישוב starvation
    int baseSteps = (grid.rows + grid.cols) * 2.5; 
    int dynamicMaxSteps = baseSteps + (snake.getSnakeLen() * 3);
    bool starved = stepsSinceLastFood > dynamicMaxSteps;

    // 3. בדיקת Game Over (חוסך את כל המשך הפונקציה אם הנחש מת)
    if (isGameOver() || starved) {
        result.done = true;
        result.reward = -1000.0f; 
        result.foodEaten = foodEaten; 
        result.snakeLen = snake.getSnakeLen(); 
        fillSafeDeadState(result);
        state = GAMEOVER;
        return result;
    }

    // 4. מילוי מצב ה-AI (כולל ה-BFS הראשי שנותן לנו את ה-accessibleSpace)
    fillAIState(result);
    result.timePressure = (float)stepsSinceLastFood / (float)dynamicMaxSteps;

    // 5. לוגיקת פרסים (Rewards)
    if (isFoodEaten()) {
        // --- מסלול "אכילה": מהיר וקל לחישוב ---
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
        // מאפסים את היחס לצעד הבא כדי שלא תהיה קפיצה מוזרה
        lastStepRatio = 2.0f; 
    } 
    else {
        // --- מסלול "תנועה רגילה": כאן נבצע את החישובים הכבדים רק אם צריך ---
        
        // עונש זמן ובונוס התקרבות בסיסי
        result.reward = -0.1f * (1.0f + result.timePressure);
        float currDist = calculateManhattanDistance();
        float delta = minDistTOFood - currDist;
        
        if (delta > 0) {
            result.reward += 1.0f;
            minDistTOFood = currDist;  
        } else if (result.fillPercentage < 0.25f) {
            result.reward -= 1.5f;
        }

        // לוגיקת עונש שטח - החלק הכבד!
        if (result.accessibleSpace < 0.45f) {
            
            // רק עכשיו, כשיש לחץ שטח, נחשב את המדדים הגיאומטריים המורכבים
            auto newHead = snake.body.front();
            auto currentTail = snake.body.back();
            float gridTotalCells = (float)(grid.rows * grid.cols);
            float freeSquares = result.accessibleSpace * gridTotalCells;

            // מרחקים לזנב
            int newDistToTail = abs((int)newHead.second - (int)currentTail.second) + abs((int)newHead.first - (int)currentTail.first);
            int distEndToTail = abs((int)result.furthestPoint.second - (int)currentTail.second) + abs((int)result.furthestPoint.first - (int)currentTail.first);
            
            // חישוב יחס שרידות (כולל קריאה ל-BFS המשני bodySegmentsInArea)
            int actualTailDelay = max(newDistToTail, bodySegmentsInArea(newHead));
            float survivalRatio = freeSquares / (actualTailDelay + 2.0f);
            float continuityFactor = 1.0f / (distEndToTail + 1.0f);

            // חישוב העונש
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

            // בונוס שיפור (מומנטום)
            if (survivalRatio >= lastStepRatio && survivalRatio > 0.7f) {
                spacePenalty *= 0.6f;
            }

            result.reward -= spacePenalty;
            lastStepRatio = survivalRatio; // עדכון רק כשיש חישוב
        } else {
            lastStepRatio = 2.0f; // שטח פתוח - יחס "מושלם"
        }
    }

    result.foodEaten = foodEaten;
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
        BFSResult bfs = calculateAccessibleSpace();
        result.accessibleSpace = bfs.count;
        result.furthestPoint = bfs.lastPoint; 
    }


    BFSResult Game::calculateAccessibleSpace() {

    //  y הוא rows, x הוא cols

    int startX = snake.body.front().second; // Column
    int startY = snake.body.front().first;  // Row

   

    int cols = grid.cols;
    int rows = grid.rows;



    std::vector<bool> visited(rows * cols, false);
    std::queue<std::pair<int, int>> q;
    pair<int,int> furthest = {startX,startY};

    q.push({startX, startY});
    visited[startY * cols + startX] = true;
    int reachableCount = 0;
    while (!q.empty()) {

        std::pair<int, int> curr = q.front();
        q.pop();
        furthest = curr;

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
    if (totalEmpty <= 0.5f) return {1.0f,furthest};

    //normalized return
    return {(float)reachableCount / (float)totalEmpty,furthest};

}

float Game::predictFutureSpace(int nextX, int nextY) {
    // 1. בדיקת גבולות (למנוע קריסה)
    if (nextX < 0 || nextX >= grid.cols || nextY < 0 || nextY >= grid.rows) return 0.0f;
    
    // 2. בדיקת מוות מיידי (האם המשבצת תפוסה על ידי הגוף, למעט הזנב שיזוז)
    auto tailPos = snake.body.back(); // {Y, X}
    if (grid.cells[nextY][nextX] == SNAKE && !(nextX == tailPos.second && nextY == tailPos.first)) {
        return 0.0f;
    }

    std::vector<bool> visited(grid.rows * grid.cols, false);
    std::queue<std::pair<int, int>> q;

    q.push({nextX, nextY});
    visited[nextY * grid.cols + nextX] = true;
    int count = 0;
    bool canReachTail = false;

    while (!q.empty()) {
        std::pair<int, int> curr = q.front(); // {X, Y}
        q.pop();
        count++;

        // בדיקה אם הגענו לזנב (נתיב מילוט)
        if (curr.first == tailPos.second && curr.second == tailPos.first) {
            canReachTail = true;
        }

        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = curr.first + dx[i];
            int ny = curr.second + dy[i];

            if (nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int index = ny * grid.cols + nx;
                // תנאי כניסה למשבצת: פנויה או שהיא המקום שבו הזנב נמצא כרגע
                if (!visited[index] && (grid.cells[ny][nx] != SNAKE || (nx == tailPos.second && ny == tailPos.first))) {
                    visited[index] = true;
                    q.push({nx, ny});
                }
            }
        }
    }

    float snakeLen = (float)snake.getSnakeLen();
    float totalEmpty = (float)(grid.rows * grid.cols) - snakeLen -1;
    if (totalEmpty <= 0.5f) return 1.0f;

    float spaceRatio = (float)count / totalEmpty;


    return spaceRatio;
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

    std::queue<std::pair<int, int>> q;
    std::vector<bool> visited(grid.rows * grid.cols, false);

    // נסמן את הראש כביכול כ"ביקרנו בו" כדי שלא נחזור אליו
    visited[startY * grid.cols + startX] = true;

    // נכניס לתור רק שכנים שהם באמת ריקים מסביב לראש
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    for(int i=0; i<4; i++) {
        int nx = startX + dx[i];
        int ny = startY + dy[i];
        if(nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
            if(grid.cells[ny][nx] == EMPTY) {
                q.push({nx, ny});
                visited[ny * grid.cols + nx] = true;
            } else if(grid.cells[ny][nx] == SNAKE) {
                bodySegments++; // סגמנט שנוגע ישירות בראש
            }
        }
    }

    while(!q.empty()) {
        auto curr = q.front(); q.pop();
        int cx = curr.first; int cy = curr.second;

        for(int i = 0; i < 4; i++) {
            int nx = cx + dx[i]; int ny = cy + dy[i];
            if(nx >= 0 && nx < grid.cols && ny >= 0 && ny < grid.rows) {
                int idx = ny * grid.cols + nx;
                if(grid.cells[ny][nx] == EMPTY && !visited[idx]) {
                    visited[idx] = true;
                    q.push({nx, ny});
                } 
                else if(grid.cells[ny][nx] == SNAKE) {
                    bodySegments++;
                }
            }
        }
    }
    return bodySegments;
}
    