import pygame
import math
import py.core.constants as constants
import py.core.state as state
import py.render.cube as cube

def our_snake(snake_block, snake_list, powerup_active=None):
    if not snake_list:
        return
    
    for x in snake_list[:-1]:
        color = constants.SNAKE_SKINS[state.current_skin]['body_color']
        if state.current_skin == 'rainbow':
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
            
        cube.draw_cube(x[0], x[1], snake_block, color, is_head=False)
    
    if snake_list:
        head = snake_list[-1]
        head_color = constants.SNAKE_SKINS[state.current_skin]['head_color']
        if state.current_skin == 'rainbow':
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
        
        cube.draw_cube(head[0], head[1], snake_block, head_color, is_head=True)
        pygame.draw.rect(state.dis, (0, 200, 0), [head[0], head[1], snake_block, snake_block], 1)
