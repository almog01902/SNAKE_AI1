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
from config import *
from utils import *
from model.actor_critic import make_models




paths = prepare_checkpoint_paths()#preare pathes for chackpoints load

episode_rewards =[]
len_max = []
policy, critic, optimizer = make_models(STATE_DIM,ACTION_DIM,LR,device)#make models
load_checkpoints(policy,critic,optimizer,episode_rewards,len_max,paths)#load chackpoints
#graph to show progress
if GRAPH:
    rewardPlotter = RewardPlotter()
    rewardPlotter.rewards = episode_rewards
    MaxLenPlotter = MaxLenPlotter()
    MaxLenPlotter.max_lens = len_max


rewards_to_save = episode_rewards
len_max_to_save = len_max

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

    #window to show agents play
    if(VISUALIZER):
        renderer = AgentRenderer(agents)
    
    
    #update graphes

    #data of agents
    last_actions = [1 for _ in range(NUM_AGENTS)]  # start going right
    done_flags = [False for _ in range(NUM_AGENTS)]
    won_flags = [False for _ in range(NUM_AGENTS)]
    #data to see progress
    ep_rewards = torch.zeros(NUM_AGENTS)
    ep_food = torch.zeros(NUM_AGENTS,dtype=torch.int32)
    ep_len = torch.zeros(NUM_AGENTS,dtype=torch.int32)

    # buffers
    all_log_probs = []
    all_values = []
    all_rewards = []
    all_dones = []

    while not all(done_flags):
        #buffer for step
        step_log_probs = []
        step_values = []
        step_rewards = []
        step_dones = []

        if(VISUALIZER):
            renderer.update()

        for i, agent in enumerate(agents):

            if done_flags[i]:#for agents who finished the game we make empty stats
                reward_value = 0.0 if won_flags[i] else -0.01
                step_rewards.append(torch.tensor(reward_value,dtype=torch.float32,device =device))
                step_dones.append(torch.tensor(1.0,dtype=torch.float32,device = device))
                step_log_probs.append(torch.tensor(0.0,dtype=torch.float32,device = device))
                step_values.append(torch.tensor(0.0,dtype=torch.float32,device = device))
                continue

            result = agent.step(last_actions[i])#get result from action

            state = torch.tensor([
            result.distFoodX, 
            result.distFoodY,
            result.headX_norm, 
            result.headY_norm,
            result.distN, 
            result.distS, 
            result.distE, 
            result.distW,
            result.distNW, 
            result.distNE, 
            result.distSW, 
            result.distSE,
            float(result.direction),
            result.fillPercentage
            ], dtype=torch.float32, device=device).unsqueeze(0)

            #get prob and value and action for next step
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

            #update reward of player
            ep_rewards[i] += result.reward
            #get the action of the model
            last_actions[i] = action_t.item()
            #chack if player finished the game
            if result.done:
                done_flags[i] = True
                ep_food[i] = result.foodEaten
                ep_len[i] = result.snakeLen
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

    # Gradient Clipping
    torch.nn.utils.clip_grad_norm_(policy.parameters(), max_norm=0.5)
    torch.nn.utils.clip_grad_norm_(critic.parameters(), max_norm=0.5)
    
    optimizer.step()

   


    #gwt avg score
    avg_reward = ep_rewards.mean().item()
    print("avg reward: " ,avg_reward)
    rewards_to_save.append(avg_reward)
    avg_food = int(ep_food.float().mean().item())
    print("avg food eaten :" ,avg_food)
    max_len = torch.max(ep_len).item()
    print("max len is :", max_len)
    len_max_to_save.append(max_len)
    

    #save the learning process
    if(SAVE):
        if episode % SAVE_INTERVAL == 0:
            save_checkpoints(policy,critic,optimizer,rewards_to_save,len_max_to_save,paths)

    if(GRAPH):
        rewardPlotter.show_rewards(rewards_to_save)
        MaxLenPlotter.show_len(len_max_to_save)
    if(VISUALIZER):
        renderer.close()

if(GRAPH):
    rewardPlotter.close()
    MaxLenPlotter.close()

    


