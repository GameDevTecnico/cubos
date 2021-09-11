cmake_minimum_required(VERSION 3.0.0)
project(cubos VERSION 0.1.0)

include_directories(
    lib/glad/include
)

include(CTest)
enable_testing()

add_executable(cubos "src/main.cpp" )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_library(glad "lib/glad/src/glad.c")

add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

target_link_libraries(cubos glfw glm glad)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
