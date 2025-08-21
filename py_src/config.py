import torch
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
POLICY_FILE = "policy.pth"
CRITIC_FILE = "critic.pth"
OPTIMIZER_FILE = "optimizer.pth"
REWARDS_FILE = "episode_rewards.pkl"

#run thr nural net on gpu
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")