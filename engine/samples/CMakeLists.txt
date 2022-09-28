# engine/samples/CMakeLists.txt
# Engine samples build configuration

# Set sample source files
set(ENGINE_SAMPLES_SOURCES
    "asset.cpp"
    "empty.cpp"
    "renderer.cpp"
    "asset.cpp"
    "cars.cpp"
    "asset_renderer.cpp"
    "scene.cpp"
    "bloom.cpp"
)

# For each sample file, create a target named after the file but without the
# file extension
foreach (file ${ENGINE_SAMPLES_SOURCES})
    # Remove file extension
    string(REGEX MATCH "^(.*)\\.[^.]*$" _ ${file})
    set(target "engine-sample." ${CMAKE_MATCH_1})
    # Replace every _ with -
    string(REPLACE "_" "-" target ${target})

    # Add sample target
    add_executable(${target} ${file})
    target_compile_definitions(${target} PUBLIC SAMPLE_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
    target_link_libraries(${target} cubos-engine)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 20)
    target_compile_features(${target} PUBLIC cxx_std_20)
endforeach ()
