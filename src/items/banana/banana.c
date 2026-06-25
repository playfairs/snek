#include "banana.h"

void draw_banana(SDL_Renderer* renderer, Food* food) {
    if (!food->active) return;

    SDL_Rect rect = {food->pos.x, food->pos.y, SNAKE_BLOCK, SNAKE_BLOCK};
    SDL_SetRenderDrawColor(renderer, 240, 220, 80, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect peel = {food->pos.x + 3, food->pos.y + 3, SNAKE_BLOCK - 6, SNAKE_BLOCK - 6};
    SDL_SetRenderDrawColor(renderer, 255, 240, 140, 255);
    SDL_RenderFillRect(renderer, &peel);

    SDL_SetRenderDrawColor(renderer, 180, 130, 20, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
