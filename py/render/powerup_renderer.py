import pygame
import math
import py.core.constants as constants
import py.core.state as state

def draw_powerup(powerup):
    if not powerup['active']:
        return
        
    color = constants.POWERUP_COLORS[powerup['type']]
    x, y = powerup['x'], powerup['y']
    
    pulse = (pygame.time.get_ticks() % 1000) / 1000.0
    pulse_size = int(3 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
    
    glow_color = (min(255, color[0] + 50), min(255, color[1] + 50), min(255, color[2] + 50))
    glow_size = constants.snake_block + 4 + pulse_size
    glow_offset = (glow_size - constants.snake_block) // 2
    pygame.draw.rect(state.dis, glow_color, [x - glow_offset, y - glow_offset, glow_size, glow_size], 1)
    
    pygame.draw.rect(state.dis, color, [x, y, constants.snake_block, constants.snake_block])
    
    highlight = (min(255, color[0] + 60), min(255, color[1] + 60), min(255, color[2] + 60))
    inner_size = constants.snake_block - 6
    inner_offset = 3
    pygame.draw.rect(state.dis, highlight, [x + inner_offset, y + inner_offset, inner_size, inner_size], 1)
    
    pygame.draw.rect(state.dis, constants.white, [x, y, constants.snake_block, constants.snake_block], 1)
    
    symbol_font = pygame.font.SysFont("Arial", int(constants.snake_block * 0.8), bold=True)
    symbol = {
        'speed_boost': '⚡',
        'slow_mo': '🐌',
        'double_points': '2X',
        'invincible': '🛡️'
    }.get(powerup['type'], '?')
    
    text_surface = symbol_font.render(symbol, True, constants.white)
    text_rect = text_surface.get_rect(center=(x + constants.snake_block//2, y + constants.snake_block//2))
    
    shadow_surface = symbol_font.render(symbol, True, (0, 0, 0))
    shadow_rect = shadow_surface.get_rect(center=(x + constants.snake_block//2 + 1, y + constants.snake_block//2 + 1))
    state.dis.blit(shadow_surface, shadow_rect)
    state.dis.blit(text_surface, text_rect)
