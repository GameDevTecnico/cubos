# tools/quadrados/CMakeLists.txt
# Quadrados build configuration

set(QUADRADOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
    "src/convert.cpp"
)

add_executable(quadrados ${QUADRADOS_SOURCE})
target_link_libraries(quadrados PUBLIC cubos-engine)
cubos_common_target_options(quadrados)
