import torch
import snake_module
import pygame
import numpy as np
from config import *
from utils import *
from model.actor_critic import make_models
from logger import setup_logger


setup_logger()

# === pygame ===
TILE_SIZE = 30
WINDOW_SIZE = GRID_SIZE * TILE_SIZE
pygame.init()
screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 150)) # שטח נוסף למטה לנתונים
pygame.display.set_caption("Snake AI - Master View")
clock = pygame.time.Clock()
font = pygame.font.SysFont("Arial", 20)

# === Load models ===
policy, critic, _ = make_models(STATE_DIM, ACTION_DIM, LR, device)
load_checkpoints(policy, critic, _, [], [], prepare_checkpoint_paths())

# === Load models ===
policy, critic, _ = make_models(STATE_DIM, ACTION_DIM, LR, device)
load_checkpoints(policy, critic, _, [], [], prepare_checkpoint_paths())

# === Initialize game ===
agent = snake_module.Game(GRID_SIZE, GRID_SIZE, GRID_SIZE // 2, GRID_SIZE // 2, INITIAL_SNAKE_LENGTH)
agent.InitilizeGrid()



# Initial state
last_action = 1  # start moving right
done = False

while not done:

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True

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
        last_action = action_t.item()

    screen.fill((20, 20, 20))

    grid = np.array(agent.getGrid())

    for r in range(GRID_SIZE):
        for c in range(GRID_SIZE):
            rect = (c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE - 1, TILE_SIZE - 1)
            val = grid[r][c]
            
            if val == 1: # ראש
                pygame.draw.rect(screen, (50, 255, 50), rect)
            elif val == 2: # גוף
                pygame.draw.rect(screen, (0, 150, 0), rect)
            elif val == 3: # אוכל
                pygame.draw.rect(screen, (255, 50, 50), rect)
            else: # משבצת ריקה
                pygame.draw.rect(screen, (40, 40, 40), rect, 1)

    #=== HUD ===
    y_text = WINDOW_SIZE + 20
    ACTIONS = ['UP', 'RIGHT', 'DOWN', 'LEFT']
    
    # שורת סטטוס
    info_str = f"Len: {result.snakeLen} | Reward: {result.reward:.2f} | Action: {ACTIONS[last_action]}"
    txt_surface = font.render(info_str, True, (255, 255, 255))
    screen.blit(txt_surface, (10, y_text))
    
    # תצוגת הסתברויות
    probs_str = " | ".join([f"{ACTIONS[i]}: {action_probs[0][i]:.2f}" for i in range(4)])
    probs_surface = font.render(probs_str, True, (200, 200, 0))
    screen.blit(probs_surface, (10, y_text + 40))

    # תצוגת מרחב נגיש
    space_str = f"Space: {result.accessibleSpace:.2f} | Time: {result.timePressure:.2f}"
    space_surface = font.render(space_str, True, (0, 200, 255))
    screen.blit(space_surface, (10, y_text + 80))

    pygame.display.flip()
    done = result.done
    clock.tick(50) # מהירות התצוגה

    
    done = result.done

    if done:
        print(f"Final Score: {result.snakeLen}")
        pygame.time.wait(2000) # מחכה 2 שניות לפני סגירה



