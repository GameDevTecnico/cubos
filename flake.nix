# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            libxkbcommon
            libGL
            libglvnd
            pkg-config
            cmake
            gcc
            glfw
            glm
            spdlog
            clang-tools
            doxygen
            graphviz
            clang_14
            doctest
            ccache
            lcov
            (python3.withPackages (ps: with ps; [
              jinja2
              pygments
            ]))
          ];

          LD_LIBRARY_PATH = "/run/opengl-driver/lib:/run/opengl-driver-32/lib";
        };
      });
}
