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
    "src/cubos/core/settings.cpp"
    "src/cubos/core/thread_pool.cpp"

    "src/cubos/core/reflection/type.cpp"
    "src/cubos/core/reflection/hints.cpp"
    "src/cubos/core/reflection/primitive.cpp"
    "src/cubos/core/reflection/object.cpp"
    "src/cubos/core/reflection/variant.cpp"
    "src/cubos/core/reflection/array.cpp"
    "src/cubos/core/reflection/dictionary.cpp"
    "src/cubos/core/reflection/external/primitives.cpp"
    "src/cubos/core/reflection/external/string.cpp"
    "src/cubos/core/reflection/external/glm.cpp"

    "src/cubos/core/memory/stream.cpp"
    "src/cubos/core/memory/standard_stream.cpp"
    "src/cubos/core/memory/buffer_stream.cpp"

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

    "src/cubos/core/al/audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.hpp"

    "src/cubos/core/ui/imgui.cpp"
    "src/cubos/core/ui/serialization.cpp"
    "src/cubos/core/ui/ecs.cpp"

    "src/cubos/core/ecs/entity_manager.cpp"
    "src/cubos/core/ecs/component_manager.cpp"
    "src/cubos/core/ecs/commands.cpp"
    "src/cubos/core/ecs/blueprint.cpp"
    "src/cubos/core/ecs/world.cpp"
    "src/cubos/core/ecs/system.cpp"
    "src/cubos/core/ecs/dispatcher.cpp"
    "src/cubos/core/ecs/registry.cpp"
)

set(CUBOS_CORE_INCLUDE
    "include/cubos/core/log.hpp"
    "include/cubos/core/settings.hpp"
    "include/cubos/core/thread_pool.hpp"

    "include/cubos/core/reflection/reflect.hpp"
    "include/cubos/core/reflection/type.hpp"
    "include/cubos/core/reflection/type_map.hpp"
    "include/cubos/core/reflection/hints.hpp"
    "include/cubos/core/reflection/visitor.hpp"
    "include/cubos/core/reflection/primitive.hpp"
    "include/cubos/core/reflection/object.hpp"
    "include/cubos/core/reflection/variant.hpp"
    "include/cubos/core/reflection/array.hpp"
    "include/cubos/core/reflection/dictionary.hpp"
    "include/cubos/core/reflection/external/primitives.hpp"
    "include/cubos/core/reflection/external/string.hpp"
    "include/cubos/core/reflection/external/variant.hpp"
    "include/cubos/core/reflection/external/vector.hpp"
    "include/cubos/core/reflection/external/unordered_map.hpp"
    "include/cubos/core/reflection/external/glm.hpp"

    "include/cubos/core/memory/stream.hpp"
    "include/cubos/core/memory/standard_stream.hpp"
    "include/cubos/core/memory/buffer_stream.hpp"
    "include/cubos/core/memory/endianness.hpp"
    "include/cubos/core/memory/type_map.hpp"
    "include/cubos/core/memory/guards.hpp"

    "include/cubos/core/data/serializer.hpp"
    "include/cubos/core/data/deserializer.hpp"
    "include/cubos/core/data/debug_serializer.hpp"
    "include/cubos/core/data/json_serializer.hpp"
    "include/cubos/core/data/json_deserializer.hpp"
    "include/cubos/core/data/binary_serializer.hpp"
    "include/cubos/core/data/binary_deserializer.hpp"
    "include/cubos/core/data/serialization_map.hpp"
    "include/cubos/core/data/package.hpp"
    "include/cubos/core/data/fs/file.hpp"
    "include/cubos/core/data/fs/file_stream.hpp"
    "include/cubos/core/data/fs/file_system.hpp"
    "include/cubos/core/data/fs/archive.hpp"
    "include/cubos/core/data/fs/standard_archive.hpp"
    "include/cubos/core/data/fs/embedded_archive.hpp"
    "include/cubos/core/data/qb_parser.hpp"
    "include/cubos/core/data/context.hpp"

    "include/cubos/core/io/window.hpp"
    "include/cubos/core/io/keyboard.hpp"
    "include/cubos/core/io/cursor.hpp"

    "include/cubos/core/geom/box.hpp"
    "include/cubos/core/geom/capsule.hpp"
    "include/cubos/core/geom/simplex.hpp"
    "include/cubos/core/geom/plane.hpp"
    
    "include/cubos/core/gl/debug.hpp"
    "include/cubos/core/gl/render_device.hpp"
    "include/cubos/core/gl/material.hpp"
    "include/cubos/core/gl/palette.hpp"
    "include/cubos/core/gl/grid.hpp"
    "include/cubos/core/gl/vertex.hpp"
    "include/cubos/core/gl/camera.hpp"
    "include/cubos/core/gl/light.hpp"
    "include/cubos/core/gl/util.hpp"

    "include/cubos/core/al/audio_device.hpp"

    "include/cubos/core/ui/imgui.hpp"
    "include/cubos/core/ui/serialization.hpp"
    "include/cubos/core/ui/ecs.hpp"

    "include/cubos/core/ecs/entity_manager.hpp"
    "include/cubos/core/ecs/resource_manager.hpp"
    "include/cubos/core/ecs/component_manager.hpp"
    "include/cubos/core/ecs/blueprint.hpp"
    "include/cubos/core/ecs/commands.hpp"
    "include/cubos/core/ecs/storage.hpp"
    "include/cubos/core/ecs/vec_storage.hpp"
    "include/cubos/core/ecs/map_storage.hpp"
    "include/cubos/core/ecs/null_storage.hpp"
    "include/cubos/core/ecs/world.hpp"
    "include/cubos/core/ecs/query.hpp"
    "include/cubos/core/ecs/system.hpp"
    "include/cubos/core/ecs/registry.hpp"
    "include/cubos/core/ecs/dispatcher.hpp"
    "include/cubos/core/ecs/event_pipe.hpp"
    "include/cubos/core/ecs/event_reader.hpp"
    "include/cubos/core/ecs/event_writer.hpp"
    "include/cubos/core/ecs/accessors.hpp"
)

# Create core library

add_library(cubos-core ${CUBOS_CORE_SOURCE} ${CUBOS_CORE_INCLUDE})
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

add_library(imgui STATIC
    "lib/imgui/imgui.cpp"
    "lib/imgui/imgui_draw.cpp"
    "lib/imgui/imgui_tables.cpp"
    "lib/imgui/imgui_widgets.cpp"
    "lib/imgui/imgui_demo.cpp"
)

target_include_directories(imgui PUBLIC "lib/imgui")

add_subdirectory(lib/stduuid)
target_link_libraries(cubos-core PUBLIC stduuid)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_compile_definitions(cubos-core PUBLIC GLM_FORCE_SILENT_WARNINGS) # Needed for compilation to succeed on MSVC
target_link_libraries(cubos-core PUBLIC glm::glm spdlog imgui fmt::fmt json ${CMAKE_DL_LIBS})
target_link_libraries(cubos-core PRIVATE glad Threads::Threads)

# Add core tests

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS)
    add_subdirectory(tests)
endif ()

# Add core samples
if (BUILD_CORE_SAMPLES)
    add_subdirectory(samples)
endif ()
