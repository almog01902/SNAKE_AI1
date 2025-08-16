# snake_module.pyi
from typing import Any

class stepResult:
    reward: float
    done: bool
    distFoodX: int
    distFoodY: int
    distToDangerForward: int
    distToDangerLeft: int
    distToDangerRight: int
    direction: int

def step(action: int) -> stepResult: ...
