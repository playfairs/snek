CC ?= gcc
CROSS_PREFIX ?=
AR ?= ar
STRIP ?= strip

SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/snek

CPPFLAGS := -I./src
CFLAGS ?= -Wall -Wextra -std=c99
DEBUG_CFLAGS := -O0 -g -DDEBUG
RELEASE_CFLAGS := -O3 -DNDEBUG -flto

LDFLAGS ?=
LDLIBS := -lm

SDL_STATIC_CFLAGS :=
SDL_STATIC_LIBS :=

# assembly stuff for quicker compilation even by a few ns
USE_ASM ?= 0
ASM_SRCS :=
ifneq ($(USE_ASM),0)
    ifeq ($(ARCH),x86_64)
        ASM_SRCS += src/arch/x86_64/snake_hotpath.S
        CPPFLAGS += -DSNEK_USE_ASM=1
    endif
endif

# static sdl linking
PKG_CONFIG_STATIC := $(shell command -v pkg-config 2>/dev/null)
ifneq ($(PKG_CONFIG_STATIC),)
    SDL_STATIC_CFLAGS := $(shell pkg-config --cflags --static sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)
    SDL_STATIC_LIBS := $(shell pkg-config --libs --static sdl2 SDL2_ttf SDL2_mixer 2>/dev/null)
endif

# explicit fallback paths for common linux and mingw installs
UNAME_S := $(shell uname -s 2>/dev/null)

SOURCES = \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/core/game/game.c \
    $(SRC_DIR)/core/game/state.c \
    $(SRC_DIR)/core/graphics/graphics.c \
    $(SRC_DIR)/core/input/input.c \
    $(SRC_DIR)/core/audio/audio.c \
    $(SRC_DIR)/gui/gui.c \
    $(SRC_DIR)/gui/layout.c \
    $(SRC_DIR)/gui/hud.c \
    $(SRC_DIR)/gui/menu.c \
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
    $(SRC_DIR)/powerups/magnet/logic.c \
    $(SRC_DIR)/powerups/freeze/logic.c \
    $(SRC_DIR)/powerups/shrink/logic.c \
    $(SRC_DIR)/powerups/grow/logic.c \
    $(SRC_DIR)/powerups/teleport/logic.c \
    $(SRC_DIR)/powerups/split/logic.c \
    $(SRC_DIR)/powerups/pathfind/logic.c \
    $(SRC_DIR)/powerups/pathfind/intelligence.c \
    $(SRC_DIR)/snake/snake.c \
    $(SRC_DIR)/snake/movement.c \
    $(SRC_DIR)/snake/collision.c \
    $(SRC_DIR)/snake/rendering.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

ASM_OBJECTS = $(ASM_SRCS:%.S=$(BUILD_DIR)/%.o)
OBJECTS += $(ASM_OBJECTS)

.PHONY: all debug release clean run windows linux-static windows-static

all: debug

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(TARGET)

release: CFLAGS += $(RELEASE_CFLAGS)
release: LDFLAGS += -s
release: $(TARGET)

# linux static-link target.
# this intentionally errors out if the static SDL libraries are unavailable,
# rather than silently depending on libSDL2.so, libSDL2_ttf.so, or libSDL2_mixer.so.
linux-static: CFLAGS += $(SDL_STATIC_CFLAGS)
linux-static: LDLIBS += $(SDL_STATIC_LIBS)
linux-static: release
	@echo "linux static release is at $(TARGET)"

# windows static-link target for MinGW/MSYS2.
# the user must have the static SDL archives installed. The build should fail fast
# with a clear message if the required .a files are absent.
windows-static: CFLAGS += $(SDL_STATIC_CFLAGS)
windows-static: LDLIBS += $(SDL_STATIC_LIBS)
windows-static: release
	@echo "windows static release is at $(TARGET)"

windows: windows-static
	@mkdir -p release/windows
	@cp $(TARGET) release/windows/snek.exe
	@cp -r assets release/windows/
	@cp run.bat release/windows/run.bat
	@cp README-Windows.txt release/windows/README.txt
	@echo "Prepared release/windows for a portable Windows distribution"

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) release

run: $(TARGET)
	./$(TARGET)

# the static-lib check is intentionally strict.
ifneq ($(SDL_STATIC_LIBS),)
else
ifeq ($(OS),Windows_NT)
windows-static: ; @echo "ERROR: static SDL libraries were not found. Install the MinGW static SDL packages or ensure pkg-config --static works."; exit 1
else
linux-static: ; @echo "ERROR: static SDL libraries were not found. Install the static SDL packages for your distro (e.g. libsdl2-dev + static libs)."; exit 1
endif
endif