# snek/components/ui/header.py
import pygame
from typing import List, Dict, Any

class Header:
    def __init__(self, x: int, y: int, width: int, height: int):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.buttons: List[Any] = []
        self.stats: Dict[str, Any] = {}
        self.font = pygame.font.SysFont('Arial', 18, bold=True)
        self.small_font = pygame.font.SysFont('Arial', 14)
        self.bg_color = (240, 240, 240)
        self.border_color = (200, 200, 200)
        self.shadow_color = (180, 180, 180)
        self.text_color = (50, 50, 50)

    def add_button(self, button: Any) -> None:
        """Add a button to the header."""
        self.buttons.append(button)

    def update_stats(self, stats: Dict[str, Any]) -> None:
        """Update the statistics to display."""
        self.stats = stats

    def draw(self, surface: pygame.Surface) -> None:
        """Draw the header and all its components with a modern look."""
        # Draw header background with shadow effect
        pygame.draw.rect(surface, self.shadow_color, (self.x + 2, self.y + 2, self.width, self.height))
        pygame.draw.rect(surface, self.bg_color, (self.x, self.y, self.width, self.height))
        
        # Draw 3D border effect
        pygame.draw.rect(surface, (255, 255, 255), (self.x, self.y, self.width, 2))  # Top border
        pygame.draw.rect(surface, (255, 255, 255), (self.x, self.y, 2, self.height))  # Left border
        pygame.draw.rect(surface, (160, 160, 160), (self.x, self.y + self.height - 2, self.width, 2))  # Bottom border
        pygame.draw.rect(surface, (160, 160, 160), (self.x + self.width - 2, self.y, 2, self.height))  # Right border
        
        # Draw stats with shadow
        stat_x = 20
        for name, value in self.stats.items():
            if name != 'snake':  # Skip the snake object
                text = f"{name}: {value}"
                
                # Draw shadow
                shadow_surf = self.font.render(text, True, (200, 200, 200))
                surface.blit(shadow_surf, (stat_x + 1, self.y + 11))
                
                # Draw main text
                text_surf = self.font.render(text, True, self.text_color)
                surface.blit(text_surf, (stat_x, self.y + 10))
                
                stat_x += 150  # Space between stats
        
        # Draw active powerups with timers
        if 'snake' in self.stats and hasattr(self.stats['snake'], 'active_powerups'):
            powerup_x = self.width - 20  # Start from right side
            current_time = pygame.time.get_ticks()
            
            # Powerup display settings with better visual hierarchy
            powerup_display = {
                'speed_boost': {'name': 'SPEED', 'color': (0, 150, 255), 'icon': '⚡'},
                'slow_mo': {'name': 'SLOW', 'color': (147, 112, 219), 'icon': '🐌'},
                'double_points': {'name': '2X', 'color': (255, 200, 0), 'icon': '✖️'},
                'invincible': {'name': 'INVINCIBLE', 'color': (255, 80, 80), 'icon': '🛡️'}
            }
            
            # Get active powerups and sort them by remaining time
            active_powerups = []
            for powerup_type, end_time in self.stats['snake'].active_powerups.items():
                if powerup_type in powerup_display and current_time < end_time:
                    remaining = (end_time - current_time) / 1000  # Convert to seconds
                    if remaining > 0:
                        active_powerups.append((powerup_type, end_time, remaining))
            
            # Sort by remaining time (shortest first)
            active_powerups.sort(key=lambda x: x[2])
            
            # Draw each powerup indicator
            for powerup_type, end_time, remaining in active_powerups:
                settings = powerup_display[powerup_type]
                
                # Create a container for the powerup
                container_width = 120
                container_rect = pygame.Rect(
                    powerup_x - container_width + 10, 
                    self.y + 5, 
                    container_width - 10, 
                    self.height - 10
                )
                
                # Draw container background
                pygame.draw.rect(surface, (230, 230, 230), container_rect, border_radius=4)
                pygame.draw.rect(surface, (200, 200, 200), container_rect, 1, border_radius=4)
                
                # Draw icon
                icon_surf = self.font.render(settings['icon'], True, settings['color'])
                icon_rect = icon_surf.get_rect(center=(powerup_x - container_width + 25, self.y + self.height//2))
                surface.blit(icon_surf, icon_rect)
                
                # Draw time remaining
                time_text = f"{remaining:.1f}s"
                time_surf = self.small_font.render(time_text, True, (100, 100, 100))
                time_rect = time_surf.get_rect(midright=(powerup_x - 10, self.y + self.height//2 + 10))
                surface.blit(time_surf, time_rect)
                
                # Draw powerup name
                name_surf = self.small_font.render(settings['name'], True, self.text_color)
                name_rect = name_surf.get_rect(midright=(powerup_x - 10, self.y + self.height//2 - 6))
                surface.blit(name_surf, name_rect)
                
                # Add a small colored indicator at the bottom
                pygame.draw.rect(
                    surface, 
                    settings['color'], 
                    (powerup_x - container_width + 10, self.y + self.height - 4, container_width - 10, 2)
                )
                
                powerup_x -= container_width + 10  # Space for next powerup

        # Draw buttons
        for button in self.buttons:
            button.draw(surface)

    def handle_event(self, event: pygame.event.Event) -> None:
        """Handle events for all buttons in the header."""
        for button in self.buttons:
            button.handle_event(event)