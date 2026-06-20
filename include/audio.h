#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "state.h"

typedef struct {
    Mix_Chunk* powerup_sound;
} AudioState;

void init_audio(AudioState* audio, double volume);
void cleanup_audio(AudioState* audio);
void play_powerup_sound(AudioState* audio, double volume);

#endif
