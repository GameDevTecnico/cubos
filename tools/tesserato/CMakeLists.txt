# tools/tesserato/CMakeLists.txt
# Tesserato build configuration

set(TESSERATO_SOURCE
    "src/main.cpp"
)

add_executable(tesserato ${TESSERATO_SOURCE})
set_property(TARGET tesserato PROPERTY CXX_STANDARD 20)
target_compile_features(tesserato PUBLIC cxx_std_20)
target_link_libraries(tesserato PUBLIC cubos-core)
target_link_libraries(tesserato PUBLIC cubos-core cubos-engine)

