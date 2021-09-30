cmake_minimum_required(VERSION 3.0.0)
project(cubos VERSION 0.1.0)

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)

if(WITH_GLFW)
    option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif()
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(YAMLCPP_USE_SUBMODULE "Compile YAML CPP from source?" ON)
option(GOOGLETEST_USE_SUBMODULE "Compile Google Test from source?" ON)
option(SPDLOG_USE_SUBMODULE "Compile SPDLOG from source" ON)

option(BUILD_SAMPLES "Build samples" OFF)

set(CUBOS_SOURCE
    "src/entry.cpp"
    "src/log.cpp"
    
    "src/io/glfw_window.cpp"

    "src/gl/ogl_render_device.cpp"
)

set(CUBOS_INCLUDE
    "include/cubos/log.hpp"

    "include/cubos/io/window.hpp"

    "include/cubos/gl/render_device.hpp"
    "include/cubos/gl/ogl_render_device.hpp"
)

add_library(cubos ${CUBOS_SOURCE} ${CUBOS_INCLUDE})
target_include_directories(cubos PUBLIC "include")
set_property(TARGET cubos PROPERTY CXX_STANDARD 20)

# Dependencies

if(WITH_OPENGL)
    add_subdirectory(lib/glad)
    target_link_libraries(cubos glad)
    target_compile_definitions(cubos PRIVATE WITH_OPENGL)
endif()

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



if (GOOGLETEST_USE_SUBMODULE)
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory(lib/googletest)
else()
    find_package(googletest REQUIRED)
endif()

if (SPDLOG_USE_SUBMODULE)
    add_subdirectory(lib/spdlog)
else()
    find_package(spdlog REQUIRED)
endif()

target_link_libraries(cubos glm glad yaml-cpp spdlog ${CMAKE_DL_LIBS})

# Enable CTest and CPack
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Enable Google Test
set(CUBOS_TESTS_SOURCE
    "tests/test_example.cpp"
)

add_executable(
    cubos_tests
    ${CUBOS_TESTS_SOURCE}
)

target_link_libraries(
    cubos_tests
    gtest_main
)

include(googletest)
gtest_discover_tests(cubos_tests)

# Add the samples subdirectory
if (BUILD_SAMPLES)
    add_subdirectory(samples)
endif()