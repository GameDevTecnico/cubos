#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/query/data.hpp>

#include "../utils.hpp"

using namespace cubos::core::ecs;

TEST_CASE("ecs::QueryData")
{
    World world{};
    setupWorld(world);

    // Create a query which matches all entities.
    QueryData<Entity> query{world, {}};
    auto view = query.view();

    REQUIRE(view.begin() == view.end());

    // Create a few entities.
    auto e1 = world.create();
    auto e2I = world.create();
    world.components(e2I).add(IntegerComponent{2});
    auto e2P = world.create();
    world.components(e2P).add(ParentComponent{e1});

    // After updating the query, it should match all entities.
    query.update();
    REQUIRE(view.begin() != view.end());
    std::unordered_set<Entity, EntityHash> entities{e1, e2I, e2P};
    for (auto [entity] : view)
    {
        REQUIRE(entities.contains(entity));
        entities.erase(entity);
    }
    REQUIRE(entities.empty());
}
