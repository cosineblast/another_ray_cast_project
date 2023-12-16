{
  description = "A very basic flake with devShell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system}; in
      {
        devShells.default =
          pkgs.mkShell {
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.clang
              pkgs.gnumake
              pkgs.SDL2
              pkgs.SDL2.dev
              pkgs.SDL2_image
            ];
          };
      });
}
