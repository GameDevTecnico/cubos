# engine/CMakeLists.txt
# Cubos engine build configuration

# Set engine source files

set(CUBOS_ENGINE_SOURCE
    "src/cubos/entry.cpp"
)

# Create cubos engine

add_executable(cubos-engine ${CUBOS_ENGINE_SOURCE})
target_link_libraries(cubos-engine cubos-core)
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-engine PUBLIC cxx_std_20)
