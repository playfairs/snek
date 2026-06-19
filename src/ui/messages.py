import pygame
import src.core.constants as constants
import src.core.state as state

def show_message(msg, color, y_displace=0, size="large"):
    if size == "small":
        text_surface = state.font_style.render(msg, True, constants.black)
        padding = 10
    elif size == "medium":
        text_surface = state.score_font.render(msg, True, constants.black)
        padding = 20
    else:
        try:
            large_font = pygame.font.SysFont("Arial", 32, bold=True)
            text_surface = large_font.render(msg, True, constants.black)
        except:
            basic_font = pygame.font.Font(None, 36)
            text_surface = basic_font.render(msg, True, constants.black)
        padding = 30
    
    text_rect = text_surface.get_rect()
    box_width = text_rect.width + padding * 2
    box_height = text_rect.height + padding * 2
    box_x = (state.dis_width - box_width) // 2
    box_y = (state.dis_height - box_height) // 2 + y_displace
    
    pygame.draw.rect(state.dis, constants.light_gray, (box_x, box_y, box_width, box_height))
    pygame.draw.rect(state.dis, constants.white, (box_x, box_y, box_width, box_height), 1)
    pygame.draw.rect(state.dis, constants.dark_gray, (box_x, box_y, box_width, box_height), 2)
    
    text_rect.center = (state.dis_width // 2, (state.dis_height // 2) + y_displace)
    state.dis.blit(text_surface, text_rect)
    
    return text_rect
