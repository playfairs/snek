import pygame
import os
import sys

project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

import src.core.constants as constants
import src.core.stats as stats

def get_asset_path(relative_path):
    """Get absolute path to asset file relative to project root."""
    return os.path.join(project_root, relative_path)

current_skin = 'default'
sound_enabled = True
music_enabled = True
volume = 0.7
difficulty = 'normal'

def init_settings():
    global current_skin, sound_enabled, music_enabled, volume
    stats.load_stats()
    current_skin = stats.game_stats.get('snake_skin', 'default')
    sound_enabled = stats.game_stats.get('sound_enabled', True)
    music_enabled = stats.game_stats.get('music_enabled', True)
    volume = stats.game_stats.get('volume', 0.7)

dis = None
clock = None

dis_width = constants.dis_width
dis_height = constants.dis_height

font_style = None
score_font = None
button_font = None
large_font = None

def init_display():
    global dis
    try:
        dis = pygame.display.set_mode((constants.dis_width, constants.dis_height))
    except pygame.error as e:
        print(f"Error initializing display: {e}")
        raise

def init_fonts():
    global font_style, score_font, button_font, large_font
    try:
        font_style = pygame.font.SysFont("Arial", 14, bold=True)
        score_font = pygame.font.SysFont("Arial", 18, bold=True)
        button_font = pygame.font.SysFont("Arial", 14, bold=True)
        large_font = pygame.font.SysFont("Arial", 24, bold=True)
    except pygame.error as e:
        print(f"Error loading system fonts, using fallback: {e}")
        try:
            font_style = pygame.font.SysFont(None, 16, bold=True)
            score_font = pygame.font.SysFont(None, 20, bold=True)
            button_font = pygame.font.SysFont(None, 16, bold=True)
            large_font = pygame.font.SysFont(None, 28, bold=True)
        except pygame.error as e2:
            print(f"Error loading fallback fonts: {e2}")
            raise

def init_clock():
    global clock
    try:
        clock = pygame.time.Clock()
    except pygame.error as e:
        print(f"Error initializing clock: {e}")
        raise
