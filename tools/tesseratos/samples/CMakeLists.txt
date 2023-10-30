# tesseratos/samples/CMakeLists.txt
# Tesseratos samples build configuration

include(QuadradosGenerate)

# Macro used to reduce the boilerplate code
macro(make_sample)
    set(options COMPONENTS ASSETS)
    set(oneValueArgs DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(MAKE_SAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Get the target name from the sample directory path
    message(STATUS "Adding sample: ${MAKE_SAMPLE_DIR}")
    string(REPLACE "/" "." target "${MAKE_SAMPLE_DIR}")
    set(target "tesseratos-sample.${target}")

    # Get the source files
    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/main.cpp")

    foreach(source IN LISTS MAKE_SAMPLE_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/${source}")
    endforeach()

    # Add the sample target
    add_executable(${target} ${sources})
    target_link_libraries(${target} tesseratos)
    cubos_common_target_options(${target})

    if(MAKE_SAMPLE_ASSETS)
        target_compile_definitions(${target} PUBLIC SAMPLE_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/assets")
    endif()

    if(MAKE_SAMPLE_COMPONENTS)
        quadrados_generate(${target} "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}")
    endif()
endmacro()

# Add samples
make_sample(DIR "main" COMPONENTS ASSETS)
