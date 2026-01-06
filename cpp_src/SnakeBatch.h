#ifndef SNAKE_BATCH_H
#define SNAKE_BATCH_H

#include <vector>
#include <tuple>
#include "Game.h" // חייבים לייבא את Game כדי להכיר את האובייקטים

class SnakeBatch {
public:
    std::vector<Game> games;
    int num_agents;
    int state_dim;

    // בנאי
    SnakeBatch(int n_agents, int rows, int cols, int startX, int startY, int len);

    // פונקציה לאיפוס מהיר
    void reset_all();

    // הפונקציה הראשית לריצת Batch
    std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>> 
    step_all(std::vector<int> actions, std::vector<bool> done_flags);

    // גישה לנחש בודד (עבור ויזואליזציה)
    Game& get_agent(int index);
};

#endif // SNAKE_BATCH_H