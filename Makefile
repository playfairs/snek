CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src
LDFLAGS = -lm

SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)
SDL2_CONFIG = $(shell command -v sdl2-config 2>/dev/null)
SDL2_FRAMEWORKS = $(shell if [ -d /Library/Frameworks/SDL2.framework ] && [ -d /Library/Frameworks/SDL2_ttf.framework ] && [ -d /Library/Frameworks/SDL2_mixer.framework ]; then echo yes; fi)

ifneq ($(SDL_CFLAGS),)
    CFLAGS += $(SDL_CFLAGS)
    LDFLAGS += $(SDL_LIBS)
else
    ifneq ($(SDL2_CONFIG),)
        CFLAGS += $(shell sdl2-config --cflags)
        LDFLAGS += $(shell sdl2-config --libs)
    else
        UNAME_S := $(shell uname -s)
        ifeq ($(UNAME_S),Darwin)
            CFLAGS += -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -I/usr/local/include -I/usr/local/include/SDL2 -I/usr/local/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2/include/SDL2
            ifeq ($(SDL2_FRAMEWORKS),yes)
                LDFLAGS += -F/Library/Frameworks -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer
            else
                LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lSDL2_ttf -lSDL2_mixer -Wl,-rpath,/opt/homebrew/lib -Wl,-rpath,/usr/local/lib
            endif
        else
            CFLAGS += -I/usr/include/SDL2
            LDFLAGS += -lSDL2 -lSDL2_ttf -lSDL2_mixer
        endif
    endif
endif

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/snek

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/core/game/game.c \
          $(SRC_DIR)/core/game/state.c \
          $(SRC_DIR)/core/graphics/graphics.c \
          $(SRC_DIR)/core/input/input.c \
          $(SRC_DIR)/core/audio/audio.c \
          $(SRC_DIR)/items/apple/apple.c \
          $(SRC_DIR)/items/banana/banana.c \
          $(SRC_DIR)/items/bomb/bomb.c \
          $(SRC_DIR)/items/registry.c \
          $(SRC_DIR)/powerups/registry.c \
          $(SRC_DIR)/powerups/speedboost/logic.c \
          $(SRC_DIR)/powerups/slowmo/logic.c \
          $(SRC_DIR)/powerups/doublepoints/logic.c \
          $(SRC_DIR)/powerups/invincible/logic.c \
          $(SRC_DIR)/powerups/frenzy/logic.c \
          $(SRC_DIR)/powerups/pathfind/logic.c \
          $(SRC_DIR)/powerups/pathfind/intelligence.c \
          $(SRC_DIR)/snake/snake.c \
          $(SRC_DIR)/snake/movement.c \
          $(SRC_DIR)/snake/collision.c \
          $(SRC_DIR)/snake/rendering.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
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
