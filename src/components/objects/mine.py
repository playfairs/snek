import pygame
import random
import math

class Mine:
    def __init__(self, x: int, y: int, size: int = 20):
        self.x = x
        self.y = y
        self.size = size
        self.color = (0, 0, 0)
        self.danger_color = (255, 0, 0)
        self.rotation = 0
        self.last_rotate = pygame.time.get_ticks()
        self.rotate_delay = 50
        self.spawn_time = 0
        self.lifetime = 10000

    def update(self) -> None:
        current_time = pygame.time.get_ticks()
        if current_time - self.last_rotate > self.rotate_delay:
            self.rotation = (self.rotation + 5) % 360
            self.last_rotate = current_time

    def draw(self, surface: pygame.Surface) -> None:
        pygame.draw.circle(surface, self.color, (self.x + self.size//2, self.y + self.size//2), self.size//2)
        
        center_x, center_y = self.x + self.size//2, self.y + self.size//2
        radius = self.size // 3
        
        angle = math.radians(self.rotation)
        cos_val = math.cos(angle) * radius
        sin_val = math.sin(angle) * radius
        
        pygame.draw.line(surface, self.danger_color,
                        (center_x - cos_val, center_y - sin_val),
                        (center_x + cos_val, center_y + sin_val), 3)
        
        pygame.draw.line(surface, self.danger_color,
                        (center_x - sin_val, center_y + cos_val),
                        (center_x + sin_val, center_y - cos_val), 3)

    def get_rect(self) -> pygame.Rect:
        return pygame.Rect(self.x, self.y, self.size, self.size)

    @classmethod
    def create_random(cls, width: int, height: int, min_x: int = 0, min_y: int = 0, size: int = 20):
        max_x = (width // size - 1) * size
        max_y = (height // size - 1) * size
        
        x = random.randint(min_x // size, max_x // size) * size
        y = random.randint(min_y // size, max_y // size) * size
        
        return cls(x, y, size)