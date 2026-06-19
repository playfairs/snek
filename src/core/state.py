import pygame
import os
import sys

project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

import src.core.constants as constants

current_skin = 'default'

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
    dis = pygame.display.set_mode((constants.dis_width, constants.dis_height))

def init_fonts():
    global font_style, score_font, button_font, large_font
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

def init_clock():
    global clock
    clock = pygame.time.Clock()
