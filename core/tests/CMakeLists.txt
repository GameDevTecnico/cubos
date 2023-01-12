# core/tests/CMakeLists.txt
# Core tests build configuration

# Set test sources
set(CUBOS_TESTS_SOURCE
    "test_settings.cpp"
    "test_thread_pool.cpp"
    "test_buffer_stream.cpp"
    "test_yaml_serialization.cpp"
    "test_yaml_deserialization.cpp"
    "test_yaml_serialization_and_deserialization.cpp"
    "test_package.cpp"
    "test_json_serialization.cpp"
    "test_json_deserialization.cpp"
    "test_std_archive.cpp"
)

# Add tests target
add_executable(cubos-core-tests ${CUBOS_TESTS_SOURCE})
target_link_libraries(cubos-core-tests cubos-core gtest_main)
set_property(TARGET cubos-core-tests PROPERTY CXX_STANDARD 20)
gtest_add_tests(TARGET cubos-core-tests)
