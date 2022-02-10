# core/CMakeLists.txt
# Cubos core build configuration

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)
if(WITH_GLFW)
    option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif()
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(YAMLCPP_USE_SUBMODULE "Compile YAML CPP from source?" ON)
option(GOOGLETEST_USE_SUBMODULE "Compile Google Test from source?" ON)
option(SPDLOG_USE_SUBMODULE "Compile SPDLOG from source" ON)

option(BUILD_CORE_SAMPLES "Build cubos core samples" OFF)
option(BUILD_CORE_TESTS "Build cubos core tests?" OFF)

message( "# Building core samples: " ${BUILD_CORE_SAMPLES})
message( "# Building core tests: " ${BUILD_CORE_TESTS})

# Set core source files

set(CUBOS_CORE_SOURCE
    "src/cubos/log.cpp"
    "src/cubos/settings.cpp"

    "src/cubos/memory/stream.cpp"
    "src/cubos/memory/std_stream.cpp"
    "src/cubos/memory/buffer_stream.cpp"
    "src/cubos/memory/serializer.cpp"
    "src/cubos/memory/deserializer.cpp"
    "src/cubos/memory/yaml_serializer.cpp"
    "src/cubos/memory/yaml_deserializer.cpp"

    "src/cubos/data/file.cpp"
    "src/cubos/data/file_system.cpp"
    "src/cubos/data/std_archive.cpp"
    "src/cubos/data/embedded_archive.cpp"

    "src/cubos/io/window.cpp"
    "src/cubos/io/glfw_window.hpp"
    "src/cubos/io/glfw_window.cpp"

    "src/cubos/io/input_manager.cpp"
    "src/cubos/io/action.cpp"

    "src/cubos/io/sources/button_press.cpp"
    "src/cubos/io/sources/single_axis.cpp"
    "src/cubos/io/sources/double_axis.cpp"

    "src/cubos/gl/debug.cpp"
    
    "src/cubos/gl/ogl_render_device.hpp"
    "src/cubos/gl/ogl_render_device.cpp"
    "src/cubos/gl/material.cpp"
    "src/cubos/gl/palette.cpp"
    "src/cubos/gl/grid.cpp"
)

set(CUBOS_CORE_INCLUDE
    "include/cubos/log.hpp"
    "include/cubos/settings.hpp"

    "include/cubos/memory/stream.hpp"
    "include/cubos/memory/std_stream.hpp"
    "include/cubos/memory/buffer_stream.hpp"
    "include/cubos/memory/serializer.hpp"
    "include/cubos/memory/deserializer.hpp"
    "include/cubos/memory/yaml_serializer.hpp"
    "include/cubos/memory/yaml_deserializer.hpp"
    "include/cubos/memory/serialization_map.hpp"
    
    "include/cubos/data/file.hpp"
    "include/cubos/data/file_stream.hpp"
    "include/cubos/data/file_system.hpp"
    "include/cubos/data/archive.hpp"
    "include/cubos/data/std_archive.hpp"
    "include/cubos/data/embedded_archive.hpp"

    "include/cubos/io/window.hpp"

    "include/cubos/gl/debug.hpp"
    "include/cubos/gl/render_device.hpp"
    "include/cubos/gl/material.hpp"
    "include/cubos/gl/palette.hpp"
    "include/cubos/gl/grid.hpp"

    "include/cubos/io/input_manager.hpp"
    "include/cubos/io/action.hpp"
    "include/cubos/io/context.hpp"
    "include/cubos/io/sources/source.hpp"
    "include/cubos/io/sources/button_press.hpp"
    "include/cubos/io/sources/single_axis.hpp"
    "include/cubos/io/sources/double_axis.hpp"
)

# Create core library

add_library(cubos-core ${CUBOS_CORE_SOURCE} ${CUBOS_CORE_INCLUDE})
target_include_directories(cubos-core PUBLIC "include" PRIVATE "src")
set_property(TARGET cubos-core PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-core PUBLIC cxx_std_20)

# Link dependencies

if(WITH_OPENGL)
    add_subdirectory(lib/glad)
    target_link_libraries(cubos-core PRIVATE glad)
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

    target_link_libraries(cubos-core PRIVATE glfw)
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

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(cubos-core PUBLIC glm::glm spdlog yaml-cpp ${CMAKE_DL_LIBS})
target_link_libraries(cubos-core PRIVATE glad Threads::Threads)

# Add core tests

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS) 
    include(CTest)
    enable_testing()

    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Add core samples
if (BUILD_CORE_SAMPLES)
    add_subdirectory(samples)
endif()

# Add doxygen documentation

find_package(Doxygen COMPONENTS dot)

if (Doxygen_FOUND)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HTML_EXTRA_FILES ${CMAKE_SOURCE_DIR}/docs/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_SOURCE_DIR}/docs/doxygen-awesome-css/doxygen-awesome.css)
    set(DOXYGEN_HTML_HEADER ${CMAKE_SOURCE_DIR}/docs/header.html)
    set(DOXYGEN_HTML_FOOTER ${CMAKE_SOURCE_DIR}/docs/footer.html)
    doxygen_add_docs(
        core-doxygen
        "include"
        COMMENT "Generate core documentation"
    )
endif()
