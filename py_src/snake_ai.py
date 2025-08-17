import os
import snake_module
import torch 
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import numpy as np

#constants
NUM_AGENTS = 4
GRID_SIZE = 20
INITIAL_SNAKE_LENGTH =3
agents = [snake_module.Game(GRID_SIZE,GRID_SIZE,GRID_SIZE//2,GRID_SIZE//2,INITIAL_SNAKE_LENGTH) for _ in range(NUM_AGENTS)]

for agent in agents:
    agent.InitilizeGrid()

