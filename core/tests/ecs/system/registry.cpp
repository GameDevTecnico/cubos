#include <doctest/doctest.h>

#include <cubos/core/ecs/system/registry.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::core::ecs;

TEST_CASE("ecs::SystemRegistry")
{
    World world{};
    SystemRegistry registry{};

    auto systemId1 = registry.add("my system 1", System<void>::make(world, []() {}, {}));
    REQUIRE(registry.name(systemId1) == "my system 1");
    REQUIRE(registry.system(systemId1).access().dataTypes.empty());

    auto conditionId1 = registry.add("my condition 1", System<bool>::make(world, []() { return false; }, {}));
    REQUIRE(registry.name(conditionId1) == "my condition 1");
    REQUIRE(registry.condition(conditionId1).access().dataTypes.empty());

    auto systemId2 = registry.add("my system 2", System<void>::make(world, []() {}, {}));
    REQUIRE(registry.name(systemId2) == "my system 2");
    REQUIRE(systemId1 != systemId2);

    auto conditionId2 = registry.add("my condition 2", System<bool>::make(world, []() { return false; }, {}));
    REQUIRE(registry.name(conditionId2) == "my condition 2");
    REQUIRE(conditionId1 != conditionId2);
}
