#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "audio.h"

typedef enum {
    GAME_RUNNING,
    GAME_OVER,
    GAME_QUIT,
    GAME_MENU
} GameStatus;

typedef struct {
    Snake snake;
    Food food;
    Powerup powerup;
    PowerupType active_powerup;
    double powerup_end_time;
    int score;
    int apples_eaten;
    double start_time;
    double powerup_spawn_time;
    int current_speed;
    GameStatus status;
} GameContext;

void init_game(GameContext* game, GameState* state);
GameStatus game_loop(GameContext* game, GameState* state, AudioState* audio);
void cleanup_game(GameContext* game);

#endif
