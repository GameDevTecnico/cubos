# tools/tesserato/CMakeLists.txt
# Tesserato build configuration

set(TESSERATO_SOURCE
    "src/main.cpp"
)

add_executable(tesserato ${TESSERATO_SOURCE})
target_link_libraries(tesserato PUBLIC cubos-core cubos-engine)
cubos_common_target_options(tesserato)

target_compile_definitions(tesserato PUBLIC TESSERATO_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
