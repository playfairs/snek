import pygame
import time
import random
import math
import src.core.constants as constants
import src.core.state as state
import src.core.stats as stats
import src.audio.sounds as sounds
import src.ui.score as score_ui
import src.ui.menus as menus
import src.render.snake_renderer as snake_renderer
import src.render.powerup_renderer as powerup_renderer
import src.render.cube as cube
import src.game.food as food
import src.game.powerups as powerups

def gameLoop():
    game_over = False
    game_close = False
    
    original_speed = constants.snake_speed
    
    if state.difficulty == 'easy':
        original_speed = 8
    elif state.difficulty == 'hard':
        original_speed = 15
    else:
        original_speed = 10
    
    player_score = 0
    apples_eaten = 0
    start_time = time.time()
    powerup = None
    powerup_active = None
    powerup_end_time = 0
    
    stats.load_stats()
    stats.game_stats['games_played'] += 1

    x1 = int((constants.dis_width // (2 * constants.snake_block)) * constants.snake_block)
    y1 = int(((constants.dis_height - constants.GAME_AREA_TOP) // (2 * constants.snake_block)) * constants.snake_block + constants.GAME_AREA_TOP)
    x1_change = 0
    y1_change = 0
    
    powerup_spawn_time = time.time() + random.randint(10, 20)

    snake_List = []
    Length_of_snake = 1

    foodx, foody = food.get_random_food_position()

    while not game_over:
        current_time = time.time()
        
        if powerup is None and current_time > powerup_spawn_time:
            powerup = powerups.spawn_powerup()
            powerup_spawn_time = current_time + random.randint(15, 30)
        
        if powerup_active and current_time > powerup_end_time:
            powerup_active = None
            constants.snake_speed = 10
        
        if powerup and powerup['active'] and current_time - powerup['time'] > 10:
            powerup = None
            
        if powerup_active == 'speed_boost':
            current_speed = 15
        elif powerup_active == 'slow_mo':
            current_speed = 5
        else:
            current_speed = original_speed

        while game_close == True:
            action = menus.game_over_screen(Length_of_snake - 1)
            if action == "play_again":
                return gameLoop()
            elif action == "quit":
                pygame.quit()
                return "quit"

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_e and pygame.key.get_mods() & pygame.KMOD_CTRL:
                    game_close = True
                elif (event.key == pygame.K_LEFT or event.key == pygame.K_a) and x1_change == 0:
                    x1_change = -constants.snake_block
                    y1_change = 0
                elif (event.key == pygame.K_RIGHT or event.key == pygame.K_d) and x1_change == 0:
                    x1_change = constants.snake_block
                    y1_change = 0
                elif (event.key == pygame.K_UP or event.key == pygame.K_w) and y1_change == 0:
                    y1_change = -constants.snake_block
                    x1_change = 0
                elif (event.key == pygame.K_DOWN or event.key == pygame.K_s) and y1_change == 0:
                    y1_change = constants.snake_block
                    x1_change = 0

        if powerup_active != 'invincible':
            if y1 < constants.GAME_AREA_TOP:
                y1 = constants.dis_height - constants.snake_block
            elif y1 >= constants.dis_height:
                y1 = constants.GAME_AREA_TOP
            if x1 < 0:
                x1 = constants.dis_width - constants.snake_block
            elif x1 >= constants.dis_width:
                x1 = 0

        if x1 >= constants.dis_width:
            x1 = 0
        elif x1 < 0:
            x1 = constants.dis_width - constants.snake_block
        if y1 >= constants.dis_height:
            y1 = 0
        elif y1 < 0:
            y1 = constants.GAME_AREA_TOP

        x1 += x1_change
        y1 += y1_change
        
        x1 = int(x1)
        y1 = int(y1)
        
        state.dis.fill(constants.black)
        
        cube.draw_grid()
        
        pulse = (pygame.time.get_ticks() % 1000) / 1000.0
        pulse_size = int(2 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
        
        pygame.draw.rect(state.dis, constants.red, [foodx, foody, constants.snake_block, constants.snake_block])
        
        highlight = (min(255, max(0, constants.red[0] + 50)), 
                    min(255, max(0, constants.red[1] - 50)), 
                    min(255, max(0, constants.red[1] - 50)))
        
        inner_size = constants.snake_block - 4 - pulse_size
        inner_offset = (constants.snake_block - inner_size) // 2
        pygame.draw.rect(state.dis, highlight, 
                        [foodx + inner_offset, 
                         foody + inner_offset, 
                         inner_size, 
                         inner_size])
        
        if powerup and powerup['active']:
            if constants.GAME_AREA_TOP <= powerup['y'] < constants.dis_height - constants.snake_block:
                powerup_renderer.draw_powerup(powerup)
                
                if (x1 == powerup['x'] and y1 == powerup['y']) or \
                   (abs(x1 - powerup['x']) < constants.snake_block and abs(y1 - powerup['y']) < constants.snake_block):
                    powerup_active = powerup['type']
                    powerup_end_time = time.time() + constants.POWERUP_DURATION
                    powerup = None
                    stats.game_stats['powerups_collected'] += 1
                    stats.save_stats()
                    sounds.play_powerup_sound()
        
        snake_Head = []
        snake_Head.append(x1)
        snake_Head.append(y1)
        snake_List.append(snake_Head)
        
        if len(snake_List) > Length_of_snake:
            del snake_List[0]

        if Length_of_snake > 1 and powerup_active != 'invincible':
            for x in snake_List[:-1]:
                if x == snake_Head:
                    game_close = True

        snake_renderer.our_snake(constants.snake_block, snake_List, powerup_active)
        
        score_ui.your_score(Length_of_snake - 1, stats.game_stats['high_score'], 
                           Length_of_snake, apples_eaten, current_speed,
                           powerup_active, powerup_end_time)

        pygame.display.update()

        if abs(x1 - foodx) < constants.snake_block and abs(y1 - foody) < constants.snake_block:
            foodx, foody = food.get_random_food_position()
            Length_of_snake += 1
            apples_eaten += 1
            stats.game_stats['total_apples'] += 1
            
            points = 10
            if powerup_active == 'double_points':
                points *= 2
            player_score += points
            
            if player_score > stats.game_stats['high_score']:
                stats.game_stats['high_score'] = player_score
            
            stats.save_stats()
            
            if constants.snake_speed < 20 and Length_of_snake % 5 == 0:
                constants.snake_speed += 0.5

        state.clock.tick(current_speed)

    pygame.quit()
