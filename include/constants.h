#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DIS_WIDTH 1000
#define DIS_HEIGHT 800
#define HEADER_HEIGHT 70
#define GAME_AREA_TOP HEADER_HEIGHT
#define SNAKE_BLOCK 20
#define SNAKE_SPEED 10
#define POWERUP_DURATION 10

#define WHITE_R 240
#define WHITE_G 240
#define WHITE_B 245

#define BLACK_R 15
#define BLACK_G 15
#define BLACK_B 20

#define LIGHT_GRAY_R 60
#define LIGHT_GRAY_G 60
#define LIGHT_GRAY_B 75

#define DARK_GRAY_R 40
#define DARK_GRAY_G 40
#define DARK_GRAY_B 50

#define BLUE_R 100
#define BLUE_G 150
#define BLUE_B 255

#define LIGHT_BLUE_R 140
#define LIGHT_BLUE_G 180
#define LIGHT_BLUE_B 255

#define RED_R 255
#define RED_G 80
#define RED_B 80

#define GREEN_R 80
#define GREEN_G 200
#define GREEN_B 120

#define LIGHT_GREEN_R 120
#define LIGHT_GREEN_G 230
#define LIGHT_GREEN_B 150

#define LIGHT_RED_R 255
#define LIGHT_RED_G 120
#define LIGHT_RED_B 120

#define ACCENT_R 255
#define ACCENT_G 180
#define ACCENT_B 50

#define ACCENT2_R 180
#define ACCENT2_G 100
#define ACCENT2_B 255

#define GRID_COLOR_R 25
#define GRID_COLOR_G 25
#define GRID_COLOR_B 35

#define MAX_SNAKE_LENGTH 1000
#define MAX_POWERUPS 6

typedef enum {
    POWERUP_SPEED_BOOST,
    POWERUP_SLOW_MO,
    POWERUP_DOUBLE_POINTS,
    POWERUP_INVINCIBLE,
    POWERUP_PATHFIND,
    POWERUP_FRENZY,
    POWERUP_NONE
} PowerupType;

typedef enum {
    SKIN_DEFAULT,
    SKIN_BLUE,
    SKIN_RED,
    SKIN_RAINBOW
} SnakeSkin;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

#endif
