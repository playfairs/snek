import pygame
import src.core.constants as constants
import src.core.state as state
import src.ui.buttons as buttons
import src.ui.messages as messages

def game_over_screen(score):
    while True:
        state.dis.fill(constants.light_gray)
        
        title_y = state.dis_height // 3
        shadow_offset = 3
        
        title_shadow = state.large_font.render("GAME OVER", True, constants.dark_gray)
        title_shadow_rect = title_shadow.get_rect(center=(state.dis_width//2 + shadow_offset, title_y + shadow_offset))
        state.dis.blit(title_shadow, title_shadow_rect)
        
        title = state.large_font.render("GAME OVER", True, constants.red)
        title_rect = title.get_rect(center=(state.dis_width//2, title_y))
        state.dis.blit(title, title_rect)
        
        score_y = title_y + 80
        score_text = f"Final Score: {score}"
        score_surface = state.score_font.render(score_text, True, constants.black)
        score_rect = score_surface.get_rect(center=(state.dis_width//2, score_y))
        
        score_bg = pygame.Rect(score_rect.x - 20, score_rect.y - 10, 
                             score_rect.width + 40, score_rect.height + 20)
        pygame.draw.rect(state.dis, constants.white, score_bg)
        pygame.draw.rect(state.dis, constants.dark_gray, score_bg, 2)
        state.dis.blit(score_surface, score_rect)
        
        button_width = 200
        button_height = 50
        button_spacing = 30
        button_y = score_y + 80
        
        play_again = buttons.button("Play Again", 
                          state.dis_width//2 - button_width - button_spacing//2, 
                          button_y, 
                          button_width, 
                          button_height, 
                          constants.light_gray, 
                          constants.white,  
                          "play_again")
        
        quit_game = buttons.button("Quit", 
                         state.dis_width//2 + button_spacing//2, 
                         button_y, 
                         button_width, 
                         button_height, 
                         constants.light_red,
                         constants.white, 
                         "quit")
        
        pygame.draw.line(state.dis, constants.dark_gray, (state.dis_width//4, button_y - 20), 
                        (3*state.dis_width//4, button_y - 20), 2)
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    pygame.quit()
                    return "quit"
                
        if play_again:
            return "play_again"
        elif quit_game:
            pygame.quit()
            return "quit"
            
        pygame.display.update()
        state.clock.tick(15)

def main_menu(game_loop_func):
    content_rect = messages.show_message("SNAKE GAME", constants.green, -100, "large")
    
    button_y = state.dis_height / 2
    button_height = 50
    button_width = 200
    button_spacing = 20
    
    running = True
    while running:
        state.dis.fill(constants.light_gray)
        messages.show_message("SNAKE GAME", constants.blue, -100, "large")
        
        play_clicked = buttons.button("Play", 
                            state.dis_width//2 - button_width - 10, 
                            button_y, 
                            button_width, 
                            button_height, 
                            constants.light_gray, 
                            constants.light_gray, 
                            "play")
        
        quit_clicked = buttons.button("Exit", 
                            state.dis_width//2 + 10, 
                            button_y, 
                            button_width, 
                            button_height, 
                            constants.light_gray, 
                            constants.light_gray, 
                            "quit")
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                
        if play_clicked:
            result = game_loop_func()
            if result == "menu":
                continue
            elif result == "quit":
                running = False
                break
        elif quit_clicked:
            running = False
            break
            
        if running:
            pygame.display.update()
            state.clock.tick(15)
        
    pygame.quit()
