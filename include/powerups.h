#ifndef POWERUPS_H
#define POWERUPS_H

#include "state.h"
#include "game.h"

typedef struct {
    PowerupType type;
    const char* name;
    int r, g, b;
    double duration;
} PowerupInfo;

void spawn_powerup(Powerup* powerup);
void update_powerup(Powerup* powerup, double current_time);
int check_powerup_collision(Snake* snake, Powerup* powerup);
void apply_powerup(GameContext* game, PowerupType type);
void update_active_powerup(GameContext* game, double current_time);
void apply_powerup_effect(GameContext* game, PowerupType type);
void update_powerup_effect(GameContext* game, PowerupType type);
void remove_powerup_effect(GameContext* game, PowerupType type);
PowerupInfo get_powerup_info(PowerupType type);

#endif
