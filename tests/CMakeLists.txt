set(CUBOS_TESTS_SOURCE
    "test_settings.cpp"
)

add_executable(cubos-core-tests ${CUBOS_TESTS_SOURCE})

target_link_libraries(
    cubos-core-tests 
    cubos-core
    gtest_main
)

gtest_add_tests(TARGET cubos-core-tests)