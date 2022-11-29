# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)

message("# Building engine samples: " ${BUILD_ENGINE_SAMPLES})

# Set engine source files

set(CUBOS_ENGINE_SOURCE
    "src/cubos/engine/cubos.cpp"

    "src/cubos/engine/ecs/draw_system.cpp"
    "src/cubos/engine/ecs/transform_system.cpp"

    "src/cubos/engine/gl/frame.cpp"
    "src/cubos/engine/gl/renderer.cpp"
    "src/cubos/engine/gl/deferred/renderer.cpp"
    "src/cubos/engine/gl/pps/bloom.cpp"
    "src/cubos/engine/gl/pps/copy_pass.cpp"
    "src/cubos/engine/gl/pps/manager.cpp"
    "src/cubos/engine/gl/pps/pass.cpp"

    "src/cubos/engine/data/meta.cpp"
    "src/cubos/engine/data/asset_manager.cpp"
    "src/cubos/engine/data/loader.cpp"
    "src/cubos/engine/data/grid.cpp" 
    "src/cubos/engine/data/palette.cpp" 
    "src/cubos/engine/data/scene.cpp" 
)

set(CUBOS_ENGINE_INCLUDE
    "include/cubos/engine/cubos.hpp"

    "include/cubos/engine/ecs/position.hpp"
    "include/cubos/engine/ecs/rotation.hpp"
    "include/cubos/engine/ecs/scale.hpp"
    "include/cubos/engine/ecs/local_to_world.hpp"
    "include/cubos/engine/ecs/grid.hpp"
    "include/cubos/engine/ecs/transform_system.hpp"
    "include/cubos/engine/ecs/draw_system.hpp"

    "include/cubos/engine/gl/frame.hpp"
    "include/cubos/engine/gl/renderer.hpp"
    "include/cubos/engine/gl/deferred/renderer.hpp"
    "include/cubos/engine/gl/pps/bloom.hpp"
    "include/cubos/engine/gl/pps/copy_pass.hpp"
    "include/cubos/engine/gl/pps/manager.hpp"
    "include/cubos/engine/gl/pps/pass.hpp"

    "include/cubos/engine/data/meta.hpp"
    "include/cubos/engine/data/asset.hpp"
    "include/cubos/engine/data/asset_manager.hpp"
    "include/cubos/engine/data/loader.hpp"
    "include/cubos/engine/data/palette.hpp"
    "include/cubos/engine/data/grid.hpp"
    "include/cubos/engine/data/scene.hpp"
)

# Create cubos engine

add_library(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
target_link_libraries(cubos-engine PUBLIC cubos-core)
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)
target_compile_options(cubos-engine PUBLIC -Wno-attributes)

# Run cubinhos generate before building engine
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include
    COMMAND ${CMAKE_BINARY_DIR}/tools/cubinhos${CMAKE_EXECUTABLE_SUFFIX} generate ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include/components
    DEPENDS ${CMAKE_BINARY_DIR}/tools/cubinhos${CMAKE_EXECUTABLE_SUFFIX} ${CUBOS_ENGINE_INCLUDE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Make cubos-engine depend on cubinhos generate
add_custom_target(cubos-engine-cubinhos DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include)
add_dependencies(cubos-engine cubos-engine-cubinhos)

target_include_directories(cubos-engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

# Add engine samples
if (BUILD_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif ()
