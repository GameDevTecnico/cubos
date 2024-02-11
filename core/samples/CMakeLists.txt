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

make_sample(DIR "logging")
make_sample(DIR "reflection/basic")
make_sample(DIR "reflection/traits/constructible")
make_sample(DIR "reflection/traits/fields")
make_sample(DIR "reflection/traits/array")
make_sample(DIR "reflection/traits/dictionary")
make_sample(DIR "reflection/traits/string_conversion")
make_sample(DIR "reflection/traits/nullable")
make_sample(DIR "reflection/traits/enum")
make_sample(DIR "reflection/traits/mask")
make_sample(DIR "data/fs/embedded_archive" SOURCES "embed.cpp")
make_sample(DIR "data/fs/standard_archive")
make_sample(DIR "data/ser/custom")
make_sample(DIR "data/ser/json")
make_sample(DIR "data/des/custom")
make_sample(DIR "data/serialization")
make_sample(DIR "ecs/events")
make_sample(DIR "ecs/relations")
make_sample(DIR "gl/compute")
make_sample(DIR "gl/debug_renderer")
make_sample(DIR "gl/quad")
