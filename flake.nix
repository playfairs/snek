{
  description = "A simple Snake game.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    treefmt-nix.url = "github:numtide/treefmt-nix";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      treefmt-nix,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        formatters = import nix/formatter.nix { inherit pkgs treefmt-nix self; };
        versionToml = builtins.readFile ./version.toml;
        versionMatch = builtins.match "version[ \t]*=[ \t]*\"([^\"]+)\"" versionToml;
        versionFromToml = if versionMatch == null then "0.0" else versionMatch [ 1 ];
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            clang
            cmake
            meson
            ninja
            pkg-config
            SDL2
            SDL2_ttf
            SDL2_mixer
          ];
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "snek";
          version = versionFromToml;
          src = ./.;

          buildInputs = with pkgs; [
            gcc
            python3
            ninja
            clang
          ];

          nativeBuildInputs = with pkgs; [
            meson
            pkg-config
            SDL2
            SDL2_ttf
            SDL2_mixer
          ];

          meta = {
            description = "A simple Snake game.";
            maintainers = [ ];
          };
        };

        formatter = formatters.wrapper;
        checks.formatting = formatters.check;
      }
    );
}
