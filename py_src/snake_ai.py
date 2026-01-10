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
import random
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

    torch.cuda.empty_cache()
    gc.collect()
    episode = len(rewards_to_save)

    # --- 2. איפוס מהיר ב-C++ ---

    MIN_GRID = 10
    MAX_GRID = 40
    grid_size = random.randint(MIN_GRID,MAX_GRID)
    batch_env = snake_module.SnakeBatch(NUM_AGENTS, grid_size, grid_size, grid_size // 2, grid_size // 2, INITIAL_SNAKE_LENGTH)

    print(f"Episode {episode} | Training on Grid: {grid_size}x{grid_size}")
    # אתחול משתנים למעקב ב-Python
    last_actions = [1 for _ in range(NUM_AGENTS)]  # כולם מתחילים ימינה
    done_flags = [False for _ in range(NUM_AGENTS)]
    
    # סטטיסטיקות
    ep_rewards = torch.zeros(NUM_AGENTS, device=device) # שים לב: ב-GPU לחישוב מהיר
    ep_food = torch.zeros(NUM_AGENTS, dtype=torch.int32)
    ep_len = torch.zeros(NUM_AGENTS, dtype=torch.int32)

    # באפרים (PPO Buffers)
    all_states = []
    all_actions = []
    all_log_probs = []
    all_values = []
    all_rewards = []
    all_dones = []

    # --- 3. הלולאה הראשית (סופר מהירה) ---
    while not all(done_flags):
        

        # === שלב ה-C++: קריאה אחת לכולם ===
        # שולחים את הפעולות ואת הסטטוס (מי חי/מת)
        # מקבלים חזרה 4 וקטורים שטוחים (Flat Vectors)
        flat_states, flat_rewards, flat_dones, flat_infos = batch_env.step_all(last_actions, done_flags)

        # === המרה ל-Tensors (על ה-GPU) ===
        # Reshape: הופכים את הרשימה השטוחה לטנזור בגודל [32, 25]
        batch_states = torch.tensor(flat_states, dtype=torch.float32, device=device).view(NUM_AGENTS, STATE_DIM)
        
        # Rewards & Dones
        rewards_tensor = torch.tensor(flat_rewards, dtype=torch.float32, device=device)
        dones_tensor = torch.tensor(flat_dones, dtype=torch.float32, device=device)

        # === עדכון לוגיקה וסטטיסטיקה בפייתון ===
        # אנחנו צריכים לעדכן את done_flags ידנית כדי שהלולאה תדע מתי לעצור
        # וגם לשמור סטטיסטיקות לנחשים שמתו בסיבוב הזה
        for i in range(NUM_AGENTS):
            if not done_flags[i]:
                # עדכון Reward מצטבר לגרף
                ep_rewards[i] += flat_rewards[i]
                
                # אם הנחש מת הרגע
                if flat_dones[i] > 0.5: # 1.0 = True
                    done_flags[i] = True
                    # שליפת נתונים מ-infos (סידרנו אותם: [food0, len0, food1, len1...])
                    ep_food[i] = int(flat_infos[i * 2])
                    ep_len[i] = int(flat_infos[i * 2 + 1])

        # === רשת נוירונית (Inference) ===
        with torch.no_grad():
            action_probs = policy(batch_states)
            values = critic(batch_states)
            
            dist = Categorical(action_probs)
            actions = dist.sample()
            log_probs = dist.log_prob(actions)

        # === שמירה לבאפרים ===
        # שומרים רק אם יש עדיין נחשים חיים (אחרת זה סתם באפר זבל בסוף)
        if not all(done_flags):
            all_states.append(batch_states)
            all_actions.append(actions)
            all_log_probs.append(log_probs)
            all_values.append(values)
            all_rewards.append(rewards_tensor)
            all_dones.append(dones_tensor)

        # === הכנה לסיבוב הבא ===
        # המרה חזרה ל-list פשוט של int עבור C++
        last_actions = actions.cpu().numpy().tolist()


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


    START_REDUCING_EPISODE = 6900
    DURATION = 1000
    START_ENTROPY = 0.0005
    END_ENTROPY = 0.0001
    entropyScheduler = EntropyScheduler(START_ENTROPY,END_ENTROPY,START_REDUCING_EPISODE,DURATION)
    epis = len(rewards_to_save)
    curr_entr = entropyScheduler.get_ent_coeff(epis)
    print(f"current entr cuff :{curr_entr}")
    
    MINI_BATCH_SIZE = 1024
    batch_size = state_b.size(0)

    for _ in range(UPDATE_STEP):
        # יצירת אינדקסים רנדומליים וערבוב שלהם
        indices = torch.randperm(batch_size)
        
        # לולאה שעוברת על הנתונים בחתיכות קטנות
        for start in range(0, batch_size, MINI_BATCH_SIZE):
            end = start + MINI_BATCH_SIZE
            mb_idx = indices[start:end]

            # שליפת החתיכה הרלוונטית מכל באפר
            mb_states = state_b[mb_idx]
            mb_actions = actions_b[mb_idx]
            mb_old_log_probs = old_log_probs_b[mb_idx]
            mb_advantages = advantages_normalized_b[mb_idx]
            mb_returns = returns_b[mb_idx]
            mb_mask = mask_b[mb_idx]

            # חישוב הסתברויות וערכים (רק על המיני-באץ')
            new_action_probs = policy(mb_states)
            new_values = critic(mb_states).squeeze()

            dist = Categorical(new_action_probs)
            new_log_probs = dist.log_prob(mb_actions)
            
            # Ratio
            ratio = torch.exp(new_log_probs - mb_old_log_probs)

            # PPO Clipped Loss
            surr1 = ratio * mb_advantages
            surr2 = torch.clamp(ratio, 1 - EPS_CLIPS, 1 + EPS_CLIPS) * mb_advantages
            
            # Loss חישוב (שמירה על הסינטקס המדויק שלך עם המסיכה)
            actor_loss = -(torch.min(surr1, surr2) * mb_mask).sum() / (mb_mask.sum() + 1e-8)
            critic_loss_raw = F.mse_loss(new_values, mb_returns.view(-1), reduction='none')
            critic_loss = (critic_loss_raw * mb_mask).sum() / (mb_mask.sum() + 1e-8)

            entropy_masked = (dist.entropy() * mb_mask).sum() / (mb_mask.sum() + 1e-8)
            
            # Loss סופי
            loss = actor_loss + 0.5 * critic_loss - curr_entr * entropy_masked

            # עדכון משקולות
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

if(GRAPH):
    rewardPlotter.close()
    MaxLenPlotter.close()

    