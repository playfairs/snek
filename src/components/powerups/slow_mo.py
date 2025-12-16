# snek/components/powerups/slow_mo.py
from .base import Powerup

class SlowMo(Powerup):
    def __init__(self, x: int, y: int):
        super().__init__(x, y, duration=8.0)
        self.color = (147, 112, 219)
        self.name = "slow_mo"

    def apply(self, target) -> None:
        super().apply(target)
        if hasattr(target, 'powerup_effects') and 'slow_mo' in target.powerup_effects:
            target.powerup_effects['slow_mo'](True)

    def remove(self, target) -> None:
        if hasattr(target, 'powerup_effects') and 'slow_mo' in target.powerup_effects:
            target.powerup_effects['slow_mo'](False)
        super().remove(target)