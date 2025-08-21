import os
import config
import torch
import pickle
import matplotlib.pyplot as plt

def prepare_checkpoint_paths():#get the dir for chackpoints
    os.makedirs(config.CHECKPOINT_DIR, exist_ok=True)
    policy_path = os.path.join(config.CHECKPOINT_DIR, config.POLICY_FILE)
    critic_path = os.path.join(config.CHECKPOINT_DIR, config.CRITIC_FILE)
    optimizer_path = os.path.join(config.CHECKPOINT_DIR, config.OPTIMIZER_FILE)
    rewards_path = os.path.join(config.CHECKPOINT_DIR, config.REWARDS_FILE)
    return policy_path, critic_path, optimizer_path, rewards_path

def save_checkpoints(policy, critic, optimizer, episode_rewards, paths):#save chackpoints in dir
    policy_path, critic_path, optimizer_path, rewards_path = paths
    torch.save(policy.state_dict(), policy_path)
    torch.save(critic.state_dict(), critic_path)
    torch.save(optimizer.state_dict(), optimizer_path)
    with open(rewards_path, "wb") as f:
        pickle.dump(episode_rewards, f)


def load_checkpoints(policy, critic, optimizer, episode_rewards,paths ):
    policy_path, critic_path, optimizer_path, rewards_path = paths
    
    if os.path.exists(policy_path):
        policy.load_state_dict(torch.load(policy_path, map_location=config.DEVICE))
    if os.path.exists(critic_path):
        critic.load_state_dict(torch.load(critic_path, map_location=config.DEVICE))
    if os.path.exists(optimizer_path):
        optimizer.load_state_dict(torch.load(optimizer_path, map_location=config.DEVICE))
    
    if os.path.exists(rewards_path):
        with open(rewards_path, "rb") as f:
            episode_rewards += pickle.load(f)  

class RewardPlotter:
    def __init__(self):
        plt.ion()
        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], marker="o", linestyle="-", label="Average Reward")
        self.ax.set_xlabel("Episode")
        self.ax.set_ylabel("Avg Reward")
        self.ax.set_title("Training Progress")
        self.ax.legend()
        self.rewards = []

    def update(self, avg_reward):
        self.rewards.append(avg_reward)
        self.ax.clear()
        self.ax.plot(self.rewards, marker="o", linestyle="-", label="Average Reward")
        self.ax.set_xlabel("Episode")
        self.ax.set_ylabel("Avg Reward")
        self.ax.set_title("Training Progress")
        self.ax.legend()
        plt.pause(0.01)

    def close(self):
        plt.ioff()
        plt.show()