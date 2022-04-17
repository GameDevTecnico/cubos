# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)

message( "# Building engine samples: " ${BUILD_ENGINE_SAMPLES})

# Set engine source files

set(CUBOS_ENGINE_SOURCE
    "src/cubos/engine/cubos.cpp"

    src/cubos/rendering/renderer.cpp
    src/cubos/rendering/deferred/deferred_renderer.cpp
    src/cubos/rendering/post_processing/post_processing_pass.cpp
    src/cubos/rendering/post_processing/copy_pass.cpp)

set(CUBOS_ENGINE_INCLUDE
    "include/cubos/engine/cubos.hpp"

    include/cubos/rendering/renderer.hpp
    include/cubos/rendering/deferred/deferred_renderer.hpp
    include/cubos/rendering/post_processing/post_processing_pass.hpp
    include/cubos/rendering/post_processing/copy_pass.hpp)

# Create cubos engine

add_library(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
target_link_libraries(cubos-engine PUBLIC cubos-core)
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)

# Add engine samples
if (BUILD_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif()