# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)

message( "# Building engine samples: " ${BUILD_ENGINE_SAMPLES})

# Set engine source files

set(CUBOS_ENGINE_SOURCE
    "src/cubos/engine/cubos.cpp"

    "src/cubos/engine/data/meta.cpp"
    "src/cubos/engine/data/asset_manager.cpp"
    "src/cubos/engine/data/qb_model.cpp"
)

set(CUBOS_ENGINE_INCLUDE
    "include/cubos/engine/cubos.hpp"

    "include/cubos/engine/data/meta.hpp"
    "include/cubos/engine/data/asset.hpp"
    "include/cubos/engine/data/asset_manager.hpp"
    "include/cubos/engine/data/loader.hpp"
    "include/cubos/engine/data/qb_model.hpp"
)

# Create cubos engine

add_library(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
target_link_libraries(cubos-engine PUBLIC cubos-core)
target_compile_features(cubos-engine PUBLIC cxx_std_20)

# Add engine samples
if (BUILD_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif()
