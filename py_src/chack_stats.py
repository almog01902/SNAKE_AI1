from utils import *
episode_rewards =[]
rewards_path = os.path.join(config.CHECKPOINT_DIR, config.REWARDS_FILE)
REWARDS_FILE = "episode_rewards.pkl"
if os.path.exists(rewards_path):
        with open(rewards_path, "rb") as f:
            episode_rewards += pickle.load(f) 

plotter = RewardPlotter()

plotter.show_rewards(episode_rewards)
plotter.close()