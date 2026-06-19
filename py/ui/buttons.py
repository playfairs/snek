import pygame
import py.core.constants as constants
import py.core.state as state

def text_objects(text, font, color=constants.white):
    text_surface = font.render(text, True, color)
    return text_surface, text_surface.get_rect()

def draw_button_3d(x, y, w, h, is_pressed=False):
    pygame.draw.rect(state.dis, constants.light_gray, (x, y, w, h))
    
    if is_pressed:
        pygame.draw.line(state.dis, constants.dark_gray, (x, y), (x + w, y), 2)
        pygame.draw.line(state.dis, constants.dark_gray, (x, y), (x, y + h), 2)
        pygame.draw.line(state.dis, constants.white, (x + w, y), (x + w, y + h), 2)
        pygame.draw.line(state.dis, constants.white, (x, y + h), (x + w, y + h), 2)
    else:
        pygame.draw.line(state.dis, constants.white, (x, y), (x + w, y), 2)
        pygame.draw.line(state.dis, constants.white, (x, y), (x, y + h), 2)
        pygame.draw.line(state.dis, constants.dark_gray, (x + w, y), (x + w, y + h), 2)
        pygame.draw.line(state.dis, constants.dark_gray, (x, y + h), (x + w, y + h), 2)

def button(msg, x, y, w, h, ic, ac, action=None):
    mouse = pygame.mouse.get_pos()
    clicked = False
    
    if x + w > mouse[0] > x and y + h > mouse[1] > y:
        if pygame.mouse.get_pressed()[0] == 1:
            draw_button_3d(x, y, w, h, is_pressed=True)
            clicked = True
            pygame.time.delay(150)
        else:
            draw_button_3d(x, y, w, h, is_pressed=False)
    else:
        draw_button_3d(x, y, w, h, is_pressed=False)
    
    text_x = x + (w / 2)
    text_y = y + (h / 2) - 2
    if clicked:
        text_x += 1
        text_y += 1
        
    text_surf, text_rect = text_objects(msg, state.button_font, constants.black)
    text_rect.center = (text_x, text_y)
    state.dis.blit(text_surf, text_rect)
    
    return clicked
