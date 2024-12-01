cmake_minimum_required(VERSION 3.20.0)
project(game VERSION 0.1.0)

option(DISTRIBUTE "Build the game for distribution" OFF)

add_executable(game
    src/main.cpp
)

find_package(cubos REQUIRED)
target_link_libraries(game cubos::engine)

target_compile_features(game PRIVATE cxx_std_20)
target_compile_options(game PRIVATE
    # Enable preprocessor conformance mode in MSVC - required for __VA_ARGS__ to work correctly
    $<$<CXX_COMPILER_ID:MSVC>: /Zc:preprocessor>
)

if(EMSCRIPTEN)
    set_target_properties(game PROPERTIES SUFFIX ".html")

    # If we're building for the web, the assets directories will be embedded in the executable
    target_link_options(game PRIVATE
		"SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
		"SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
	)
    target_compile_definitions(game PRIVATE
		APP_ASSETS_PATH="/assets"
		BUILTIN_ASSETS_PATH="/builtin"
	)
elseif(DISTRIBUTE)
    # If we're building for distribution, we want to use the assets directories in the same directory as the executable
    target_compile_definitions(game PRIVATE
		APP_ASSETS_PATH="assets"
		BUILTIN_ASSETS_PATH="builtin"
	)
else()
    # If we're in development, we want to use the assets directory in the project directory
    target_compile_definitions(game PRIVATE
		APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
		BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
	)
endif()

if(DISTRIBUTE)
    # On installation, copy both the executable and the assets directories to the installation directory
    install(TARGETS game RUNTIME DESTINATION .)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION builtin)
endif()
