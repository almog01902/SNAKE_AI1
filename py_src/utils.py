import os
import config
import torch
import pickle
import matplotlib.pyplot as plt
import time
import numpy as np

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
        policy.load_state_dict(torch.load(policy_path, map_location=config.device))
    if os.path.exists(critic_path):
        critic.load_state_dict(torch.load(critic_path, map_location=config.device))
    if os.path.exists(optimizer_path):
        optimizer.load_state_dict(torch.load(optimizer_path, map_location=config.device))
    
    if os.path.exists(rewards_path):
        with open(rewards_path, "rb") as f:
            episode_rewards += pickle.load(f)  

#class for graph
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
        x = list(range (1,len(self.rewards)+1))
        self.line.set_data(x,self.rewards)
        self.ax.relim()        
        self.ax.autoscale()   
        plt.pause(0.01)
    
    def show_rewards(self,rewards):
        plt.figure(figsize=(10,5))
        plt.plot(range(1, len(rewards)+1), rewards, marker="o", linestyle="-", label="Average Reward")
        plt.xlabel("Episode")
        plt.ylabel("Avg Reward")
        plt.title("Training Progress")
        plt.legend()
        plt.show()

    def close(self):
        plt.ioff()
        plt.show()

#class for visulation of agents
class AgentRenderer:
    def __init__(self,agents,rows =2,cols =5):
        self.agents = agents
        self.num_agents = len(agents)
        self.rows = rows
        self.cols = cols

        self.fig , self.axes = plt.subplots(rows,cols, figsize=(4*cols,4*rows))
        self.axes = self.axes.flatten()

        self.images =[]
        for i in range(self.num_agents):
            grid = np.array(agents[i].getGrid())
            img = self.axes[i].imshow(grid,cmap="viridis",interpolation="none")
            self.axes[i].set_title(f"agent {i}")
            self.axes[i].axis("off")
            self.images.append(img)
        plt.ion()
        plt.show()

    def update(self,pause=0.1):
        for i,agent in enumerate(self.agents):
            grid = np.array(agent.getGrid())
            self.images[i].set_data(grid)
        plt.pause(pause)
        plt.draw()

    def close(self):
        plt.ioff()
        plt.close(self.fig)

class NetworkVisualizer:
    def __init__(self, policy):
        self.policy = policy
        self.layers = [policy.fc1, policy.fc2, policy.out]
        self.layer_sizes = [self.layers[0].in_features] + [l.out_features for l in self.layers]

        self.fig, self.ax = plt.subplots(figsize=(12, 6))
        self.ax.axis('off')
        self.x_positions = np.arange(len(self.layer_sizes))
        self.y_positions = [np.linspace(0, 1, size) for size in self.layer_sizes]

        # ציור ראשוני של נוירונים
        self.neuron_scatter = []
        for i, layer_y in enumerate(self.y_positions):
            s = self.ax.scatter([self.x_positions[i]]*len(layer_y), layer_y, s=100, color='black', zorder=10)
            self.neuron_scatter.append(s)

        # יצירת קווים ראשוניים (רק כדי לשמור את האובייקטים)
        self.lines = []
        for i, (layer, next_layer_y) in enumerate(zip(self.layers, self.y_positions[1:])):
            weights = layer.weight.detach().cpu().numpy()
            layer_lines = []
            for pre_idx, pre_y in enumerate(self.y_positions[i]):
                for post_idx, post_y in enumerate(next_layer_y):
                    line, = self.ax.plot([self.x_positions[i], self.x_positions[i+1]], [pre_y, post_y],
                                         color='gray', linewidth=0.5, alpha=0.5)
                    layer_lines.append(line)
            self.lines.append(layer_lines)

        plt.ion()
        plt.show()

    def update(self):
        # עדכון המשקלים של הקווים
        for i, (layer, next_layer_y) in enumerate(zip(self.layers, self.y_positions[1:])):
            weights = layer.weight.detach().cpu().numpy()
            idx = 0
            for pre_idx, pre_y in enumerate(self.y_positions[i]):
                for post_idx, post_y in enumerate(next_layer_y):
                    w = weights[post_idx, pre_idx]
                    color = 'blue' if w > 0 else 'red'
                    self.lines[i][idx].set_color(color)
                    self.lines[i][idx].set_linewidth(abs(w)*2)
                    idx += 1
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()
    def close(self):
        plt.ioff()
        plt.close()
