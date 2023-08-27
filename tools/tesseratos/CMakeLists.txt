# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

set(TESSERATOS_SOURCE
    "src/main.cpp"
)

add_executable(tesseratos ${TESSERATOS_SOURCE})
target_link_libraries(tesseratos PUBLIC cubos-core cubos-engine)
cubos_common_target_options(tesseratos)

target_compile_definitions(tesseratos PUBLIC TESSERATOS_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
