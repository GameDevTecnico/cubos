# core/CMakeLists.txt
# Cubos core build configuration

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)
if (WITH_GLFW)
    option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif ()
option(WITH_OPENAL "With OpenAL?" ON)
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(DOCTEST_USE_SUBMODULE "Compile doctest from source?" ON)
option(SPDLOG_USE_SUBMODULE "Compile SPDLOG from source?" ON)
option(FMT_USE_SUBMODULE "Compile FMT from source?" ON)

set(CUBOS_CORE_ECS_MAX_COMPONENTS "63" CACHE STRING "The maximum number of components registered in an ECS world.")
set(CUBOS_CORE_DISPATCHER_MAX_CONDITIONS "64" CACHE STRING "The maximum number of conditions available for the dispatcher.")

option(BUILD_CORE_SAMPLES "Build cubos core samples" OFF)
option(BUILD_CORE_TESTS "Build cubos core tests?" OFF)

message("# Building core samples: " ${BUILD_CORE_SAMPLES})
message("# Building core tests: " ${BUILD_CORE_TESTS})

# Set core source files

set(CUBOS_CORE_SOURCE
    "src/cubos/core/log.cpp"
    "src/cubos/core/thread_pool.cpp"

    "src/cubos/core/memory/stream.cpp"
    "src/cubos/core/memory/standard_stream.cpp"
    "src/cubos/core/memory/buffer_stream.cpp"

    "src/cubos/core/reflection/type.cpp"
    "src/cubos/core/reflection/traits/constructible.cpp"
    "src/cubos/core/reflection/traits/fields.cpp"
    "src/cubos/core/reflection/external/primitives.cpp"

    "src/cubos/core/data/serializer.cpp"
    "src/cubos/core/data/deserializer.cpp"
    "src/cubos/core/data/debug_serializer.cpp"
    "src/cubos/core/data/json_serializer.cpp"
    "src/cubos/core/data/json_deserializer.cpp"
    "src/cubos/core/data/binary_serializer.cpp"
    "src/cubos/core/data/binary_deserializer.cpp"
    "src/cubos/core/data/package.cpp"
    "src/cubos/core/data/fs/file.cpp"
    "src/cubos/core/data/fs/file_system.cpp"
    "src/cubos/core/data/fs/standard_archive.cpp"
    "src/cubos/core/data/fs/embedded_archive.cpp"
    "src/cubos/core/data/qb_parser.cpp"
    "src/cubos/core/data/context.cpp"

    "src/cubos/core/io/window.cpp"
    "src/cubos/core/io/cursor.cpp"
    "src/cubos/core/io/glfw_window.hpp"
    "src/cubos/core/io/glfw_window.cpp"
    "src/cubos/core/io/keyboard.cpp"
    "src/cubos/core/io/gamepad.cpp"

    "src/cubos/core/gl/debug.cpp"
    "src/cubos/core/gl/render_device.cpp"
    "src/cubos/core/gl/ogl_render_device.hpp"
    "src/cubos/core/gl/ogl_render_device.cpp"
    "src/cubos/core/gl/material.cpp"
    "src/cubos/core/gl/palette.cpp"
    "src/cubos/core/gl/grid.cpp"
    "src/cubos/core/gl/util.cpp"
    "src/cubos/core/gl/vertex.cpp"

    "src/cubos/core/al/audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.hpp"

    "src/cubos/core/ecs/entity_manager.cpp"
    "src/cubos/core/ecs/component_manager.cpp"
    "src/cubos/core/ecs/commands.cpp"
    "src/cubos/core/ecs/blueprint.cpp"
    "src/cubos/core/ecs/world.cpp"
    "src/cubos/core/ecs/system.cpp"
    "src/cubos/core/ecs/dispatcher.cpp"
    "src/cubos/core/ecs/registry.cpp"
)

# Create core library

add_library(cubos-core ${CUBOS_CORE_SOURCE})
target_include_directories(cubos-core PUBLIC "include" PRIVATE "src")
target_compile_definitions(cubos-core PUBLIC
    -DCUBOS_CORE_ECS_MAX_COMPONENTS=${CUBOS_CORE_ECS_MAX_COMPONENTS}
    -DCUBOS_CORE_DISPATCHER_MAX_CONDITIONS=${CUBOS_CORE_DISPATCHER_MAX_CONDITIONS}
)
cubos_common_target_options(cubos-core)

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

if (WITH_OPENAL)
    set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(lib/openal-soft)
    target_include_directories(cubos-core PRIVATE lib/openal-soft/include)
    target_link_libraries(cubos-core PRIVATE OpenAL)
    target_compile_definitions(cubos-core PRIVATE WITH_OPENAL)
endif()

if (GLM_USE_SUBMODULE)
    add_subdirectory(lib/glm)
else ()
    find_package(glm REQUIRED)
endif ()

if (DOCTEST_USE_SUBMODULE)
    add_subdirectory(lib/doctest)
else ()
    find_package(doctest REQUIRED)
endif ()

if (FMT_USE_SUBMODULE)
    add_subdirectory(lib/fmt)
else ()
    find_package(fmt REQUIRED)
endif ()

if (SPDLOG_USE_SUBMODULE)
    add_subdirectory(lib/spdlog)
else ()
    set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
    find_package(spdlog REQUIRED)
endif ()

add_subdirectory(lib/json)

add_subdirectory(lib/stduuid)
target_link_libraries(cubos-core PUBLIC stduuid)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_compile_definitions(cubos-core PUBLIC GLM_FORCE_SILENT_WARNINGS) # Needed for compilation to succeed on MSVC
target_link_libraries(cubos-core PUBLIC glm::glm spdlog fmt::fmt json ${CMAKE_DL_LIBS})
target_link_libraries(cubos-core PRIVATE glad Threads::Threads)

# Add core tests

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS)
    add_subdirectory(tests)
endif ()

# Add core samples
if (BUILD_CORE_SAMPLES)
    add_subdirectory(samples)
endif ()
