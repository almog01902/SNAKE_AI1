#include "SnakeBatch.h"

// מימוש הבנאי
SnakeBatch::SnakeBatch(int n_agents, int rows, int cols, int startX, int startY, int len) 
    : num_agents(n_agents), state_dim(25) { 
    
    games.reserve(num_agents);
    for (int i = 0; i < num_agents; ++i) {
        games.emplace_back(rows, cols, startX, startY, len);
        games[i].initilizeGrid(); // שים לב ל-I הגדולה שתיקנו
    }
}

// מימוש reset_all
void SnakeBatch::reset_all() {
    for (auto& game : games) {
        game.initilizeGrid(); 
    }
}

// מימוש get_agent
Game& SnakeBatch::get_agent(int index) {
    return games[index];
}

// מימוש step_all - המנוע המהיר
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>> 
SnakeBatch::step_all(std::vector<int> actions, std::vector<bool> done_flags) {
    
    std::vector<float> batch_states;
    batch_states.reserve(num_agents * state_dim);
    
    std::vector<float> batch_rewards;
    batch_rewards.reserve(num_agents);
    
    std::vector<float> batch_dones;
    batch_dones.reserve(num_agents);

    std::vector<float> batch_infos; 
    batch_infos.reserve(num_agents * 2); 

    for (int i = 0; i < num_agents; ++i) {
        if (done_flags[i]) {
            // נחש מת - אפסים
            for(int j=0; j<state_dim; ++j) batch_states.push_back(0.0f);
            batch_rewards.push_back(0.0f);
            batch_dones.push_back(1.0f);
            batch_infos.push_back(0.0f);
            batch_infos.push_back(0.0f);
        } 
        else {
            // ביצוע צעד
            stepResult res = games[i].step(actions[i]);

            // דחיפת נתונים לוקטור השטוח
            batch_states.push_back(res.distFoodX);
            batch_states.push_back(res.distFoodY);
            batch_states.push_back(res.headX_norm);
            batch_states.push_back(res.headY_norm);
            batch_states.push_back(res.distN);
            batch_states.push_back(res.distS);
            batch_states.push_back(res.distE);
            batch_states.push_back(res.distW);
            batch_states.push_back(res.distNW);
            batch_states.push_back(res.distNE);
            batch_states.push_back(res.distSW);
            batch_states.push_back(res.distSE);
            batch_states.push_back(res.isUp);
            batch_states.push_back(res.isDown);
            batch_states.push_back(res.isLeft);
            batch_states.push_back(res.isRight);
            batch_states.push_back(res.fillPercentage);
            batch_states.push_back(res.accessibleSpace);
            batch_states.push_back(res.accessibleSpaceN);
            batch_states.push_back(res.accessibleSpaceS);
            batch_states.push_back(res.accessibleSpaceE);
            batch_states.push_back(res.accessibleSpaceW);
            batch_states.push_back(res.diffX);
            batch_states.push_back(res.diffy);
            batch_states.push_back(res.timePressure);
            


            batch_rewards.push_back(res.reward);
            batch_dones.push_back(res.done ? 1.0f : 0.0f);

            batch_infos.push_back((float)res.foodEaten);
            batch_infos.push_back((float)res.snakeLen);
        }
    }

    return std::make_tuple(batch_states, batch_rewards, batch_dones, batch_infos);
}