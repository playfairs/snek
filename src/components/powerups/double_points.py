# snek/components/powerups/double_points.py
from .base import Powerup

class DoublePoints(Powerup):
    def __init__(self, x: int, y: int):
        super().__init__(x, y, duration=15.0)
        self.color = (255, 215, 0)
        self.name = "double_points"

    def apply(self, target) -> None:
        super().apply(target)
        if hasattr(target, 'powerup_effects') and 'double_points' in target.powerup_effects:
            target.powerup_effects['double_points'](True)

    def remove(self, target) -> None:
        if hasattr(target, 'powerup_effects') and 'double_points' in target.powerup_effects:
            target.powerup_effects['double_points'](False)
        super().remove(target)