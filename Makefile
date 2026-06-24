CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include
LDFLAGS = -lm

SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)

ifneq ($(SDL_CFLAGS),)
    CFLAGS += $(SDL_CFLAGS)
    LDFLAGS += $(SDL_LIBS)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        CFLAGS += -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -I/usr/local/include -I/usr/local/include/SDL2
        LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lSDL2_ttf -lSDL2_mixer -Wl,-rpath,/opt/homebrew/lib -Wl,-rpath,/usr/local/lib
    else
        CFLAGS += -I/usr/include/SDL2
        LDFLAGS += -lSDL2 -lSDL2_ttf -lSDL2_mixer
    endif
endif

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = $(BUILD_DIR)/snek

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/state.c \
          $(SRC_DIR)/game.c \
          $(SRC_DIR)/graphics.c \
          $(SRC_DIR)/input.c \
          $(SRC_DIR)/powerups.c \
          $(SRC_DIR)/powerups/powerup.c \
          $(SRC_DIR)/powerups/speedboost.c \
          $(SRC_DIR)/powerups/slowmo.c \
          $(SRC_DIR)/powerups/doublepoints.c \
          $(SRC_DIR)/powerups/invincible.c \
          $(SRC_DIR)/powerups/frenzy.c \
          $(SRC_DIR)/powerups/pathfind.c \
          $(SRC_DIR)/items/apple.c \
          $(SRC_DIR)/items/banana.c \
          $(SRC_DIR)/items/bomb.c \
          $(SRC_DIR)/items/items.c \
          $(SRC_DIR)/audio.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/powerups/%.o: $(SRC_DIR)/powerups/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

PREFIX ?= /usr/local
INSTALL = install -m 0755

install: $(TARGET)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(TARGET) $(DESTDIR)$(PREFIX)/bin/snek

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/snek
