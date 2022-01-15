# engine/CMakeLists.txt
# Cubos engine build configuration

# Set engine source files

set(CUBOS_ENGINE_SOURCE
        "src/cubos/rendering/renderer.cpp"

        "src/cubos/entry.cpp"
        )

set(CUBOS_ENGINE_INCLUDE
        "include/cubos/rendering/renderer.hpp"
        )

# Create cubos engine

add_executable(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_link_libraries(cubos-engine cubos-core)
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)