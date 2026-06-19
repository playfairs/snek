import pygame
import src.core.constants as constants
import src.core.state as state

def draw_powerup_indicator(powerup_type, time_left, x, y):
    powerup_display = {
        'speed_boost': {
            'color': (0, 191, 255),
            'icon': '⚡',
            'name': 'SPEED BOOST'
        },
        'slow_mo': {
            'color': (147, 112, 219),
            'icon': '🐌',
            'name': 'SLOW MOTION'
        },
        'double_points': {
            'color': (255, 215, 0),
            'icon': '2X',
            'name': '2X POINTS'
        },
        'invincible': {
            'color': (255, 0, 255),
            'icon': '🛡️',
            'name': 'INVINCIBLE'
        }
    }
    
    powerup = powerup_display.get(powerup_type, next(iter(powerup_display.values())))
    
    icon_size = 32
    padding = 8
    text_padding = 10
    
    name_surf = state.button_font.render(powerup['name'], True, constants.black)
    time_text = f"{time_left:.1f}s"
    time_surf = state.button_font.render(time_text, True, constants.black)
    
    total_width = icon_size + max(name_surf.get_width(), time_surf.get_width()) + (padding * 3)
    total_height = constants.HEADER_HEIGHT - 20
    
    pygame.draw.rect(state.dis, powerup['color'], (x, y + 10, total_width, total_height))
    
    pygame.draw.rect(state.dis, (255, 255, 255), (x, y + 10, total_width, total_height), 1)
    pygame.draw.rect(state.dis, (100, 100, 100), (x + 1, y + 11, total_width - 2, total_height - 2), 1)
    
    icon_surf = state.button_font.render(powerup['icon'], True, constants.black)
    icon_x = x + padding
    icon_y = y + (constants.HEADER_HEIGHT - icon_surf.get_height()) // 2
    state.dis.blit(icon_surf, (icon_x, icon_y))
    
    name_x = x + icon_size + padding * 2
    state.dis.blit(name_surf, (name_x, y + 15))
    state.dis.blit(time_surf, (name_x, y + 35))
    
    return x + total_width + 15
