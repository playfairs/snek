#ifndef GAME_H
#define GAME_H

#include "core/game/state.h"
#include "core/audio/audio.h"

typedef enum {
    GAME_RUNNING,
    GAME_OVER,
    GAME_QUIT,
    GAME_MENU
} GameStatus;

typedef struct {
    PowerupType type;
    double end_time;
} ActivePowerup;

typedef struct {
    Snake snake;
    Food food;
    Food secondary_food;
    Powerup powerup;
    ActivePowerup active_powerups[MAX_POWERUPS];
    int powerup_count;
    double mode_end_time;
    GameMode mode;
    int score;
    int apples_eaten;
    double start_time;
    double powerup_spawn_time;
    int base_speed;
    int current_speed;
    int frozen;
    char last_powerup_text[64];
    double last_powerup_display_time;
    GameStatus status;
} GameContext;

void init_game(GameContext* game, GameState* state);
GameStatus game_loop(GameContext* game, GameState* state, AudioState* audio);
void cleanup_game(GameContext* game);

#endif
