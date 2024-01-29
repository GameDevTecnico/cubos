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
    "src/cubos/core/memory/any_value.cpp"
    "src/cubos/core/memory/any_vector.cpp"

    "src/cubos/core/reflection/type.cpp"
    "src/cubos/core/reflection/type_registry.cpp"
    "src/cubos/core/reflection/traits/constructible.cpp"
    "src/cubos/core/reflection/traits/fields.cpp"
    "src/cubos/core/reflection/traits/array.cpp"
    "src/cubos/core/reflection/traits/dictionary.cpp"
    "src/cubos/core/reflection/traits/string_conversion.cpp"
    "src/cubos/core/reflection/traits/nullable.cpp"
    "src/cubos/core/reflection/traits/enum.cpp"
    "src/cubos/core/reflection/external/primitives.cpp"
    "src/cubos/core/reflection/external/cstring.cpp"
    "src/cubos/core/reflection/external/string.cpp"
    "src/cubos/core/reflection/external/string_view.cpp"
    "src/cubos/core/reflection/external/uuid.cpp"
    "src/cubos/core/reflection/external/glm.cpp"

    "src/cubos/core/data/old/serializer.cpp"
    "src/cubos/core/data/old/deserializer.cpp"
    "src/cubos/core/data/old/json_serializer.cpp"
    "src/cubos/core/data/old/json_deserializer.cpp"
    "src/cubos/core/data/old/binary_serializer.cpp"
    "src/cubos/core/data/old/binary_deserializer.cpp"
    "src/cubos/core/data/old/package.cpp"
    "src/cubos/core/data/old/context.cpp"

    "src/cubos/core/data/fs/file.cpp"
    "src/cubos/core/data/fs/file_system.cpp"
    "src/cubos/core/data/fs/standard_archive.cpp"
    "src/cubos/core/data/fs/embedded_archive.cpp"
    "src/cubos/core/data/ser/serializer.cpp"
    "src/cubos/core/data/ser/json.cpp"
    "src/cubos/core/data/ser/debug.cpp"
    "src/cubos/core/data/des/deserializer.cpp"
    "src/cubos/core/data/des/json.cpp"

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
    "src/cubos/core/gl/util.cpp"

    "src/cubos/core/al/audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.cpp"
    "src/cubos/core/al/oal_audio_device.hpp"

    "src/cubos/core/ecs/entity/entity.cpp"
    "src/cubos/core/ecs/entity/hash.cpp"
    "src/cubos/core/ecs/entity/manager.cpp"
    "src/cubos/core/ecs/entity/archetype_graph.cpp"
    "src/cubos/core/ecs/entity/archetype_id.cpp"
    "src/cubos/core/ecs/entity/pool.cpp"
    "src/cubos/core/ecs/table/column.cpp"
    "src/cubos/core/ecs/table/dense/table.cpp"
    "src/cubos/core/ecs/table/dense/registry.cpp"
    "src/cubos/core/ecs/table/sparse_relation/id.cpp"
    "src/cubos/core/ecs/table/sparse_relation/table.cpp"
    "src/cubos/core/ecs/table/sparse_relation/registry.cpp"
    "src/cubos/core/ecs/table/tables.cpp"
    "src/cubos/core/ecs/component/storage.cpp"
    "src/cubos/core/ecs/component/manager.cpp"
    "src/cubos/core/ecs/system/system.cpp"
    "src/cubos/core/ecs/system/dispatcher.cpp"
    "src/cubos/core/ecs/system/commands.cpp"
    "src/cubos/core/ecs/system/query.cpp"
    "src/cubos/core/ecs/query/term.cpp"
    "src/cubos/core/ecs/query/access.cpp"
    "src/cubos/core/ecs/query/data.cpp"
    "src/cubos/core/ecs/query/filter.cpp"
    "src/cubos/core/ecs/query/fetcher.cpp"
    "src/cubos/core/ecs/query/opt.cpp"
    "src/cubos/core/ecs/reflection.cpp"
    "src/cubos/core/ecs/blueprint.cpp"
    "src/cubos/core/ecs/world.cpp"
    "src/cubos/core/ecs/command_buffer.cpp"
    "src/cubos/core/ecs/types.cpp"
    "src/cubos/core/ecs/cubos.cpp"

    "src/cubos/core/geom/box.cpp"
    "src/cubos/core/geom/capsule.cpp"
)

# Create core library

add_library(cubos-core ${CUBOS_CORE_SOURCE})
target_include_directories(cubos-core PUBLIC "include")
target_compile_definitions(cubos-core PUBLIC
    -DCUBOS_CORE_ECS_MAX_COMPONENTS=${CUBOS_CORE_ECS_MAX_COMPONENTS}
    -DCUBOS_CORE_DISPATCHER_MAX_CONDITIONS=${CUBOS_CORE_DISPATCHER_MAX_CONDITIONS}
)
cubos_common_target_options(cubos-core)

# Link dependencies

if (WITH_OPENGL)
    set(GLAD_SOUURCES_DIR "lib/glad")
    add_subdirectory("${GLAD_SOUURCES_DIR}/cmake" glad_cmake SYSTEM)
    glad_add_library(glad REPRODUCIBLE API gl:core=3.3)
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
    add_subdirectory(lib/glm SYSTEM)
else ()
    find_package(glm REQUIRED)
endif ()

if (DOCTEST_USE_SUBMODULE)
    add_subdirectory(lib/doctest)
else ()
    find_package(doctest REQUIRED)
endif ()

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(lib/json)

add_subdirectory(lib/stduuid)
target_link_libraries(cubos-core PUBLIC stduuid)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_compile_definitions(cubos-core PUBLIC GLM_FORCE_SILENT_WARNINGS) # Needed for compilation to succeed on MSVC
target_link_libraries(cubos-core PUBLIC glm::glm nlohmann_json::nlohmann_json ${CMAKE_DL_LIBS})
target_link_libraries(cubos-core PRIVATE Threads::Threads)

# Add core tests

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS)
    add_subdirectory(tests)
endif ()

# Add core samples
if (BUILD_CORE_SAMPLES)
    add_subdirectory(samples)
endif ()
