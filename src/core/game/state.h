#ifndef STATE_H
#define STATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "core/game/constants.h"

typedef struct {
    int r;
    int g;
    int b;
} Color;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position segments[MAX_SNAKE_LENGTH];
    int length;
    int dx;
    int dy;
    SnakeSkin skin;
} Snake;

typedef struct {
    Position pos;
    ItemType type;
    int active;
    double spawn_time;
    double lifetime;
} Food;

typedef struct {
    Position pos;
    PowerupType type;
    int active;
    double spawn_time;
} Powerup;

typedef struct {
    int high_score;
    int total_apples;
    int games_played;
    int powerups_collected;
    double best_time;
} GameStats;

typedef struct {
    int sound_enabled;
    int music_enabled;
    double volume;
    SnakeSkin current_skin;
    Difficulty difficulty;
    GameMode mode;
} GameSettings;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font_style;
    TTF_Font* score_font;
    TTF_Font* button_font;
    TTF_Font* large_font;
    GameStats stats;
    GameSettings settings;
} GameState;

void init_state(GameState* state);
void cleanup_state(GameState* state);
int load_stats(GameStats* stats);
void save_stats(GameStats* stats);
int load_settings(GameSettings* settings);
void save_settings(GameSettings* settings);

#endif
