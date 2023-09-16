# engine/CMakeLists.txt
# Cubos engine build configuration

include(QuadradosGenerate)

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)
option(BUILD_ENGINE_TESTS "Build cubos engine tests?" OFF)

message("# Building engine samples: " ${BUILD_ENGINE_SAMPLES})
message("# Building engine tests: " ${BUILD_ENGINE_TESTS})

# Set engine source files
set(CUBOS_ENGINE_SOURCE
    "src/cubos/engine/cubos.cpp"

    "src/cubos/engine/settings/plugin.cpp"
    "src/cubos/engine/settings/settings.cpp"

    "src/cubos/engine/window/plugin.cpp"

    "src/cubos/engine/imgui/plugin.cpp"

    "src/cubos/engine/tools/asset_explorer/plugin.cpp"
    "src/cubos/engine/tools/settings_inspector/plugin.cpp"
    "src/cubos/engine/tools/entity_selector/plugin.cpp"
    "src/cubos/engine/tools/world_inspector/plugin.cpp"
    "src/cubos/engine/tools/entity_inspector/plugin.cpp"
    "src/cubos/engine/tools/scene_editor/plugin.cpp"

    "src/cubos/engine/transform/plugin.cpp"

    "src/cubos/engine/assets/plugin.cpp"
    "src/cubos/engine/assets/assets.cpp"
    "src/cubos/engine/assets/bridge.cpp"
    "src/cubos/engine/assets/asset.cpp"
    "src/cubos/engine/assets/meta.cpp"
    "src/cubos/engine/assets/bridges/file.cpp"

    "src/cubos/engine/scene/plugin.cpp"
    "src/cubos/engine/scene/bridge.cpp"

    "src/cubos/engine/voxels/plugin.cpp"

    "src/cubos/engine/collisions/plugin.cpp"
    "src/cubos/engine/collisions/broad_phase.cpp"
    "src/cubos/engine/collisions/broad_phase_collisions.cpp"

    "src/cubos/engine/input/plugin.cpp"
    "src/cubos/engine/input/input.cpp"
    "src/cubos/engine/input/bindings.cpp"
    "src/cubos/engine/input/action.cpp"
    "src/cubos/engine/input/axis.cpp"

    "src/cubos/engine/renderer/plugin.cpp"
    "src/cubos/engine/renderer/frame.cpp"
    "src/cubos/engine/renderer/renderer.cpp"
    "src/cubos/engine/renderer/deferred_renderer.cpp"
    "src/cubos/engine/renderer/pps/bloom.cpp"
    "src/cubos/engine/renderer/pps/copy_pass.cpp"
    "src/cubos/engine/renderer/pps/manager.cpp"
    "src/cubos/engine/renderer/pps/pass.cpp"
)

set(CUBOS_ENGINE_INCLUDE
    "include/cubos/engine/cubos.hpp"

    "include/cubos/engine/settings/plugin.hpp"

    "include/cubos/engine/window/plugin.hpp"

    "include/cubos/engine/imgui/plugin.hpp"

    "include/cubos/engine/tools/asset_explorer/plugin.hpp"
    "include/cubos/engine/tools/settings_inspector/plugin.hpp"
    "include/cubos/engine/tools/entity_selector/plugin.hpp"
    "include/cubos/engine/tools/world_inspector/plugin.hpp"
    "include/cubos/engine/tools/entity_inspector/plugin.hpp"
    "include/cubos/engine/tools/scene_editor/plugin.hpp"

    "include/cubos/engine/transform/position.hpp"
    "include/cubos/engine/transform/rotation.hpp"
    "include/cubos/engine/transform/scale.hpp"
    "include/cubos/engine/transform/local_to_world.hpp"
    "include/cubos/engine/transform/plugin.hpp"

    "include/cubos/engine/assets/plugin.hpp"
    "include/cubos/engine/assets/assets.hpp"
    "include/cubos/engine/assets/bridge.hpp"
    "include/cubos/engine/assets/asset.hpp"
    "include/cubos/engine/assets/meta.hpp"
    "include/cubos/engine/assets/bridges/file.hpp"
    "include/cubos/engine/assets/bridges/json.hpp"
    "include/cubos/engine/assets/bridges/binary.hpp"

    "include/cubos/engine/scene/plugin.hpp"
    "include/cubos/engine/scene/scene.hpp"
    "include/cubos/engine/scene/bridge.hpp"

    "include/cubos/engine/voxels/plugin.hpp"

    "include/cubos/engine/collisions/plugin.hpp"
    "include/cubos/engine/collisions/broad_phase_collisions.hpp"
    "include/cubos/engine/collisions/aabb.hpp"
    "include/cubos/engine/collisions/colliders/box.hpp"
    "include/cubos/engine/collisions/colliders/capsule.hpp"
    "include/cubos/engine/collisions/colliders/plane.hpp"
    "include/cubos/engine/collisions/colliders/simplex.hpp"

    "include/cubos/engine/input/plugin.hpp"
    "include/cubos/engine/input/input.hpp"
    "include/cubos/engine/input/bindings.hpp"
    "include/cubos/engine/input/action.hpp"
    "include/cubos/engine/input/axis.hpp"

    "include/cubos/engine/renderer/plugin.hpp"
    "include/cubos/engine/renderer/frame.hpp"
    "include/cubos/engine/renderer/renderer.hpp"
    "include/cubos/engine/renderer/deferred_renderer.hpp"
    "include/cubos/engine/renderer/environment.hpp"
    "include/cubos/engine/renderer/spot_light.hpp"
    "include/cubos/engine/renderer/directional_light.hpp"
    "include/cubos/engine/renderer/point_light.hpp"
    "include/cubos/engine/renderer/pps/bloom.hpp"
    "include/cubos/engine/renderer/pps/copy_pass.hpp"
    "include/cubos/engine/renderer/pps/manager.hpp"
    "include/cubos/engine/renderer/pps/pass.hpp"
)

# Create cubos engine
add_library(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
target_link_libraries(cubos-engine PUBLIC cubos-core)
cubos_common_target_options(cubos-engine)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(cubos-engine PUBLIC -Wno-attributes)
endif()

# Generate component headers
quadrados_generate(cubos-engine ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add engine tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_ENGINE_TESTS)
    add_subdirectory(tests)
endif()

# Add engine samples
if(BUILD_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif()
