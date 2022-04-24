# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)

message("# Building engine samples: " ${BUILD_ENGINE_SAMPLES})

# Set engine source files

set(CUBOS_ENGINE_SOURCE
    "src/cubos/engine/cubos.cpp"

    src/cubos/engine/gl/renderer.cpp
    src/cubos/engine/gl/deferred/renderer.cpp
    src/cubos/engine/gl/pps/pass.cpp
    src/cubos/engine/gl/pps/copy_pass.cpp)

set(CUBOS_ENGINE_INCLUDE
    "include/cubos/engine/cubos.hpp"

    include/cubos/engine/gl/renderer.hpp
    include/cubos/engine/gl/deferred/renderer.hpp
    include/cubos/engine/gl/pps/pass.hpp
    include/cubos/engine/gl/pps/copy_pass.hpp)

# Create cubos engine

add_library(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
target_link_libraries(cubos-engine PUBLIC cubos-core)
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)

# Add engine samples
if (BUILD_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif ()