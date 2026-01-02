import torch
import snake_module
from config import *
from utils import *
from model.actor_critic import make_models
from logger import setup_logger

setup_logger()
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
        result.accessibleSpaceN,
        result.accessibleSpaceS,
        result.accessibleSpaceE,
        result.accessibleSpaceW,
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


    ACTIONS = ['UP', 'RIGHT', 'DOWN', 'LEFT']
    probs_dict = {ACTIONS[i]: f"{action_probs[0][i]:.2f}" for i in range(4)}
    print(f"\n" + "="*40)
    print(f"Step Result | Length: {result.snakeLen} | Reward: {result.reward:.2f}")
    print(f"Action to be taken: {ACTIONS[action_t]}")
    print(f"Probabilities:  {probs_dict}")

    grid_size = 20 

    # המרחק מהמרכז (0.5) כפול 2 נותן ערך בין 0 ל-1, ואז כפול גודל הלוח
    dist_x_tiles = abs(result.distFoodX - 0.5) * 2 * (grid_size - 1)
    dist_y_tiles = abs(result.distFoodY - 0.5) * 2 * (grid_size - 1)
    current_manhattan = dist_x_tiles + dist_y_tiles

    print(f"\n--- Distance Diagnostics ---")
    print(f"Estimated Distance to Food: {current_manhattan:.2f} tiles")
    print(f"Raw Inputs: X={result.distFoodX:.2f}, Y={result.distFoodY:.2f}")
        
    print(f"\n--- Movement Strategy ---")
    print(f"Probabilities:  {probs_dict}")
    print(f"Action Chosen:  {['UP', 'RIGHT', 'DOWN', 'LEFT'][last_action]}")
    
    print(f"\n--- Spatial Awareness (Look-ahead) ---")
    print(f"Current Space:  {result.accessibleSpace:.2f}")
    print(f"Future N: {result.accessibleSpaceN:.2f} | Future S: {result.accessibleSpaceS:.2f}")
    print(f"Future E: {result.accessibleSpaceE:.2f} | Future W: {result.accessibleSpaceW:.2f}")
    
    print(f"\n--- State Info ---")
    print(f"Hunger (Time): {result.timePressure:.2f} | Fill%: {result.fillPercentage:.2f}")
    print("="*40)

if VISUALIZER:
    renderer.close()
