import os
import random
import math
from typing import Tuple, List, Any, Optional, Union, Dict

def clamp(value: float, min_val: float, max_val: float) -> float:
    """Clamp a value between a minimum and maximum."""
    return max(min_val, min(value, max_val))

def calculate_distance(pos1: Tuple[float, float], pos2: Tuple[float, float]) -> float:
    """Calculate the Euclidean distance between two points."""
    return math.sqrt((pos2[0] - pos1[0])**2 + (pos2[1] - pos1[1])**2)

def lerp(start: float, end: float, amount: float) -> float:
    """Linear interpolation between two values."""
    return start + (end - start) * amount

def get_random_position(bounds: Tuple[int, int, int, int], 
                       grid_size: int = 1) -> Tuple[int, int]:
    """
    Get a random position within the given bounds, snapped to a grid.
    
    Args:
        bounds: (x, y, width, height) - The bounds to generate position within
        grid_size: Size of the grid to snap to
        
    Returns:
        Tuple of (x, y) coordinates
    """
    x = random.randrange(bounds[0], bounds[0] + bounds[2], grid_size)
    y = random.randrange(bounds[1], bounds[1] + bounds[3], grid_size)
    return x, y

def format_time(seconds: float) -> str:
    """Format seconds into MM:SS format."""
    minutes = int(seconds // 60)
    seconds = int(seconds % 60)
    return f"{minutes:02d}:{seconds:02d}"

def load_json(file_path: str) -> Dict[str, Any]:
    """Load JSON data from a file."""
    import json
    with open(file_path, 'r') as f:
        return json.load(f)

def save_json(data: Dict[str, Any], file_path: str) -> None:
    """Save data to a JSON file."""
    import json
    os.makedirs(os.path.dirname(file_path), exist_ok=True)
    with open(file_path, 'w') as f:
        json.dump(data, f, indent=2)

def get_asset_path(*path_parts: str) -> str:
    """Get the absolute path to an asset file."""
    import sys
    base_path = getattr(sys, '_MEIPASS', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    return os.path.join(base_path, 'assets', *path_parts)

def scale_rect(rect: pygame.Rect, scale: float) -> pygame.Rect:
    """Scale a pygame.Rect around its center."""
    new_width = rect.width * scale
    new_height = rect.height * scale
    return pygame.Rect(
        rect.centerx - new_width // 2,
        rect.centery - new_height // 2,
        new_width,
        new_height
    )

def ease_out_quad(x: float) -> float:
    """Easing function for smooth animations."""
    return 1 - (1 - x) * (1 - x)

def is_point_in_rect(point: Tuple[float, float], rect: pygame.Rect) -> bool:
    """Check if a point is inside a rectangle."""
    return (rect.left <= point[0] <= rect.right and 
            rect.top <= point[1] <= rect.bottom)

def get_alpha_surface(size: Tuple[int, int], alpha: int = 180) -> pygame.Surface:
    """Create a semi-transparent surface."""
    surf = pygame.Surface(size, pygame.SRCALPHA)
    surf.fill((0, 0, 0, alpha))
    return surf