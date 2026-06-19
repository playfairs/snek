import pygame
import sys
import os

project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

from py.core.constants import dis_width, dis_height
from py.core.state import init_display, init_fonts, init_clock, init_settings, dis
from py.core.stats import load_stats
from py.audio.sounds import init_sounds
from py.ui.menus import main_menu
from py.game.game import gameLoop

if __name__ == "__main__":
    pygame.init()
    pygame.font.init()
    
    init_display()
    init_fonts()
    init_clock()
    init_settings()
    init_sounds()
    
    pygame.display.set_caption('Snek')
    
    if pygame.version.vernum[0] >= 2:
        try:
            import ctypes
            ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("com.windows98.snake")
        except:
            pass
    
    try:
        main_menu(gameLoop)
    finally:
        pygame.quit()
