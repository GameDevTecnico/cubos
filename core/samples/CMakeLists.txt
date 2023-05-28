# core/samples/CMakeLists.txt
# Core samples build configuration

# Set sample source files
set(CORE_SAMPLES_SOURCES
    "logging.cpp"
    "render_device.cpp"
    "render_device_compute.cpp"
    "debug_renderer.cpp"
    "serialization.cpp"
    "embedded_archive.cpp"
    "ecs.cpp"
    "ui.cpp"
    "event_system.cpp"
)

# For each sample file, create a target named after the file but without the
# file extension
foreach(file ${CORE_SAMPLES_SOURCES})
    # Remove file extension
    string(REGEX MATCH "^(.*)\\.[^.]*$" _ ${file})
    set(target "core-sample." ${CMAKE_MATCH_1})
    # Replace every _ with -
    string(REPLACE "_" "-" target ${target})

    # Add sample target
    add_executable(${target} ${file})
    target_link_libraries(${target} cubos-core)
    cubos_common_target_options(${target})
endforeach()
