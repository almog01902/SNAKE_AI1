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
from entropy_decline import EntropyScheduler




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
    print("Episode:", len(rewards_to_save))

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
    all_states = []
    all_actions = []
    all_log_probs = []
    all_values = []
    all_rewards = []
    all_dones = []

    while not all(done_flags):
        #buffer for step
        step_state = []
        step_actions = []
        step_log_probs = []
        step_values = []
        step_rewards = []
        step_dones = []

        if(VISUALIZER):
            renderer.update()

        for i, agent in enumerate(agents):

            if done_flags[i]:#for agents who finished the game we make empty stats
                reward_value = 0.0 
                step_state.append(torch.zeros(STATE_DIM,device=device))
                step_actions.append(torch.tensor(0, device=device))
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
            result.isUp,
            result.isDown,
            result.isLeft,
            result.isRight,
            result.fillPercentage,
            result.accessibleSpace,
            result.accessibleSpaceN,
            result.accessibleSpaceS,
            result.accessibleSpaceE,
            result.accessibleSpaceW,
            result.diffX,
            result.diffY,
            result.timePressure
            ], dtype=torch.float32, device=device).unsqueeze(0)

            #get prob and value and action for next step
            action_probs = policy(state)
            value = critic(state)
            dist = Categorical(action_probs)
            action_t = dist.sample()
            log_prob = dist.log_prob(action_t)

            # save step data
            step_state.append(state.squeeze())
            step_actions.append(action_t.squeeze())
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
        all_states.append(torch.stack(step_state))
        all_actions.append(torch.stack(step_actions))
        all_log_probs.append(torch.stack(step_log_probs))
        all_values.append(torch.stack(step_values))
        all_rewards.append(torch.stack(step_rewards))
        all_dones.append(torch.stack(step_dones))


    #calaulate adventeges
    state_b = torch.stack(all_states).view(-1,STATE_DIM).detach()
    actions_b = torch.stack(all_actions).view(-1).detach()
    old_log_probs_b = torch.stack(all_log_probs).view(-1).detach()
    mask_b = (1.0 - torch.stack(all_dones).view(-1)).detach()
    # === GAE / returns ===
    returns = []
    G = torch.zeros(NUM_AGENTS,device=device)
    for r, d in zip(reversed(all_rewards), reversed(all_dones)):
        G = r + GAMMA * G * (torch.ones(NUM_AGENTS,device=device) - d)
        returns.insert(0, G.clone())
    returns = torch.stack(returns)

    values_b = torch.stack(all_values).view(-1).detach() 
    returns_b = returns.detach().view(-1)

    # advantages
    values = torch.stack(all_values)
    advantages_b = returns_b - values_b
    valid_advantages = advantages_b[mask_b > 0.5] # רק הצעדים שבהם הנחש חי
    if valid_advantages.numel() > 0:
        adj_mean = valid_advantages.mean()
        adj_std = valid_advantages.std()
        advantages_normalized_b = (advantages_b - adj_mean) / (adj_std + 1e-8)
    else:
        advantages_normalized_b = advantages_b

    advantages_normalized_b = advantages_normalized_b * mask_b



    

    #####
    for _ in range(UPDATE_STEP):
        #calaulate the probibality
        new_action_probs = policy(state_b)
        new_values = critic(state_b).squeeze()
        

        dist = Categorical(new_action_probs)
        new_log_probs = dist.log_prob(actions_b)
        entropy = dist.entropy().mean()

        # Ratio 
        ratio = torch.exp(new_log_probs - old_log_probs_b)

        # PPO Clipped Loss 
        surr1 = ratio * advantages_normalized_b
        surr2 = torch.clamp(ratio, 1 - EPS_CLIPS, 1 + EPS_CLIPS) * advantages_normalized_b
        
        actor_loss = -(torch.min(surr1, surr2)* mask_b).sum() / (mask_b.sum()+1e-8)
        critic_loss_raw = F.mse_loss(new_values, returns_b.view(-1), reduction='none')
        critic_loss = (critic_loss_raw*mask_b).sum() / (mask_b.sum()+ 1e-8)

        entropy_masked = (dist.entropy() * mask_b).sum() / (mask_b.sum() + 1e-8)
        
        # Loss 
        loss = actor_loss + 0.5 * critic_loss - CURR_ENTR * entropy_masked

        optimizer.zero_grad()
        loss.backward()
        # Gradient Clipping
        torch.nn.utils.clip_grad_norm_(policy.parameters(), max_norm=0.5)
        torch.nn.utils.clip_grad_norm_(critic.parameters(), max_norm=0.5)
        optimizer.step()

    #get avg score
    avg_reward = ep_rewards.mean().item()
    print("avg reward: " ,avg_reward)
    rewards_to_save.append(avg_reward)
    avg_food = int(ep_food.float().mean().item())
    print("avg food eaten :" ,avg_food)
    max_len = torch.max(ep_len).item()
    print("max len is :", max_len)
    len_max_to_save.append(max_len)
    print(f"entropy is : {entropy_masked.item():.4f}")
    print(f"loss is :{loss.item():.4f}")
    

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

    