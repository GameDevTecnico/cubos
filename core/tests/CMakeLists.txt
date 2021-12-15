# core/tests/CMakeLists.txt
# Core tests build configuration

# Set test sources
set(CUBOS_TESTS_SOURCE
    "test_settings.cpp"
    "test_buffer_stream.cpp"
    "test_yaml_serialization.cpp"
)

# Add tests target
add_executable(cubos-core-tests ${CUBOS_TESTS_SOURCE})
target_link_libraries(cubos-core-tests cubos-core gtest_main)
set_property(TARGET cubos-core-tests PROPERTY CXX_STANDARD 20)
gtest_add_tests(TARGET cubos-core-tests)
