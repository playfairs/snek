import json
import os

game_stats = {
    'high_score': 0,
    'total_apples': 0,
    'games_played': 0,
    'powerups_collected': 0,
    'best_time': 0
}

def load_stats():
    global game_stats
    try:
        if os.path.exists('data/snek_stats.json'):
            with open('data/snek_stats.json', 'r') as f:
                game_stats = json.load(f)
    except:
        print("Could not load game stats, using defaults")

def save_stats():
    try:
        with open('data/snek_stats.json', 'w') as f:
            json.dump(game_stats, f)
    except:
        print("Could not save game stats")
