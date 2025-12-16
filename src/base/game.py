# snek/base/game.py
import pygame
import time
import random
import sys
import math
from pathlib import Path
from typing import Dict, Any, Optional, List

from src.config.settings import *
from src.base.state import GameState
from src.base.snake import Snake
from src.components.food.food import Food
from src.components.powerups.base import Powerup
from src.components.ui.button import Button
from src.components.ui.header import Header
from src.components.objects.mine import Mine

class Game:
    def __init__(self):
        pygame.init()
        pygame.mixer.init()
        
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption(GAME_TITLE)
        self.clock = pygame.time.Clock()
        
        self.running = False
        self.game_over = False
        self.paused = False
        self.state = GameState()
        self.snake = None
        self.food = None
        self.powerups: List[Powerup] = []
        self.last_powerup_time = 0
        self.powerup_spawn_interval = 15
        self.mines: List[Mine] = []
        self.last_mine_spawn = 0
        self.mine_spawn_interval = 10
        self.score = 0
        self.start_time = 0
        self.elapsed_time = 0
        self.last_move_time = 0
        
        self.powerup_notification = None
        self.notification_start_time = 0
        self.notification_duration = 3000
        
        self.setup_ui()
        
        self.load_assets()
        
    def toggle_pause(self) -> None:
        """Toggle the paused state of the game."""
        self.paused = not self.paused
    
    def show_main_menu(self) -> None:
        """Show the main menu with Windows 98 style."""
        menu_active = True
        
        window_width = 500
        window_height = 500
        window_x = (WINDOW_WIDTH - window_width) // 2
        window_y = (WINDOW_HEIGHT - window_height) // 2
        
        background = self.screen.copy()
        
        while menu_active and self.running:
            mouse_pos = pygame.mouse.get_pos()
            mouse_clicked = False
            
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                    menu_active = False
                    return
                
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        self.running = False
                        menu_active = False
                        return
                    elif event.key == pygame.K_RETURN:
                        menu_active = False
                        self.new_game()
                        return
                
                if event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:
                        mouse_clicked = True
            
            self.screen.blit(background, (0, 0))
            
            content_top = self.draw_win98_frame(
                window_x, window_y, window_width, window_height, 
                "Snake Game"
            )
            
            title_font = pygame.font.SysFont('Arial', 48, bold=True)
            title = title_font.render("SNAKE GAME", True, (0, 0, 128))
            title_rect = title.get_rect(centerx=window_x + window_width // 2, 
                                      y=window_y + 80)
            self.screen.blit(title, title_rect)
            
            version_font = pygame.font.SysFont('Arial', 12)
            version_text = version_font.render("Version 1.0", True, (100, 100, 100))
            version_rect = version_text.get_rect(bottomright=(window_x + window_width - 10, 
                                                           window_y + window_height - 10))
            self.screen.blit(version_text, version_rect)
            
            button_width = 250
            button_height = 40
            button_x = window_x + (window_width - button_width) // 2
            button_spacing = 20
            
            start_rect = pygame.Rect(
                button_x, window_y + 180,
                button_width, button_height
            )
            self._draw_win98_button(start_rect, "Start Game (Enter)")
            
            settings_rect = pygame.Rect(
                button_x, window_y + 180 + button_height + button_spacing,
                button_width, button_height
            )
            self._draw_win98_button(settings_rect, "Settings")
            
            quit_rect = pygame.Rect(
                button_x, window_y + 180 + (button_height + button_spacing) * 2,
                button_width, button_height
            )
            self._draw_win98_button(quit_rect, "Quit (ESC)")
            
            if mouse_clicked:
                if start_rect.collidepoint(mouse_pos) or \
                   (pygame.key.get_pressed()[pygame.K_RETURN] and not pygame.key.get_pressed()[pygame.K_ESCAPE]):
                    menu_active = False
                    self.new_game()
                    return
                elif settings_rect.collidepoint(mouse_pos):
                    self.show_settings_menu()
                    background = self.screen.copy()
                elif quit_rect.collidepoint(mouse_pos) or pygame.key.get_pressed()[pygame.K_ESCAPE]:
                    self.running = False
                    menu_active = False
                    return
            
            pygame.display.flip()
            self.clock.tick(60)
    
    def show_settings_menu(self) -> None:
        """Show the settings menu."""
        menu_active = True
        
        title_font = pygame.font.SysFont('Arial', 48)
        large_font = pygame.font.SysFont('Arial', 32)
        medium_font = pygame.font.SysFont('Arial', 24)
        
        current_volume = 0.7
        if self.sounds and 'eat' in self.sounds:
            current_volume = self.sounds['eat'].get_volume()
        
        vol_down = Button(
            x=WINDOW_WIDTH//2 - 150, y=200, width=40, height=40,
            text="-", action=None, font=large_font
        )
        vol_up = Button(
            x=WINDOW_WIDTH//2 + 110, y=200, width=40, height=40,
            text="+", action=None, font=large_font
        )
        back_button = Button(
            x=WINDOW_WIDTH//2 - 50, y=400, width=100, height=40,
            text="Back", action=None, font=medium_font
        )
        
        buttons = [vol_down, vol_up, back_button]
        
        while menu_active and self.running:
            mouse_pos = pygame.mouse.get_pos()
            
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                    menu_active = False
                    return
                    
                if event.type == pygame.MOUSEBUTTONDOWN:
                    for button in buttons:
                        if button.is_hovered(mouse_pos):
                            if button == vol_down:
                                current_volume = max(0.0, current_volume - 0.1)
                            elif button == vol_up:
                                current_volume = min(1.0, current_volume + 0.1)
                            elif button == back_button:
                                menu_active = False
                                return
            
            for sound in self.sounds.values():
                sound.set_volume(current_volume)
            
            pygame.display.flip()
            self.clock.tick(60)
    
    def show_pause_menu(self) -> None:
        """Show the pause menu with Windows 98 style."""
        was_paused = self.paused
        self.paused = True
        menu_active = True
        in_settings = False
        
        background = self.screen.copy()
        
        window_width = 400
        window_height = 350
        window_x = (WINDOW_WIDTH - window_width) // 2
        window_y = (WINDOW_HEIGHT - window_height) // 2
        
        current_volume = 0.7
        if self.sounds and 'eat' in self.sounds:
            current_volume = self.sounds['eat'].get_volume()
        
        while menu_active and self.running:
            mouse_pos = pygame.mouse.get_pos()
            mouse_clicked = False
            
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                    menu_active = False
                    return
                    
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        if in_settings:
                            in_settings = False
                        else:
                            menu_active = False
                            self.paused = was_paused
                            return
                    elif event.key == pygame.K_p and not in_settings:
                        menu_active = False
                        self.paused = False
                        return
                    elif event.key == pygame.K_r and not in_settings:
                        menu_active = False
                        self.paused = False
                        return
                    elif event.key == pygame.K_q and not in_settings:
                        menu_active = False
                        self.paused = False
                        self.show_main_menu()
                        return
                
                if event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:
                        mouse_clicked = True
            
            self.screen.blit(background, (0, 0))
            
            content_top = self.draw_win98_frame(
                window_x, window_y, window_width, window_height, 
                "Game Paused"
            )
            
            if in_settings:
                content_rect = pygame.Rect(
                    window_x + 8, content_top,
                    window_width - 16, window_height - content_top - 8
                )
                
                font = pygame.font.SysFont('Arial', 16)
                vol_text = font.render(f"Volume: {int(current_volume * 100)}%", True, (0, 0, 0))
                vol_rect = vol_text.get_rect(x=window_x + 50, y=content_rect.y + 20)
                self.screen.blit(vol_text, vol_rect)
                
                vol_down_rect = pygame.Rect(
                    window_x + window_width - 120, content_rect.y + 15,
                    30, 25
                )
                vol_up_rect = pygame.Rect(
                    window_x + window_width - 80, content_rect.y + 15,
                    30, 25
                )
                
                if mouse_clicked:
                    if vol_down_rect.collidepoint(mouse_pos):
                        current_volume = max(0.0, current_volume - 0.1)
                    elif vol_up_rect.collidepoint(mouse_pos):
                        current_volume = min(1.0, current_volume + 0.1)
                
                for sound in self.sounds.values():
                    sound.set_volume(current_volume)
                
                self._draw_win98_button(vol_down_rect, "-")
                self._draw_win98_button(vol_up_rect, "+")
                
                back_rect = pygame.Rect(
                    window_x + (window_width - 100) // 2, window_y + window_height - 60,
                    100, 25
                )
                self._draw_win98_button(back_rect, "Back")
                
                if mouse_clicked and back_rect.collidepoint(mouse_pos):
                    in_settings = False
            else:
                content_rect = pygame.Rect(
                    window_x + 8, content_top,
                    window_width - 16, window_height - content_top - 8
                )
                
                button_width = 200
                button_height = 28
                button_x = window_x + (window_width - button_width) // 2
                button_spacing = 40
                
                resume_rect = pygame.Rect(
                    button_x, content_rect.y + 30,
                    button_width, button_height
                )
                self._draw_win98_button(resume_rect, "Resume (ESC or P)")
                
                settings_rect = pygame.Rect(
                    button_x, resume_rect.bottom + 15,
                    button_width, button_height
                )
                self._draw_win98_button(settings_rect, "Settings")
                
                quit_rect = pygame.Rect(
                    button_x, settings_rect.bottom + 15,
                    button_width, button_height
                )
                self._draw_win98_button(quit_rect, "Quit to Menu (Q)")
                
                if mouse_clicked:
                    if resume_rect.collidepoint(mouse_pos):
                        menu_active = False
                        self.paused = was_paused
                    elif settings_rect.collidepoint(mouse_pos):
                        in_settings = True
                        background = self.screen.copy()
                    elif quit_rect.collidepoint(mouse_pos):
                        menu_active = False
                        self.paused = False
                        self.show_main_menu()
                        return
            
            pygame.display.flip()
            self.clock.tick(60)
        
        self.paused = was_paused

    def setup_ui(self) -> None:
        """Set up the game's user interface."""
        self.header = Header(0, 0, WINDOW_WIDTH, HEADER_HEIGHT)
        
        self.header_stats = {
            "Score": 0,
            "High Score": self.state.high_score,
            "Time": "0:00"
        }
        self.header.update_stats(self.header_stats)
        
    def play_sound(self, sound_name: str) -> None:
        """Play a sound effect if it exists.
        
        Args:
            sound_name: Name of the sound to play (must be a key in self.sounds)
        """
        print(f"Attempting to play sound: {sound_name}")
        print(f"Available sounds: {list(self.sounds.keys())}")
        
        if sound_name in self.sounds and self.sounds[sound_name]:
            try:
                print(f"Playing sound: {sound_name}")
                self.sounds[sound_name].play()
                print("Sound played successfully")
            except Exception as e:
                print(f"Error playing sound {sound_name}: {e}")
        else:
            print(f"Sound {sound_name} not found or not loaded")
            
    def load_assets(self) -> None:
        """Load game assets."""
        self.sounds = {}
        try:
            sounds_path = Path(SOUNDS_DIR) if isinstance(SOUNDS_DIR, str) else SOUNDS_DIR
            powerup_sound = sounds_path / 'powerUp.wav'
            explosion_sound = sounds_path / 'explosion.wav'
            
            print(f"Looking for sound files in: {sounds_path}")
            print(f"Powerup sound exists: {powerup_sound.exists()}")
            print(f"Explosion sound exists: {explosion_sound.exists()}")
            
            pickup_sound = sounds_path / 'pickup.wav'
            
            if powerup_sound.exists():
                self.sounds['powerup'] = pygame.mixer.Sound(str(powerup_sound))
            if explosion_sound.exists():
                self.sounds['game_over'] = pygame.mixer.Sound(str(explosion_sound))
            if pickup_sound.exists():
                self.sounds['eat'] = pygame.mixer.Sound(str(pickup_sound))
                print(f"Loaded game_over sound: {self.sounds['game_over']}")
                
            for name, sound in self.sounds.items():
                sound.set_volume(0.5)
                print(f"Set volume for {name} to 0.5")
                
            print(f"Loaded sounds: {list(self.sounds.keys())}")
                
        except Exception as e:
            print(f"Error loading sounds: {e}")
            self.sounds = {}
            
        try:
            self.font = pygame.font.SysFont("Arial", 24, bold=True)
            self.large_font = pygame.font.SysFont("Arial", 48, bold=True)
        except:
            self.font = pygame.font.SysFont(None, 28, bold=True)
            self.large_font = pygame.font.SysFont(None, 56, bold=True)
    
    def run(self) -> None:
        """Run the main game loop."""
        self.running = True
        self.show_main_menu()
        self.clock = pygame.time.Clock()
        
        while self.running:
            self.handle_events()
            if not self.paused and not self.game_over:
                self.update()
            self.draw()
            self.clock.tick(FPS)
            
        pygame.quit()
        sys.exit()
        
    def new_game(self) -> None:
        """Start a new game."""
        self.game_over = False
        self.paused = False
        self.score = 0
        self.header_stats["Score"] = 0
        self.powerups.clear()
        self.last_powerup_time = 0
        
        start_x = (WINDOW_WIDTH // (2 * SNAKE_BLOCK)) * SNAKE_BLOCK
        start_y = ((WINDOW_HEIGHT - GAME_AREA_TOP) // (2 * SNAKE_BLOCK) * SNAKE_BLOCK 
                  + GAME_AREA_TOP)
        self.snake = Snake(start_x, start_y, SNAKE_BLOCK, self.state.snake_skin)
        
        self.spawn_food()
        self.spawn_powerup()
        self.spawn_mine()
        
        self.start_time = time.time()
        self.elapsed_time = 0
        
        self.header_stats["High Score"] = self.state.high_score
        self.header.update_stats(self.header_stats)
        
    def spawn_food(self) -> None:
        """Spawn food at a random position, ensuring it doesn't appear in the header area."""
        if not self.snake:
            return
            
        play_area = pygame.Rect(0, GAME_AREA_TOP, WINDOW_WIDTH, WINDOW_HEIGHT - GAME_AREA_TOP)
        
        occupied = [(seg.x, seg.y) for seg in self.snake.segments]
        if hasattr(self, 'food') and self.food:
            occupied.append((self.food.x, self.food.y))
            
        for _ in range(100):  
            x = random.randrange(0, WINDOW_WIDTH - SNAKE_BLOCK, SNAKE_BLOCK)
            y = random.randrange(GAME_AREA_TOP, WINDOW_HEIGHT - SNAKE_BLOCK, SNAKE_BLOCK)
            
            if (x, y) not in occupied:
                self.food = Food(x, y, SNAKE_BLOCK)
                return
                
        x = random.randrange(0, WINDOW_WIDTH - SNAKE_BLOCK, SNAKE_BLOCK)
        y = random.randrange(GAME_AREA_TOP, WINDOW_HEIGHT - SNAKE_BLOCK, SNAKE_BLOCK)
        self.food = Food(x, y, SNAKE_BLOCK)
        
    def spawn_powerup(self) -> None:
        """Spawn a random powerup at a random position."""
        if not self.snake:
            return
            
        from src.components.powerups.speed_boost import SpeedBoost
        from src.components.powerups.slow_mo import SlowMo
        from src.components.powerups.double_points import DoublePoints
        from src.components.powerups.invincible import Invincible
        
        powerup_info = {
            'SpeedBoost': {'name': 'SPEED BOOST', 'color': (0, 191, 255)},
            'SlowMo': {'name': 'SLOW MOTION', 'color': (147, 112, 219)},
            'DoublePoints': {'name': 'DOUBLE POINTS', 'color': (255, 215, 0)},
            'Invincible': {'name': 'INVINCIBILITY', 'color': (255, 0, 255)}
        }
        
        powerup_weights = [
            (SpeedBoost, 4.0),
            (SlowMo, 3.0),
            (DoublePoints, 2.0),
            (Invincible, 1.0)
        ]
        
        powerup_choices = []
        for powerup_type, weight in powerup_weights:
            powerup_choices.extend([powerup_type] * int(weight * 10))
        
        attempts = 0
        max_attempts = 20
        
        while attempts < max_attempts:
            attempts += 1
            x = random.randrange(0, WINDOW_WIDTH - SNAKE_BLOCK, SNAKE_BLOCK)
            y = random.randrange(GAME_AREA_TOP, WINDOW_HEIGHT - SNAKE_BLOCK, SNAKE_BLOCK)
            
            position_occupied = (
                any(seg.x == x and seg.y == y for seg in self.snake.segments) or
                (self.food and x == self.food.x and y == self.food.y) or
                any(p.x == x and p.y == y for p in self.powerups)
            )
            
            if not position_occupied:
                PowerupClass = random.choice(powerup_choices)
                powerup = PowerupClass(x, y)
                self.powerups.append(powerup)
                
                return
                
    def spawn_mine(self) -> None:
        """Spawn a mine at a random position."""
        mine = Mine.create_random(WINDOW_WIDTH, WINDOW_HEIGHT, min_y=GAME_AREA_TOP)
        
        mine_rect = mine.get_rect()
        valid_position = True
        
        for segment in self.snake.segments:
            if mine_rect.colliderect(pygame.Rect(segment.x, segment.y, SNAKE_BLOCK, SNAKE_BLOCK)):
                valid_position = False
                break
        
        if valid_position and mine_rect.colliderect(self.food.get_rect()):
            valid_position = False
            
        if valid_position:
            for powerup in self.powerups:
                if mine_rect.colliderect(pygame.Rect(powerup.x, powerup.y, SNAKE_BLOCK, SNAKE_BLOCK)):
                    valid_position = False
                    break
        
        if valid_position:
            self.mines.append(mine)
    
    def handle_events(self) -> None:
        """Handle pygame events."""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
                continue
                
            self.header.handle_event(event)
            
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    if not self.game_over:
                        self.show_pause_menu()
                elif not self.game_over and not self.paused:
                    if event.key in (pygame.K_UP, pygame.K_w):
                        self.snake.change_direction("UP")
                    elif event.key in (pygame.K_DOWN, pygame.K_s):
                        self.snake.change_direction("DOWN")
                    elif event.key in (pygame.K_LEFT, pygame.K_a):
                        self.snake.change_direction("LEFT")
                    elif event.key in (pygame.K_RIGHT, pygame.K_d):
                        self.snake.change_direction("RIGHT")
                    elif event.key == pygame.K_p:
                        self.show_pause_menu()
                    elif event.key == pygame.K_SPACE:
                        self.snake.grow()
                        
                elif self.game_over:
                    if event.key == pygame.K_r:
                        self.new_game()
                    elif event.key == pygame.K_q:
                        self.running = False
                        
    def update(self) -> None:
        """Update game state."""
        if self.paused or self.game_over:
            return
            
        current_time = pygame.time.get_ticks()
        
        self.snake.move(current_time)
        
        head = self.snake.segments[0]
        
        if len(self.snake.segments) > 4:
            for segment in self.snake.segments[4:]:
                if head.x == segment.x and head.y == segment.y:
                    if not self.snake.invincible:
                        self.game_over = True
                        self.play_sound('game_over')
                        self.state.update_high_score(self.score)
                        return
                    else:
                        self.play_sound('powerup')
                        return
            
        if (self.snake.get_head_rect().colliderect(self.food.get_rect())):
            points = self.food.value * 2 if hasattr(self.snake, 'double_points') and self.snake.double_points else self.food.value
            
            self.snake.grow(self.food.value)
            self.score += points
            self.header_stats["Score"] = self.score
            self.header.update_stats(self.header_stats)
            self.spawn_food()
            self.play_sound('eat')
            
            if random.random() < 0.3:
                self.spawn_powerup()
        
        for mine in self.mines[:]:
            if self.snake.get_head_rect().colliderect(mine.get_rect()):
                if not self.snake.invincible:
                    self.game_over = True
                    self.play_sound('game_over')
                    self.state.update_high_score(self.score)
                    return
                else:
                    self.mines.remove(mine)
                    self.play_sound('powerup')
                    continue
        
        for powerup in self.powerups[:]:
            if self.snake.get_head_rect().colliderect(pygame.Rect(powerup.x, powerup.y, SNAKE_BLOCK, SNAKE_BLOCK)):
                powerup.apply(self.snake)
                
                powerup_name = powerup.__class__.__name__
                powerup_info = {
                    'SpeedBoost': {'name': 'SPEED BOOST', 'color': (0, 191, 255)},
                    'SlowMo': {'name': 'SLOW MOTION', 'color': (147, 112, 219)},
                    'DoublePoints': {'name': 'DOUBLE POINTS', 'color': (255, 215, 0)},
                    'Invincible': {'name': 'INVINCIBILITY', 'color': (255, 0, 255)}
                }
                
                if powerup_name in powerup_info:
                    self.powerup_notification = {
                        'text': f"POWERUP: {powerup_info[powerup_name]['name']}",
                        'color': powerup_info[powerup_name]['color']
                    }
                    self.notification_start_time = pygame.time.get_ticks()
                
                self.powerups.remove(powerup)
                self.play_sound('eat')
                self.state.powerups_collected += 1
        
        current_time = time.time()
        if (current_time - self.last_powerup_time > self.powerup_spawn_interval and
            len(self.powerups) < 5):
            self.spawn_powerup()
            self.last_powerup_time = current_time
            
        current_time = pygame.time.get_ticks()
        
        if hasattr(self.snake, 'active_powerups'):
            powerups_to_deactivate = []
            for powerup_type, end_time in list(self.snake.active_powerups.items()):
                if current_time >= end_time:
                    if hasattr(self.snake, 'powerup_effects') and powerup_type in self.snake.powerup_effects:
                        self.snake.powerup_effects[powerup_type](False)
                        print(f"Deactivating powerup: {powerup_type}")
                    powerups_to_deactivate.append(powerup_type)
            
            for powerup_type in powerups_to_deactivate:
                if powerup_type in self.snake.active_powerups:
                    del self.snake.active_powerups[powerup_type]
        
        powerups_to_remove = []
        for powerup in self.powerups[:]:
            if not powerup.update():
                if powerup.active:
                    powerup.remove(self.snake)
                powerups_to_remove.append(powerup)
                continue
                
            if not powerup.active and current_time - powerup.spawn_time > powerup.lifetime:
                powerups_to_remove.append(powerup)
        
        for powerup in powerups_to_remove:
            if powerup in self.powerups:
                self.powerups.remove(powerup)
        
        for mine in self.mines:
            mine.update()
        
        current_time = time.time()
        if current_time - self.last_mine_spawn > self.mine_spawn_interval and len(self.mines) < 5:  # Max 5 mines at once
            self.spawn_mine()
            self.last_mine_spawn = current_time
        
        if hasattr(self.snake, 'active_powerups'):
            expired_powerups = []
            for powerup_type, end_time in list(self.snake.active_powerups.items()):
                if current_time >= end_time:
                    if powerup_type in self.snake.powerup_effects:
                        self.snake.powerup_effects[powerup_type](False)
                    expired_powerups.append(powerup_type)
            
            for powerup_type in expired_powerups:
                if powerup_type in self.snake.active_powerups:
                    del self.snake.active_powerups[powerup_type]
        
        if hasattr(self, 'header') and hasattr(self, 'header_stats'):
            stats = self.header_stats.copy()
            stats['snake'] = self.snake
            self.header.update_stats(stats)
                
        self.elapsed_time = time.time() - self.start_time
        minutes = int(self.elapsed_time // 60)
        seconds = int(self.elapsed_time % 60)
        self.header_stats["Time"] = f"{minutes}:{seconds:02d}"
        self.header.update_stats(self.header_stats)
        
    def draw(self) -> None:
        """Draw the game state."""
        self.screen.fill((0, 0, 0))
        
        pygame.draw.rect(
            self.screen, 
            (0, 0, 0),
            (0, GAME_AREA_TOP, WINDOW_WIDTH, WINDOW_HEIGHT - GAME_AREA_TOP)
        )
        
        for mine in self.mines:
            mine.draw(self.screen)
        
        if self.food:
            self.food.draw(self.screen)
            
        for powerup in self.powerups:
            powerup.draw(self.screen)
            
        if self.snake:
            self.snake.draw(self.screen)
            
        self.draw_ui()
        
        if self.paused:
            self.draw_pause_screen()
            
        if self.game_over:
            self.draw_game_over()
            
        pygame.display.flip()
        
    def draw_grid(self) -> None:
        pass
        
    def draw_ui(self) -> None:
        if hasattr(self, 'snake'):
            self.header.stats['snake'] = self.snake
            
        self.header.draw(self.screen)
        
        if 'snake' in self.header.stats:
            del self.header.stats['snake']
            
        if self.powerup_notification:
            current_time = pygame.time.get_ticks()
            time_elapsed = current_time - self.notification_start_time
            opacity = min(255, 255 - (time_elapsed / self.notification_duration) * 255)
            
            text = self.powerup_notification['text']
            color = self.powerup_notification['color']
            
            font = pygame.font.SysFont('Arial', 20, bold=True)
            text_surface = font.render(text, True, color)
            
            bg_rect = text_surface.get_rect()
            bg_rect.inflate_ip(20, 10)
            bg_rect.midtop = (WINDOW_WIDTH // 2, GAME_AREA_TOP + 20)
            
            s = pygame.Surface((bg_rect.width, bg_rect.height), pygame.SRCALPHA)
            s.fill((40, 40, 40, int(255 * 0.7)))
            s.set_alpha(int(opacity))
            self.screen.blit(s, bg_rect.topleft)
            
            text_rect = text_surface.get_rect(center=bg_rect.center)
            text_surface.set_alpha(int(opacity))
            self.screen.blit(text_surface, text_rect)
                
    def draw_pause_screen(self) -> None:
        pass
        text_rect = text.get_rect(center=(WINDOW_WIDTH//2, WINDOW_HEIGHT//2))
        self.screen.blit(text, text_rect)
        
        font = pygame.font.Font(None, 36)
        resume_text = font.render("Press P or ESC to resume", True, (200, 200, 200))
        resume_rect = resume_text.get_rect(center=(WINDOW_WIDTH//2, WINDOW_HEIGHT//2 + 60))
        self.screen.blit(resume_text, resume_rect)
        
    def draw_win98_frame(self, x, y, width, height, title=""):
        WHITE = (255, 255, 255)
        LIGHT_GRAY = (192, 192, 192)
        DARK_GRAY = (128, 128, 128)
        BLACK = (0, 0, 0)
        BLUE = (0, 0, 128)
        
        pygame.draw.rect(self.screen, LIGHT_GRAY, (x, y, width, height))
        
        pygame.draw.line(self.screen, WHITE, (x, y), (x + width - 1, y), 2)
        pygame.draw.line(self.screen, WHITE, (x, y), (x, y + height - 1), 2)
        
        pygame.draw.line(self.screen, DARK_GRAY, (x + 1, y + 1), (x + width - 2, y + 1), 1)
        pygame.draw.line(self.screen, DARK_GRAY, (x + 1, y + 1), (x + 1, y + height - 2), 1)
        
        pygame.draw.line(self.screen, BLACK, (x, y + height - 1), (x + width - 1, y + height - 1), 2)
        pygame.draw.line(self.screen, BLACK, (x + width - 1, y), (x + width - 1, y + height - 1), 2)
        
        pygame.draw.line(self.screen, DARK_GRAY, (x + 1, y + height - 2), (x + width - 2, y + height - 2), 1)
        pygame.draw.line(self.screen, DARK_GRAY, (x + width - 2, y + 1), (x + width - 2, y + height - 2), 1)
        
        if title:
            title_bar = pygame.Rect(x + 2, y + 2, width - 4, 24)
            pygame.draw.rect(self.screen, BLUE, title_bar)
            
            font = pygame.font.SysFont('Arial', 12, bold=True)
            text = font.render(title, True, WHITE)
            text_rect = text.get_rect(midleft=(x + 8, y + 13))
            self.screen.blit(text, text_rect)
            
            close_rect = pygame.Rect(x + width - 20, y + 5, 14, 14)
            pygame.draw.rect(self.screen, LIGHT_GRAY, close_rect)
            pygame.draw.rect(self.screen, BLACK, close_rect, 1)
            
            font = pygame.font.SysFont('Arial', 10, bold=True)
            text = font.render('x', True, BLACK)
            self.screen.blit(text, (close_rect.x + 3, close_rect.y + 1))
            
            return title_bar.bottom + 2
        return y + 4

    def draw_game_over(self) -> None:
        self.screen.fill((192, 220, 192))
        
        window_width = 400
        window_height = 300
        window_x = (WINDOW_WIDTH - window_width) // 2
        window_y = (WINDOW_HEIGHT - window_height) // 2
        
        content_top = self.draw_win98_frame(
            window_x, window_y, window_width, window_height, 
            "Game Over"
        )
        
        content_rect = pygame.Rect(
            window_x + 8, content_top,
            window_width - 16, window_height - content_top - 8
        )
        font = pygame.font.SysFont('Arial', 32, bold=True)
        text = font.render("GAME OVER", True, (128, 0, 0))
        text_rect = text.get_rect(centerx=WINDOW_WIDTH // 2, y=content_rect.y + 20)
        self.screen.blit(text, text_rect)
        
        font = pygame.font.SysFont('Arial', 16)
        
        score_text = font.render(f"Final Score: {self.score}", True, (0, 0, 0))
        score_rect = score_text.get_rect(centerx=WINDOW_WIDTH // 2, y=text_rect.bottom + 40)
        self.screen.blit(score_text, score_rect)
        
        high_text = font.render(f"High Score: {self.state.high_score}", True, (0, 0, 0))
        high_rect = high_text.get_rect(centerx=WINDOW_WIDTH // 2, y=score_rect.bottom + 20)
        self.screen.blit(high_text, high_rect)
        
        button_font = pygame.font.SysFont('Arial', 12, bold=True)
        
        restart_rect = pygame.Rect(
            window_x + 50, high_rect.bottom + 40,
            100, 28
        )
        self._draw_win98_button(restart_rect, "Restart (R)")
        
        quit_rect = pygame.Rect(
            window_x + window_width - 150, high_rect.bottom + 40,
            100, 28
        )
        self._draw_win98_button(quit_rect, "Quit (Q)")
        
        font = pygame.font.SysFont('Arial', 12)
        instr_text = font.render("Press R to restart or Q to quit", True, (0, 0, 0))
        instr_rect = instr_text.get_rect(centerx=WINDOW_WIDTH // 2, y=restart_rect.bottom + 30)
        self.screen.blit(instr_text, instr_rect)
        
    def _draw_win98_button(self, rect, text):
        """Draw a Windows 98 style button."""
        WHITE = (255, 255, 255)
        LIGHT_GRAY = (192, 192, 192)
        DARK_GRAY = (128, 128, 128)
        BLACK = (0, 0, 0)
        
        pygame.draw.rect(self.screen, LIGHT_GRAY, rect)
        
        pygame.draw.line(self.screen, WHITE, (rect.left, rect.top), (rect.right - 1, rect.top))
        pygame.draw.line(self.screen, WHITE, (rect.left, rect.top), (rect.left, rect.bottom - 1))
        pygame.draw.line(self.screen, BLACK, (rect.left, rect.bottom - 1), (rect.right - 1, rect.bottom - 1))
        pygame.draw.line(self.screen, BLACK, (rect.right - 1, rect.top), (rect.right - 1, rect.bottom - 1))
        
        pygame.draw.line(self.screen, DARK_GRAY, (rect.left + 1, rect.top + 1), (rect.right - 2, rect.top + 1))
        pygame.draw.line(self.screen, DARK_GRAY, (rect.left + 1, rect.top + 1), (rect.left + 1, rect.bottom - 2))
        
        font = pygame.font.SysFont('Arial', 12, bold=True)
        text_surface = font.render(text, True, BLACK)
        text_rect = text_surface.get_rect(center=rect.center)
        self.screen.blit(text_surface, text_rect)