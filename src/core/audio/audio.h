#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "core/game/state.h"

typedef struct {
    Mix_Chunk* pickup_sound;
    Mix_Chunk* powerup_sound;
    Mix_Chunk* explosion_sound;
} AudioState;

void init_audio(AudioState* audio, double volume);
void cleanup_audio(AudioState* audio);
void play_pickup_sound(AudioState* audio, double volume);
void play_powerup_sound(AudioState* audio, double volume);
void play_explosion_sound(AudioState* audio, double volume);

#endif
