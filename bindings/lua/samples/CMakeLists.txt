# bindings/lua/samples/CMakeLists.txt
# Lua bindings samples build configuration

# Function used to reduce the boilerplate code
# Function used to reduce the boilerplate code
function(make_sample)
    set(options COMPONENTS ASSETS NO_WEB)
    set(oneValueArgs DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(MAKE_SAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Skip the sample if we're on Emscripten and it is not supported on Emscripten
    if(EMSCRIPTEN AND MAKE_SAMPLE_NO_WEB)
        return()
    endif()

    # Get the target name from the sample directory path
    message(STATUS "Adding sample: ${MAKE_SAMPLE_DIR}")
    string(REPLACE "/" "." target "${MAKE_SAMPLE_DIR}")
    set(target "bindings-lua-sample.${target}")

    # Get the source files
    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/main.cpp")

    foreach(source IN LISTS MAKE_SAMPLE_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/${source}")
    endforeach()

    # Add the sample target
    add_executable(${target} ${sources})
    target_link_libraries(${target} cubos::engine cubos-bindings-lua)
    cubos_common_target_options(${target})

    if(MAKE_SAMPLE_ASSETS)
      if(EMSCRIPTEN)
        target_link_options(${target} PRIVATE
          "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/scripts@/scripts"
          "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
        )
        target_compile_definitions(${target} PRIVATE
            BUILTIN_ASSETS_PATH="/builtin"
            APP_SCRIPTS_PATH="/scripts"
        )
      else()
        target_compile_definitions(${target} PRIVATE
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
            APP_SCRIPTS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_DIR}/scripts"
        )
      endif()
    endif()
endfunction()

# Add samples
make_sample(DIR "hello_world" ASSETS)
