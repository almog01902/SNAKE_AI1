import torch
#constants
NUM_AGENTS = 32
GRID_SIZE = 20
INITIAL_SNAKE_LENGTH =3
STATE_DIM = 25
ACTION_DIM =4
HIDDEN_SIZE = 256
GAMMA = 0.999
LR = 1e-6
EPS_CLIPS =0.2
CURR_ENTR = 0.02
UPDATE_STEP = 8
MAX_STEPS = 100
NUM_EPISODES = 100000
SAVE_INTERVAL = 10
CHECKPOINT_DIR = "checkpoints"
POLICY_FILE = "policy.pth"
CRITIC_FILE = "critic.pth"
OPTIMIZER_FILE = "optimizer.pth"
REWARDS_FILE = "episode_rewards.pkl"
LENGTH_FILE="max_len.pkl"
VISUALIZER = False
GRAPH = False
SAVE = True

#run thr nural net on gpu
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")