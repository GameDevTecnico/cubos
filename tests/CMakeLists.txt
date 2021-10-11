set(CUBOS_TESTS_SOURCE
    "test_example.cpp"
)

add_executable(cubos_tests ${CUBOS_TESTS_SOURCE})

target_link_libraries(
    cubos_tests 
    cubos_lib
    gtest_main
)

gtest_add_tests(TARGET cubos_tests)