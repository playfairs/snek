# Game settings
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 800
GAME_TITLE = "Snek Game"

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
GRAY = (200, 200, 200)
DARK_GRAY = (150, 150, 150)
LIGHT_GRAY = (230, 230, 230)

# Game settings
SNAKE_BLOCK = 20
SNAKE_SPEED = 10
FPS = 60  # Frames per second
GAME_AREA_TOP = 70
HEADER_HEIGHT = 70

# Powerup settings
POWERUP_DURATION = 10  # seconds
POWERUP_SPAWN_INTERVAL = 15  # seconds

# File paths
import os
from pathlib import Path

# Get the directory where this file is located
BASE_DIR = Path(__file__).resolve().parent.parent.parent  # Go up one more level to reach the root
ASSETS_DIR = os.path.join(BASE_DIR, 'assets')
SOUNDS_DIR = os.path.join(ASSETS_DIR, 'sfx')

# Debug output
print(f"Assets directory: {ASSETS_DIR}")
print(f"Sounds directory: {SOUNDS_DIR}")
print(f"Powerup sound exists: {os.path.exists(os.path.join(SOUNDS_DIR, 'powerUp.wav'))}")

# Ensure directories exist
os.makedirs(SOUNDS_DIR, exist_ok=True)

# Snake skins
SNAKE_SKINS = {
    'default': {
        'head_color': (0, 255, 0),  # Green
        'body_color': (0, 200, 0)   # Darker green
    },
    'blue': {
        'head_color': (0, 0, 255),
        'body_color': (0, 0, 200)
    },
    'red': {
        'head_color': (255, 0, 0),
        'body_color': (200, 0, 0)
    },
    'rainbow': {
        'head_color': (255, 0, 0),  # Will be overridden with rainbow colors
        'body_color': (255, 0, 0)   # Will be overridden with rainbow colors
    }
}

# Powerup colors
POWERUP_COLORS = {
    'speed_boost': (0, 191, 255),    # Blue
    'slow_mo': (147, 112, 219),      # Purple
    'double_points': (255, 215, 0),  # Gold
    'invincible': (255, 0, 255)      # Magenta
}
