#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/query/data.hpp>

#include "../utils.hpp"

using namespace cubos::core::ecs;

TEST_CASE("ecs::QueryData")
{
    World world{};
    setupWorld(world);

    SUBCASE("match all entities")
    {
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

    SUBCASE("query tree relation")
    {
        // Create some entities.
        auto e1 = world.create();
        auto e2 = world.create();
        auto e3 = world.create();

        // Create a tree such that e1 is on depth 0, e2 is on depth 1 and e3 is on depth 2.
        world.relate(e3, e2, TreeRelation{.value = 32});
        world.relate(e2, e1, TreeRelation{.value = 21});

        // Add a component to e2, causing the relations to move tables (archetype change).
        world.components(e2).add(IntegerComponent{});

        // Query over the relation.
        QueryData<Entity, TreeRelation&, Entity> query{world, {}};
        auto view = query.view();

        auto it = view.begin();
        REQUIRE(it != view.end());
        REQUIRE(std::get<0>(*it) == e2);
        REQUIRE(std::get<1>(*it).value == 21);
        REQUIRE(std::get<2>(*it) == e1);

        ++it;
        REQUIRE(it != view.end());
        REQUIRE(std::get<0>(*it) == e3);
        REQUIRE(std::get<1>(*it).value == 32);
        REQUIRE(std::get<2>(*it) == e2);

        ++it;
        REQUIRE(it == view.end());
    }
}
