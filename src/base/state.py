# snek/core/state.py
import json
from pathlib import Path
from dataclasses import dataclass, asdict
from typing import Dict, Any

@dataclass
class GameState:
    high_score: int = 0
    total_apples: int = 0
    games_played: int = 0
    powerups_collected: int = 0
    best_time: int = 0
    snake_skin: str = 'default'
    sound_enabled: bool = True
    music_enabled: bool = True
    volume: float = 0.7

    def __post_init__(self):
        self.load()

    def load(self) -> None:
        try:
            save_path = Path('snek_stats.json')
            if save_path.exists():
                with open(save_path, 'r') as f:
                    data = json.load(f)
                    for key, value in data.items():
                        if hasattr(self, key):
                            setattr(self, key, value)
        except Exception as e:
            print(f"Error loading game state: {e}")

    def save(self) -> None:
        try:
            with open('snek_stats.json', 'w') as f:
                json.dump(asdict(self), f, indent=2)
        except Exception as e:
            print(f"Error saving game state: {e}")

    def update_high_score(self, score: int) -> None:
        if score > self.high_score:
            self.high_score = score
            self.save()

    def update_best_time(self, time_seconds: int) -> None:
        if time_seconds > self.best_time:
            self.best_time = time_seconds
            self.save()