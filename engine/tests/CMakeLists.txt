# engine/tests/CMakeLists.txt
# Engine tests build configuration

add_executable(
    cubos-engine-tests
    main.cpp
    raycast.cpp
    transform.cpp
    settings.cpp
)

target_link_libraries(cubos-engine-tests cubos-engine doctest::doctest)
cubos_common_target_options(cubos-engine-tests)

add_test(NAME cubos-engine-tests COMMAND cubos-engine-tests)
