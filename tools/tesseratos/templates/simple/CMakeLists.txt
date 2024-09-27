cmake_minimum_required(VERSION 3.20.0)
project(game VERSION 0.1.0)

option(DISTRIBUTE "Build the game for distribution" OFF)

add_executable(game
    src/main.cpp
)

find_package(cubos REQUIRED)
target_link_libraries(game cubos::engine)

target_compile_features(game cxx_std_20)
target_compile_options(game
    # Enable preprocessor conformance mode in MSVC - required for __VA_ARGS__ to work correctly
    $<$<CXX_COMPILER_ID:MSVC>: /Zc:preprocessor>
)

if(DISTRIBUTE)
    # If we're building for distribution, we want to use the assets directory in the same directory as the executable
    target_compile_definitions(game PROJECT_ASSETS_FOLDER="assets")

    # On installation, copy both the executable and the assets directory to the installation directory
    install(TARGETS game RUNTIME DESTINATION .)
    install(DIRECTORY assets DESTINATION .)
else()
    # If we're in development, we want to use the assets directory in the project directory
    target_compile_definitions(game PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()
