import pygame
import py.core.state as state

powerup_sound = None

def init_sounds():
    global powerup_sound
    try:
        pygame.mixer.init()
        sound_path = state.get_asset_path('assets/sfx/powerUp.wav')
        powerup_sound = pygame.mixer.Sound(sound_path)
        powerup_sound.set_volume(state.volume)
    except Exception as e:
        print(f"Could not load sound effects: {e}")
        powerup_sound = None

def play_powerup_sound():
    if powerup_sound and state.sound_enabled:
        try:
            powerup_sound.set_volume(state.volume)
            powerup_sound.play()
        except Exception as e:
            print(f"Could not play sound: {e}")
            pass
