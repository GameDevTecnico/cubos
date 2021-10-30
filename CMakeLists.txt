cmake_minimum_required(VERSION 3.9.0)
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
option(BUILD_ENGINE_TESTS "Build cubos tests?" OFF)
message( "# Building Samples: " ${BUILD_SAMPLES})
message( "# Building Tests: " ${BUILD_ENGINE_TESTS})

set(CUBOS_SOURCE
    "src/log.cpp"
    "src/settings.cpp"

    "src/io/glfw_window.cpp"
    "src/gl/ogl_render_device.cpp"
)

set(CUBOS_INCLUDE
    "include/cubos/log.hpp"
    "include/cubos/settings.hpp"
    
    "include/cubos/io/keyboard.hpp"
    "include/cubos/io/window.hpp"
    "include/cubos/io/glfw_window.hpp"

    "include/cubos/gl/render_device.hpp"
    "include/cubos/gl/ogl_render_device.hpp"
)

set(CUBOS_ENTRYPOINT
    "src/entry.cpp"
)

# Create Cubos Library
add_library(cubos-core ${CUBOS_SOURCE} ${CUBOS_INCLUDE})
target_include_directories(cubos-core PUBLIC "include")
set_property(TARGET cubos-core PROPERTY CXX_STANDARD 20)

# Dependencies
if(WITH_OPENGL)
    add_subdirectory(lib/glad)
    target_link_libraries(cubos-core glad)
    target_compile_definitions(cubos-core PRIVATE WITH_OPENGL)
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

    target_link_libraries(cubos-core glfw)
    target_compile_definitions(cubos-core PRIVATE WITH_GLFW)
endif()

if(GLM_USE_SUBMODULE)
    add_subdirectory(lib/glm)
else()
    find_package(glm REQUIRED)
endif()

if(YAMLCPP_USE_SUBMODULE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
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

target_link_libraries(cubos-core glm glad yaml-cpp spdlog ${CMAKE_DL_LIBS})

# Add the tests for cubos-core
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_ENGINE_TESTS) 
    include(CTest)
    enable_testing()

    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    include(CPack)

    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Create cubos library
add_library(cubos-engine ${CUBOS_ENTRYPOINT})
target_link_libraries(cubos-engine cubos-core)
set_property(TARGET cubos-engine PROPERTY CXX_STANDARD 20)

# Add samples
if (BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

# Doxygen documentation
find_package(Doxygen COMPONENTS dot)

if (Doxygen_FOUND)
    set(DOXYGEN_SHOW_NAMESPACES NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HTML_EXTRA_FILES docs/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET docs/doxygen-awesome-css/doxygen-awesome.css)
    set(DOXYGEN_HTML_HEADER docs/header.html)
    set(DOXYGEN_HTML_FOOTER docs/footer.html)
    doxygen_add_docs(
        doxygen
        "include"
        COMMENT "Generate documentation"
    )
endif()
