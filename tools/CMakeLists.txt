# tools/CMakeLists.txt
# Cubos tools (cubinhos) build configuration

set(CUBINHOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
)

add_executable(cubinhos ${CUBINHOS_SOURCE})
set_property(TARGET cubinhos PROPERTY CXX_STANDARD 20)
target_compile_features(cubinhos PUBLIC cxx_std_20)
