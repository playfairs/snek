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

dis_width = 1000
dis_height = 800

HEADER_HEIGHT = 70
GAME_AREA_TOP = HEADER_HEIGHT

snake_block = 20
snake_speed = 10
