# core/samples/CMakeLists.txt
# Core samples build configuration


# Macro used to reduce the boilerplate code
macro(make_sample)
    set(oneValueArgs DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(MAKE_SAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Get the target name from the sample directory path
    message(STATUS "Adding sample: ${MAKE_SAMPLE_DIR}")
    string(REPLACE "_" "-" target "${MAKE_SAMPLE_DIR}")
    string(REPLACE "/" "." target "${MAKE_SAMPLE_DIR}")
    set(target "core-sample.${target}")

    # Get the source files
    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/main.cpp")
    foreach(source IN LISTS MAKE_SAMPLE_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/${source}")
    endforeach()

    # Add the sample target
    add_executable(${target} ${sources})
    target_link_libraries(${target} cubos-core)
    cubos_common_target_options(${target})
endmacro()

# Set sample source files
set(CORE_SAMPLES_SOURCES
    "logging.cpp"
    "render_device.cpp"
    "render_device_compute.cpp"
    "debug_renderer.cpp"
    "serialization.cpp"
    "ecs.cpp"
    "ui.cpp"
    "event_system.cpp"
)

make_sample(DIR "data/embedded_archive" SOURCES "embed.cpp")

# For each sample file, create a target named after the file but without the
# file extension
foreach(file ${CORE_SAMPLES_SOURCES})
    # Remove file extension
    string(REGEX MATCH "^(.*)\\.[^.]*$" _ ${file})
    set(target "core-sample." ${CMAKE_MATCH_1})
    # Replace every _ with -
    string(REPLACE "_" "-" target ${target})
    # Replace every / with .
    string(REPLACE "/" "." target ${target})

    # Add sample target
    add_executable(${target} ${file})
    target_link_libraries(${target} cubos-core)
    cubos_common_target_options(${target})
endforeach()
