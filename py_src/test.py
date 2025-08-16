import os
import sys


import snake_module
done = False
count =0
while not done:
    count += 1
    action = count % 4  # Example action, replace with your logic
    result = snake_module.step(action)
    print(result.reward, result.done,result.distFoodX, result.distFoodY,)
    done = result.done
    
