# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs:
    inputs.flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import inputs.nixpkgs {inherit system;};

      sysLibs = with pkgs; [
        libGL
        xorg.libX11
        xorg.libXrandr
        xorg.libXinerama
        xorg.libXcursor
        xorg.libXi
        xorg.libXdmcp
      ];
    in {
      devShell = pkgs.mkShell {
        hardeningDisable = ["all"];

        packages = with pkgs;
          [
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
            doctest
          ]
          ++ sysLibs;

        LD_LIBRARY_PATH = "$LD_LIBRARY_PATH:${pkgs.lib.makeLibraryPath sysLibs}";
      };

      formatter = pkgs.alejandra;
    });
}
