import pygame
import time
import src.core.constants as constants
import src.core.state as state
import src.ui.powerup_indicator as powerup_indicator

def your_score(score, high_score=None, powerup_active=None, powerup_end_time=None):
    pygame.draw.rect(state.dis, (200, 200, 200), (0, 0, state.dis_width, constants.HEADER_HEIGHT))
    pygame.draw.rect(state.dis, constants.dark_gray, (0, 0, state.dis_width, constants.HEADER_HEIGHT), 1)
    
    pygame.draw.line(state.dis, (255, 255, 255), (0, 0), (state.dis_width, 0), 2)
    pygame.draw.line(state.dis, (255, 255, 255), (0, 0), (0, constants.HEADER_HEIGHT), 2)
    pygame.draw.line(state.dis, (128, 128, 128), (0, constants.HEADER_HEIGHT-1), (state.dis_width-1, constants.HEADER_HEIGHT-1), 2)
    pygame.draw.line(state.dis, (128, 128, 128), (state.dis_width-1, 0), (state.dis_width-1, constants.HEADER_HEIGHT-1), 2)
    
    score_text = f"SCORE: {score}"
    text_surf = pygame.font.SysFont("Arial", 18, bold=True).render(score_text, True, constants.black)
    shadow_surf = pygame.font.SysFont("Arial", 18, bold=True).render(score_text, True, (100, 100, 100))
    state.dis.blit(shadow_surf, (21, 21))
    state.dis.blit(text_surf, (20, 20))
    
    if high_score is not None:
        high_score_text = f"HIGH SCORE: {high_score}"
        high_score_surf = pygame.font.SysFont("Arial", 16, bold=True).render(high_score_text, True, constants.black)
        shadow_high = pygame.font.SysFont("Arial", 16, bold=True).render(high_score_text, True, (100, 100, 100))
        
        high_score_x = state.dis_width - high_score_surf.get_width() - 20
        state.dis.blit(shadow_high, (high_score_x + 1, 21))
        state.dis.blit(high_score_surf, (high_score_x, 20))
    
    if powerup_active and powerup_end_time:
        time_left = max(0, powerup_end_time - time.time())
        if time_left > 0:
            x_pos = 200
            x_pos = powerup_indicator.draw_powerup_indicator(powerup_active, time_left, x_pos, 0)
    
    pygame.draw.line(state.dis, constants.dark_gray, (state.dis_width//2, 8), (state.dis_width//2, constants.HEADER_HEIGHT-8), 1)
