# core/CMakeLists.txt
# Cubos core build configuration

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)
if (WITH_GLFW)
    option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif ()
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(YAMLCPP_USE_SUBMODULE "Compile YAML CPP from source?" ON)
option(GOOGLETEST_USE_SUBMODULE "Compile Google Test from source?" ON)
option(SPDLOG_USE_SUBMODULE "Compile SPDLOG from source?" ON)
option(FMT_USE_SUBMODULE "Compile FMT from source?" ON)

option(BUILD_CORE_SAMPLES "Build cubos core samples" OFF)
option(BUILD_CORE_TESTS "Build cubos core tests?" OFF)

message("# Building core samples: " ${BUILD_CORE_SAMPLES})
message("# Building core tests: " ${BUILD_CORE_TESTS})

# Set core source files

set(CUBOS_CORE_SOURCE
    "src/cubos/core/log.cpp"
    "src/cubos/core/settings.cpp"

    "src/cubos/core/memory/stream.cpp"
    "src/cubos/core/memory/std_stream.cpp"
    "src/cubos/core/memory/buffer_stream.cpp"

    "src/cubos/core/data/serializer.cpp"
    "src/cubos/core/data/deserializer.cpp"
    "src/cubos/core/data/yaml_serializer.cpp"
    "src/cubos/core/data/yaml_deserializer.cpp"
    "src/cubos/core/data/file.cpp"
    "src/cubos/core/data/file_system.cpp"
    "src/cubos/core/data/std_archive.cpp"
    "src/cubos/core/data/embedded_archive.cpp"
    "src/cubos/core/data/qb_parser.cpp"

    "src/cubos/core/io/window.cpp"
    "src/cubos/core/io/glfw_window.hpp"
    "src/cubos/core/io/glfw_window.cpp"

    "src/cubos/core/io/input_manager.cpp"
    "src/cubos/core/io/action.cpp"
    "src/cubos/core/io/context.cpp"

    "src/cubos/core/io/sources/button_press.cpp"
    "src/cubos/core/io/sources/single_axis.cpp"
    "src/cubos/core/io/sources/double_axis.cpp"

    "src/cubos/core/gl/debug.cpp"
    "src/cubos/core/gl/render_device.cpp"
    "src/cubos/core/gl/ogl_render_device.hpp"
    "src/cubos/core/gl/ogl_render_device.cpp"
    "src/cubos/core/gl/material.cpp"
    "src/cubos/core/gl/palette.cpp"
    "src/cubos/core/gl/grid.cpp"
    "src/cubos/core/gl/light.cpp"
    "src/cubos/core/gl/util.cpp"
    "src/cubos/core/gl/vertex.cpp"

    "src/cubos/core/ecs/world.cpp"
)

set(CUBOS_CORE_INCLUDE
    "include/cubos/core/log.hpp"
    "include/cubos/core/settings.hpp"

    "include/cubos/core/memory/stream.hpp"
    "include/cubos/core/memory/std_stream.hpp"
    "include/cubos/core/memory/buffer_stream.hpp"
    "include/cubos/core/memory/endianness.hpp"

    "include/cubos/core/data/serializer.hpp"
    "include/cubos/core/data/deserializer.hpp"
    "include/cubos/core/data/yaml_serializer.hpp"
    "include/cubos/core/data/yaml_deserializer.hpp"
    "include/cubos/core/data/serialization_map.hpp"
    "include/cubos/core/data/file.hpp"
    "include/cubos/core/data/file_stream.hpp"
    "include/cubos/core/data/file_system.hpp"
    "include/cubos/core/data/archive.hpp"
    "include/cubos/core/data/std_archive.hpp"
    "include/cubos/core/data/embedded_archive.hpp"
    "include/cubos/core/data/qb_parser.hpp"

    "include/cubos/core/io/window.hpp"

    "include/cubos/core/gl/debug.hpp"
    "include/cubos/core/gl/render_device.hpp"
    "include/cubos/core/gl/material.hpp"
    "include/cubos/core/gl/palette.hpp"
    "include/cubos/core/gl/grid.hpp"
    "include/cubos/core/gl/vertex.hpp"
    "include/cubos/core/gl/camera.hpp"
    "include/cubos/core/gl/light.hpp"
    "include/cubos/core/gl/util.hpp"

    "include/cubos/core/io/input_manager.hpp"
    "include/cubos/core/io/action.hpp"
    "include/cubos/core/io/context.hpp"
    "include/cubos/core/io/sources/source.hpp"
    "include/cubos/core/io/sources/button_press.hpp"
    "include/cubos/core/io/sources/single_axis.hpp"
    "include/cubos/core/io/sources/double_axis.hpp"

    "include/cubos/core/ecs/world.hpp"
    "include/cubos/core/ecs/world_view.hpp"
    "include/cubos/core/ecs/storage.hpp"
    "include/cubos/core/ecs/vec_storage.hpp"
    "include/cubos/core/ecs/map_storage.hpp"
    "include/cubos/core/ecs/null_storage.hpp"
)

# Create core library

add_library(cubos-core ${CUBOS_CORE_SOURCE} ${CUBOS_CORE_INCLUDE})
target_include_directories(cubos-core PUBLIC "include" PRIVATE "src")
set_property(TARGET cubos-core PROPERTY CXX_STANDARD 20)
target_compile_features(cubos-core PUBLIC cxx_std_20)

# Link dependencies

if (WITH_OPENGL)
    add_subdirectory(lib/glad)
    target_link_libraries(cubos-core PRIVATE glad)
    target_compile_definitions(cubos-core PRIVATE WITH_OPENGL)
endif ()

if (WITH_GLFW)
    if (GLFW_USE_SUBMODULE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(lib/glfw)
    else ()
        find_package(glfw3 REQUIRED)
    endif ()

    target_link_libraries(cubos-core PRIVATE glfw)
    target_compile_definitions(cubos-core PRIVATE WITH_GLFW)
endif ()

if (GLM_USE_SUBMODULE)
    add_subdirectory(lib/glm)
else ()
    find_package(glm REQUIRED)
endif ()

if (YAMLCPP_USE_SUBMODULE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    add_subdirectory(lib/yaml-cpp)
else ()
    find_package(yaml-cpp REQUIRED)
endif ()

if (GOOGLETEST_USE_SUBMODULE)
    if (WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif ()
    add_subdirectory(lib/googletest)
else ()
    find_package(googletest REQUIRED)
endif ()

if (SPDLOG_USE_SUBMODULE)
    add_subdirectory(lib/spdlog)
else ()
    find_package(spdlog REQUIRED)
endif ()

if (FMT_USE_SUBMODULE)
    add_subdirectory(lib/fmt)
else ()
    find_package(fmt REQUIRED)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(cubos-core PUBLIC glm::glm spdlog yaml-cpp fmt::fmt ${CMAKE_DL_LIBS})
target_link_libraries(cubos-core PRIVATE glad Threads::Threads)

# Add core tests

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS)
    include(CTest)
    enable_testing()

    include(GoogleTest)
    add_subdirectory(tests)
endif ()

# Add core samples
if (BUILD_CORE_SAMPLES)
    add_subdirectory(samples)
endif ()

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
endif ()
