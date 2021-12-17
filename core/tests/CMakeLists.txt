# core/tests/CMakeLists.txt
# Core tests build configuration

# Set test sources
set(CUBOS_TESTS_SOURCE
    "test_settings.cpp"
    "test_buffer_stream.cpp"
)

# Add tests target
add_executable(cubos-core-tests ${CUBOS_TESTS_SOURCE})
target_link_libraries(cubos-core-tests cubos-core gtest_main)
gtest_add_tests(TARGET cubos-core-tests)
