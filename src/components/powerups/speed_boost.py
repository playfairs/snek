# snek/components/powerups/speed_boost.py
from .base import Powerup

class SpeedBoost(Powerup):
    def __init__(self, x: int, y: int):
        super().__init__(x, y, duration=10.0)
        self.color = (0, 191, 255)
        self.name = "speed_boost"

    def apply(self, target) -> None:
        super().apply(target)

    def remove(self, target) -> None:
        super().remove(target)