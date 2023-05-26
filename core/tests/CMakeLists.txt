# core/tests/CMakeLists.txt
# Core tests build configuration

add_executable(
    cubos-core-tests
    main.cpp
    data/context.cpp
)

target_link_libraries(cubos-core-tests cubos-core doctest::doctest)
cubos_common_target_options(cubos-core-tests)

add_test(NAME cubos-core-tests COMMAND cubos-core-tests)
