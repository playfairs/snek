{
  pkgs ? import <nixpkgs> { },
}:

let
  stdenv = pkgs.stdenv;
  meson = pkgs.meson;
  ninja = pkgs.ninja;
  pkgconfig = pkgs.pkg-config;

  versionToml = builtins.readFile ./version.toml;
  versionMatch = builtins.match "version[ \\t]*=[ \\t]*\"([^\"]+)\"" versionToml;
  versionFromToml = if versionMatch == null then "0.0" else versionMatch [ 1 ];
in

stdenv.mkDerivation {
  pname = "snek";
  version = versionFromToml;

  src = ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkgconfig
  ];

  buildInputs = with pkgs; [
    sdl2
    sdl2_image
    sdl2_mixer
  ];

  configurePhase = ''
    meson setup --prefix=$out build
  '';

  buildPhase = ''
    ninja -C build
  '';

  installPhase = ''
    ninja -C build install
  '';

  meta = with pkgs.lib; {
    description = "snek - a small snake game built with Meson";
    license = licenses.gpl3;
    platforms = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
    ];
    maintainers = [ ];
  };
}
