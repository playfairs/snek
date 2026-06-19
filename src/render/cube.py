import pygame
import src.core.state as state

def draw_cube(x, y, size, color, is_head=False):
    base_r, base_g, base_b = color
    
    if is_head:
        highlight = (min(255, base_r + 80), min(255, base_g + 80), min(255, base_b + 40))
        shadow = (max(0, base_r - 60), max(0, base_g - 40), max(0, base_b - 20))
        
        pygame.draw.rect(state.dis, color, [x, y, size, size])
        
        pygame.draw.line(state.dis, highlight, (x, y), (x + size-1, y), 2)
        pygame.draw.line(state.dis, highlight, (x, y), (x, y + size-1), 2)
        
        pygame.draw.line(state.dis, shadow, (x + 1, y + size-1), (x + size-1, y + size-1), 2)
        pygame.draw.line(state.dis, shadow, (x + size-1, y + 1), (x + size-1, y + size-1), 2)
        
        inner_highlight = (min(255, base_r + 100), min(255, base_g + 100), min(255, base_b + 60))
        pygame.draw.rect(state.dis, inner_highlight, [x + 2, y + 2, size//3, size//3])
    else:
        highlight = (min(255, base_r + 50), min(255, base_g + 50), min(255, base_b + 30))
        shadow = (max(0, base_r - 30), max(0, base_g - 20), max(0, base_b - 10))
        
        pygame.draw.rect(state.dis, color, [x, y, size, size])
        
        pygame.draw.line(state.dis, highlight, (x, y), (x + size-1, y), 1)
        pygame.draw.line(state.dis, highlight, (x, y), (x, y + size-1), 1)
        
        pygame.draw.line(state.dis, shadow, (x + 1, y + size-1), (x + size-1, y + size-1), 1)
        pygame.draw.line(state.dis, shadow, (x + size-1, y + 1), (x + size-1, y + size-1), 1)
