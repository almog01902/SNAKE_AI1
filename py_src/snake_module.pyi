from typing import Any, List

class stepResult:
    # Metadata
    reward: float
    done: bool
    won: bool
    foodEaten: int
    snakeLen: int
    
    # Observation State (14 inputs)
    distFoodX: float
    distFoodY: float
    headX_norm: float
    headY_norm: float
    
    # Radar (8 directions)
    distN: float
    distS: float
    distE: float
    distW: float
    distNW: float
    distNE: float
    distSW: float
    distSE: float

    isUp : float
    isDown: float
    isLeft: float
    isRight:float
    
    # Extras
    direction: int
    fillPercentage: float

class Game:
    def __init__(self, gridRows: int, gridCols: int, startX: int, startY: int, initialLength: int) -> None: ...
    
    def step(self, action: int) -> stepResult: ...

    def InitilizeGrid(self) -> None: ...

    def render(self) -> None: ...

    def getGrid(self) -> List[List[int]]: ...