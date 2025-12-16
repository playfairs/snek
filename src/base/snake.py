# snek/core/snake.py
import pygame
from dataclasses import dataclass
from typing import List, Tuple
import math
import random

from src.config.settings import (
    SNAKE_BLOCK, POWERUP_DURATION, SNAKE_SKINS, 
    POWERUP_COLORS, WHITE, GAME_AREA_TOP
)

@dataclass
class Segment:
    x: int
    y: int

class Snake:
    def __init__(self, x: int, y: int, block_size: int = SNAKE_BLOCK, skin: str = 'default'):
        self.segments: List[Segment] = [Segment(x, y)]
        self.block_size = block_size
        self.x_change = block_size
        self.y_change = 0
        self.length = 1
        self.score = 0
        self.original_speed = 10
        self.speed = self.original_speed
        self.powerup_active = None
        self.powerup_end_time = 0
        self.active_powerups = {}
        self.powerup_effects = {
            'speed_boost': self._apply_speed_boost,
            'slow_mo': self._apply_slow_mo,
            'double_points': self._apply_double_points,
            'invincible': self._apply_invincible
        }
        self.skin = skin
        self.invincible = False
        self.double_points = False
        self.last_move_time = 0
        self.move_delay = 100

    def _apply_speed_boost(self, activate: bool = True) -> None:
        current_time = pygame.time.get_ticks()
        
        if activate:
            if not hasattr(self, 'original_speed'):
                self.original_speed = self.speed
            if 'speed_boost' not in self.active_powerups:
                self.speed = self.original_speed * 1.5
            self.active_powerups['speed_boost'] = current_time + 10000
        else:
            if 'speed_boost' in self.active_powerups and \
               self.active_powerups['speed_boost'] <= current_time:
                if hasattr(self, 'original_speed'):
                    self.speed = self.original_speed
                del self.active_powerups['speed_boost']
            
    def _apply_slow_mo(self, activate: bool = True) -> None:
        if activate:
            self.speed = self.original_speed * 0.5
            self.active_powerups['slow_mo'] = pygame.time.get_ticks() + 8000
        else:
            self.speed = self.original_speed
            if 'slow_mo' in self.active_powerups:
                del self.active_powerups['slow_mo']

    def _apply_double_points(self, activate: bool = True) -> None:
        self.double_points = activate
        if activate:
            self.active_powerups['double_points'] = pygame.time.get_ticks() + 15000
        elif 'double_points' in self.active_powerups:
            del self.active_powerups['double_points']

    def _apply_invincible(self, activate: bool = True) -> None:
        self.invincible = activate
        if activate:
            self.active_powerups['invincible'] = pygame.time.get_ticks() + 12000
        elif 'invincible' in self.active_powerups:
            del self.active_powerups['invincible']

    def move(self, current_time: int) -> None:
        adjusted_delay = max(50, 200 - (self.speed * 10))
        
        if current_time - self.last_move_time < adjusted_delay:
            return

        self.last_move_time = current_time
        head = self.segments[0]
        
        new_x = head.x + self.x_change
        new_y = head.y + self.y_change
        
        screen_info = pygame.display.get_surface().get_size()
        screen_width = screen_info[0]
        screen_height = screen_info[1]
        
        if new_x >= screen_width:
            new_x = 0
        elif new_x < 0:
            new_x = screen_width - self.block_size
            
        if new_y < GAME_AREA_TOP:
            new_y = screen_height - self.block_size
        elif new_y >= screen_height:
            new_y = GAME_AREA_TOP
        
        new_head = Segment(new_x, new_y)
        self.segments.insert(0, new_head)
        if len(self.segments) > self.length:
            self.segments.pop()

    def grow(self, points: int = 10) -> None:
        self.length += 1
        self.score += points * (2 if self.double_points else 1)

    def change_direction(self, direction: str) -> None:
        if direction == "UP" and self.y_change <= 0:
            self.x_change = 0
            self.y_change = -self.block_size
        elif direction == "DOWN" and self.y_change >= 0:
            self.x_change = 0
            self.y_change = self.block_size
        elif direction == "LEFT" and self.x_change <= 0:
            self.x_change = -self.block_size
            self.y_change = 0
        elif direction == "RIGHT" and self.x_change >= 0:
            self.x_change = self.block_size
            self.y_change = 0

    def check_collision(self, width: int, height: int, game_area_top: int) -> bool:
        if self.invincible:
            return False

        head = self.segments[0]
        
        for segment in self.segments[1:]:
            if head.x == segment.x and head.y == segment.y:
                return True
        return False

    def apply_powerup(self, powerup_type: str, duration: int) -> None:
        if powerup_type not in POWERUP_TYPES:
            return

        if self.powerup_active:
            self.remove_powerup()

        self.powerup_active = powerup_type
        self.powerup_end_time = pygame.time.get_ticks() + (duration * 1000)
        
        if powerup_type in self.powerup_effects:
            self.powerup_effects[powerup_type](True)

    def remove_powerup(self) -> None:
        if self.powerup_active and self.powerup_active in self.powerup_effects:
            self.powerup_effects[self.powerup_active](False)
        self.powerup_active = None

    def update_powerup(self) -> None:
        if self.powerup_active and pygame.time.get_ticks() > self.powerup_end_time:
            self.remove_powerup()

    def draw(self, surface: pygame.Surface) -> None:
        for i, segment in enumerate(self.segments):
            is_head = (i == 0)
            color = (SNAKE_SKINS[self.skin]['head_color'] if is_head 
                    else SNAKE_SKINS[self.skin]['body_color'])
            
            pygame.draw.rect(surface, color, 
                           [segment.x, segment.y, self.block_size, self.block_size])
            
            if is_head:
                eye_size = self.block_size // 5
                eye_offset = self.block_size // 4
                
                pygame.draw.circle(
                    surface, 
                    WHITE, 
                    (segment.x + eye_offset, segment.y + eye_offset), 
                    eye_size
                )
                
                pygame.draw.circle(
                    surface, 
                    WHITE, 
                    (segment.x + self.block_size - eye_offset, 
                     segment.y + eye_offset), 
                    eye_size
                )

    def get_head_rect(self) -> pygame.Rect:
        head = self.segments[0]
        return pygame.Rect(head.x, head.y, self.block_size, self.block_size)

    def get_rects(self) -> List[pygame.Rect]:
        return [pygame.Rect(seg.x, seg.y, self.block_size, self.block_size) 
                for seg in self.segments]