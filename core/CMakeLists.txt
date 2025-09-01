# core/CMakeLists.txt
# Cubos core build configuration

# ----------------------- Declare core library options ------------------------

option(CUBOS_CORE_SAMPLES "Build cubos::core samples" ${PROJECT_IS_TOP_LEVEL})
option(CUBOS_CORE_TESTS "Build cubos::core tests?" ${PROJECT_IS_TOP_LEVEL})
option(CUBOS_CORE_SHARED "Build cubos::core as shared library?" ON)

option(CUBOS_CORE_GLFW "Build cubos::core with GLFW support?" ON)
option(CUBOS_CORE_OPENGL "Build cubos::core with OpenGL support?" ON)
option(CUBOS_CORE_MINIAUDIO "Build cubos::core with MiniAudio support?" ON)

option(CUBOS_CORE_PROFILING "Should cubos::core profiling macros be compiled?" ON)
set(CUBOS_CORE_METRIC_MAX_ENTRIES "1024" CACHE STRING "Maximum number of entries each cubos::core metric can store")

message("# Building cubos::core samples: " ${CUBOS_CORE_SAMPLES})
message("# Building cubos::core tests: " ${CUBOS_CORE_TESTS})
message("# Building cubos::core as shared library: " ${CUBOS_CORE_SHARED})

if (EMSCRIPTEN AND CUBOS_CORE_SHARED)
	message(WARNING "Cannot build core as shared library on Emscripten")
	set(CUBOS_CORE_SHARED OFF CACHE BOOL "Build cubos::core as shared library?" FORCE)
endif()

# --------------------- Define core library source files ----------------------

set(CUBOS_CORE_SOURCE
	"src/api.cpp"

	"src/tel/logging.cpp"
	"src/tel/metrics.cpp"
	"src/tel/tracing.cpp"
	"src/tel/level.cpp"

	"src/thread/pool.cpp"
	"src/thread/process.cpp"
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
	"src/reflection/type_client_server_defaults.cpp"
	"src/reflection/type_client.cpp"
	"src/reflection/type_server.cpp"
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
	"src/reflection/traits/vector.cpp"
	"src/reflection/traits/wrapper.cpp"
	"src/reflection/traits/constant.cpp"
    "src/reflection/traits/hidden.cpp"
	"src/reflection/external/primitives.cpp"
	"src/reflection/external/const.cpp"
	"src/reflection/external/carray.cpp"
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
	"src/data/ser/binary.cpp"
	"src/data/des/deserializer.cpp"
	"src/data/des/json.cpp"
	"src/data/des/binary.cpp"
    "src/data/transparent/string_equal.cpp"
    "src/data/transparent/string_hash.cpp"

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

	"src/al/audio_context.cpp"
	"src/al/miniaudio_context.cpp"

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
	"src/ecs/debugger.cpp"

    "src/geom/box.cpp"
    "src/geom/capsule.cpp"
	"src/geom/frustum.cpp"
    "src/geom/intersections.cpp"
	"src/geom/plane.cpp"
	"src/geom/utils.cpp"

    "src/net/address.cpp"
    "src/net/tcp_stream.cpp"
    "src/net/tcp_listener.cpp"
    "src/net/utils.cpp"
    "src/net/udp_socket.cpp"
)

# ----------------------- Configure core library target -----------------------

if(CUBOS_CORE_SHARED)
	add_library(cubos-core SHARED ${CUBOS_CORE_SOURCE})
	target_compile_definitions(cubos-core
		PRIVATE -DCUBOS_CORE_EXPORT # When compiling library source files, we export symbols
		PUBLIC -DCUBOS_CORE_IMPORT  # When compiling client source files, we import symbols
	)
else()
	add_library(cubos-core STATIC ${CUBOS_CORE_SOURCE})
endif()
add_library(cubos::core ALIAS cubos-core) # Alias so that we can use cubos::core inside the build tree
set_target_properties(cubos-core PROPERTIES EXPORT_NAME core) # Will be within the cubos namespace when exported
cubos_common_target_options(cubos-core)

target_include_directories(cubos-core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/cubos-core> # On installation, the include tree will have been copied here 
)

# Configure profiling and metrics
if(CUBOS_CORE_PROFILING)
	target_compile_definitions(cubos-core PUBLIC -DCUBOS_CORE_PROFILING)
endif()
target_compile_options(cubos-core PRIVATE -DCUBOS_CORE_METRIC_MAX_ENTRIES=${CUBOS_CORE_METRIC_MAX_ENTRIES})

# Link small libraries
target_link_libraries(cubos-core PUBLIC ${CMAKE_DL_LIBS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
if(NOT EMSCRIPTEN)
	find_package(Threads REQUIRED)
	target_link_libraries(cubos-core PRIVATE Threads::Threads)
endif()

# ------------- Fetch and link core private library dependencies --------------

include(FetchContent)

if(NOT EMSCRIPTEN)
	# Fetch and link cpptrace - only available (and needed) on non-Emscripten platforms
	FetchContent_Declare(
		cpptrace
		GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
		GIT_TAG        v0.7.1
		SYSTEM
		FIND_PACKAGE_ARGS
	)
	FetchContent_MakeAvailable(cpptrace)
	target_link_libraries(cubos-core PRIVATE cpptrace::cpptrace)
endif()

# If OpenGL support is enabled, fetch and link glad
if(CUBOS_CORE_OPENGL)
	if(EMSCRIPTEN)
		# Unless we're on Emscripten, which statically links to OpenGL for us, and is thus more efficient
		# than using glad
		target_link_libraries(cubos-core PRIVATE GL)
		target_link_options(cubos-core PUBLIC -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2)
	else()
		FetchContent_Declare(
			glad
			GIT_REPOSITORY https://github.com/GameDevTecnico/cubos-glad.git
			GIT_TAG        bdf4d66072830f9822512f8f7dc3ca3fd72d9f69
			SYSTEM
		)
		if(CUBOS_CORE_SHARED)
			set(GLAD_INSTALL OFF CACHE BOOL "Don't install glad" FORCE)
		else()
			set(GLAD_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install glad" FORCE)
		endif()
		FetchContent_MakeAvailable(glad)
		if(CUBOS_CORE_SHARED)
			set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)
		endif()
		target_link_libraries(cubos-core PRIVATE glad)
		target_compile_definitions(cubos-core PRIVATE GLAD_GLAPI_EXPORT GLAD_GLAPI_EXPORT_BUILD)
	endif()
	target_compile_definitions(cubos-core PRIVATE CUBOS_CORE_OPENGL)
endif()

# If GLFW support is enabled, fetch and link it
if(CUBOS_CORE_GLFW)
	if(EMSCRIPTEN)
		# Emscripten ships its own GLFW
		# We use the contrib port of GLFW because it supports gamepad input and some other features
        target_link_options(cubos-core PUBLIC --use-port=contrib.glfw3)
	else()
		FetchContent_Declare(
			glfw
			GIT_REPOSITORY https://github.com/glfw/glfw.git
			GIT_TAG        3.3.8
			SYSTEM 
			FIND_PACKAGE_ARGS NAMES glfw3
		)

		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples" FORCE)
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests" FORCE)
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW documentation" FORCE)
		if(CUBOS_CORE_SHARED)
			set(GLFW_INSTALL OFF CACHE BOOL "Don't install GLFW" FORCE)
		else()
			set(GLFW_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Don't install GLFW" FORCE)
		endif()
		set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build GLFW as a static library" FORCE)
		FetchContent_MakeAvailable(glfw)
	endif()

	target_link_libraries(cubos-core PRIVATE glfw)
	target_compile_definitions(cubos-core PRIVATE CUBOS_CORE_GLFW)
endif()

# If MiniAudio support is enabled, fetch and link it
if(CUBOS_CORE_MINIAUDIO)
	FetchContent_Declare(
		miniaudio
		GIT_REPOSITORY https://github.com/mackron/miniaudio
		GIT_TAG        0.11.21
		SYSTEM
	)
	FetchContent_MakeAvailable(miniaudio)

	# We need to define our own miniaudio target as the repository doesn't have its own CMakeLists.txt
	add_library(miniaudio INTERFACE)
	target_include_directories(miniaudio SYSTEM INTERFACE ${miniaudio_SOURCE_DIR})

	# Link miniaudio
	target_link_libraries(cubos-core PRIVATE $<BUILD_INTERFACE:miniaudio>)

	# Define the CUBOS_CORE_MINIAUDIO to toggle MiniAudio support
	target_compile_definitions(cubos-core PRIVATE CUBOS_CORE_MINIAUDIO)

	if(EMSCRIPTEN)
		target_link_options(cubos-core PUBLIC -sEXPORTED_RUNTIME_METHODS=HEAPF32)
	endif()
endif()

# -------------- Fetch and link core public library dependencies --------------

# Fetch and link stduuid
FetchContent_Declare(
	stduuid
	GIT_REPOSITORY https://github.com/mariusbancila/stduuid.git
	GIT_TAG        v1.2.3
	SYSTEM
	FIND_PACKAGE_ARGS
)
set(UUID_ENABLE_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install stduuid" FORCE)
FetchContent_MakeAvailable(stduuid)
target_link_libraries(cubos-core PUBLIC stduuid)

# Fetch and link glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/GameDevTecnico/cubos-glm.git
	GIT_TAG        c1c3790818795f9ebb9da9049830c1cc0ba8a721
	SYSTEM
	FIND_PACKAGE_ARGS
)
set(GLM_BUILD_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install glm" FORCE)
FetchContent_MakeAvailable(glm)
target_compile_definitions(cubos-core
	PUBLIC
		GLM_FORCE_SILENT_WARNINGS # Needed for compilation to succeed on MSVC
		GLM_ENABLE_EXPERIMENTAL   # We use some experimental features
) 
target_link_libraries(cubos-core PUBLIC glm::glm)

# Fetch and link nlohmann json
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG        v3.11.3
	SYSTEM
	FIND_PACKAGE_ARGS NAMES nlohmann_json
)
set(JSON_BuildTests OFF CACHE BOOL "Don't build json tests" FORCE)
set(JSON_Install ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install nlohmann json" FORCE)
FetchContent_MakeAvailable(json)
target_link_libraries(cubos-core PUBLIC nlohmann_json::nlohmann_json)

# ------------------------ Configure tests and samples ------------------------

if(CUBOS_CORE_TESTS)
	add_subdirectory(tests)
endif()

if(CUBOS_CORE_SAMPLES)
	add_subdirectory(samples)
endif()

# -------------------- Configure core library installation --------------------

if(CUBOS_ENABLE_INSTALL)
	# Install core target
    install(TARGETS cubos-core
        EXPORT cubos-core-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cubos-core)
endif()
