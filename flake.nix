# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.11";
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
            lcov
          ];
        };
      });
}
