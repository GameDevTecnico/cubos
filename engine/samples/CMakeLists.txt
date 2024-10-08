# engine/samples/CMakeLists.txt
# Engine samples build configuration

# Macro used to reduce the boilerplate code
macro(make_sample)
  set(options COMPONENTS ASSETS)
  set(oneValueArgs DIR)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(MAKE_SAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # Get the target name from the sample directory path
  message(STATUS "Adding sample: ${MAKE_SAMPLE_DIR}")
  string(REPLACE "/" "." target "${MAKE_SAMPLE_DIR}")
  set(target "engine-sample.${target}")

  # Get the source files
  set(sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/main.cpp")

  foreach(source IN LISTS MAKE_SAMPLE_SOURCES)
    list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/${source}")
  endforeach()

  # Add the sample target
  add_executable(${target} ${sources})
  target_link_libraries(${target} cubos-engine)
  cubos_common_target_options(${target})

  if(MAKE_SAMPLE_ASSETS)
    target_compile_definitions(${target} PUBLIC SAMPLE_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/assets")
  endif()
endmacro()

# Add samples
make_sample(DIR "hello-cubos")
make_sample(DIR "settings")
make_sample(DIR "events")
make_sample(DIR "input" ASSETS)
make_sample(DIR "assets/bridge" ASSETS)
make_sample(DIR "assets/json" ASSETS)
make_sample(DIR "assets/saving" ASSETS)
make_sample(DIR "render/main" ASSETS)
make_sample(DIR "render/shadows" ASSETS)
make_sample(DIR "imgui")
make_sample(DIR "collisions" ASSETS)
make_sample(DIR "scene" ASSETS)
make_sample(DIR "voxels" ASSETS)
make_sample(DIR "gizmos")
make_sample(DIR "ui" ASSETS)
make_sample(DIR "physics" ASSETS)
make_sample(DIR "complex_physics" ASSETS)
make_sample(DIR "games/cubosurfers" ASSETS SOURCES "spawner.cpp" "obstacle.cpp" "player.cpp")
