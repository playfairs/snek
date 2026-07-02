{
  description = "Snek - C Snake Game";

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
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            SDL2
            SDL2_ttf
            SDL2_mixer
            pkg-config
          ];

          shellHook = ''
            export PKG_CONFIG_PATH="${pkgs.SDL2}/lib/pkgconfig:${pkgs.SDL2_ttf}/lib/pkgconfig:${pkgs.SDL2_mixer}/lib/pkgconfig:$PKG_CONFIG_PATH"
            export LD_LIBRARY_PATH="${pkgs.SDL2}/lib:${pkgs.SDL2_ttf}/lib:${pkgs.SDL2_mixer}/lib:$LD_LIBRARY_PATH"
          '';
        };

        formatter = formatters.wrapper;
        checks.formatting = formatters.check self;
      }
    );
}
