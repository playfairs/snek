from .base import Powerup

class Invincible(Powerup):
    def __init__(self, x: int, y: int):
        super().__init__(x, y, duration=12.0)
        self.color = (255, 0, 255)
        self.name = "invincible"
        self.duration = 12.0

    def apply(self, target) -> None:
        super().apply(target)
        self._apply_effect(target)

    def remove(self, target) -> None:
        if hasattr(target, 'invincible'):
            target.invincible = False
        super().remove(target)