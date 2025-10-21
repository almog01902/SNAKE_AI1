from utils import *
episode_rewards =[]
episode_max_len  =[]
rewards_path = os.path.join(config.CHECKPOINT_DIR, config.REWARDS_FILE)
len_path = os.path.join(config.CHECKPOINT_DIR,config.LENGTH_FILE)
if os.path.exists(rewards_path):
        with open(rewards_path, "rb") as f:
            episode_rewards += pickle.load(f) 
if os.path.exists(len_path):
    with open(len_path,"rb") as f:
        episode_max_len += pickle.load(f)

rewardPlotter = RewardPlotter()
max_len_plotter = MaxLenPlotter()

max_len_plotter.show_len(episode_max_len)
rewardPlotter.show_rewards(episode_rewards)
max_len_plotter.close()
rewardPlotter.close()