# core/tests/CMakeLists.txt
# Core tests build configuration

include(CubinhosGenerate)

add_executable(
    cubos-core-tests
    main.cpp

    data/context.cpp

    ecs/registry.cpp
    ecs/world.cpp
    ecs/query.cpp
    ecs/blueprint.cpp
    ecs/commands.cpp
    ecs/system.cpp
    ecs/dispatcher.cpp
)

target_link_libraries(cubos-core-tests cubos-core doctest::doctest)
cubos_common_target_options(cubos-core-tests)
cubinhos_generate(cubos-core-tests ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME cubos-core-tests COMMAND cubos-core-tests)
