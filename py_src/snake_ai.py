import os
import snake_module
import torch 
import torch.nn as nn
import torch.optim as optim
from torch.distributions import Categorical
import torch.nn.functional as F
import numpy as np

#constants
NUM_AGENTS = 4
GRID_SIZE = 20
INITIAL_SNAKE_LENGTH =3
STATE_DIM = 6
ACTION_DIM =4
HIDDEN_SIZE = 64
GAMMA = 0.99
LR = 3e-4
EPS_CLIPS =0.2
UPDATE_STEP = 5
MAX_STEPS = 1000

#policy and value network

class ActorCritic(nn.Module):#actor class to chose an acion
    def __init__(self, state_dim, action_dim):
        super().__init__()
        self.fc1 = nn.Linear(state_dim,128)#first layer 
        self.fc2 = nn.Linear(128,128)#second layer
        self.out = nn.Linear(128,action_dim)#output layer
    
    def forward(self,x):#we dont want all to be linear
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return F.softmax(self.out(x),dim=-1)
    
class Critic(nn.Module):#critic class to rank actor action
    def __init__(self,state_dim):
        super().__init__()
        self.fc1 = nn.Linear(state_dim,128)
        self.fc2 = nn.Linear(128,128)
        self.out = nn.Linear(128,1)
    
    def forward(self,x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return self.out(x)
#make agents
agents = [snake_module.Game(GRID_SIZE,GRID_SIZE,GRID_SIZE//2,GRID_SIZE//2,INITIAL_SNAKE_LENGTH) for _ in range(NUM_AGENTS)]

for agent in agents:
    agent.InitilizeGrid()

#make policy and critic
policy = ActorCritic(STATE_DIM,ACTION_DIM)
critic = Critic(STATE_DIM)
optimizer = optim.Adam(policy.parameters(),lr= LR)

#rollout
all_logs_probs = []#data for all agent probs
all_values = []#critic for each agent action
all_rewards = []#reward for each agent action
last_actions = [1 for _ in range(NUM_AGENTS)]#each agent start when they go right
done_flags = [False for _ in range(NUM_AGENTS)]#flags for when the agent finished the game

for step in range(MAX_STEPS):#give each agent test in playing the game
    log_probs_step = []#probolity for each step
    values_step = []#value for each step
    rewards_step =[]#reward for each step
    for i,agent in enumerate(agents):
        if(done_flags[i]):
            continue

        #print("agent " ,i )
        result = agent.step(last_actions[i])

        #print(result.direction)
        #print(result.distFoodX)
        #print(result.distFoodY)
        #print(result.distToDangerForward)
        state = torch.tensor( #make the inputs for first layer
            [
                result.distFoodX,
                result.distFoodY,
                result.distToDangerForward,
                result.distToDangerLeft,
                result.distToDangerRight,
                result.direction
            ],dtype =torch.float32).unsqueeze(0)
        
        with torch.no_grad():
            action_probs = policy(state)#get each action prob 
            value = critic(state)#get value for each probolity
            dist = Categorical(action_probs)#get the probs
            action_t = dist.sample()#get action by probility
            log_prob = dist.log_prob(action_t)#get the log probility of action
            ##rint("action " ,action_t.item(), "log prob :" , log_prob.item())

        #gatter data for all agents in current step
        log_probs_step.append(log_prob)
        values_step.append(value)
        rewards_step.append(torch.tensor(result.reward,dtype=torch.float32))

        #get action
        last_actions[i] = action_t.item()
        if result.done:
            done_flags[i] = True
            ##print("agent",i, "done at step" ,step)

    #gatter all the data for agents
    all_logs_probs.append(torch.stack(log_probs_step))
    all_values.append(torch.stack(values_step))
    all_rewards.append(torch.stack(rewards_step))

    if all(done_flags):
        break


    


