# tools/quadrados/CMakeLists.txt
# Quadrados build configuration

set(QUADRADOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
    "src/convert.cpp"
    "src/generate.cpp"
)

add_executable(quadrados ${QUADRADOS_SOURCE})
set_property(TARGET quadrados PROPERTY CXX_STANDARD 20)
target_compile_features(quadrados PUBLIC cxx_std_20)
target_link_libraries(quadrados PUBLIC cubos-core)
cubos_common_target_options(quadrados)
