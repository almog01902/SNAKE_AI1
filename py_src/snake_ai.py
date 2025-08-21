import os
import snake_module
import time
import torch 
import torch.nn as nn
import torch.optim as optim
from torch.distributions import Categorical
import torch.nn.functional as F
import numpy as np
import matplotlib.pyplot as plt
import gc
import pickle


#constants
NUM_AGENTS = 10
GRID_SIZE = 20
INITIAL_SNAKE_LENGTH =3
STATE_DIM = 6
ACTION_DIM =4
HIDDEN_SIZE = 64
GAMMA = 0.99
LR = 3e-4
EPS_CLIPS =0.2
UPDATE_STEP = 5
MAX_STEPS = 100
NUM_EPISODES = 5
SAVE_INTERVAL = 10
CHECKPOINT_DIR = "checkpoints"

os.makedirs(CHECKPOINT_DIR,exist_ok=True)
policy_path = os.path.join(CHECKPOINT_DIR, "policy.pth")
critic_path = os.path.join(CHECKPOINT_DIR, "critic.pth")
optimizer_path = os.path.join(CHECKPOINT_DIR, "optimizer.pth")
rewards_path = os.path.join(CHECKPOINT_DIR, "episode_rewards.pkl")

#run thr nural net on gpu
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
#policy and value network
class ActorCritic(nn.Module):#actor class to chose an acion
    def __init__(self, state_dim, action_dim):
        super().__init__()
        self.fc1 = nn.Linear(state_dim,128)#first layer 
        self.fc2 = nn.Linear(128,128)#second layer
        self.out = nn.Linear(128,action_dim)#output layer
    
    def forward(self,x):#we dont want all to be linear for batter learning result
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


#make policy and critic
    
policy = ActorCritic(STATE_DIM,ACTION_DIM).to(device=device)
critic = Critic(STATE_DIM).to(device=device)
optimizer = optim.Adam(policy.parameters(),lr= LR)

if os.path.exists(policy_path):
    policy.load_state_dict(torch.load(policy_path, map_location=device))
if os.path.exists(critic_path):
    critic.load_state_dict(torch.load(critic_path, map_location=device))
if os.path.exists(optimizer_path):
    optimizer.load_state_dict(torch.load(optimizer_path, map_location=device))

episode_rewards = []
if os.path.exists(rewards_path):
    with open(rewards_path,"rb") as f:
        episode_rewards = pickle.load(f)


#graph to show progress

plt.ion()  
fig_reward, ax_reward = plt.subplots()
line, = ax_reward.plot([], [])
ax_reward.set_xlabel("Episode")
ax_reward.set_ylabel("Avg Reward")
ax_reward.set_title("Training Progress")

#window to show agents play




# rollout
for episode in range(NUM_EPISODES):
    print("Episode:", episode)

    # make agents
    agents = [
        snake_module.Game(GRID_SIZE, GRID_SIZE, GRID_SIZE // 2, GRID_SIZE // 2, INITIAL_SNAKE_LENGTH)
        for _ in range(NUM_AGENTS)
    ]
    for agent in agents:
        agent.InitilizeGrid()

    last_actions = [1 for _ in range(NUM_AGENTS)]  # start going right
    done_flags = [False for _ in range(NUM_AGENTS)]
    won_flags = [False for _ in range(NUM_AGENTS)]
    ep_rewards = torch.zeros(NUM_AGENTS)
    ep_food = torch.zeros(NUM_AGENTS,dtype=torch.int32)

    # buffers
    all_log_probs = []
    all_values = []
    all_rewards = []
    all_dones = []

    while not all(done_flags):
        step_log_probs = []
        step_values = []
        step_rewards = []
        step_dones = []

        for i, agent in enumerate(agents):

            if done_flags[i]:#for agents who finished the game we make empty stats
                reward_value = 0.0 if won_flags[i] else -0.01
                step_rewards.append(torch.tensor(reward_value,dtype=torch.float32,device =device))
                step_dones.append(torch.tensor(1.0,dtype=torch.float32,device = device))
                step_log_probs.append(torch.tensor(0.0,dtype=torch.float32,device = device))
                step_values.append(torch.tensor(0.0,dtype=torch.float32,device = device))
                continue

            result = agent.step(last_actions[i])

            state = torch.tensor(
                [
                    result.distFoodX,
                    result.distFoodY,
                    result.distToDangerForward,
                    result.distToDangerLeft,
                    result.distToDangerRight,
                    result.direction,
                ],
                dtype=torch.float32,
                device= device,
            ).unsqueeze(0)


            action_probs = policy(state)
            value = critic(state)
            dist = Categorical(action_probs)
            action_t = dist.sample()
            log_prob = dist.log_prob(action_t)

            # save step data
            step_log_probs.append(log_prob.squeeze())
            step_values.append(value.squeeze())
            step_rewards.append(torch.tensor(result.reward, dtype=torch.float32,device=device))
            step_dones.append(torch.tensor(float(result.done),device=device))

            ep_rewards[i] += result.reward
            last_actions[i] = action_t.item()
            if result.done:
                done_flags[i] = True
                ep_food[i] = result.foodEaten
                if i == 0:
                    print("eaten " , result.foodEaten)
                if result.won:
                    won_flags[i] = True
                


        # save per-step tensors
        all_log_probs.append(torch.stack(step_log_probs))
        all_values.append(torch.stack(step_values))
        all_rewards.append(torch.stack(step_rewards))
        all_dones.append(torch.stack(step_dones))

    # === GAE / returns ===
    returns = []
    G = torch.zeros(NUM_AGENTS,device=device)
    for r, d in zip(reversed(all_rewards), reversed(all_dones)):
        G = r + GAMMA * G * (torch.ones(NUM_AGENTS,device=device) - d)
        returns.insert(0, G.clone())
    returns = torch.stack(returns)

    # advantages
    values = torch.stack(all_values)
    advantages = returns - values

    # losses
    log_probs = torch.stack(all_log_probs).squeeze(-1)
    actor_loss = -(log_probs * advantages).mean()
    critic_loss = F.mse_loss(values, returns)
    loss = actor_loss + 0.5 * critic_loss

    #update grad
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    #gwt avg score
    avg_reward = ep_rewards.mean().item()
    episode_rewards.append(avg_reward)
    print("avg reward: " ,avg_reward)

    avg_food = int(ep_food.float().mean().item())
    print("avg food eaten :" ,avg_food)
    

    #save the learning process
    if episode % SAVE_INTERVAL == 0:
        torch.save(policy.state_dict(), policy_path)
        torch.save(critic.state_dict(), critic_path)
        torch.save(optimizer.state_dict(), optimizer_path)
        with open(rewards_path, "wb") as f:
            pickle.dump(episode_rewards, f)

    #graph update
    ax_reward.clear()
    ax_reward.plot(episode_rewards, marker="o", linestyle="-", label="Average Reward")
    ax_reward.set_xlabel("Episode")
    ax_reward.set_ylabel("Reward")
    ax_reward.set_title("Training Progress")
    ax_reward.legend()

    plt.pause(0.01)  # חשוב בשביל עדכון בזמן אמת

plt.ioff()
plt.show()

    


