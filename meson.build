project('snek', 'c',
  version: run_command('python3', '-c', 'import tomllib, pathlib; data = tomllib.loads(pathlib.Path("version.toml").read_text()); print(data["project"]["version"])').stdout().strip(),
  meson_version: '>=' + run_command('python3', '-c', 'import tomllib, pathlib; data = tomllib.loads(pathlib.Path("version.toml").read_text()); print(data["minimum_requirements"]["meson"])').stdout().strip(),
  default_options : ['c_std=c99', 'warning_level=2']
)

inc = include_directories('src')

sdl2 = dependency('sdl2')
sdl2_ttf = dependency('sdl2_ttf')
sdl2_mixer = dependency('sdl2_mixer')

sources = files(
  'src/main.c',
  'src/core/audio/audio.c',
  'src/core/game/game.c',
  'src/core/game/state.c',
  'src/core/graphics/graphics.c',
  'src/core/input/input.c',
  'src/gui/gui.c',
  'src/gui/hud.c',
  'src/gui/layout.c',
  'src/gui/menu.c',
  'src/items/apple/apple.c',
  'src/items/banana/banana.c',
  'src/items/bomb/bomb.c',
  'src/items/registry.c',
  'src/powerups/registry.c',
  'src/powerups/speedboost/logic.c',
  'src/powerups/slowmo/logic.c',
  'src/powerups/doublepoints/logic.c',
  'src/powerups/invincible/logic.c',
  'src/powerups/frenzy/logic.c',
  'src/powerups/magnet/logic.c',
  'src/powerups/freeze/logic.c',
  'src/powerups/shrink/logic.c',
  'src/powerups/grow/logic.c',
  'src/powerups/teleport/logic.c',
  'src/powerups/split/logic.c',
  'src/powerups/pathfind/logic.c',
  'src/powerups/pathfind/intelligence.c',
  'src/snake/snake.c',
  'src/snake/movement.c',
  'src/snake/collision.c',
  'src/snake/rendering.c'
)

add_project_arguments('-Wall', '-Wextra', language: 'c')

executable('snek', sources,
  include_directories: inc,
  dependencies: [sdl2, sdl2_ttf, sdl2_mixer],
  install: true,
  install_dir: get_option('bindir'),
)

install_subdir('assets/sfx', install_dir: join_paths(get_option('datadir'), 'snek'))
