# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem ( system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkg-config
            cmake
            gcc
            glfw
            glm
            libyamlcpp
            spdlog
            clang-tools
            doxygen
            graphviz
          ];
        };
      }
    );
}
