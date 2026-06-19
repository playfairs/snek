import random
import time
import src.core.constants as constants

def spawn_powerup():
    def get_random_position():
        while True:
            x = round(random.randrange(0, constants.dis_width - constants.snake_block) / constants.snake_block) * constants.snake_block
            y = round(random.randrange(constants.GAME_AREA_TOP, constants.dis_height - constants.snake_block) / constants.snake_block) * constants.snake_block
            
            x = int(x)
            y = int(y)
            
            if (constants.GAME_AREA_TOP <= y < constants.dis_height - constants.snake_block and 
                0 <= x < constants.dis_width - constants.snake_block):
                return x, y
    
    powerup_x, powerup_y = get_random_position()
    powerup_type = random.choice(constants.POWERUP_TYPES)
    powerup_time = time.time()
    return {
        'type': powerup_type,
        'x': powerup_x,
        'y': powerup_y,
        'time': powerup_time,
        'active': True
    }
