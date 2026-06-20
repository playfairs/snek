#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/state.h"
#include "../include/game.h"
#include "../include/graphics.h"
#include "../include/input.h"
#include "../include/audio.h"

typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_GAME_OVER
} MenuState;

static int draw_button(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h, int mouse_x, int mouse_y, int* clicked) {
    SDL_Rect rect = {x, y, w, h};
    
    int is_hover = (mouse_x >= x && mouse_x <= x + w && mouse_y >= y && mouse_y <= y + h);
    int is_pressed = is_hover && *clicked;
    
    SDL_Color light_gray = {LIGHT_GRAY_R, LIGHT_GRAY_G, LIGHT_GRAY_B, 255};
    SDL_Color dark_gray = {DARK_GRAY_R, DARK_GRAY_G, DARK_GRAY_B, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color black = {BLACK_R, BLACK_G, BLACK_B, 255};
    
    SDL_SetRenderDrawColor(renderer, light_gray.r, light_gray.g, light_gray.b, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    if (is_pressed) {
        SDL_SetRenderDrawColor(renderer, dark_gray.r, dark_gray.g, dark_gray.b, 255);
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 255);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
    } else {
        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 255);
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_SetRenderDrawColor(renderer, dark_gray.r, dark_gray.g, dark_gray.b, 255);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
    }
    
    SDL_Surface* text_surf = TTF_RenderText_Blended(font, text, black);
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
    int button_width = 200;
    int button_height = 50;
    int button_spacing = 20;
    int start_x = (DIS_WIDTH - (button_width * 3 + button_spacing * 2)) / 2;
    int button_y = DIS_HEIGHT / 2;
    
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
        
        int play_clicked = draw_button(state->renderer, state->button_font, "Play",
                                       start_x, button_y, button_width, button_height,
                                       mouse_x, mouse_y, &mouse_clicked);
        int settings_clicked = draw_button(state->renderer, state->button_font, "Settings",
                                           start_x + button_width + button_spacing, button_y,
                                           button_width, button_height, mouse_x, mouse_y, &mouse_clicked);
        int quit_clicked = draw_button(state->renderer, state->button_font, "Exit",
                                       start_x + (button_width + button_spacing) * 2, button_y,
                                       button_width, button_height, mouse_x, mouse_y, &mouse_clicked);
        
        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);
        
        if (play_clicked) {
            return MENU_GAME_OVER;
        }
        if (settings_clicked) {
            return MENU_SETTINGS;
        }
        if (quit_clicked) {
            running = 0;
        }
    }
    
    return MENU_MAIN;
}

static MenuState settings_menu_loop(GameState* state) {
    int running = 1;
    int current_option = 0;
    int num_options = 6;
    
    const char* skin_names[] = {"Default", "Blue", "Red", "Rainbow"};
    const char* sound_names[] = {"On", "Off"};
    const char* music_names[] = {"On", "Off"};
    const char* volume_names[] = {"30%", "50%", "70%", "100%"};
    const char* difficulty_names[] = {"Easy", "Normal", "Hard"};
    
    int skin_index = state->settings.current_skin;
    int sound_index = state->settings.sound_enabled ? 0 : 1;
    int music_index = state->settings.music_enabled ? 0 : 1;
    int volume_index = 2;
    int difficulty_index = state->settings.difficulty;
    
    while (running) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            InputAction action = handle_input_settings(&event, &current_option, num_options);
            
            if (action == INPUT_QUIT) {
                return MENU_MAIN;
            }
            if (action == INPUT_ESCAPE) {
                running = 0;
            }
            if (action == INPUT_ENTER) {
                switch (current_option) {
                    case 0:
                        skin_index = (skin_index + 1) % 4;
                        state->settings.current_skin = skin_index;
                        save_settings(&state->settings);
                        break;
                    case 1:
                        sound_index = (sound_index + 1) % 2;
                        state->settings.sound_enabled = sound_index == 0;
                        save_settings(&state->settings);
                        break;
                    case 2:
                        music_index = (music_index + 1) % 2;
                        state->settings.music_enabled = music_index == 0;
                        save_settings(&state->settings);
                        break;
                    case 3:
                        volume_index = (volume_index + 1) % 4;
                        state->settings.volume = 0.3 + volume_index * 0.2;
                        save_settings(&state->settings);
                        break;
                    case 4:
                        difficulty_index = (difficulty_index + 1) % 3;
                        state->settings.difficulty = difficulty_index;
                        save_settings(&state->settings);
                        break;
                    case 5:
                        running = 0;
                        break;
                }
            }
        }
        
        draw_settings_menu(state->renderer, state->large_font, state->score_font,
                          state->button_font, state, current_option);
        
        int option_y = DIS_HEIGHT / 4 + 80;
        int option_height = 50;
        
        for (int i = 0; i < num_options; i++) {
            char option_text[64];
            
            switch (i) {
                case 0:
                    snprintf(option_text, sizeof(option_text), "Snake Skin: %s", skin_names[skin_index]);
                    break;
                case 1:
                    snprintf(option_text, sizeof(option_text), "Sound: %s", sound_names[sound_index]);
                    break;
                case 2:
                    snprintf(option_text, sizeof(option_text), "Music: %s", music_names[music_index]);
                    break;
                case 3:
                    snprintf(option_text, sizeof(option_text), "Volume: %s", volume_names[volume_index]);
                    break;
                case 4:
                    snprintf(option_text, sizeof(option_text), "Difficulty: %s", difficulty_names[difficulty_index]);
                    break;
                case 5:
                    snprintf(option_text, sizeof(option_text), "Back");
                    break;
            }
            
            SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
            SDL_Color light_gray = {LIGHT_GRAY_R, LIGHT_GRAY_G, LIGHT_GRAY_B, 255};
            SDL_Color accent = {ACCENT_R, ACCENT_G, ACCENT_B, 255};
            SDL_Color dark_gray = {DARK_GRAY_R, DARK_GRAY_G, DARK_GRAY_B, 255};
            
            SDL_Surface* option_surf = TTF_RenderText_Blended(state->score_font, option_text,
                                                             i == current_option ? white : light_gray);
            SDL_Texture* option_tex = SDL_CreateTextureFromSurface(state->renderer, option_surf);
            SDL_Rect option_rect = {(DIS_WIDTH - option_surf->w) / 2, option_y + i * option_height,
                                   option_surf->w, option_surf->h};
            
            if (i == current_option) {
                SDL_Rect bg_rect = {option_rect.x - 20, option_rect.y - 10,
                                   option_rect.w + 40, option_rect.h + 20};
                SDL_SetRenderDrawColor(state->renderer, dark_gray.r, dark_gray.g, dark_gray.b, 255);
                SDL_RenderFillRect(state->renderer, &bg_rect);
                SDL_SetRenderDrawColor(state->renderer, accent.r, accent.g, accent.b, 255);
                SDL_RenderDrawRect(state->renderer, &bg_rect);
            }
            
            SDL_RenderCopy(state->renderer, option_tex, NULL, &option_rect);
            SDL_FreeSurface(option_surf);
            SDL_DestroyTexture(option_tex);
        }
        
        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 15);
    }
    
    return MENU_MAIN;
}

static MenuState game_over_loop(GameState* state, int score) {
    int running = 1;
    int button_width = 200;
    int button_height = 50;
    int button_spacing = 30;
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
        
        int play_again_clicked = draw_button(state->renderer, state->button_font, "Play Again",
                                             start_x,
                                             button_y, button_width, button_height,
                                             mouse_x, mouse_y, &mouse_clicked);
        int quit_clicked = draw_button(state->renderer, state->button_font, "Quit",
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
