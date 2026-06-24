#ifndef POWERUP_H
#define POWERUP_H

#include "constants.h"
#include "game.h"

typedef struct {
    PowerupType type;
    const char* name;
    int r, g, b;
    double duration;
} PowerupInfo;

PowerupInfo get_powerup_info(PowerupType type);

void apply_powerup_effect(GameContext* game, PowerupType type);

void update_powerup_effect(GameContext* game, PowerupType type);

void remove_powerup_effect(GameContext* game, PowerupType type);

#endif
