# snek/components/food/food.py
import pygame
import random
from dataclasses import dataclass
from typing import Optional, Tuple

from src.config.settings import SNAKE_BLOCK as SNAKE_BLOCK_SIZE, GAME_AREA_TOP

@dataclass
class Food:
    x: int
    y: int
    food_type: str = 'normal'
    value: int = 10
    color: Tuple[int, int, int] = (255, 0, 0)  # Red by default
    size: int = SNAKE_BLOCK_SIZE

    @classmethod
    def create_random(cls, width: int, height: int, 
                     snake_positions: list, 
                     food_type: str = 'normal') -> 'Food':
        """Create food at a random position not occupied by the snake."""
        while True:
            x = random.randrange(0, width - SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE)
            y = random.randrange(GAME_AREA_TOP, height - SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE)
            
            # Check if position is not occupied by snake
            if not any(seg.x == x and seg.y == y for seg in snake_positions):
                if food_type == 'special':
                    return cls(x, y, 'special', 20, (255, 165, 0))  # Orange
                return cls(x, y)

    def draw(self, surface: pygame.Surface) -> None:
        """Draw the food on the given surface."""
        pygame.draw.rect(surface, self.color, 
                        [self.x, self.y, self.size, self.size])
        
        # Add some visual interest to special food
        if self.food_type == 'special':
            center = (self.x + self.size // 2, self.y + self.size // 2)
            pygame.draw.circle(surface, (255, 255, 0), center, self.size // 3)

    def get_rect(self) -> pygame.Rect:
        """Get the rectangle for collision detection."""
        return pygame.Rect(self.x, self.y, self.size, self.size)