import pygame
import math
import py.core.constants as constants
import py.core.state as state
import py.ui.buttons as buttons
import py.ui.messages as messages
import py.core.stats as stats

def settings_menu():
    """Display settings menu with configurable options."""
    global state
    
    options = ['Snake Skin', 'Sound', 'Music', 'Volume', 'Difficulty', 'Back']
    current_option = 0
    
    skin_options = list(constants.SNAKE_SKINS.keys())
    current_skin_index = skin_options.index(state.current_skin)
    
    sound_options = ['On', 'Off']
    current_sound_index = 0 if state.sound_enabled else 1
    
    music_options = ['On', 'Off']
    current_music_index = 0 if state.music_enabled else 1
    
    volume_options = [0.3, 0.5, 0.7, 1.0]
    current_volume_index = volume_options.index(state.volume) if state.volume in volume_options else 2
    
    difficulty_options = ['Easy', 'Normal', 'Hard']
    difficulty_index = {'easy': 0, 'normal': 1, 'hard': 2}.get(state.difficulty, 1)
    
    running = True
    while running:
        # Modern gradient background
        state.dis.fill(constants.black)
        for i in range(state.dis_height):
            alpha = int(20 * (i / state.dis_height))
            color = (15 + alpha, 15 + alpha, 20 + alpha)
            pygame.draw.line(state.dis, color, (0, i), (state.dis_width, i))
        
        title_y = state.dis_height // 4
        title = state.large_font.render("SETTINGS", True, constants.accent)
        title_rect = title.get_rect(center=(state.dis_width//2, title_y))
        state.dis.blit(title, title_rect)
        
        option_y = title_y + 80
        option_height = 50
        
        for i, option in enumerate(options):
            option_color = constants.white if i == current_option else constants.light_gray
            option_text = option
            
            if option == 'Snake Skin':
                option_text = f"Snake Skin: {skin_options[current_skin_index]}"
            elif option == 'Sound':
                option_text = f"Sound: {sound_options[current_sound_index]}"
            elif option == 'Music':
                option_text = f"Music: {music_options[current_music_index]}"
            elif option == 'Volume':
                option_text = f"Volume: {int(volume_options[current_volume_index] * 100)}%"
            elif option == 'Difficulty':
                option_text = f"Difficulty: {difficulty_options[difficulty_index]}"
            
            option_surf = state.score_font.render(option_text, True, option_color)
            option_rect = option_surf.get_rect(center=(state.dis_width//2, option_y + i * option_height))
            
            if i == current_option:
                bg_rect = pygame.Rect(option_rect.x - 20, option_rect.y - 10, 
                                    option_rect.width + 40, option_rect.height + 20)
                pygame.draw.rect(state.dis, constants.dark_gray, bg_rect)
                pygame.draw.rect(state.dis, constants.accent, bg_rect, 2)
            
            state.dis.blit(option_surf, option_rect)
        
        instr_text = "Use Arrow Keys or WASD to navigate, Enter to select, ESC to go back"
        instr_surf = state.font_style.render(instr_text, True, constants.light_gray)
        instr_rect = instr_surf.get_rect(center=(state.dis_width//2, state.dis_height - 50))
        state.dis.blit(instr_surf, instr_rect)
        
        pygame.display.update()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return "quit"
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key == pygame.K_UP or event.key == pygame.K_w:
                    current_option = (current_option - 1) % len(options)
                elif event.key == pygame.K_DOWN or event.key == pygame.K_s:
                    current_option = (current_option + 1) % len(options)
                elif event.key == pygame.K_RETURN or event.key == pygame.K_SPACE:
                    if options[current_option] == 'Snake Skin':
                        current_skin_index = (current_skin_index + 1) % len(skin_options)
                        state.current_skin = skin_options[current_skin_index]
                        stats.game_stats['snake_skin'] = state.current_skin
                        stats.save_stats()
                    elif options[current_option] == 'Sound':
                        current_sound_index = (current_sound_index + 1) % len(sound_options)
                        state.sound_enabled = (current_sound_index == 0)
                        stats.game_stats['sound_enabled'] = state.sound_enabled
                        stats.save_stats()
                    elif options[current_option] == 'Music':
                        current_music_index = (current_music_index + 1) % len(music_options)
                        state.music_enabled = (current_music_index == 0)
                        stats.game_stats['music_enabled'] = state.music_enabled
                        stats.save_stats()
                    elif options[current_option] == 'Volume':
                        current_volume_index = (current_volume_index + 1) % len(volume_options)
                        state.volume = volume_options[current_volume_index]
                        stats.game_stats['volume'] = state.volume
                        stats.save_stats()
                    elif options[current_option] == 'Difficulty':
                        difficulty_index = (difficulty_index + 1) % len(difficulty_options)
                        state.difficulty = difficulty_options[difficulty_index].lower()
                    elif options[current_option] == 'Back':
                        running = False
        
        state.clock.tick(15)
    
    return "menu"

def game_over_screen(score):
    while True:
        state.dis.fill(constants.black)
        
        # Modern gradient background
        for i in range(state.dis_height):
            alpha = int(20 * (i / state.dis_height))
            color = (15 + alpha, 15 + alpha, 20 + alpha)
            pygame.draw.line(state.dis, color, (0, i), (state.dis_width, i))
        
        title_y = state.dis_height // 3
        shadow_offset = 3
        
        pulse = (pygame.time.get_ticks() % 1500) / 1500.0
        title_scale = 1.0 + 0.05 * math.sin(pulse * 2 * math.pi)
        
        # Modern title with glow
        title_glow = state.large_font.render("GAME OVER", True, constants.red)
        glow_rect = title_glow.get_rect(center=(state.dis_width//2 + shadow_offset, title_y + shadow_offset))
        state.dis.blit(title_glow, glow_rect)
        
        title = state.large_font.render("GAME OVER", True, constants.white)
        title_rect = title.get_rect(center=(state.dis_width//2, title_y))
        state.dis.blit(title, title_rect)
        
        score_y = title_y + 80
        score_text = f"Final Score: {score}"
        score_surface = state.score_font.render(score_text, True, constants.white)
        score_rect = score_surface.get_rect(center=(state.dis_width//2, score_y))
        
        # Modern score background
        bg_padding = 40
        score_bg = pygame.Rect(score_rect.x - bg_padding, score_rect.y - bg_padding//2, 
                             score_rect.width + bg_padding*2, score_rect.height + bg_padding)
        
        # Gradient background for score
        pygame.draw.rect(state.dis, constants.dark_gray, score_bg)
        pygame.draw.rect(state.dis, constants.accent, score_bg, 2)
        pygame.draw.rect(state.dis, constants.light_gray, score_bg, 1)
        state.dis.blit(score_surface, score_rect)
        
        button_width = min(200, state.dis_width // 3)
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
    button_y = state.dis_height / 2
    button_height = 50
    button_width = min(200, state.dis_width // 3)
    button_spacing = 20
    
    button_count = 3
    total_width = button_width * button_count + button_spacing * (button_count - 1)
    start_x = (state.dis_width - total_width) // 2
    
    running = True
    while running:
        # Modern gradient background
        state.dis.fill(constants.black)
        for i in range(state.dis_height):
            alpha = int(25 * (i / state.dis_height))
            color = (15 + alpha, 15 + alpha, 20 + alpha)
            pygame.draw.line(state.dis, color, (0, i), (state.dis_width, i))
        
        # Animated title with modern glow
        pulse = (pygame.time.get_ticks() % 2000) / 2000.0
        title_color = (
            max(0, min(255, int(constants.blue[0] + 50 * math.sin(pulse * 2 * math.pi)))),
            max(0, min(255, int(constants.blue[1] + 50 * math.sin(pulse * 2 * math.pi)))),
            max(0, min(255, int(constants.blue[2] + 50 * math.sin(pulse * 2 * math.pi))))
        )
        
        title_glow = state.large_font.render("SNAKE GAME", True, title_color)
        glow_rect = title_glow.get_rect(center=(state.dis_width//2 + 2, state.dis_height // 3 + 2))
        state.dis.blit(title_glow, glow_rect)
        
        title = state.large_font.render("SNAKE GAME", True, constants.white)
        title_rect = title.get_rect(center=(state.dis_width//2, state.dis_height // 3))
        state.dis.blit(title, title_rect)
        
        play_clicked = buttons.button("Play", 
                            start_x, 
                            button_y, 
                            button_width, 
                            button_height, 
                            constants.light_gray, 
                            constants.light_gray, 
                            "play")
        
        settings_clicked = buttons.button("Settings", 
                               start_x + button_width + button_spacing, 
                               button_y, 
                               button_width, 
                               button_height, 
                               constants.light_gray, 
                               constants.light_gray, 
                               "settings")
        
        quit_clicked = buttons.button("Exit", 
                            start_x + (button_width + button_spacing) * 2, 
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
        elif settings_clicked:
            result = settings_menu()
            if result == "quit":
                running = False
                break
        elif quit_clicked:
            running = False
            break
            
        if running:
            pygame.display.update()
            state.clock.tick(15)
        
    pygame.quit()
