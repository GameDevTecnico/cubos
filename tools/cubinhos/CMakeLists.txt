# tools/cubinhos/CMakeLists.txt
# Cubinhos build configuration

set(CUBINHOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
    "src/convert.cpp"
    "src/generate.cpp"
)

add_executable(cubinhos ${CUBINHOS_SOURCE})
set_property(TARGET cubinhos PROPERTY CXX_STANDARD 20)
target_compile_features(cubinhos PUBLIC cxx_std_20)
target_link_libraries(cubinhos PUBLIC cubos-core)
