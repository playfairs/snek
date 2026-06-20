# Snek

A Snake game with powerups, statistics tracking, and multiple snake skins.

## Installation

### Dependencies

Requires SDL2, SDL2_ttf, and SDL2_mixer.

#### Nix (Recommended)

```bash
nix develop
make
```

#### macOS

```bash
brew install sdl2 sdl2_ttf sdl2_mixer
make
```

#### Linux

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
make
```

## Running

```bash
./build/snek
```

## Project Structure

- `include/` - Header files
- `src/` - Implementation files
- `assets/sfx/` - Sound effect files
- `data/` - Game statistics and save data
