import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from config import *

#policy and value network
class ActorCritic(nn.Module):#actor class to chose an acion
    def __init__(self, state_dim, action_dim):
        super().__init__()
        self.fc1 = nn.Linear(state_dim,HIDDEN_SIZE)#first layer 
        self.fc2 = nn.Linear(HIDDEN_SIZE,HIDDEN_SIZE)#second layer
        self.out = nn.Linear(HIDDEN_SIZE,action_dim)#output layer
    
    def forward(self,x):#we dont want all to be linear for batter learning result
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return F.softmax(self.out(x),dim=-1)
    
class Critic(nn.Module):#critic class to rank actor action
    def __init__(self,state_dim):
        super().__init__()
        self.fc1 = nn.Linear(state_dim,HIDDEN_SIZE)
        self.fc2 = nn.Linear(HIDDEN_SIZE,HIDDEN_SIZE)
        self.out = nn.Linear(HIDDEN_SIZE,1)
    
    def forward(self,x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return self.out(x)


#make policy and critic
    
def make_models(state_dim, action_dim, lr, device):
    policy = ActorCritic(state_dim, action_dim).to(device)
    critic = Critic(state_dim).to(device)
    all_params = list(policy.parameters()) + list(critic.parameters())
    optimizer = optim.Adam(all_params,lr=lr)
    return policy, critic, optimizer