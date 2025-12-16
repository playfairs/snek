# snek/components/powerups/base.py
import pygame
from abc import ABC, abstractmethod
from typing import Optional, Dict, Any

class Powerup(ABC):
    def __init__(self, x: int, y: int, duration: float = 10.0):
        self.x = x
        self.y = y
        self.duration = duration
        self.active = False
        self.start_time = 0
        self.spawn_time = pygame.time.get_ticks()
        self.lifetime = 10000
        self.color = (255, 255, 0)
        self.original_color = self.color
        self.size = 20
        self.collected = False
        self.should_flash = False
        self.last_flash = 0
        self.visible = True

    @abstractmethod
    def apply(self, target) -> None:
        current_time = pygame.time.get_ticks()
        self.active = True
        self.collected = True
        self.start_time = current_time
        
        if not hasattr(self, 'original_values'):
            self.original_values = {}
            if hasattr(target, 'original_speed'):
                self.original_values['speed'] = target.original_speed
            elif hasattr(target, 'speed'):
                self.original_values['speed'] = target.speed
            if hasattr(target, 'double_points'):
                self.original_values['double_points'] = target.double_points
            if hasattr(target, 'invincible'):
                self.original_values['invincible'] = target.invincible
        
        powerup_effect_name = getattr(self, 'name', '').lower()
        if not powerup_effect_name:
            powerup_effect_name = self.__class__.__name__.lower()
            
        if hasattr(target, 'powerup_effects') and powerup_effect_name in target.powerup_effects:
            target.powerup_effects[powerup_effect_name](True)
            
            if hasattr(target, 'active_powerups'):
                target.active_powerups[powerup_effect_name] = current_time + int(self.duration * 1000)
        
        if hasattr(self, '_apply_effect'):
            self._apply_effect(target)
            import re
            powerup_effect_name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', powerup_effect_name)
            powerup_effect_name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', powerup_effect_name).lower()
    
        if hasattr(target, 'powerup_effects') and powerup_effect_name in target.powerup_effects:
            target.powerup_effects[powerup_effect_name](True)
            
            if hasattr(target, 'active_powerups'):
                end_time = current_time + int(self.duration * 1000)
                target.active_powerups[powerup_effect_name] = end_time
        else:
            print(f"[Powerup.apply] WARNING: Could not apply powerup effect for {powerup_effect_name}")
            if not hasattr(target, 'powerup_effects'):
                print("[Powerup.apply] Target has no powerup_effects attribute")
            elif powerup_effect_name not in target.powerup_effects:
                print(f"[Powerup.apply] {powerup_effect_name} not in target's powerup_effects")

    @abstractmethod
    def remove(self, target) -> None:
        if not self.active:
            return
            
        powerup_effect_name = getattr(self, 'name', '').lower()
        if not powerup_effect_name:
            powerup_effect_name = self.__class__.__name__.lower()
            import re
            powerup_effect_name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', powerup_effect_name)
            powerup_effect_name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', powerup_effect_name).lower()
        
        print(f"[Powerup.remove] Removing powerup effect: {powerup_effect_name}")
            
        if hasattr(self, 'original_values'):
            if 'speed' in self.original_values and hasattr(target, 'original_speed'):
                target.speed = target.original_speed
                print(f"[Powerup.remove] Restored speed to {target.speed}")
            if 'double_points' in self.original_values and hasattr(target, 'double_points'):
                target.double_points = False
                print("[Powerup.remove] Disabled double points")
            if 'invincible' in self.original_values and hasattr(target, 'invincible'):
                target.invincible = False
                print("[Powerup.remove] Disabled invincibility")
        
        if hasattr(target, 'powerup_effects') and powerup_effect_name in target.powerup_effects:
            target.powerup_effects[powerup_effect_name](False)
        
        if hasattr(target, 'active_powerups') and powerup_effect_name in target.active_powerups:
            del target.active_powerups[powerup_effect_name]
                
        self.active = False
        print(f"[Powerup.remove] Powerup {powerup_effect_name} removed")

    def update(self) -> bool:
        current_time = pygame.time.get_ticks()
        time_since_spawn = current_time - self.spawn_time
        
        if not self.active and time_since_spawn > self.lifetime:
            return False
            
        if self.active and (current_time - self.start_time) / 1000 > self.duration:
            return False
            
        time_left = self.lifetime - time_since_spawn
        if not self.active and time_left < 3000:
            self.should_flash = True
            flash_speed = max(50, time_left / 60)
            if current_time - self.last_flash > flash_speed:
                self.visible = not self.visible
                self.last_flash = current_time
        
        return True

    def draw(self, surface: pygame.Surface) -> None:
        if not self.collected and self.visible:
            pygame.draw.rect(surface, self.color, 
                           [self.x, self.y, self.size, self.size])
            
            current_time = pygame.time.get_ticks()
            pulse = int(5 * (current_time % 1000) / 1000)
            
            flash_factor = 1.0
            if self.should_flash and not self.visible:
                flash_factor = 0.5
                
            flash_color = (
                min(255, int(self.color[0] * flash_factor * 1.2)),
                min(255, int(self.color[1] * flash_factor * 1.2)),
                min(255, int(self.color[2] * flash_factor * 1.2))
            )
            
            pygame.draw.rect(surface, flash_color, 
                           [self.x + pulse, self.y + pulse, 
                            self.size - 2*pulse, self.size - 2*pulse], 1)

    def get_rect(self) -> pygame.Rect:
        return pygame.Rect(self.x, self.y, self.size, self.size)

    @classmethod
    def create_random(cls, x: int, y: int) -> 'Powerup':
        import random
        from .speed_boost import SpeedBoost
        from .slow_mo import SlowMo
        from .double_points import DoublePoints
        from .invincible import Invincible

        powerup_classes = [SpeedBoost, SlowMo, DoublePoints, Invincible]
        return random.choice(powerup_classes)(x, y)