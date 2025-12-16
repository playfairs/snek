import os
import pygame
from typing import Dict, Optional
from pathlib import Path

class AudioManager:
    """Manages loading and playing audio assets."""
    
    _instance = None
    
    def __new__(cls):
        """Implement singleton pattern."""
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance
    
    def __init__(self):
        """Initialize the audio manager."""
        if self._initialized:
            return
            
        # Initialize mixer if not already done
        if not pygame.mixer.get_init():
            pygame.mixer.pre_init(44100, -16, 2, 2048)
            pygame.mixer.init()
            
        self.sounds: Dict[str, Optional[pygame.mixer.Sound]] = {}
        self.music_volume = 0.5  # Default volume
        self.sfx_volume = 0.7    # Default volume
        self._initialized = True
        
    def _get_asset_path(self, *path_parts: str) -> str:
        """Get the absolute path to an audio file."""
        import sys
        base_path = getattr(sys, '_MEIPASS', 
                          os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
        return str(Path(base_path, 'assets', 'sounds', *path_parts))
    
    def load_sound(self, name: str, path: str) -> None:
        """
        Load and store a sound effect.
        
        Args:
            name: Unique identifier for the sound
            path: Relative path from assets/sounds/ directory
        """
        if name in self.sounds:
            return
            
        try:
            full_path = self._get_asset_path(path)
            if not os.path.exists(full_path):
                print(f"Sound file not found: {full_path}")
                self.sounds[name] = None
                return
                
            sound = pygame.mixer.Sound(full_path)
            sound.set_volume(self.sfx_volume)
            self.sounds[name] = sound
        except pygame.error as e:
            print(f"Error loading sound {path}: {e}")
            self.sounds[name] = None
    
    def play_sound(self, name: str, loops: int = 0, fade_ms: int = 0) -> None:
        """Play a sound effect by name."""
        sound = self.sounds.get(name)
        if sound:
            sound.play(loops=loops, fade_ms=fade_ms)
    
    def stop_sound(self, name: str) -> None:
        """Stop a specific sound effect."""
        sound = self.sounds.get(name)
        if sound:
            sound.stop()
    
    def set_sfx_volume(self, volume: float) -> None:
        """Set the volume for all sound effects (0.0 to 1.0)."""
        self.sfx_volume = max(0.0, min(1.0, volume))
        for sound in self.sounds.values():
            if sound:
                sound.set_volume(self.sfx_volume)
    
    def load_music(self, path: str) -> None:
        """Load a music file."""
        try:
            full_path = self._get_asset_path(path)
            if not os.path.exists(full_path):
                print(f"Music file not found: {full_path}")
                return
            pygame.mixer.music.load(full_path)
            pygame.mixer.music.set_volume(self.music_volume)
        except pygame.error as e:
            print(f"Error loading music {path}: {e}")
    
    def play_music(self, loops: int = -1, start: float = 0.0, fade_ms: int = 0) -> None:
        """Play the loaded music."""
        pygame.mixer.music.play(loops=loops, start=start, fade_ms=fade_ms)
    
    def stop_music(self) -> None:
        """Stop the currently playing music."""
        pygame.mixer.music.stop()
    
    def set_music_volume(self, volume: float) -> None:
        """Set the music volume (0.0 to 1.0)."""
        self.music_volume = max(0.0, min(1.0, volume))
        pygame.mixer.music.set_volume(self.music_volume)
    
    def pause_music(self) -> None:
        """Pause the music."""
        pygame.mixer.music.pause()
    
    def unpause_music(self) -> None:
        """Resume paused music."""
        pygame.mixer.music.unpause()
    
    def preload_sounds(self, sound_list: list[dict[str, str]]) -> None:
        """Preload multiple sounds from a list of dictionaries with 'name' and 'path'."""
        for sound_def in sound_list:
            self.load_sound(sound_def['name'], sound_def['path'])

# Create a global instance for easy access
audio = AudioManager()