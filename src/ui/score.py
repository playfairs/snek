import pygame
import time
import src.core.constants as constants
import src.core.state as state
import src.ui.powerup_indicator as powerup_indicator

def your_score(score, high_score=None, snake_length=0, apples_collected=0, 
               current_speed=10, powerup_active=None, powerup_end_time=None):
    try:
        # Modern gradient background for HUD
        pygame.draw.rect(state.dis, constants.black, (0, 0, state.dis_width, constants.HEADER_HEIGHT))
        
        # Add subtle gradient effect
        for i in range(constants.HEADER_HEIGHT):
            alpha = int(30 * (i / constants.HEADER_HEIGHT))
            color = (40 + alpha, 40 + alpha, 50 + alpha)
            pygame.draw.line(state.dis, color, (0, i), (state.dis_width, i))
        
        # Modern border
        pygame.draw.line(state.dis, constants.accent, (0, 0), (state.dis_width, 0), 2)
        pygame.draw.line(state.dis, constants.accent, (0, constants.HEADER_HEIGHT-1), (state.dis_width, constants.HEADER_HEIGHT-1), 2)
        
        # Calculate responsive positions
        base_x = 20
        spacing = 130
        font_size_large = 18
        font_size_small = 14
        
        # Score with modern styling
        score_text = f"SCORE: {score}"
        text_surf = pygame.font.SysFont("Arial", font_size_large, bold=True).render(score_text, True, constants.white)
        glow_surf = pygame.font.SysFont("Arial", font_size_large, bold=True).render(score_text, True, constants.accent)
        state.dis.blit(glow_surf, (base_x + 2, 22))
        state.dis.blit(text_surf, (base_x, 20))
        
        # High score (right-aligned)
        if high_score is not None:
            high_score_text = f"HIGH: {high_score}"
            high_score_surf = pygame.font.SysFont("Arial", font_size_small, bold=True).render(high_score_text, True, constants.white)
            glow_high = pygame.font.SysFont("Arial", font_size_small, bold=True).render(high_score_text, True, constants.accent2)
            
            high_score_x = state.dis_width - high_score_surf.get_width() - 20
            state.dis.blit(glow_high, (high_score_x + 2, 21))
            state.dis.blit(high_score_surf, (high_score_x, 20))
        
        # Snake length
        length_x = base_x + spacing
        length_text = f"LEN: {snake_length}"
        length_surf = pygame.font.SysFont("Arial", font_size_small, bold=True).render(length_text, True, constants.white)
        length_glow = pygame.font.SysFont("Arial", font_size_small, bold=True).render(length_text, True, constants.green)
        state.dis.blit(length_glow, (length_x + 2, 21))
        state.dis.blit(length_surf, (length_x, 20))
        
        # Apples collected
        apples_x = length_x + spacing
        apples_text = f"APPLES: {apples_collected}"
        apples_surf = pygame.font.SysFont("Arial", font_size_small, bold=True).render(apples_text, True, constants.white)
        apples_glow = pygame.font.SysFont("Arial", font_size_small, bold=True).render(apples_text, True, constants.red)
        state.dis.blit(apples_glow, (apples_x + 2, 21))
        state.dis.blit(apples_surf, (apples_x, 20))
        
        # Current speed
        speed_x = apples_x + spacing
        speed_text = f"SPEED: {current_speed}"
        speed_surf = pygame.font.SysFont("Arial", font_size_small, bold=True).render(speed_text, True, constants.white)
        speed_glow = pygame.font.SysFont("Arial", font_size_small, bold=True).render(speed_text, True, constants.light_blue)
        state.dis.blit(speed_glow, (speed_x + 2, 21))
        state.dis.blit(speed_surf, (speed_x, 20))
        
        # Powerup indicator
        if powerup_active and powerup_end_time:
            time_left = max(0, powerup_end_time - time.time())
            if time_left > 0:
                x_pos = speed_x + spacing
                x_pos = powerup_indicator.draw_powerup_indicator(powerup_active, time_left, x_pos, 0)
        
        # Modern divider
        pygame.draw.line(state.dis, constants.dark_gray, (state.dis_width//2, 8), (state.dis_width//2, constants.HEADER_HEIGHT-8), 1)
    except Exception as e:
        print(f"Error rendering HUD: {e}")
        try:
            simple_text = f"Score: {score}"
            simple_surf = state.score_font.render(simple_text, True, constants.white)
            state.dis.blit(simple_surf, (10, 10))
        except:
            pass
