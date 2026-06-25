#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "core/game/state.h"
#include "core/game/game.h"
#include "core/graphics/graphics.h"
#include "core/input/input.h"
#include "core/audio/audio.h"
#include "snake/snake.h"
#include "powerups/registry.h"
#include "items/registry.h"

typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_STATS,
    MENU_GAME_OVER
} MenuState;

static void draw_stats_menu_local(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                                  TTF_Font* button_font, GameState* state);

static int draw_button(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h, int mouse_x, int mouse_y, int* clicked) {
    SDL_Rect rect = {x, y, w, h};
    SDL_Rect shadow = {x + 4, y + 5, w, h};

    int is_hover = (mouse_x >= x && mouse_x <= x + w && mouse_y >= y && mouse_y <= y + h);
    int is_pressed = is_hover && *clicked;

    SDL_Color bg_color;
    SDL_Color border_color;

    if (is_pressed) {
        bg_color = (SDL_Color){50, 85, 120, 255};
        border_color = (SDL_Color){90, 150, 200, 255};
    } else if (is_hover) {
        bg_color = (SDL_Color){60, 100, 140, 255};
        border_color = (SDL_Color){110, 170, 230, 255};
    } else {
        bg_color = (SDL_Color){28, 42, 55, 255};
        border_color = (SDL_Color){62, 86, 98, 255};
    }

    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color accent = {124, 218, 184, 255};

    SDL_SetRenderDrawColor(renderer, 5, 8, 12, 255);
    SDL_RenderFillRect(renderer, &shadow);

    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, 255);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Rect accent_strip = {x, y, 5, h};
    SDL_SetRenderDrawColor(renderer, accent.r, accent.g, accent.b, 255);
    SDL_RenderFillRect(renderer, &accent_strip);

    SDL_Surface* text_surf = TTF_RenderText_Blended(font, text, white);
    SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, text_surf);
    SDL_Rect text_rect = {x + (w - text_surf->w) / 2, y + (h - text_surf->h) / 2, text_surf->w, text_surf->h};
    SDL_RenderCopy(renderer, text_tex, NULL, &text_rect);
    SDL_FreeSurface(text_surf);
    SDL_DestroyTexture(text_tex);

    int result = is_hover && *clicked;
    if (result) {
        *clicked = 0;
    }

    return result;
}

static MenuState main_menu_loop(GameState* state, AudioState* audio) {
    (void)audio;
    int running = 1;
    int button_width = 220;
    int button_height = 55;
    int button_spacing = 18;
    int start_x = 625;
    int button_y = 300;

    while (running) {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        draw_main_menu(state->renderer, state->large_font, state->button_font);

        int play_clicked = draw_button(state->renderer, state->button_font, "PLAY GAME",
                                       start_x, button_y, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);
        int settings_clicked = draw_button(state->renderer, state->button_font, "SETTINGS",
                                           start_x, button_y + button_height + button_spacing,
                                           button_width, button_height, mouse_x, mouse_y, &mouse_clicked);
        int stats_clicked = draw_button(state->renderer, state->button_font, "STATS",
                        start_x, button_y + (button_height + button_spacing) * 2,
                        button_width, button_height, mouse_x, mouse_y, &mouse_clicked);
        int quit_clicked = draw_button(state->renderer, state->button_font, "EXIT",
                           start_x, button_y + (button_height + button_spacing) * 3,
                           button_width, button_height, mouse_x, mouse_y, &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (play_clicked) {
            return MENU_GAME_OVER;
        }
        if (settings_clicked) {
            return MENU_SETTINGS;
        }
        if (stats_clicked) {
            return MENU_STATS;
        }
        if (quit_clicked) {
            running = 0;
        }
    }

    return MENU_MAIN;
}

static MenuState settings_menu_loop(GameState* state) {
    int running = 1;

    const char* skin_names[] = {"Default", "Blue", "Red", "Rainbow", "Purple", "Gold", "Cyan", "Neon"};
    const char* sound_names[] = {"On", "Off"};
    const char* music_names[] = {"On", "Off"};
    const char* volume_names[] = {"30%", "50%", "70%", "100%"};
    const double volume_values[] = {0.3, 0.5, 0.7, 1.0};
    const char* mode_names[] = {"Classic", "Challenge", "Time Attack"};
    const char* difficulty_names[] = {"Easy", "Normal", "Hard"};

    int skin_index = state->settings.current_skin;
    int sound_index = state->settings.sound_enabled ? 0 : 1;
    int music_index = state->settings.music_enabled ? 0 : 1;
    int volume_index = 0;
    if (state->settings.volume >= 0.95) {
        volume_index = 3;
    } else if (state->settings.volume >= 0.6) {
        volume_index = 2;
    } else if (state->settings.volume >= 0.4) {
        volume_index = 1;
    }
    int difficulty_index = state->settings.difficulty;
    int mode_index = state->settings.mode;

    int button_width = 380;
    int button_height = 50;
    int button_spacing = 14;
    int start_x = 474;
    int start_y = 224;

    while (running) {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        draw_settings_menu(state->renderer, state->large_font, state->score_font,
                  state->button_font, state, 0);

        char skin_text[64];
        snprintf(skin_text, sizeof(skin_text), "Skin: %s", skin_names[skin_index]);
        int skin_clicked = draw_button(state->renderer, state->button_font, skin_text,
                                      start_x, start_y, button_width, button_height,
                                      mouse_x, mouse_y, &mouse_clicked);
        if (skin_clicked) {
            skin_index = (skin_index + 1) % SKIN_COUNT;
            state->settings.current_skin = skin_index;
            save_settings(&state->settings);
        }

        char sound_text[64];
        snprintf(sound_text, sizeof(sound_text), "Sound: %s", sound_names[sound_index]);
        int sound_clicked = draw_button(state->renderer, state->button_font, sound_text,
                                       start_x, start_y + (button_height + button_spacing), button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);
        if (sound_clicked) {
            sound_index = (sound_index + 1) % 2;
            state->settings.sound_enabled = sound_index == 0;
            save_settings(&state->settings);
        }

        char music_text[64];
        snprintf(music_text, sizeof(music_text), "Music: %s", music_names[music_index]);
        int music_clicked = draw_button(state->renderer, state->button_font, music_text,
                                        start_x, start_y + (button_height + button_spacing) * 2, button_width, button_height,
                                        mouse_x, mouse_y, &mouse_clicked);
        if (music_clicked) {
            music_index = (music_index + 1) % 2;
            state->settings.music_enabled = music_index == 0;
            save_settings(&state->settings);
        }

        char volume_text[64];
        snprintf(volume_text, sizeof(volume_text), "Volume: %s", volume_names[volume_index]);
        int volume_clicked = draw_button(state->renderer, state->button_font, volume_text,
                                         start_x, start_y + (button_height + button_spacing) * 3, button_width, button_height,
                                         mouse_x, mouse_y, &mouse_clicked);
        if (volume_clicked) {
            volume_index = (volume_index + 1) % 4;
            state->settings.volume = volume_values[volume_index];
            save_settings(&state->settings);
        }

        char mode_text[64];
        snprintf(mode_text, sizeof(mode_text), "Mode: %s", mode_names[mode_index]);
        int mode_clicked = draw_button(state->renderer, state->button_font, mode_text,
                                       start_x, start_y + (button_height + button_spacing) * 4, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);
        if (mode_clicked) {
            mode_index = (mode_index + 1) % MODE_COUNT;
            state->settings.mode = mode_index;
            save_settings(&state->settings);
        }

        char difficulty_text[64];
        snprintf(difficulty_text, sizeof(difficulty_text), "Difficulty: %s", difficulty_names[difficulty_index]);
        int difficulty_clicked = draw_button(state->renderer, state->button_font, difficulty_text,
                                              start_x, start_y + (button_height + button_spacing) * 5, button_width, button_height,
                                              mouse_x, mouse_y, &mouse_clicked);
        if (difficulty_clicked) {
            difficulty_index = (difficulty_index + 1) % DIFFICULTY_COUNT;
            state->settings.difficulty = difficulty_index;
            save_settings(&state->settings);
        }

        int back_clicked = draw_button(state->renderer, state->button_font, "BACK",
                                       start_x, start_y + (button_height + button_spacing) * 6, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);
        if (back_clicked) {
            running = 0;
        }

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);
    }

    return MENU_MAIN;
}

static MenuState stats_menu_loop(GameState* state) {
    int running = 1;
    int button_width = 220;
    int button_height = 55;
    int start_x = (DIS_WIDTH - button_width) / 2;
    int start_y = DIS_HEIGHT - 140;

    while (running) {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return MENU_MAIN;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        draw_stats_menu_local(state->renderer, state->large_font, state->score_font, state->button_font, state);

        int back_clicked = draw_button(state->renderer, state->button_font, "BACK",
                                       start_x, start_y, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (back_clicked) {
            return MENU_MAIN;
        }
    }

    return MENU_MAIN;
}

static void draw_stats_menu_local(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                                  TTF_Font* button_font, GameState* state) {
    SDL_SetRenderDrawColor(renderer, 10, 14, 22, 255);
    SDL_RenderClear(renderer);

    SDL_Rect panel = {80, 90, DIS_WIDTH - 160, DIS_HEIGHT - 180};
    SDL_SetRenderDrawColor(renderer, 22, 28, 40, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 80, 120, 170, 255);
    SDL_RenderDrawRect(renderer, &panel);

    SDL_Color accent = {150, 220, 255, 255};
    SDL_Color gray = {170, 180, 190, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};

    SDL_Surface* title_surf = TTF_RenderText_Blended(large_font, "STATISTICS", accent);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {(DIS_WIDTH - title_surf->w) / 2, 120, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);

    char buf[64];
    int y = 180;
    int spacing = 36;

    snprintf(buf, sizeof(buf), "High Score: %d", state->stats.high_score);
    SDL_Surface* s1 = TTF_RenderText_Blended(score_font, buf, white);
    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Rect r1 = {120, y, s1->w, s1->h};
    SDL_RenderCopy(renderer, t1, NULL, &r1);
    SDL_FreeSurface(s1); SDL_DestroyTexture(t1);
    y += spacing;

    snprintf(buf, sizeof(buf), "Total Apples: %d", state->stats.total_apples);
    SDL_Surface* s2 = TTF_RenderText_Blended(score_font, buf, white);
    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);
    SDL_Rect r2 = {120, y, s2->w, s2->h};
    SDL_RenderCopy(renderer, t2, NULL, &r2);
    SDL_FreeSurface(s2); SDL_DestroyTexture(t2);
    y += spacing;

    snprintf(buf, sizeof(buf), "Games Played: %d", state->stats.games_played);
    SDL_Surface* s3 = TTF_RenderText_Blended(score_font, buf, white);
    SDL_Texture* t3 = SDL_CreateTextureFromSurface(renderer, s3);
    SDL_Rect r3 = {120, y, s3->w, s3->h};
    SDL_RenderCopy(renderer, t3, NULL, &r3);
    SDL_FreeSurface(s3); SDL_DestroyTexture(t3);
    y += spacing;

    snprintf(buf, sizeof(buf), "Powerups Collected: %d", state->stats.powerups_collected);
    SDL_Surface* s4 = TTF_RenderText_Blended(score_font, buf, white);
    SDL_Texture* t4 = SDL_CreateTextureFromSurface(renderer, s4);
    SDL_Rect r4 = {120, y, s4->w, s4->h};
    SDL_RenderCopy(renderer, t4, NULL, &r4);
    SDL_FreeSurface(s4); SDL_DestroyTexture(t4);
    y += spacing;

    snprintf(buf, sizeof(buf), "Best Time: %.1fs", state->stats.best_time);
    SDL_Surface* s5 = TTF_RenderText_Blended(score_font, buf, gray);
    SDL_Texture* t5 = SDL_CreateTextureFromSurface(renderer, s5);
    SDL_Rect r5 = {120, y, s5->w, s5->h};
    SDL_RenderCopy(renderer, t5, NULL, &r5);
    SDL_FreeSurface(s5); SDL_DestroyTexture(t5);
}

static MenuState game_over_loop(GameState* state, int score) {
    int running = 1;
    int button_width = 220;
    int button_height = 55;
    int button_spacing = 25;
    int button_y = DIS_HEIGHT / 3 + 160;

    while (running) {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return MENU_MAIN;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        draw_game_over(state->renderer, state->large_font, state->score_font, state->button_font, score);

        int total_width = button_width * 2 + button_spacing;
        int start_x = (DIS_WIDTH - total_width) / 2;

        int play_again_clicked = draw_button(state->renderer, state->button_font, "PLAY AGAIN",
                                             start_x,
                                             button_y, button_width, button_height,
                                             mouse_x, mouse_y, &mouse_clicked);
        int quit_clicked = draw_button(state->renderer, state->button_font, "EXIT",
                                       start_x + button_width + button_spacing,
                                       button_y, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (play_again_clicked) {
            return MENU_GAME_OVER;
        }
        if (quit_clicked) {
            running = 0;
        }
    }

    return MENU_MAIN;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    if (TTF_Init() < 0) {
        SDL_Quit();
        return 1;
    }

    GameState state;
    init_state(&state);

    state.window = SDL_CreateWindow("Snek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    DIS_WIDTH, DIS_HEIGHT, 0);
    if (!state.window) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED);
    if (!state.renderer) {
        SDL_DestroyWindow(state.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    state.font_style = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 14);
    if (!state.font_style) {
        state.font_style = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 14);
    }

    state.score_font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 18);
    if (!state.score_font) {
        state.score_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 18);
    }

    state.button_font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 14);
    if (!state.button_font) {
        state.button_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 14);
    }

    state.large_font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
    if (!state.large_font) {
        state.large_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    }

    AudioState audio;
    init_audio(&audio, state.settings.volume);

    MenuState current_menu = MENU_MAIN;
    int running = 1;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        switch (current_menu) {
            case MENU_MAIN:
                current_menu = main_menu_loop(&state, &audio);
                if (current_menu == MENU_MAIN) {
                    running = 0;
                }
                break;
            case MENU_SETTINGS:
                current_menu = settings_menu_loop(&state);
                break;
            case MENU_STATS:
                current_menu = stats_menu_loop(&state);
                break;
            case MENU_GAME_OVER: {
                GameContext game;
                init_game(&game, &state);
                GameStatus status = game_loop(&game, &state, &audio);
                cleanup_game(&game);

                if (status == GAME_QUIT) {
                    running = 0;
                } else {
                    current_menu = game_over_loop(&state, game.score);
                }
                break;
            }
        }
    }

    cleanup_audio(&audio);
    cleanup_state(&state);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
