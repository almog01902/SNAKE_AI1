#include "Grid.h"




    Grid::Grid(int rows, int cols) : rows(rows), cols(cols)// Initialize the grid with given rows and columns
    {
        cells = vector<vector<int>>(rows, vector<int>(cols, 0));
        std::random_device rd; // Random device for seeding
        gen = std::mt19937(rd()); // Initialize random number generator with a random seed
        randX = std::uniform_int_distribution<>(0, rows - 1); // Distribution for x coordinate
        randY = std::uniform_int_distribution<>(0, cols - 1); // Distribution        
        
    }

    
    void Grid::draw() {// Draw the grid
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
    
    void Grid::placeSnake(const Snake& snake) {// Place the snake on the grid
        for (const auto& segment : snake.body) {
            cells[segment.first][segment.second] = SNAKE;
            
        }
    }

    void Grid::reset()// Reset the grid to empty state
    {
        for(int i = 0; i <rows;i++)
        {
            for(int j =0; j< cols;j++)
            {
                cells[i][j] = EMPTY; // Set all cells to empty
            }
        }
    }

    void Grid::update(const Snake& snake)// update the grid with the snake's position and food
    {
        for(int i = 0; i <rows;i++)
        {
            for(int j =0; j< cols;j++)
            {
                cells[i][j] = 0;
            }
        }

        cells[foodPosition.first][foodPosition.second] = FOOD; // Restore food
        placeSnake(snake); // Place the snake on the grid
    }
    void Grid::placeFood()// Place food in a random empty cell
    {
        int x = randX(gen);
        int y = randY(gen);
        while (cells[x][y] != EMPTY) { // Ensure food is placed in an empty cell
            x = randX(gen);
            y = randY(gen);
        }
        foodPosition = make_pair(x, y);
        cells[x][y] = FOOD; // Place food

    }
