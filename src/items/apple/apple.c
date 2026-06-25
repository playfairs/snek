#include "apple.h"

void draw_apple(SDL_Renderer* renderer, Food* food) {
    if (!food->active) return;

    SDL_Rect rect = {food->pos.x, food->pos.y, SNAKE_BLOCK, SNAKE_BLOCK};
    SDL_SetRenderDrawColor(renderer, RED_R, RED_G, RED_B, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect inner = {food->pos.x + 4, food->pos.y + 4, SNAKE_BLOCK - 8, SNAKE_BLOCK - 8};
    SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
    SDL_RenderFillRect(renderer, &inner);

    SDL_SetRenderDrawColor(renderer, WHITE_R, WHITE_G, WHITE_B, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
