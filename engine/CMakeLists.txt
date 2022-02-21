# engine/CMakeLists.txt
# Cubos engine build configuration

# Set engine source files

set(CUBOS_ENGINE_SOURCE
        src/cubos/rendering/renderer.cpp

        src/cubos/entry.cpp
        src/cubos/rendering/deferred/deferred_renderer.cpp
        src/cubos/rendering/post_processing/post_processing_pass.cpp
        src/cubos/rendering/post_processing/copy_pass.cpp
        src/cubos/rendering/shadow_mapping/shadow_mapper.cpp
        )

set(CUBOS_ENGINE_INCLUDE
        include/cubos/rendering/renderer.hpp
        include/cubos/rendering/deferred/deferred_renderer.hpp
        include/cubos/rendering/post_processing/post_processing_pass.hpp
        include/cubos/rendering/post_processing/copy_pass.hpp
        include/cubos/rendering/shadow_mapping/shadow_mapper.h
        )

# Create cubos engine

add_executable(cubos-engine ${CUBOS_ENGINE_SOURCE} ${CUBOS_ENGINE_INCLUDE})
target_link_libraries(cubos-engine cubos-core)
target_include_directories(cubos-engine PUBLIC "include" PRIVATE "src")
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)