# core/tests/CMakeLists.txt
# Core tests build configuration

include(QuadradosGenerate)

add_executable(
    cubos-core-tests
    main.cpp

    reflection/reflect.cpp
    reflection/type.cpp
    reflection/traits/constructible.cpp
    reflection/traits/fields.cpp
    reflection/external/primitives.cpp
    reflection/external/glm.cpp
    reflection/external/vector.cpp
    reflection/external/map.cpp
    reflection/external/unordered_map.cpp

    data/fs/embedded_archive.cpp
    data/fs/standard_archive.cpp
    data/fs/file_system.cpp
    data/context.cpp

    ecs/registry.cpp
    ecs/world.cpp
    ecs/query.cpp
    ecs/blueprint.cpp
    ecs/commands.cpp
    ecs/system.cpp
    ecs/dispatcher.cpp

    geom/box.cpp
    geom/capsule.cpp
)

target_link_libraries(cubos-core-tests cubos-core doctest::doctest)
cubos_common_target_options(cubos-core-tests)
quadrados_generate(cubos-core-tests ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME cubos-core-tests COMMAND cubos-core-tests)
