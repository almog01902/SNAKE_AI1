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

class Game:
    def __init__(self, gridRows: int, gridCols: int, startX: int, startY: int, initialLength: int) -> None: ...
    
    def toggleRender(self) -> None: ...
    
    def step(self, action: int) -> stepResult: ...

    def InitilizeGrid(self) -> None: ...

    def render(self) -> None: ...
    
    # Additional methods can be added here if needed
