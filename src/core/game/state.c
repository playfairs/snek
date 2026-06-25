#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "core/game/state.h"
#include "core/game/constants.h"

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

static char* trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static void parse_stats_line(const char* raw_line, GameStats* stats) {
    char line[256];
    strncpy(line, raw_line, sizeof(line));
    line[sizeof(line) - 1] = '\0';
    char* trimmed = trim_whitespace(line);
    int value;

    if (sscanf(trimmed, "high_score = %d", &value) == 1) {
        stats->high_score = value;
    } else if (sscanf(trimmed, "total_apples = %d", &value) == 1) {
        stats->total_apples = value;
    } else if (sscanf(trimmed, "games_played = %d", &value) == 1) {
        stats->games_played = value;
    } else if (sscanf(trimmed, "powerups_collected = %d", &value) == 1) {
        stats->powerups_collected = value;
    } else if (sscanf(trimmed, "best_time = %lf", &stats->best_time) == 1) {
    }
}

static int load_stats_file(const char* path, GameStats* stats) {
    char line[256];
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    while (fgets(line, sizeof(line), file)) {
        parse_stats_line(line, stats);
    }
    fclose(file);
    return 1;
}

static const char* get_xdg_config_dir(char* buffer, size_t size) {
    const char* config_home = getenv("XDG_CONFIG_HOME");
    if (config_home && *config_home) {
        int n = snprintf(buffer, size, "%s/snek", config_home);
        if (n >= 0 && (size_t)n < size) {
            return buffer;
        }
    }
    return NULL;
}

static const char* get_home_config_dir(char* buffer, size_t size) {
    const char* home = getenv("HOME");
    if (home && *home) {
        int n = snprintf(buffer, size, "%s/.config/snek", home);
        if (n >= 0 && (size_t)n < size) {
            return buffer;
        }
    }
    return NULL;
}

int load_stats(GameStats* stats) {
    char config_dir[PATH_MAX];
    char user_path[PATH_MAX];

    load_stats_file("data/stats.toml", stats);

    const char* stats_dir = get_xdg_config_dir(config_dir, sizeof(config_dir));
    if (stats_dir) {
        int n = snprintf(user_path, sizeof(user_path), "%s/stats.toml", stats_dir);
        if (n >= 0 && (size_t)n < sizeof(user_path)) {
            load_stats_file(user_path, stats);
        }
    }

    stats_dir = get_home_config_dir(config_dir, sizeof(config_dir));
    if (stats_dir) {
        int n = snprintf(user_path, sizeof(user_path), "%s/stats.toml", stats_dir);
        if (n >= 0 && (size_t)n < sizeof(user_path)) {
            load_stats_file(user_path, stats);
        }
    }

    return 1;
}

void save_stats(GameStats* stats) {
    char dir[PATH_MAX];
    char path[PATH_MAX];
    const char* stats_dir = get_home_config_dir(dir, sizeof(dir));
    if (stats_dir) {
        struct stat st = {0};
        if (stat(stats_dir, &st) == -1) {
            mkdir(stats_dir, 0700);
        }
        int n = snprintf(path, sizeof(path), "%s/stats.toml", stats_dir);
        if (n < 0 || (size_t)n >= sizeof(path)) {
            strncpy(path, "data/stats.toml", sizeof(path));
            path[sizeof(path)-1] = '\0';
        }
    } else {
        snprintf(path, sizeof(path), "data/stats.toml");
    }
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "save_stats: failed to open '%s' for writing\n", path);
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
    char path[PATH_MAX];
    char* home = getenv("HOME");
    if (home) {
        int n = snprintf(path, sizeof(path), "%s/.config/snek/settings.toml", home);
        if (n < 0 || (size_t)n >= sizeof(path)) {
            strncpy(path, "data/settings.toml", sizeof(path));
            path[sizeof(path)-1] = '\0';
        }
    } else {
        snprintf(path, sizeof(path), "data/settings.toml");
    }
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "sound_enabled", sizeof("sound_enabled") - 1) == 0) {
            sscanf(line, "sound_enabled = %d", &settings->sound_enabled);
        } else if (strncmp(line, "music_enabled", sizeof("music_enabled") - 1) == 0) {
            sscanf(line, "music_enabled = %d", &settings->music_enabled);
        } else if (strncmp(line, "volume", sizeof("volume") - 1) == 0) {
            sscanf(line, "volume = %lf", &settings->volume);
        } else if (strncmp(line, "current_skin", sizeof("current_skin") - 1) == 0) {
            sscanf(line, "current_skin = %d", (int*)&settings->current_skin);
        } else if (strncmp(line, "difficulty", sizeof("difficulty") - 1) == 0) {
            sscanf(line, "difficulty = %d", (int*)&settings->difficulty);
        } else if (strncmp(line, "mode", sizeof("mode") - 1) == 0) {
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
    char dir[PATH_MAX];
    char path[PATH_MAX];
    char* home = getenv("HOME");
    if (home) {
        int nd = snprintf(dir, sizeof(dir), "%s/.config/snek", home);
        if (nd < 0 || (size_t)nd >= sizeof(dir)) {
            strncpy(dir, "data", sizeof(dir));
            dir[sizeof(dir)-1] = '\0';
        }
        struct stat st = {0};
        if (stat(dir, &st) == -1) {
            mkdir(dir, 0700);
        }
        int n = snprintf(path, sizeof(path), "%s/settings.toml", dir);
        if (n < 0 || (size_t)n >= sizeof(path)) {
            strncpy(path, "data/settings.toml", sizeof(path));
            path[sizeof(path)-1] = '\0';
        }
    } else {
        snprintf(path, sizeof(path), "data/settings.toml");
    }
    FILE* file = fopen(path, "w");
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
