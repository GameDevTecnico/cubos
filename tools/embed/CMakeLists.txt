# tools/embed/CMakeLists.txt
# Cubos embed tool build configuration

# Set engine source files

set(CUBOS_EMBED_SOURCE
    "src/embed.cpp"
)

# Create cubos engine

add_executable(cubos-embed ${CUBOS_EMBED_SOURCE})
set_property(TARGET cubos-embed PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-embed PUBLIC cxx_std_20)
