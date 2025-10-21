import torch
import snake_module
from config import *
from utils import *
from model.actor_critic import make_models

# === Load models ===
policy, critic, _ = make_models(STATE_DIM, ACTION_DIM, LR, device)
load_checkpoints(policy, critic, _, [], [], prepare_checkpoint_paths())

# === Initialize game ===
agent = snake_module.Game(GRID_SIZE, GRID_SIZE, GRID_SIZE // 2, GRID_SIZE // 2, INITIAL_SNAKE_LENGTH)
agent.InitilizeGrid()

# Optional: visualizer

renderer = AgentRenderer([agent])

# Initial state
last_action = 1  # start moving right
done = False

while not done:

    renderer.update()

    # Get state
    result = agent.step(last_action)
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
        device=device,
    ).unsqueeze(0)

    # Get action
    with torch.no_grad():  # לא רוצים לעדכן גרדיאנטים
        action_probs = policy(state)
        action_t = torch.argmax(action_probs)  # תמיד בוחר את הטוב ביותר

    last_action = action_t.item()
    done = result.done

    # Optional: print progress
    print(f"Snake length: {result.snakeLen}, Food eaten: {result.foodEaten}, Reward: {result.reward} ,Danger: {result.distToDangerForward}")

if VISUALIZER:
    renderer.close()
