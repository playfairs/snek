#include "../../include/items.h"

void draw_bomb(SDL_Renderer* renderer, Food* food) {
    if (!food->active) return;

    SDL_Rect rect = {food->pos.x, food->pos.y, SNAKE_BLOCK, SNAKE_BLOCK};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 180, 50, 50, 255);
    SDL_RenderDrawLine(renderer, rect.x + 4, rect.y + 4, rect.x + rect.w - 5, rect.y + rect.h - 5);
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 5, rect.y + 4, rect.x + 4, rect.y + rect.h - 5);
    SDL_RenderDrawRect(renderer, &rect);
}
