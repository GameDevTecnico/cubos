# tools/quadrados-gen/CMakeLists.txt
# Quadrados generate build configuration

add_executable(quadrados-gen "src/main.cpp")
target_link_libraries(quadrados-gen PUBLIC cubos-core)
cubos_common_target_options(quadrados-gen)
