#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/state.h"
#include "../include/constants.h"

void init_state(GameState* state) {
    state->window = NULL;
    state->renderer = NULL;
    state->font_style = NULL;
    state->score_font = NULL;
    state->button_font = NULL;
    state->large_font = NULL;
    
    memset(&state->stats, 0, sizeof(GameStats));
    
    state->settings.sound_enabled = 1;
    state->settings.music_enabled = 1;
    state->settings.volume = 0.7;
    state->settings.current_skin = SKIN_DEFAULT;
    state->settings.difficulty = DIFFICULTY_NORMAL;
    state->settings.mode = MODE_CLASSIC;
    
    load_stats(&state->stats);
    load_settings(&state->settings);
}

void cleanup_state(GameState* state) {
    if (state->large_font) {
        TTF_CloseFont(state->large_font);
        state->large_font = NULL;
    }
    if (state->button_font) {
        TTF_CloseFont(state->button_font);
        state->button_font = NULL;
    }
    if (state->score_font) {
        TTF_CloseFont(state->score_font);
        state->score_font = NULL;
    }
    if (state->font_style) {
        TTF_CloseFont(state->font_style);
        state->font_style = NULL;
    }
    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
        state->renderer = NULL;
    }
    if (state->window) {
        SDL_DestroyWindow(state->window);
        state->window = NULL;
    }
}

int load_stats(GameStats* stats) {
    FILE* file = fopen("data/stats.toml", "r");
    if (!file) {
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "high_score", 10) == 0) {
            sscanf(line, "high_score = %d", &stats->high_score);
        } else if (strncmp(line, "total_apples", 12) == 0) {
            sscanf(line, "total_apples = %d", &stats->total_apples);
        } else if (strncmp(line, "games_played", 13) == 0) {
            sscanf(line, "games_played = %d", &stats->games_played);
        } else if (strncmp(line, "powerups_collected", 19) == 0) {
            sscanf(line, "powerups_collected = %d", &stats->powerups_collected);
        } else if (strncmp(line, "best_time", 9) == 0) {
            sscanf(line, "best_time = %lf", &stats->best_time);
        }
    }
    fclose(file);
    return 1;
}

void save_stats(GameStats* stats) {
    FILE* file = fopen("data/stats.toml", "w");
    if (!file) {
        return;
    }
    
    fprintf(file, "high_score = %d\n", stats->high_score);
    fprintf(file, "total_apples = %d\n", stats->total_apples);
    fprintf(file, "games_played = %d\n", stats->games_played);
    fprintf(file, "powerups_collected = %d\n", stats->powerups_collected);
    fprintf(file, "best_time = %.1f\n", stats->best_time);
    
    fclose(file);
}

int load_settings(GameSettings* settings) {
    FILE* file = fopen("data/settings.toml", "r");
    if (!file) {
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "sound_enabled", 14) == 0) {
            sscanf(line, "sound_enabled = %d", &settings->sound_enabled);
        } else if (strncmp(line, "music_enabled", 14) == 0) {
            sscanf(line, "music_enabled = %d", &settings->music_enabled);
        } else if (strncmp(line, "volume", 6) == 0) {
            sscanf(line, "volume = %lf", &settings->volume);
        } else if (strncmp(line, "current_skin", 12) == 0) {
            sscanf(line, "current_skin = %d", (int*)&settings->current_skin);
        } else if (strncmp(line, "difficulty", 10) == 0) {
            sscanf(line, "difficulty = %d", (int*)&settings->difficulty);
        } else if (strncmp(line, "mode", 4) == 0) {
            sscanf(line, "mode = %d", (int*)&settings->mode);
        }
    }
    fclose(file);

    if (settings->current_skin < 0 || settings->current_skin >= SKIN_COUNT) {
        settings->current_skin = SKIN_DEFAULT;
    }
    if (settings->difficulty < 0 || settings->difficulty >= DIFFICULTY_COUNT) {
        settings->difficulty = DIFFICULTY_NORMAL;
    }
    if (settings->mode < 0 || settings->mode >= MODE_COUNT) {
        settings->mode = MODE_CLASSIC;
    }
    if (settings->volume < 0.0) {
        settings->volume = 0.0;
    } else if (settings->volume > 1.0) {
        settings->volume = 1.0;
    }

    return 1;
}

void save_settings(GameSettings* settings) {
    FILE* file = fopen("data/settings.toml", "w");
    if (!file) {
        return;
    }
    
    fprintf(file, "sound_enabled = %d\n", settings->sound_enabled);
    fprintf(file, "music_enabled = %d\n", settings->music_enabled);
    fprintf(file, "volume = %.1f\n", settings->volume);
    fprintf(file, "current_skin = %d\n", settings->current_skin);
    fprintf(file, "difficulty = %d\n", settings->difficulty);
    fprintf(file, "mode = %d\n", settings->mode);
    
    fclose(file);
}
