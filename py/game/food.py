import random
import py.core.constants as constants

def get_random_food_position():
    while True:
        x = round(random.randrange(0, constants.dis_width - constants.snake_block) / constants.snake_block) * constants.snake_block
        y = round(random.randrange(constants.GAME_AREA_TOP, constants.dis_height - constants.snake_block) / constants.snake_block) * constants.snake_block
        
        x = int(x)
        y = int(y)
        
        if (constants.GAME_AREA_TOP <= y < constants.dis_height - constants.snake_block and 
            0 <= x < constants.dis_width - constants.snake_block):
            return x, y
