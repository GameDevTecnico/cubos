# core/CMakeLists.txt
# Cubos core build configuration

option(WITH_GLFW "With GLFW?" ON)
option(WITH_OPENGL "With OpenGL?" ON)

if(WITH_GLFW)
	option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
endif()

option(WITH_OPENAL "With OpenAL?" OFF)
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)

set(CUBOS_CORE_ECS_MAX_COMPONENTS "63" CACHE STRING "The maximum number of components registered in an ECS world.")

option(BUILD_CORE_SAMPLES "Build cubos core samples" OFF)
option(BUILD_CORE_TESTS "Build cubos core tests?" OFF)
option(BUILD_CORE_SHARED "Build cubos core as shared library?" ON)

option(ENABLE_PROFILING "Should profiling macros be compiled?" ON)
set(CUBOS_METRIC_MAX_ENTRIES "1024" CACHE STRING "Maximum number of entries each metric can store")

message("# Building core samples: " ${BUILD_CORE_SAMPLES})
message("# Building core tests: " ${BUILD_CORE_TESTS})
message("# Building core as shared library: " ${BUILD_CORE_SHARED})

# Set core source files
set(CUBOS_CORE_SOURCE
	"src/api.cpp"
	"src/log.cpp"
	"src/metrics.cpp"

	"src/thread/pool.cpp"
	"src/thread/task.cpp"

	"src/memory/stream.cpp"
	"src/memory/standard_stream.cpp"
	"src/memory/buffer_stream.cpp"
	"src/memory/any_value.cpp"
	"src/memory/any_vector.cpp"
	"src/memory/opt.cpp"

	"src/reflection/reflect.cpp"
	"src/reflection/type.cpp"
	"src/reflection/comparison.cpp"
	"src/reflection/type_registry.cpp"
	"src/reflection/traits/constructible.cpp"
	"src/reflection/traits/fields.cpp"
	"src/reflection/traits/array.cpp"
	"src/reflection/traits/dictionary.cpp"
	"src/reflection/traits/string_conversion.cpp"
	"src/reflection/traits/nullable.cpp"
	"src/reflection/traits/enum.cpp"
	"src/reflection/traits/mask.cpp"
	"src/reflection/traits/inherits.cpp"
	"src/reflection/external/primitives.cpp"
	"src/reflection/external/cstring.cpp"
	"src/reflection/external/string.cpp"
	"src/reflection/external/string_view.cpp"
	"src/reflection/external/uuid.cpp"
	"src/reflection/external/glm.cpp"

	"src/data/fs/file.cpp"
	"src/data/fs/file_system.cpp"
	"src/data/fs/standard_archive.cpp"
	"src/data/fs/embedded_archive.cpp"
	"src/data/ser/serializer.cpp"
	"src/data/ser/json.cpp"
	"src/data/ser/debug.cpp"
	"src/data/des/deserializer.cpp"
	"src/data/des/json.cpp"

	"src/io/window.cpp"
	"src/io/cursor.cpp"
	"src/io/glfw_window.hpp"
	"src/io/glfw_window.cpp"
	"src/io/keyboard.cpp"
	"src/io/gamepad.cpp"

	"src/gl/render_device.cpp"
	"src/gl/ogl_render_device.hpp"
	"src/gl/ogl_render_device.cpp"
	"src/gl/util.cpp"

	"src/al/audio_device.cpp"
	"src/al/oal_audio_device.cpp"
	"src/al/oal_audio_device.hpp"

	"src/ecs/entity/entity.cpp"
	"src/ecs/entity/hash.cpp"
	"src/ecs/entity/archetype_graph.cpp"
	"src/ecs/entity/archetype_id.cpp"
	"src/ecs/entity/pool.cpp"

	"src/ecs/table/column.cpp"
	"src/ecs/table/dense/table.cpp"
	"src/ecs/table/dense/registry.cpp"
	"src/ecs/table/sparse_relation/id.cpp"
	"src/ecs/table/sparse_relation/table.cpp"
	"src/ecs/table/sparse_relation/registry.cpp"
	"src/ecs/table/tables.cpp"

	"src/ecs/system/access.cpp"
	"src/ecs/system/schedule.cpp"
	"src/ecs/system/registry.cpp"
	"src/ecs/system/planner.cpp"
	"src/ecs/system/tag.cpp"
	"src/ecs/system/system.cpp"
	"src/ecs/system/options.cpp"
	"src/ecs/system/arguments/commands.cpp"
	"src/ecs/system/arguments/plugins.cpp"
	"src/ecs/system/arguments/query.cpp"
	"src/ecs/system/arguments/resources.cpp"
	"src/ecs/system/arguments/world.cpp"

	"src/ecs/observer/id.cpp"
	"src/ecs/observer/observers.cpp"

	"src/ecs/query/term.cpp"
	"src/ecs/query/data.cpp"
	"src/ecs/query/filter.cpp"
	"src/ecs/query/fetcher.cpp"
	"src/ecs/query/node/node.cpp"
	"src/ecs/query/node/archetype.cpp"
	"src/ecs/query/node/related.cpp"

	"src/ecs/reflection.cpp"
	"src/ecs/blueprint.cpp"
    "src/ecs/name.cpp"
	"src/ecs/world.cpp"
	"src/ecs/command_buffer.cpp"
	"src/ecs/plugin_queue.cpp"
	"src/ecs/types.cpp"
	"src/ecs/cubos.cpp"
	"src/ecs/dynamic.cpp"

    "src/geom/box.cpp"
    "src/geom/capsule.cpp"
    "src/geom/intersections.cpp"
	"src/geom/plane.cpp"

    "src/net/address.cpp"
    "src/net/tcp_stream.cpp"
    "src/net/tcp_listener.cpp"
    "src/net/utils.cpp"
    "src/net/udp_socket.cpp"
)

# Create core library
if(BUILD_CORE_SHARED)
	add_library(cubos-core SHARED ${CUBOS_CORE_SOURCE})
	target_compile_definitions(cubos-core
		PRIVATE -DCUBOS_CORE_EXPORT
		PUBLIC -DCUBOS_CORE_IMPORT
	)
else()
	add_library(cubos-core STATIC ${CUBOS_CORE_SOURCE})
endif()
target_include_directories(cubos-core PUBLIC "include")
target_compile_definitions(cubos-core PUBLIC
	-DCUBOS_CORE_ECS_MAX_COMPONENTS=${CUBOS_CORE_ECS_MAX_COMPONENTS}
)
cubos_common_target_options(cubos-core)

# Link dependencies
if(WITH_OPENGL)
	set(GLAD_SOUURCES_DIR "lib/glad")
	add_subdirectory("${GLAD_SOUURCES_DIR}/cmake" glad_cmake SYSTEM)
	glad_add_library(glad REPRODUCIBLE API gl:core=3.3)
	if(BUILD_CORE_SHARED)
		set_property(TARGET glad PROPERTY POSITION_INDEPENDENT_CODE ON)
	endif()
	target_link_libraries(cubos-core PRIVATE glad)
	target_compile_definitions(cubos-core PRIVATE WITH_OPENGL GLAD_GLAPI_EXPORT GLAD_GLAPI_EXPORT_BUILD)
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

if(WITH_OPENAL)
	set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
	set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "" FORCE)
	set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory(lib/openal-soft)
	target_include_directories(cubos-core PRIVATE lib/openal-soft/include)
	target_link_libraries(cubos-core PRIVATE OpenAL)
	target_compile_definitions(cubos-core PRIVATE WITH_OPENAL)
endif()

if(GLM_USE_SUBMODULE)
	add_subdirectory(lib/glm SYSTEM)
else()
	find_package(glm REQUIRED)
endif()

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
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_CORE_TESTS)
	add_subdirectory(tests)
endif()

# Add core samples
if(BUILD_CORE_SAMPLES)
	add_subdirectory(samples)
endif()

if(ENABLE_PROFILING)
	target_compile_definitions(cubos-core
		PUBLIC 
		-DCUBOS_PROFILING=${ENABLE_PROFILING}
		-DCUBOS_METRIC_MAX_ENTRIES=${CUBOS_METRIC_MAX_ENTRIES}
	)
endif()
