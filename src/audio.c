#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../include/audio.h"

void init_audio(AudioState* audio, double volume) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return;
    }
    
    audio->powerup_sound = Mix_LoadWAV("assets/sfx/powerUp.wav");
    if (audio->powerup_sound) {
        Mix_VolumeChunk(audio->powerup_sound, (int)(volume * MIX_MAX_VOLUME));
    }
}

void cleanup_audio(AudioState* audio) {
    if (audio->powerup_sound) {
        Mix_FreeChunk(audio->powerup_sound);
        audio->powerup_sound = NULL;
    }
    Mix_CloseAudio();
}

void play_powerup_sound(AudioState* audio, double volume) {
    if (audio->powerup_sound) {
        Mix_VolumeChunk(audio->powerup_sound, (int)(volume * MIX_MAX_VOLUME));
        Mix_PlayChannel(-1, audio->powerup_sound, 0);
    }
}
