Snake AI - Reinforcement Learning Agent
A hybrid project featuring a high-performance Snake game engine written in C++ and an autonomous agent trained using Deep Reinforcement Learning (PPO) in Python.

The project demonstrates the integration of low-level systems with high-level AI frameworks through Pybind11.

Overview
The primary objective of this project was to develop a highly generalized AI agent capable of playing Snake on any grid size without retraining.

Unlike standard approaches that use CNNs (which often overfit to specific board dimensions and spatial features), this project focuses on a minimalist input space. By using a carefully selected set of relative features rather than raw pixels, the model achieves robust performance across diverse environments.

🛠 Tech Stack
Game Engine: C++ (Object-Oriented Design)

AI Framework: Python (PyTorch / Stable Baselines3)

Communication: Pybind11 (C++/Python bindings)

Algorithm: Proximal Policy Optimization (PPO)

Key Features
Performance: High-speed game simulation using a custom-built C++ engine.

Hybrid Architecture: Seamless real-time data transfer between the C++ environment and the Python AI agent.

Observation Space: Custom-designed feature extraction (e.g., proximity to walls, food direction, and self-collision risks).

Robust Training: The model is trained on dynamic grid sizes to increase generalization and robustness.

Architecture
C++ Engine: Manages the game state, snake movement, collision detection, and score tracking.

Pybind11 Wrapper: Exposes the C++ classes to Python, allowing the agent to "see" the game state and "send" actions.

Python Trainer: Handles the neural network training loop, reward calculation, and hyperparameter tuning.

Running the Project
Testing the Model
To run the pre-trained model and watch the AI play:


python .\py_src\test_model.py
Note: You can adjust the grid dimensions in the configuration file to see how the agent adapts.

Training the Model
To start a new training session:

python .\py_src\train.py
The environment is designed to randomize the grid size each episode, ensuring the agent learns a robust strategy that isn't overfitted to a specific board size