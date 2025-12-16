import random
from enum import Enum, auto
from dataclasses import dataclass
from typing import Dict, List, Tuple, Optional

class GameSettings:
    SCREEN_WIDTH = 800
    SCREEN_HEIGHT = 600
    FPS = 60
    GRID_SIZE = 20
    GRID_WIDTH = SCREEN_WIDTH // GRID_SIZE
    GRID_HEIGHT = SCREEN_HEIGHT // GRID_SIZE
    GAME_TITLE = "Snek Evolution"
    BACKGROUND_COLOR = (25, 25, 25)
    GRID_COLOR = (40, 40, 40)
    GRID_OPACITY = 30

class Colors:
    WHITE = (255, 255, 255)
    BLACK = (0, 0, 0)
    
    LIGHT_GRAY = (192, 192, 192)
    MEDIUM_GRAY = (128, 128, 128)
    DARK_GRAY = (64, 64, 64)
    
    PRIMARY = (46, 204, 113)      # Emerald
    SECONDARY = (52, 152, 219)    # Peter River
    DANGER = (231, 76, 60)        # Alizarin
    WARNING = (241, 196, 15)      # Sun Flower
    SUCCESS = (39, 174, 96)       # Nephritis
    INFO = (41, 128, 185)         # Belize Hole
    
    SNAKE_HEAD = (26, 188, 156)   # Turquoise
    SNAKE_BODY = (22, 160, 133)   # Green Sea
    FOOD = (230, 126, 34)         # Carrot Orange
    OBSTACLE = (149, 165, 166)    # Concrete
    
    @staticmethod
    def random_color() -> Tuple[int, int, int]:
        return (random.randint(50, 200), random.randint(50, 200), random.randint(50, 200))

class GameMode(Enum):
    CLASSIC = auto()
    ARCADE = auto()
    TIME_ATTACK = auto()
    ZEN = auto()
    CHALLENGE = auto()

@dataclass
class SnakeConfig:
    BASE_HEALTH: int = 100  # Changing this works universally for the Difficulty "Health Modifier"
    INITIAL_LENGTH: int = 3
    INITIAL_SPEED: float = 5.0
    SPEED_INCREMENT: float = 0.2
    MAX_SPEED: float = 15.0
    TURN_RADIUS: int = 1

    HEAD_COLOR: Tuple[int, int, int] = Colors.SNAKE_HEAD
    BODY_COLOR: Tuple[int, int, int] = Colors.SNAKE_BODY
    TRAIL_EFFECT: bool = True
    TRAIL_LENGTH: int = 5
    
    CAN_PASS_THROUGH_WALLS: bool = False
    CAN_COLLIDE_WITH_SELF: bool = True
    
    def get_speed_ms(self) -> float:
        """Convert speed from grid cells per second to milliseconds per cell"""
        return 1000.0 / (self.INITIAL_SPEED * GameSettings.GRID_SIZE)
        
    @classmethod
    def get_max_health(cls, difficulty: str) -> int:
        """Get max health based on difficulty"""
        difficulty_modifier = Difficulty[difficulty].value.get('health_modifier', 1.0)
        return int(cls.BASE_HEALTH * difficulty_modifier)

@dataclass
class FoodConfig:
    BASE_SCORE: int = 10
    SPAWN_RATE: float = 1.0
    MAX_ON_SCREEN: int = 5
    
    class FoodType(Enum):
        NORMAL = auto()
        GOLDEN = auto()
        POISON = auto()
        
    FOOD_COLORS = {
        FoodType.NORMAL: (46, 204, 113),  # Green
        FoodType.GOLDEN: (241, 196, 15),  # Yellow
        FoodType.POISON: (192, 57, 43)    # Red
    }
    
    FOOD_VALUES = {
        FoodType.NORMAL: 10,
        FoodType.GOLDEN: 50,
        FoodType.POISON: -20
    }

@dataclass
class PowerUpConfig:
    class PowerUpType(Enum):
        SPEED_BOOST = auto()
        SLOW_MO = auto()
        DOUBLE_POINTS = auto()
        INVINCIBILITY = auto()
        SHRINK = auto()
        GHOST = auto()
        
    DURATIONS = {
        PowerUpType.SPEED_BOOST: 10.0,
        PowerUpType.SLOW_MO: 8.0,
        PowerUpType.DOUBLE_POINTS: 15.0,
        PowerUpType.INVINCIBILITY: 5.0,
        PowerUpType.SHRINK: 12.0,
        PowerUpType.GHOST: 10.0
    }
    
    COLORS = {
        PowerUpType.SPEED_BOOST: (52, 152, 219),    # Blue
        PowerUpType.SLOW_MO: (155, 89, 182),        # Purple
        PowerUpType.DOUBLE_POINTS: (241, 196, 15),  # Yellow
        PowerUpType.INVINCIBILITY: (236, 240, 241), # Silver
        PowerUpType.SHRINK: (230, 126, 34),         # Orange
        PowerUpType.GHOST: (52, 73, 94)             # Midnight Blue
    }
    
    SPAWN_CHANCE: float = 0.2
    MAX_ACTIVE: int = 2

class AudioConfig:
    ENABLED: bool = True
    VOLUME: float = 0.5

    SOUNDS = {
        'powerup': 'powerUp.wav',
        'explosion': 'explosion.wav'
    }
    
    @classmethod
    def get_sound_path(cls, sound_name: str) -> str:
        import os
        base_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        return os.path.join(base_path, 'sfx', cls.SOUNDS[sound_name])

class ParticleConfig:
    ENABLED: bool = True
    MAX_PARTICLES: int = 500
    
    class ParticleType(Enum):
        FOOD = auto()
        SNAKE = auto()
        POWERUP = auto()
    
    COLORS = {
        ParticleType.FOOD: [(255, 255, 0), (255, 165, 0)],
        ParticleType.SNAKE: [(46, 204, 113), (39, 174, 96)],
        ParticleType.POWERUP: [(155, 89, 182), (142, 68, 173)]
    }
    
    LIFETIME_RANGE = (0.5, 2.0)  # In seconds
    SIZE_RANGE = (2, 8)          # In pixels
    SPEED_RANGE = (50, 200)

class Difficulty(Enum):
    EASY = {
        'snake_speed': 0.75,
        'food_spawn_rate': 1.2,
        'powerup_spawn_chance': 0.15,
        'starting_lives': 3,
        'health_modifier': 1.5,  # 150 health (100 * 1.5)
        'description': 'Easier gameplay with more health and lives'
    }
    
    NORMAL = {
        'snake_speed': 1.0,
        'food_spawn_rate': 1.0,
        'powerup_spawn_chance': 0.1,
        'starting_lives': 1,
        'health_modifier': 1.0,  # 100 health (100 * 1.0)
        'description': 'Standard gameplay experience'
    }
    
    HARD = {
        'snake_speed': 1.5,
        'food_spawn_rate': 0.8,
        'powerup_spawn_chance': 0.05,
        'starting_lives': 1,
        'obstacles': True,
        'health_modifier': 0.5,  # 50 health (100 * 0.5), maths :3
        'description': 'Challenging gameplay with reduced health'
    }

class Theme(Enum):
    CLASSIC = {
        'background': (25, 25, 25),
        'grid': (40, 40, 40),
        'snake_head': (46, 204, 113),
        'snake_body': (39, 174, 96),
        'food': (230, 126, 34)
    }
    
    DARK = {
        'background': (15, 15, 20),
        'grid': (30, 30, 35),
        'snake_head': (52, 152, 219),
        'snake_body': (41, 128, 185),
        'food': (231, 76, 60)
    }
    
    RETRO = {
        'background': (0, 0, 0),
        'grid': (0, 40, 0),
        'snake_head': (0, 255, 0),
        'snake_body': (0, 200, 0),
        'food': (255, 0, 0)
    }
