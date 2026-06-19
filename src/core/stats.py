import json
import os
import src.core.state as state

game_stats = {
    'high_score': 0,
    'total_apples': 0,
    'games_played': 0,
    'powerups_collected': 0,
    'best_time': 0
}

def get_stats_file_path():
    """Get the path to the stats file."""
    return os.path.join(state.project_root, 'data', 'snek_stats.json')

def load_stats():
    global game_stats
    try:
        stats_path = get_stats_file_path()
        if os.path.exists(stats_path):
            with open(stats_path, 'r') as f:
                loaded_stats = json.load(f)
                for key, value in loaded_stats.items():
                    if key in game_stats:
                        game_stats[key] = value
    except (json.JSONDecodeError, IOError, OSError) as e:
        print(f"Error loading stats: {e}. Using default values.")
    except Exception as e:
        print(f"Unexpected error loading stats: {e}. Using default values.")

def save_stats():
    try:
        stats_path = get_stats_file_path()
        stats_dir = os.path.dirname(stats_path)
        
        if not os.path.exists(stats_dir):
            os.makedirs(stats_dir)
        
        with open(stats_path, 'w') as f:
            json.dump(game_stats, f, indent=2)
    except (IOError, OSError) as e:
        print(f"Error saving stats: {e}")
    except Exception as e:
        print(f"Unexpected error saving stats: {e}")
