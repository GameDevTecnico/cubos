# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs:
    inputs.flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import inputs.nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell {
          packages = with pkgs; [
            # = build tools =
            cmake
            ccache
            pkg-config

            # = formatting =
            clang-tools

            # = docs =
            doxygen
            (python3.withPackages (ps: [
              ps.jinja2
              ps.pygments
            ]))

            # = libs =
            glfw
            glm
            # spdlog
            # fmt
            doctest

            # = system libs =
            libGL
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            xorg.libXdmcp
          ];
        };
      });
}
