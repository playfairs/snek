# snek/components/ui/button.py
import pygame
from dataclasses import dataclass
from typing import Optional, Callable, Tuple

@dataclass
class Button:
    x: int
    y: int
    width: int
    height: int
    text: str
    action: Optional[Callable] = None
    color: Tuple[int, int, int] = (200, 200, 200)
    hover_color: Tuple[int, int, int] = (180, 180, 180)
    text_color: Tuple[int, int, int] = (0, 0, 0)
    font: Optional[pygame.font.Font] = None
    padding: int = 10
    is_pressed: bool = False
    visible: bool = True

    def __post_init__(self):
        if self.font is None:
            self.font = pygame.font.SysFont('Arial', 20)

    def draw(self, surface: pygame.Surface) -> None:
        """Draw the button on the given surface."""
        if not self.visible:
            return

        mouse_pos = pygame.mouse.get_pos()
        is_hovered = self.is_hovered(mouse_pos)
        
        # Draw button background
        color = self.hover_color if is_hovered or self.is_pressed else self.color
        pygame.draw.rect(surface, color, (self.x, self.y, self.width, self.height))
        
        # Draw border
        border_color = (100, 100, 100) if is_hovered or self.is_pressed else (150, 150, 150)
        pygame.draw.rect(surface, border_color, (self.x, self.y, self.width, self.height), 2)
        
        # Draw text
        text_surf = self.font.render(self.text, True, self.text_color)
        text_rect = text_surf.get_rect(center=(self.x + self.width // 2, 
                                             self.y + self.height // 2))
        
        # Add slight offset when pressed
        if self.is_pressed:
            text_rect.y += 2
            
        surface.blit(text_surf, text_rect)

    def is_hovered(self, pos: Tuple[int, int]) -> bool:
        """Check if the mouse is hovering over the button."""
        return (self.x <= pos[0] <= self.x + self.width and 
                self.y <= pos[1] <= self.y + self.height)

    def handle_event(self, event: pygame.event.Event) -> bool:
        """Handle pygame events and return True if the button was clicked."""
        if not self.visible:
            return False

        if event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1 and self.is_hovered(event.pos):
                self.is_pressed = True
                return True

        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1 and self.is_pressed:
                self.is_pressed = False
                if self.is_hovered(event.pos) and self.action:
                    self.action()
                    return True

        return False

    def set_position(self, x: int, y: int) -> None:
        """Set the button's position."""
        self.x = x
        self.y = y

    def set_text(self, text: str) -> None:
        """Update the button's text."""
        self.text = text