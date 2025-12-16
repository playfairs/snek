import pygame
import time
import random
import os
import json
import math
import sys

pygame.init()
pygame.font.init()

white = (255, 255, 255)
black = (0, 0, 0)
light_gray = (192, 192, 192)
dark_gray = (128, 128, 128)
blue = (0, 0, 128)
light_blue = (0, 120, 215)
red = (192, 0, 0)
green = (0, 128, 0)
light_green = (144, 238, 144)
light_red = (255, 182, 193)

game_stats = {
    'high_score': 0,
    'total_apples': 0,
    'games_played': 0,
    'powerups_collected': 0,
    'best_time': 0
}

POWERUP_TYPES = ['speed_boost', 'slow_mo', 'double_points', 'invincible']
POWERUP_COLORS = {
    'speed_boost': (0, 191, 255),
    'slow_mo': (147, 112, 219),
    'double_points': (255, 215, 0),
    'invincible': (255, 0, 255)
}
POWERUP_DURATION = 10

SNAKE_SKINS = {
    'default': {
        'head_color': (0, 128, 0),
        'body_color': (0, 100, 0)
    },
    'blue': {
        'head_color': (0, 0, 255),
        'body_color': (0, 0, 200)
    },
    'red': {
        'head_color': (200, 0, 0),
        'body_color': (150, 0, 0)
    },
    'rainbow': {
        'head_color': (255, 0, 0),
        'body_color': (0, 0, 255)
    }
}

current_skin = 'default'

def load_stats():
    global game_stats
    try:
        if os.path.exists('snek_stats.json'):
            with open('snek_stats.json', 'r') as f:
                game_stats = json.load(f)
    except:
        print("Could not load game stats, using defaults")

def save_stats():
    try:
        with open('snek_stats.json', 'w') as f:
            json.dump(game_stats, f)
    except:
        print("Could not save game stats")

dis_width = 1000
dis_height = 800

HEADER_HEIGHT = 70
GAME_AREA_TOP = HEADER_HEIGHT

snake_block = 20
snake_speed = 10

dis = pygame.display.set_mode((dis_width, dis_height))
pygame.display.set_caption('Snake - Windows 98')

try:
    font_style = pygame.font.SysFont("Arial", 14, bold=True)
    score_font = pygame.font.SysFont("Arial", 18, bold=True)
    button_font = pygame.font.SysFont("Arial", 14, bold=True)
    large_font = pygame.font.SysFont("Arial", 24, bold=True)
except:
    font_style = pygame.font.SysFont(None, 16, bold=True)
    score_font = pygame.font.SysFont(None, 20, bold=True)
    button_font = pygame.font.SysFont(None, 16, bold=True)
    large_font = pygame.font.SysFont(None, 28, bold=True)

clock = pygame.time.Clock()

try:
    pygame.mixer.init()
    powerup_sound = pygame.mixer.Sound('sfx/powerUp.wav')
    powerup_sound.set_volume(0.5)
except Exception as e:
    print(f"Could not load sound effects: {e}")
    powerup_sound = None

if pygame.version.vernum[0] >= 2:
    try:
        import ctypes
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("com.windows98.snake")
    except:
        pass

def text_objects(text, font, color=white):
    text_surface = font.render(text, True, color)
    return text_surface, text_surface.get_rect()

def draw_button_3d(x, y, w, h, is_pressed=False):
    pygame.draw.rect(dis, light_gray, (x, y, w, h))
    
    if is_pressed:
        pygame.draw.line(dis, dark_gray, (x, y), (x + w, y), 2)
        pygame.draw.line(dis, dark_gray, (x, y), (x, y + h), 2)
        pygame.draw.line(dis, white, (x + w, y), (x + w, y + h), 2)
        pygame.draw.line(dis, white, (x, y + h), (x + w, y + h), 2)
    else:
        pygame.draw.line(dis, white, (x, y), (x + w, y), 2)
        pygame.draw.line(dis, white, (x, y), (x, y + h), 2)
        pygame.draw.line(dis, dark_gray, (x + w, y), (x + w, y + h), 2)
        pygame.draw.line(dis, dark_gray, (x, y + h), (x + w, y + h), 2)
def button(msg, x, y, w, h, ic, ac, action=None):
    mouse = pygame.mouse.get_pos()
    clicked = False
    
    if x + w > mouse[0] > x and y + h > mouse[1] > y:
        if pygame.mouse.get_pressed()[0] == 1:
            draw_button_3d(x, y, w, h, is_pressed=True)
            clicked = True
            pygame.time.delay(150)
        else:
            draw_button_3d(x, y, w, h, is_pressed=False)
    else:
        draw_button_3d(x, y, w, h, is_pressed=False)
    
    text_x = x + (w / 2)
    text_y = y + (h / 2) - 2
    if clicked:
        text_x += 1
        text_y += 1
        
    text_surf, text_rect = text_objects(msg, button_font, black)
    text_rect.center = (text_x, text_y)
    dis.blit(text_surf, text_rect)
    
    return clicked

def draw_powerup_indicator(powerup_type, time_left, x, y):
    """Draw a powerup indicator with icon, name, and timer"""
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
    
    name_surf = button_font.render(powerup['name'], True, black)
    time_text = f"{time_left:.1f}s"
    time_surf = button_font.render(time_text, True, black)
    
    total_width = icon_size + max(name_surf.get_width(), time_surf.get_width()) + (padding * 3)
    total_height = HEADER_HEIGHT - 20
    
    pygame.draw.rect(dis, powerup['color'], (x, y + 10, total_width, total_height))
    
    pygame.draw.rect(dis, (255, 255, 255), (x, y + 10, total_width, total_height), 1)
    pygame.draw.rect(dis, (100, 100, 100), (x + 1, y + 11, total_width - 2, total_height - 2), 1)
    
    icon_surf = button_font.render(powerup['icon'], True, black)
    icon_x = x + padding
    icon_y = y + (HEADER_HEIGHT - icon_surf.get_height()) // 2
    dis.blit(icon_surf, (icon_x, icon_y))
    
    name_x = x + icon_size + padding * 2
    dis.blit(name_surf, (name_x, y + 15))
    dis.blit(time_surf, (name_x, y + 35))
    
    return x + total_width + 15

def your_score(score, high_score=None, powerup_active=None, powerup_end_time=None):
    pygame.draw.rect(dis, (200, 200, 200), (0, 0, dis_width, HEADER_HEIGHT))
    pygame.draw.rect(dis, dark_gray, (0, 0, dis_width, HEADER_HEIGHT), 1)
    
    pygame.draw.line(dis, (255, 255, 255), (0, 0), (dis_width, 0), 2)
    pygame.draw.line(dis, (255, 255, 255), (0, 0), (0, HEADER_HEIGHT), 2)
    pygame.draw.line(dis, (128, 128, 128), (0, HEADER_HEIGHT-1), (dis_width-1, HEADER_HEIGHT-1), 2)
    pygame.draw.line(dis, (128, 128, 128), (dis_width-1, 0), (dis_width-1, HEADER_HEIGHT-1), 2)
    
    score_text = f"SCORE: {score}"
    text_surf = pygame.font.SysFont("Arial", 18, bold=True).render(score_text, True, black)
    shadow_surf = pygame.font.SysFont("Arial", 18, bold=True).render(score_text, True, (100, 100, 100))
    dis.blit(shadow_surf, (21, 21))
    dis.blit(text_surf, (20, 20))
    
    if high_score is not None:
        high_score_text = f"HIGH SCORE: {high_score}"
        high_score_surf = pygame.font.SysFont("Arial", 16, bold=True).render(high_score_text, True, black)
        shadow_high = pygame.font.SysFont("Arial", 16, bold=True).render(high_score_text, True, (100, 100, 100))
        
        high_score_x = dis_width - high_score_surf.get_width() - 20
        dis.blit(shadow_high, (high_score_x + 1, 21))
        dis.blit(high_score_surf, (high_score_x, 20))
    
    if powerup_active and powerup_end_time:
        time_left = max(0, powerup_end_time - time.time())
        if time_left > 0:
            x_pos = 200
            x_pos = draw_powerup_indicator(powerup_active, time_left, x_pos, 0)
    
    pygame.draw.line(dis, dark_gray, (dis_width//2, 8), (dis_width//2, HEADER_HEIGHT-8), 1)

def draw_cube(x, y, size, color, is_head=False):
    """Draw a 3D cube at the given position with the given color"""
    base_r, base_g, base_b = color
    
    if is_head:
        highlight = (min(255, base_r + 80), min(255, base_g + 80), min(255, base_b + 40))
        shadow = (max(0, base_r - 60), max(0, base_g - 40), max(0, base_b - 20))
        
        pygame.draw.rect(dis, color, [x, y, size, size])
        
        pygame.draw.line(dis, highlight, (x, y), (x + size-1, y), 2)
        pygame.draw.line(dis, highlight, (x, y), (x, y + size-1), 2)
        
        pygame.draw.line(dis, shadow, (x + 1, y + size-1), (x + size-1, y + size-1), 2)
        pygame.draw.line(dis, shadow, (x + size-1, y + 1), (x + size-1, y + size-1), 2)
        
        inner_highlight = (min(255, base_r + 100), min(255, base_g + 100), min(255, base_b + 60))
        pygame.draw.rect(dis, inner_highlight, [x + 2, y + 2, size//3, size//3])
    else:
        highlight = (min(255, base_r + 50), min(255, base_g + 50), min(255, base_b + 30))
        shadow = (max(0, base_r - 30), max(0, base_g - 20), max(0, base_b - 10))
        
        pygame.draw.rect(dis, color, [x, y, size, size])
        
        pygame.draw.line(dis, highlight, (x, y), (x + size-1, y), 1)
        pygame.draw.line(dis, highlight, (x, y), (x, y + size-1), 1)
        
        pygame.draw.line(dis, shadow, (x + 1, y + size-1), (x + size-1, y + size-1), 1)
        pygame.draw.line(dis, shadow, (x + size-1, y + 1), (x + size-1, y + size-1), 1)


def our_snake(snake_block, snake_list, powerup_active=None):
    if not snake_list:
        return
    
    for x in snake_list[:-1]:
        color = SNAKE_SKINS[current_skin]['body_color']
        if current_skin == 'rainbow':
            color = (
                int((pygame.time.get_ticks() // 100 + x[0]) % 256),
                int((pygame.time.get_ticks() // 100 + x[1]) % 256),
                int((pygame.time.get_ticks() // 100 + x[0] + x[1]) % 256)
            )
        
        if powerup_active == 'invincible':
            pulse = (pygame.time.get_ticks() % 1000) / 1000.0
            highlight = 55 + int(100 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
            color = (min(255, color[0] + highlight), 
                    min(255, color[1] + highlight), 
                    min(255, color[2] + highlight))
            
        draw_cube(x[0], x[1], snake_block, color, is_head=False)
    
    if snake_list:
        head = snake_list[-1]
        head_color = SNAKE_SKINS[current_skin]['head_color']
        if current_skin == 'rainbow':
            head_color = (
                int((pygame.time.get_ticks() // 100 + head[0]) % 256),
                int((pygame.time.get_ticks() // 100 + head[1]) % 256),
                int((pygame.time.get_ticks() // 100 + head[0] + head[1]) % 256)
            )
        
        if powerup_active == 'invincible':
            pulse = (pygame.time.get_ticks() % 1000) / 1000.0
            highlight = 55 + int(100 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
            head_color = (min(255, head_color[0] + highlight), 
                         min(255, head_color[1] + highlight), 
                         min(255, head_color[2] + highlight))
        
        draw_cube(head[0], head[1], snake_block, head_color, is_head=True)
        pygame.draw.rect(dis, (0, 200, 0), [head[0], head[1], snake_block, snake_block], 1)

def show_message(msg, color, y_displace=0, size="large"):
    if size == "small":
        text_surface = font_style.render(msg, True, black)
        padding = 10
    elif size == "medium":
        text_surface = score_font.render(msg, True, black)
        padding = 20
    else:
        try:
            large_font = pygame.font.SysFont("Arial", 32, bold=True)
            text_surface = large_font.render(msg, True, black)
        except:
            basic_font = pygame.font.Font(None, 36)
            text_surface = basic_font.render(msg, True, black)
        padding = 30
    
    text_rect = text_surface.get_rect()
    box_width = text_rect.width + padding * 2
    box_height = text_rect.height + padding * 2
    box_x = (dis_width - box_width) // 2
    box_y = (dis_height - box_height) // 2 + y_displace
    
    pygame.draw.rect(dis, light_gray, (box_x, box_y, box_width, box_height))
    pygame.draw.rect(dis, white, (box_x, box_y, box_width, box_height), 1)
    pygame.draw.rect(dis, dark_gray, (box_x, box_y, box_width, box_height), 2)
    
    text_rect.center = (dis_width // 2, (dis_height // 2) + y_displace)
    dis.blit(text_surface, text_rect)
    
    return text_rect

def game_over_screen(score):
    while True:
        dis.fill(light_gray)
        
        title_y = dis_height // 3
        shadow_offset = 3
        
        title_shadow = large_font.render("GAME OVER", True, dark_gray)
        title_shadow_rect = title_shadow.get_rect(center=(dis_width//2 + shadow_offset, title_y + shadow_offset))
        dis.blit(title_shadow, title_shadow_rect)
        
        title = large_font.render("GAME OVER", True, red)
        title_rect = title.get_rect(center=(dis_width//2, title_y))
        dis.blit(title, title_rect)
        
        score_y = title_y + 80
        score_text = f"Final Score: {score}"
        score_surface = score_font.render(score_text, True, black)
        score_rect = score_surface.get_rect(center=(dis_width//2, score_y))
        
        score_bg = pygame.Rect(score_rect.x - 20, score_rect.y - 10, 
                             score_rect.width + 40, score_rect.height + 20)
        pygame.draw.rect(dis, white, score_bg)
        pygame.draw.rect(dis, dark_gray, score_bg, 2)
        dis.blit(score_surface, score_rect)
        
        button_width = 200
        button_height = 50
        button_spacing = 30
        button_y = score_y + 80
        
        play_again = button("Play Again", 
                          dis_width//2 - button_width - button_spacing//2, 
                          button_y, 
                          button_width, 
                          button_height, 
                          light_gray, 
                          white,  
                          "play_again")
        
        quit_game = button("Quit", 
                         dis_width//2 + button_spacing//2, 
                         button_y, 
                         button_width, 
                         button_height, 
                         light_red,
                         white, 
                         "quit")
        
        pygame.draw.line(dis, dark_gray, (dis_width//4, button_y - 20), 
                        (3*dis_width//4, button_y - 20), 2)
        
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
        clock.tick(15)

def spawn_powerup():
    def get_random_position():
        while True:
            x = round(random.randrange(0, dis_width - snake_block) / snake_block) * snake_block
            y = round(random.randrange(GAME_AREA_TOP, dis_height - snake_block) / snake_block) * snake_block
            if (GAME_AREA_TOP <= y < dis_height - snake_block and 
                0 <= x < dis_width - snake_block):
                return x, y
    
    powerup_x, powerup_y = get_random_position()
    powerup_type = random.choice(POWERUP_TYPES)
    powerup_time = time.time()
    return {
        'type': powerup_type,
        'x': powerup_x,
        'y': powerup_y,
        'time': powerup_time,
        'active': True
    }

def draw_powerup(powerup):
    if not powerup['active']:
        return
        
    color = POWERUP_COLORS[powerup['type']]
    x, y = powerup['x'], powerup['y']
    
    pulse = (pygame.time.get_ticks() % 1000) / 1000.0
    pulse_size = int(2 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
    
    pygame.draw.rect(dis, color, [x, y, snake_block, snake_block])
    pygame.draw.rect(dis, white, [x, y, snake_block, snake_block], 1)
    
    highlight = (min(255, color[0] + 30), 
                min(255, color[1] + 30), 
                min(255, color[2] + 30))
    pygame.draw.rect(dis, highlight, [x + 2, y + 2, snake_block - 4, snake_block - 4], 1)
    
    symbol_font = pygame.font.SysFont("Arial", snake_block, bold=True)
    symbol = {
        'speed_boost': '⚡',
        'slow_mo': '🐌',
        'double_points': '2X',
        'invincible': '🛡️'
    }.get(powerup['type'], '?')
    
    text_surface = symbol_font.render(symbol, True, white)
    text_rect = text_surface.get_rect(center=(x + snake_block//2, y + snake_block//2))
    dis.blit(text_surface, text_rect)

def gameLoop():
    global game_stats
    global snake_speed
    
    game_over = False
    game_close = False
    
    original_speed = snake_speed
    
    score = 0
    apples_eaten = 0
    start_time = time.time()
    powerup = None
    powerup_active = None
    powerup_end_time = 0
    
    load_stats()
    game_stats['games_played'] += 1

    x1 = (dis_width // (2 * snake_block)) * snake_block
    y1 = ((dis_height - GAME_AREA_TOP) // (2 * snake_block)) * snake_block + GAME_AREA_TOP
    x1_change = 0
    y1_change = 0
    
    powerup_spawn_time = time.time() + random.randint(10, 20)

    snake_List = []
    Length_of_snake = 1

    def get_random_food_position():
        while True:
            x = round(random.randrange(0, dis_width - snake_block) / snake_block) * snake_block
            y = round(random.randrange(GAME_AREA_TOP, dis_height - snake_block) / snake_block) * snake_block
            if (GAME_AREA_TOP <= y < dis_height - snake_block and 
                0 <= x < dis_width - snake_block):
                return x, y
    
    foodx, foody = get_random_food_position()

    while not game_over:
        current_time = time.time()
        
        if powerup is None and current_time > powerup_spawn_time:
            powerup = spawn_powerup()
            powerup_spawn_time = current_time + random.randint(15, 30)
        
        if powerup_active and current_time > powerup_end_time:
            powerup_active = None
            snake_speed = 10
        
        if powerup and powerup['active'] and current_time - powerup['time'] > 10:
            powerup = None
            
        if powerup_active == 'speed_boost':
            current_speed = 15
        elif powerup_active == 'slow_mo':
            current_speed = 5
        else:
            current_speed = original_speed

        while game_close == True:
            action = game_over_screen(Length_of_snake - 1)
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
                    x1_change = -snake_block
                    y1_change = 0
                elif (event.key == pygame.K_RIGHT or event.key == pygame.K_d) and x1_change == 0:
                    x1_change = snake_block
                    y1_change = 0
                elif (event.key == pygame.K_UP or event.key == pygame.K_w) and y1_change == 0:
                    y1_change = -snake_block
                    x1_change = 0
                elif (event.key == pygame.K_DOWN or event.key == pygame.K_s) and y1_change == 0:
                    y1_change = snake_block
                    x1_change = 0

        if powerup_active != 'invincible':
            if y1 < GAME_AREA_TOP:
                y1 = dis_height - snake_block
            elif y1 >= dis_height:
                y1 = GAME_AREA_TOP
            if x1 < 0:
                x1 = dis_width - snake_block
            elif x1 >= dis_width:
                x1 = 0

        if x1 >= dis_width:
            x1 = 0
        elif x1 < 0:
            x1 = dis_width - snake_block
        if y1 >= dis_height:
            y1 = 0
        elif y1 < 0:
            y1 = dis_height - snake_block

        x1 += x1_change
        y1 += y1_change
        dis.fill(black)
        pulse = (pygame.time.get_ticks() % 1000) / 1000.0
        pulse_size = int(2 * (0.5 + 0.5 * math.sin(pulse * 2 * math.pi)))
        
        pygame.draw.rect(dis, red, [foodx, foody, snake_block, snake_block])
        
        highlight = (min(255, max(0, red[0] + 50)), 
                    min(255, max(0, red[1] - 50)), 
                    min(255, max(0, red[1] - 50)))
        
        inner_size = snake_block - 4 - pulse_size
        inner_offset = (snake_block - inner_size) // 2
        pygame.draw.rect(dis, highlight, 
                        [foodx + inner_offset, 
                         foody + inner_offset, 
                         inner_size, 
                         inner_size])
        
        if powerup and powerup['active']:
            if GAME_AREA_TOP <= powerup['y'] < dis_height - snake_block:
                draw_powerup(powerup)
                
                if (x1 == powerup['x'] and y1 == powerup['y']) or \
                   (abs(x1 - powerup['x']) < snake_block and abs(y1 - powerup['y']) < snake_block):
                    powerup_active = powerup['type']
                    powerup_end_time = time.time() + POWERUP_DURATION
                    powerup = None
                    game_stats['powerups_collected'] += 1
                    save_stats()
                    if powerup_sound:
                        try:
                            pygame.mixer.Sound.play(powerup_sound)
                        except:
                            pass
        
        snake_Head = []
        snake_Head.append(x1)
        snake_Head.append(y1)
        snake_List.append(snake_Head)
        
        if len(snake_List) > Length_of_snake:
            del snake_List[0]

        if Length_of_snake > 1:
            for x in snake_List[:-1]:
                if x == snake_Head:
                    game_close = True

        our_snake(snake_block, snake_List, powerup_active)
        
        your_score(Length_of_snake - 1, game_stats['high_score'])

        pygame.display.update()

        if abs(x1 - foodx) < snake_block and abs(y1 - foody) < snake_block:
            foodx, foody = get_random_food_position()
            Length_of_snake += 1
            apples_eaten += 1
            game_stats['total_apples'] += 1
            
            points = 10
            if powerup_active == 'double_points':
                points *= 2
            score += points
            
            if score > game_stats['high_score']:
                game_stats['high_score'] = score
            
            save_stats()
            
            if snake_speed < 20 and Length_of_snake % 5 == 0:
                snake_speed += 0.5

        clock.tick(current_speed)

    pygame.quit()
    quit()

def main_menu():
    global dis
    
    content_rect = show_message("SNAKE GAME", green, -100, "large")
    
    button_y = dis_height / 2
    button_height = 50
    button_width = 200
    button_spacing = 20
    
    running = True
    while running:
        dis.fill(light_gray)
        show_message("SNAKE GAME", blue, -100, "large")
        
        play_clicked = button("Play", 
                            dis_width//2 - button_width - 10, 
                            button_y, 
                            button_width, 
                            button_height, 
                            light_gray, 
                            light_gray, 
                            "play")
        
        quit_clicked = button("Exit", 
                            dis_width//2 + 10, 
                            button_y, 
                            button_width, 
                            button_height, 
                            light_gray, 
                            light_gray, 
                            "quit")
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                
        if play_clicked:
            result = gameLoop()
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
            clock.tick(15)
        
    pygame.quit()

if __name__ == "__main__":
    dis = pygame.display.set_mode((dis_width, dis_height))
    pygame.display.set_caption('Snek')
    
    try:
        main_menu()
    finally:
        pygame.quit()
