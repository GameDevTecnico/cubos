# Flake used for development with nix
{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-23.11";
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
            (doxygen.overrideAttrs {
              version = "1.8.20";
              src = fetchFromGitHub {
                owner = "doxygen";
                repo = "doxygen";
                rev = "Release_1_8_20";
                sha256 = "sha256-MBe8fmDb35MS9C6XzbpoX3ZhivvxVg3KITIDFEfckJ0=";
              };
            })

            (python3.withPackages (ps: [
              ps.jinja2
              ps.pygments
            ]))

            # = libs =
            glfw
            glm
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
