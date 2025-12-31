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

    result = agent.step(last_action)
    # Get state
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
        result.diffX,
        result.diffY,
        result.timePressure
        ], dtype=torch.float32, device=device).unsqueeze(0)

    # Get action
    with torch.no_grad():  # לא רוצים לעדכן גרדיאנטים
        action_probs = policy(state)
        action_t = torch.argmax(action_probs)  # תמיד בוחר את הטוב ביותר
        state_value = critic(state).item()
        probs_dict = {
        "Up": f"{action_probs[0][0]:.2f}",
        "Down": f"{action_probs[0][1]:.2f}",
        "Left": f"{action_probs[0][2]:.2f}",
        "Right": f"{action_probs[0][3]:.2f}"
    }

    last_action = action_t.item()
    done = result.done

    print(f"--- Step Info ---")
    print(f"Action Probs: {probs_dict}")
    print(f"Critic Value: {state_value:.2f}")
    print(f"Accessible Space: {result.accessibleSpace:.2f}, Hunger: {result.timePressure:.2f}")
    print(f"Length: {result.snakeLen}, Reward: {result.reward:.2f}")

if VISUALIZER:
    renderer.close()
