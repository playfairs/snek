#include "core/audio/audio.h"
#include "core/game/game.h"
#include "core/game/state.h"
#include "core/input/input.h"
#include "gui/gui.h"
#include "items/registry.h"
#include "powerups/registry.h"
#include "snake/snake.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_STATS,
    MENU_GAME_OVER
} MenuState;

static MenuState main_menu_loop(GameState *state,
                                AudioState *audio,
                                GuiContext *gui)
{
    (void)audio;
    int running = 1;
    
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(state->renderer, &screen_w, &screen_h);
    
    int shell_x = (int)(screen_w * 0.08f);
    int shell_w = (int)(screen_w * 0.84f);
    int left_col_w = (int)(shell_w * 0.55f);
    int left_col_x = shell_x + (int)(shell_w * 0.03f);
    int right_col_x = left_col_x + left_col_w + 20;
    int right_col_w = shell_w - left_col_w - (int)(shell_w * 0.06f) - 20;
    
    int shell_y = (int)(screen_h * 0.08f);
    int shell_h = (int)(screen_h * 0.84f);
    int action_y = shell_y + (int)(shell_h * 0.15f);
    int action_h = shell_h - (int)(shell_h * 0.15f) - (int)(shell_h * 0.05f);
    
    int button_width = right_col_w - 40;
    int button_height = 50;
    int button_spacing = 12;
    int start_x = right_col_x + 20;
    int button_y = action_y + 80;

    while (running)
    {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        gui_render_main_menu(gui,
                             state->settings.current_skin,
                             &state->stats);

        int play_clicked = gui_draw_button(gui,
                                           "PLAY GAME",
                                           start_x,
                                           button_y,
                                           button_width,
                                           button_height,
                                           mouse_x,
                                           mouse_y,
                                           &mouse_clicked);
        int settings_clicked = gui_draw_button(
            gui,
            "SETTINGS",
            start_x,
            button_y + button_height + button_spacing,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        int stats_clicked = gui_draw_button(
            gui,
            "STATS",
            start_x,
            button_y + (button_height + button_spacing) * 2,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        int quit_clicked = gui_draw_button(
            gui,
            "EXIT",
            start_x,
            button_y + (button_height + button_spacing) * 3,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (play_clicked)
        {
            return MENU_GAME_OVER;
        }
        if (settings_clicked)
        {
            return MENU_SETTINGS;
        }
        if (stats_clicked)
        {
            return MENU_STATS;
        }
        if (quit_clicked)
        {
            running = 0;
        }
    }

    return MENU_MAIN;
}

static MenuState settings_menu_loop(GameState *state,
                                    AudioState *audio,
                                    GuiContext *gui)
{
    int running = 1;

    const char *skin_names[] = {"Default",
                                "Blue",
                                "Red",
                                "Rainbow",
                                "Purple",
                                "Gold",
                                "Cyan",
                                "Neon",
                                "Holographic",
                                "Monochrome",
                                "Chromatic",
                                "Cosmic",
                                "Chrome",
                                "Bubble",
                                "Shadow",
                                "Solar",
                                "Aqua",
                                "Midnight"};
    const char *sound_names[] = {"On", "Off"};
    const char *music_names[] = {"On", "Off"};
    const char *volume_names[] = {"30%",
                                  "50%",
                                  "70%",
                                  "100%"};
    const double volume_values[] = {0.3, 0.5, 0.7, 1.0};
    const char *mode_names[] = {"Classic",
                                "Challenge",
                                "Time Attack",
                                "Endless",
                                "Berserk",
                                "Maze",
                                "Survival",
                                "Ghost Run",
                                "Inverse",
                                "Rainbow Run"};
    const char *difficulty_names[] = {"Easy",
                                      "Normal",
                                      "Hard"};

    int skin_index = state->settings.current_skin;
    int sound_index = state->settings.sound_enabled ? 0 : 1;
    int music_index = state->settings.music_enabled ? 0 : 1;
    int volume_index = 0;
    if (state->settings.volume >= 0.95)
    {
        volume_index = 3;
    }
    else if (state->settings.volume >= 0.6)
    {
        volume_index = 2;
    }
    else if (state->settings.volume >= 0.4)
    {
        volume_index = 1;
    }
    int difficulty_index = state->settings.difficulty;
    int mode_index = state->settings.mode;

    if (skin_index < 0 || skin_index >= SKIN_COUNT)
    {
        skin_index = SKIN_DEFAULT;
    }
    if (difficulty_index < 0
        || difficulty_index >= DIFFICULTY_COUNT)
    {
        difficulty_index = DIFFICULTY_NORMAL;
    }
    if (mode_index < 0 || mode_index >= MODE_COUNT)
    {
        mode_index = MODE_CLASSIC;
    }

    int button_width = 220;
    int button_height = 50;
    int button_spacing = 18;
    
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(state->renderer, &screen_w, &screen_h);
    
    int frame_x = (int)(screen_w * 0.08f);
    int frame_w = (int)(screen_w * 0.84f);
    int left_col_w = (int)(frame_w * 0.50f);
    int left_col_x = frame_x + (int)(frame_w * 0.03f);
    int right_col_x = left_col_x + left_col_w + 20;
    int right_col_w = frame_w - left_col_w - (int)(frame_w * 0.06f) - 20;
    
    int frame_y = (int)(screen_h * 0.08f);
    int frame_h = (int)(screen_h * 0.84f);
    int settings_y = frame_y + (int)(frame_h * 0.13f);
    
    int start_x = right_col_x + (right_col_w - button_width) / 2;
    int start_y = settings_y + 60;

    while (running)
    {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_right_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mouse_right_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        gui_render_settings_menu(gui, &state->settings, &state->stats);

        char skin_text[64];
        snprintf(skin_text,
                 sizeof(skin_text),
                 "Skin: %s",
                 skin_names[skin_index]);
        int skin_clicked = gui_draw_button(gui,
                                           skin_text,
                                           start_x,
                                           start_y,
                                           button_width,
                                           button_height,
                                           mouse_x,
                                           mouse_y,
                                           &mouse_clicked);
        if (skin_clicked)
        {
            skin_index = (skin_index + 1) % SKIN_COUNT;
            state->settings.current_skin = skin_index;
            save_settings(&state->settings);
        }
        else if (mouse_right_clicked)
        {
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= start_y && mouse_y <= start_y + button_height)
            {
                skin_index = (skin_index - 1 + SKIN_COUNT) % SKIN_COUNT;
                state->settings.current_skin = skin_index;
                save_settings(&state->settings);
                mouse_right_clicked = 0;
            }
        }

        char sound_text[64];
        snprintf(sound_text,
                 sizeof(sound_text),
                 "Sound: %s",
                 sound_names[sound_index]);
        int sound_clicked = gui_draw_button(
            gui,
            sound_text,
            start_x,
            start_y + (button_height + button_spacing),
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (sound_clicked)
        {
            sound_index = (sound_index + 1) % 2;
            state->settings.sound_enabled =
                sound_index == 0;
            save_settings(&state->settings);
            if (state->settings.sound_enabled && audio)
            {
                play_pickup_sound(audio,
                                  state->settings.volume);
            }
        }
        else if (mouse_right_clicked)
        {
            int sound_y = start_y + (button_height + button_spacing);
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= sound_y && mouse_y <= sound_y + button_height)
            {
                sound_index = (sound_index - 1 + 2) % 2;
                state->settings.sound_enabled =
                    sound_index == 0;
                save_settings(&state->settings);
                if (state->settings.sound_enabled && audio)
                {
                    play_pickup_sound(audio,
                                      state->settings.volume);
                }
                mouse_right_clicked = 0;
            }
        }

        char music_text[64];
        snprintf(music_text,
                 sizeof(music_text),
                 "Music: %s",
                 music_names[music_index]);
        int music_clicked = gui_draw_button(
            gui,
            music_text,
            start_x,
            start_y + (button_height + button_spacing) * 2,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (music_clicked)
        {
            music_index = (music_index + 1) % 2;
            state->settings.music_enabled =
                music_index == 0;
            save_settings(&state->settings);
        }
        else if (mouse_right_clicked)
        {
            int music_y = start_y + (button_height + button_spacing) * 2;
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= music_y && mouse_y <= music_y + button_height)
            {
                music_index = (music_index - 1 + 2) % 2;
                state->settings.music_enabled =
                    music_index == 0;
                save_settings(&state->settings);
                mouse_right_clicked = 0;
            }
        }

        char volume_text[64];
        snprintf(volume_text,
                 sizeof(volume_text),
                 "Volume: %s",
                 volume_names[volume_index]);
        int volume_clicked = gui_draw_button(
            gui,
            volume_text,
            start_x,
            start_y + (button_height + button_spacing) * 3,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (volume_clicked)
        {
            volume_index = (volume_index + 1) % 4;
            state->settings.volume =
                volume_values[volume_index];
            save_settings(&state->settings);
        }
        else if (mouse_right_clicked)
        {
            int volume_y = start_y + (button_height + button_spacing) * 3;
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= volume_y && mouse_y <= volume_y + button_height)
            {
                volume_index = (volume_index - 1 + 4) % 4;
                state->settings.volume =
                    volume_values[volume_index];
                save_settings(&state->settings);
                mouse_right_clicked = 0;
            }
        }

        char mode_text[64];
        snprintf(mode_text,
                 sizeof(mode_text),
                 "Mode: %s",
                 mode_names[mode_index]);
        int mode_clicked = gui_draw_button(
            gui,
            mode_text,
            start_x,
            start_y + (button_height + button_spacing) * 4,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (mode_clicked)
        {
            mode_index = (mode_index + 1) % MODE_COUNT;
            state->settings.mode = mode_index;
            save_settings(&state->settings);
        }
        else if (mouse_right_clicked)
        {
            int mode_y = start_y + (button_height + button_spacing) * 4;
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= mode_y && mouse_y <= mode_y + button_height)
            {
                mode_index = (mode_index - 1 + MODE_COUNT) % MODE_COUNT;
                state->settings.mode = mode_index;
                save_settings(&state->settings);
                mouse_right_clicked = 0;
            }
        }

        char difficulty_text[64];
        snprintf(difficulty_text,
                 sizeof(difficulty_text),
                 "Difficulty: %s",
                 difficulty_names[difficulty_index]);
        int difficulty_clicked = gui_draw_button(
            gui,
            difficulty_text,
            start_x,
            start_y + (button_height + button_spacing) * 5,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (difficulty_clicked)
        {
            difficulty_index =
                (difficulty_index + 1) % DIFFICULTY_COUNT;
            state->settings.difficulty = difficulty_index;
            save_settings(&state->settings);
        }
        else if (mouse_right_clicked)
        {
            int difficulty_y = start_y + (button_height + button_spacing) * 5;
            if (mouse_x >= start_x && mouse_x <= start_x + button_width &&
                mouse_y >= difficulty_y && mouse_y <= difficulty_y + button_height)
            {
                difficulty_index = (difficulty_index - 1 + DIFFICULTY_COUNT) % DIFFICULTY_COUNT;
                state->settings.difficulty = difficulty_index;
                save_settings(&state->settings);
                mouse_right_clicked = 0;
            }
        }

        int back_clicked = gui_draw_button(
            gui,
            "BACK",
            start_x,
            start_y + (button_height + button_spacing) * 6,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);
        if (back_clicked)
        {
            running = 0;
        }

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);
    }

    return MENU_MAIN;
}

static MenuState stats_menu_loop(GameState *state,
                                  GuiContext *gui)
{
    int running = 1;
    int button_width = 220;
    int button_height = 55;
    int start_x = (DIS_WIDTH - button_width) / 2;
    int start_y = DIS_HEIGHT - 140;

    while (running)
    {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return MENU_MAIN;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        gui_render_stats_menu(gui, &state->stats);

        int back_clicked = gui_draw_button(gui,
                                           "BACK",
                                           start_x,
                                           start_y,
                                           button_width,
                                           button_height,
                                           mouse_x,
                                           mouse_y,
                                           &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (back_clicked)
        {
            return MENU_MAIN;
        }
    }

    return MENU_MAIN;
}

static MenuState game_over_loop(GameState *state,
                                 GuiContext *gui,
                                 int score)
{
    int running = 1;
    int button_width = 220;
    int button_height = 55;
    int button_spacing = 25;
    int button_y = DIS_HEIGHT / 3 + 160;

    while (running)
    {
        SDL_Event event;
        int mouse_clicked = 0;
        int mouse_x = 0, mouse_y = 0;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return MENU_MAIN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_clicked = 1;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return MENU_MAIN;
                }
            }
        }

        SDL_GetMouseState(&mouse_x, &mouse_y);

        gui_render_game_over(gui, score);

        int total_width = button_width * 2 + button_spacing;
        int start_x = (DIS_WIDTH - total_width) / 2;

        int play_again_clicked =
            gui_draw_button(gui,
                            "PLAY AGAIN",
                            start_x,
                            button_y,
                            button_width,
                            button_height,
                            mouse_x,
                            mouse_y,
                            &mouse_clicked);
        int quit_clicked = gui_draw_button(
            gui,
            "EXIT",
            start_x + button_width + button_spacing,
            button_y,
            button_width,
            button_height,
            mouse_x,
            mouse_y,
            &mouse_clicked);

        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / 60);

        if (play_again_clicked)
        {
            return MENU_GAME_OVER;
        }
        if (quit_clicked)
        {
            running = 0;
        }
    }

    return MENU_MAIN;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    if (TTF_Init() < 0)
    {
        SDL_Quit();
        return 1;
    }

    GameState state;
    init_state(&state);

    state.window = SDL_CreateWindow("Snek",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    DIS_WIDTH,
                                    DIS_HEIGHT,
                                    0);
    if (!state.window)
    {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    state.renderer =
        SDL_CreateRenderer(state.window,
                           -1,
                           SDL_RENDERER_ACCELERATED);
    if (!state.renderer)
    {
        SDL_DestroyWindow(state.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    state.font_style =
        TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc",
                     14);
    if (!state.font_style)
    {
        state.font_style =
            TTF_OpenFont("/usr/share/fonts/truetype/dejavu/"
                         "DejaVuSans-Bold.ttf",
                         14);
    }

    state.score_font =
        TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc",
                     18);
    if (!state.score_font)
    {
        state.score_font =
            TTF_OpenFont("/usr/share/fonts/truetype/dejavu/"
                         "DejaVuSans-Bold.ttf",
                         18);
    }

    state.button_font =
        TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc",
                     14);
    if (!state.button_font)
    {
        state.button_font =
            TTF_OpenFont("/usr/share/fonts/truetype/dejavu/"
                         "DejaVuSans-Bold.ttf",
                         14);
    }

    state.large_font =
        TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc",
                     24);
    if (!state.large_font)
    {
        state.large_font =
            TTF_OpenFont("/usr/share/fonts/truetype/dejavu/"
                         "DejaVuSans-Bold.ttf",
                         24);
    }

    AudioState audio;
    init_audio(&audio, state.settings.volume);

    GuiContext gui;
    gui_init(&gui,
             state.renderer,
             state.font_style,
             state.button_font,
             state.score_font,
             state.large_font);

    MenuState current_menu = MENU_MAIN;
    int running = 1;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        switch (current_menu)
        {
        case MENU_MAIN:
            current_menu = main_menu_loop(&state, &audio, &gui);
            if (current_menu == MENU_MAIN)
            {
                running = 0;
            }
            break;
        case MENU_SETTINGS:
            current_menu =
                settings_menu_loop(&state, &audio, &gui);
            break;
        case MENU_STATS:
            current_menu = stats_menu_loop(&state, &gui);
            break;
        case MENU_GAME_OVER:
        {
            GameContext game;
            init_game(&game, &state);
            GameStatus status =
                    game_loop(&game, &state, &audio, &gui);
            double end_time = SDL_GetTicks() / 1000.0;
            double elapsed = end_time - game.start_time;
            if (elapsed > 0.0
                && elapsed > state.stats.best_time)
            {
                state.stats.best_time = elapsed;
                save_stats(&state.stats);
            }

            cleanup_game(&game);

            if (status == GAME_QUIT)
            {
                running = 0;
            }
            else
            {
                current_menu =
                    game_over_loop(&state, &gui, game.score);
            }
            break;
        }
        }
    }

    cleanup_audio(&audio);
    gui_cleanup(&gui);
    cleanup_state(&state);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
