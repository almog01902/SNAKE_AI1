#include "Snake.h"


    

    Snake::Snake(int startX,int startY,int initialLength) : direction(RIGHT), length(initialLength)
    {
        for(int i=0; i< initialLength;i++)
        {
            body.push_back(make_pair(startX, startY - i)); // Initialize snake vertically
        }
    } 
    void Snake::move()// Move the snake in the current direction
    {
        pair<int,int> head = body.front();
        switch(direction) {
            case UP:
                head.first--;// Move up
                break;
            case RIGHT:
                head.second++;// Move right
                break;
            case DOWN:
                head.first++;// Move down
                break;
            case LEFT:
                head.second--;// Move left
                break;
        }
        for(int i = body.size() - 1; i > 0; i--) {
            body[i] = body[i - 1]; // Move the body segments
        }
        body[0] = head; // Update the head position
    }
    void Snake::changeDirection(int newDirection) {
        // Prevent the snake from reversing direction
        if ((direction == UP && newDirection != DOWN) ||
            (direction == DOWN && newDirection != UP) ||
            (direction == LEFT && newDirection != RIGHT) ||
            (direction == RIGHT && newDirection != LEFT)) {
            direction = newDirection;
        }
    }
    void Snake::grow() {
        length++;
        body.push_back(body.back()); // Add a new segment at the tail
    }
    int  Snake::getSnakeLen()
    {
        return length;
    }
    

