import pygame
import math
import src.core.constants as constants
import src.core.state as state

def draw_powerup(powerup):
    if not powerup['active']:
        return
        
    color = constants.POWERUP_COLORS[powerup['type']]
    x, y = powerup['x'], powerup['y']
    
    pulse = (pygame.time.get_ticks() % 1000) / 1000.0
    pulse_size = int(2 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
    
    pygame.draw.rect(state.dis, color, [x, y, constants.snake_block, constants.snake_block])
    pygame.draw.rect(state.dis, constants.white, [x, y, constants.snake_block, constants.snake_block], 1)
    
    highlight = (min(255, color[0] + 30), 
                min(255, color[1] + 30), 
                min(255, color[2] + 30))
    pygame.draw.rect(state.dis, highlight, [x + 2, y + 2, constants.snake_block - 4, constants.snake_block - 4], 1)
    
    symbol_font = pygame.font.SysFont("Arial", constants.snake_block, bold=True)
    symbol = {
        'speed_boost': '⚡',
        'slow_mo': '🐌',
        'double_points': '2X',
        'invincible': '🛡️'
    }.get(powerup['type'], '?')
    
    text_surface = symbol_font.render(symbol, True, constants.white)
    text_rect = text_surface.get_rect(center=(x + constants.snake_block//2, y + constants.snake_block//2))
    state.dis.blit(text_surface, text_rect)
