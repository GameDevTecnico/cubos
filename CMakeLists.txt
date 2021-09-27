cmake_minimum_required(VERSION 3.0.0)
project(cubos VERSION 0.1.0)

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)

if(WITH_GLFW)
    option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif()
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(YAMLCPP_USE_SUBMODULE "Compile YAML CPP from source?" ON)

set(CUBOS_SOURCE
    "src/main.cpp"

    "src/io/glfw_window.cpp"

    "src/gl/ogl_render_device.cpp"
)

set(CUBOS_INCLUDE
    "include/cubos/io/window.hpp"

    "include/cubos/gl/render_device.hpp"
    "include/cubos/gl/ogl_render_device.hpp"
)

add_library(cubos ${CUBOS_SOURCE} ${CUBOS_INCLUDE})
target_include_directories(cubos PUBLIC "include")

# Dependencies
if(WITH_GLFW)
    if(GLFW_USE_SUBMODULE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(lib/glfw)
    else()
        find_package(glfw3 REQUIRED)
    endif()

    target_link_libraries(cubos glfw)
    target_compile_definitions(cubos PRIVATE WITH_GLFW)
endif()

if(GLM_USE_SUBMODULE)
    add_subdirectory(lib/glm)
else()
    find_package(glm REQUIRED)
endif()

if(YAMLCPP_USE_SUBMODULE)
    add_subdirectory(lib/yaml-cpp)
else()
    find_package(yaml-cpp REQUIRED)
endif()

if(WITH_OPENGL)
    add_subdirectory(lib/glad)
    target_link_libraries(cubos glad)
    target_compile_definitions(cubos PRIVATE WITH_OPENGL)
endif()

target_link_libraries(cubos glm glad yaml-cpp ${CMAKE_DL_LIBS})

# Enable CTest and CPack
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
