{ lib
, stdenv
, cmake
, doctest
, glfw
, glm
, stduuid
, nlohmann_json
, zstd
, fetchFromGitHub
}:

let
  libdwarf-lite = fetchFromGitHub {
    owner = "jeremy-rifkin";
    repo = "libdwarf-lite";
    rev = "main";
    sha256 = "sha256-S2KDfWqqdQfK5+eQny2X5k0A5u9npkQ8OFRLBmTulao=";
  };

  cpptrace = fetchFromGitHub {
    owner = "jeremy-rifkin";
    repo = "cpptrace";
    rev = "main";
    sha256 = "sha256-w1KObCtzEyYGsH1GuNkKBL/6EfybeAFi1JTili2X8fU=";
  };

  glad = fetchFromGitHub {
    owner = "GameDevTecnico";
    repo = "cubos-glad";
    rev = "496c56587442591557a89551d90906b1c53f639f";
    sha256 = "sha256-566u+oiPUJQ+LQ4gE+lE58uw60ftheWUSiklgRge4Jc=";
  };

  stb_image = fetchFromGitHub {
    owner = "GameDevTecnico";
    repo = "cubos-stb";
    rev = "5c340b5ee24ac74e69bf92b5bacdfbac6bbaa4a8";
    sha256 = "sha256-D4fWoezHR+rp3Ecnlj6BhaVu5IhB7Yr2cJH2dgFHZnY=";
  };

  imgui = fetchFromGitHub {
    owner = "ocornut";
    repo = "imgui";
    rev = "v1.89.9";
    sha256 = "sha256-0k9jKrJUrG9piHNFQaBBY3zgNIKM23ZA879NY+MNYTU=";
  };

  implot = fetchFromGitHub {
    owner = "epezent";
    repo = "implot";
    rev = "1f7a8c0314d838a76695bccebe0f66864f507bc0";
    sha256 = "sha256-7tBfG6hZrLKAL65q667XDx+n6bfgbUqtsmho+q/h+nE=";
  };

in

stdenv.mkDerivation {
  name = "cubos";
  version = "main";

  src = ./.;

  nativeBuildInputs = [ cmake ];
  buildInputs = [ doctest glfw glm stduuid nlohmann_json ];

  cmakeFlags = [
    "-DCMAKE_INSTALL_PREFIX=$out"
    "-DFETCHCONTENT_FULLY_DISCONNECTED:BOOL=ON"
    "-DFETCHCONTENT_SOURCE_DIR_GLAD:PATH=${glad}"
    "-DFETCHCONTENT_SOURCE_DIR_STB_IMAGE:PATH=${stb_image}"
    "-DFETCHCONTENT_SOURCE_DIR_IMGUI:PATH=${imgui}"
    "-DFETCHCONTENT_SOURCE_DIR_IMPLOT:PATH=${implot}"
    "-DFETCHCONTENT_SOURCE_DIR_LIBDWARF:PATH=${libdwarf-lite}"
    "-DFETCHCONTENT_SOURCE_DIR_ZSTD:PATH=${zstd}"
    "-DFETCHCONTENT_SOURCE_DIR_CPPTRACE:PATH=${cpptrace}"
    "-DCUBOS_CORE_SAMPLES:BOOL=OFF"
    "-DCUBOS_CORE_TESTS:BOOL=OFF"
    "-DCUBOS_ENGINE_SAMPLES:BOOL=OFF"
    "-DCUBOS_ENGINE_TESTS:BOOL=OFF"
    "-DTESSERATOS_DISTRIBUTE:BOOL=ON"
  ];

  meta = with lib; {
    homepage = "https://cubosengine.org";
    description = "A modern C++ game engine";
    license = licenses.mit;
    platforms = with platforms; linux ++ darwin;
  };
}
