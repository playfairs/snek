import pygame
import src.core.constants as constants
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

def draw_grid():
    """Draw a subtle grid aligned with snake_block size."""
    grid_size = constants.snake_block
    
    for x in range(0, constants.dis_width, grid_size):
        pygame.draw.line(state.dis, constants.grid_color, (x, constants.GAME_AREA_TOP), (x, constants.dis_height), 1)
    
    for y in range(constants.GAME_AREA_TOP, constants.dis_height, grid_size):
        pygame.draw.line(state.dis, constants.grid_color, (0, y), (constants.dis_width, y), 1)
