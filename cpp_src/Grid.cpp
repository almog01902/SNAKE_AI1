#include "Grid.h"




    Grid::Grid(int rows, int cols) : rows(rows), cols(cols)
    {
        cells = vector<vector<int>>(rows, vector<int>(cols, 0));
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
        int x = rand() % rows;
        int y = rand() % cols;
        while (cells[x][y] != EMPTY) { // Ensure food is placed in an empty cell
            x = rand() % rows;
            y = rand() % cols;
        }
        foodPosition = make_pair(x, y);
        cells[x][y] = FOOD; // Place food

    }
