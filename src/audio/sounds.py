import pygame

powerup_sound = None

def init_sounds():
    global powerup_sound
    try:
        pygame.mixer.init()
        powerup_sound = pygame.mixer.Sound('assets/sfx/powerUp.wav')
        powerup_sound.set_volume(0.5)
    except Exception as e:
        print(f"Could not load sound effects: {e}")
        powerup_sound = None

def play_powerup_sound():
    if powerup_sound:
        try:
            pygame.mixer.Sound.play(powerup_sound)
        except:
            pass
