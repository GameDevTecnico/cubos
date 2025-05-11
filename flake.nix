# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-24.11";
    nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs:
    inputs.flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import inputs.nixpkgs { inherit system; };
        pkgs-unstable = import inputs.nixpkgs-unstable { inherit system; };

        sysLibs = with pkgs; [
          libGL
          xorg.libX11
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXcursor
          xorg.libXi
          xorg.libXdmcp
          libpulseaudio
        ];
      in
      {
        name = "cubos";

        devShell = pkgs.mkShell {
          hardeningDisable = [ "all" ];

          packages = with pkgs; [
            # = build tools =
            cmake
            ccache
            pkg-config
            pkgs-unstable.emscripten
            ninja

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

            # = debug =
            gdb
          ]
          ++ sysLibs;

          LD_LIBRARY_PATH = "$LD_LIBRARY_PATH:${pkgs.lib.makeLibraryPath sysLibs}";

          shellHook = ''
            export EMSCRIPTEN=${pkgs-unstable.emscripten}/share/emscripten
            export EM_CACHE=$(git rev-parse --show-toplevel)/.em_cache
            mkdir -p $EM_CACHE
            cp -r $EMSCRIPTEN/cache/* $EM_CACHE
            chmod u+rwx -R $EM_CACHE

            if [ -n "$WAYLAND_DISPLAY" ]; then
              LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(nix-build '<nixpkgs>' -A wayland --no-out-link)/lib"
            fi
          '';
        };

        packages.default = pkgs.callPackage ./default.nix { };

        formatter = pkgs.alejandra;
      });
}
