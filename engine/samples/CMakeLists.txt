# engine/samples/CMakeLists.txt
# Engine samples build configuration

include(CubinhosGenerate)

# Macro used to reduce the boilerplate code
macro(make_sample)
    set(options COMPONENTS ASSETS)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(MAKE_SAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_NAME}/main.cpp")
    foreach(source IN LISTS MAKE_SAMPLE_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_NAME}/${source}")
    endforeach()

    set(target "engine-sample.${MAKE_SAMPLE_NAME}")
    add_executable(${target} ${sources})
    target_link_libraries(${target} cubos-engine)
    cubos_common_target_options(${target})

    if(MAKE_SAMPLE_ASSETS)
        target_compile_definitions(${target} PUBLIC SAMPLE_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_NAME}/assets")
    endif()

    if(MAKE_SAMPLE_COMPONENTS)
        cubinhos_generate(${target} "${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_SAMPLE_NAME}/components")
    endif()
endmacro()

# Add samples
make_sample(NAME "empty")
make_sample(NAME "events")
make_sample(NAME "systems")
make_sample(NAME "assets" ASSETS)
#make_sample(NAME "renderer")
#make_sample(NAME "bloom")
#make_sample(NAME "ssao" ASSETS)
#make_sample(NAME "custom-asset" ASSETS)
#make_sample(NAME "scene" COMPONENTS ASSETS)
#make_sample(NAME "cars" COMPONENTS ASSETS)
