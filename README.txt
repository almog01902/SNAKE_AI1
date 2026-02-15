Project Objective
The primary goal of this project was to explore how an agent can learn complex 
decision-making through Reinforcement Learning (RL) and, 
more importantly, achieve Zero-Shot Generalization.

The challenge was to train an agent on a fixed-size environment with the minimum amount of inputs and 
enable it to perform optimally across various board dimensions 
without any architecture changes or re-training.

The Generalization Breakthrough
Unlike standard implementations that are "locked" to a specific grid size, this model focuses on Spatial Invariance:

Train Once, Run Anywhere: The agent was trained exclusively on a 20x20 grid.

Result: Due to the relative feature representation and robust Reward Shaping, 
the resulting model can successfully navigate and complete Snake boards of any size, 
demonstrating true algorithmic generalization.

Key Technical Features
Hybrid Engine: High-performance game logic implemented in C++20 for maximum sample throughput, 
exposed to Python via Pybind11.

PPO Architecture: Utilizes Proximal Policy Optimization with an emphasis on entropy management 
to balance exploration and exploitation.

Reward Engineering: Developed a dense reward function that guides the agent toward the objective 
while penalizing sub-optimal "looping" behaviors.

Reward Shaping LogicThe agent's success is driven by a carefully engineered 
reward function designed to mitigate the "sparse reward" problem. The step-wise reward 
R is defined as:

R = W1 * FOOD_REWARD + W2 * DISTANCE_REWARD  + W3 * accessibility_reward


🛠 Technical Stack
Engine: C++20 .

ML Framework: PyTorch.

Binding: Pybind11.

Optimization: Adam Optimizer with custom learning rate scheduling.

Installation & Usage
Train: python snake_ai.py 
Test: python test_model.py
Chack stats: python chack_stats.py
you can change the board size and more in the config.py file