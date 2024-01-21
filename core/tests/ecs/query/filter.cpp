#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/query/filter.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;

using namespace cubos::core::ecs;

TEST_CASE("ecs::QueryFilter")
{
    World world{};
    setupWorld(world);
    auto integerComponent = world.types().id(reflect<IntegerComponent>());
    auto parentComponent = world.types().id(reflect<ParentComponent>());

    // Create some entities.
    auto e1 = world.create();
    auto e2I = world.create();
    world.components(e2I).add(IntegerComponent{2});
    auto e3I = world.create();
    world.components(e3I).add(IntegerComponent{3});
    auto e4P = world.create();
    world.components(e4P).add(ParentComponent{e1});

    auto a = ArchetypeId::Empty;
    auto aI = world.archetypeGraph().with(a, DenseColumnId::make(integerComponent));
    auto aP = world.archetypeGraph().with(a, DenseColumnId::make(parentComponent));

    SUBCASE("with a single target")
    {
        std::vector<QueryTerm> terms{};
        bool no_terms = false;

        SUBCASE("no terms")
        {
            no_terms = true;
        }

        SUBCASE("optional integer")
        {
            terms = {QueryTerm::makeOptComponent(integerComponent, 0)};
        }

        if (no_terms || !terms.empty())
        {
            QueryFilter filter{world, terms};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == a);
                REQUIRE(it->entities[0] == e1);
                ++it;
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e2I);
                ++it;
                REQUIRE(it.cursorRows()[0] == 1);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e3I);
                ++it;
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aP);
                REQUIRE(it->entities[0] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e1")
            {
                auto view = filter.view().pin(0, e1);
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == a);
                REQUIRE(it->entities[0] == e1);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e3I")
            {
                auto view = filter.view().pin(0, e3I);
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 1);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e3I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e4P")
            {
                auto view = filter.view().pin(0, e4P);
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aP);
                REQUIRE(it->entities[0] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }
        }

        SUBCASE("with integer")
        {
            QueryFilter filter{world, {QueryTerm::makeWithComponent(integerComponent, 0)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e2I);
                ++it;
                REQUIRE(it.cursorRows()[0] == 1);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e3I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e3I")
            {
                auto view = filter.view().pin(0, e3I);
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 1);
                REQUIRE(it.targetArchetypes()[0] == aI);
                REQUIRE(it->entities[0] == e3I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e4P")
            {
                auto view = filter.view().pin(0, e4P);
                REQUIRE(view.begin() == view.end());
            }
        }

        SUBCASE("without integer")
        {
            QueryFilter filter{world, {QueryTerm::makeWithoutComponent(integerComponent, 0)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == a);
                REQUIRE(it->entities[0] == e1);
                ++it;
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aP);
                REQUIRE(it->entities[0] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("pinned on e2I")
            {
                auto view = filter.view().pin(0, e3I);
                REQUIRE(view.begin() == view.end());
            }

            SUBCASE("pinned on e4P")
            {
                auto view = filter.view().pin(0, e4P);
                auto it = view.begin();
                REQUIRE(it.cursorRows()[0] == 0);
                REQUIRE(it.targetArchetypes()[0] == aP);
                REQUIRE(it->entities[0] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }
        }

        SUBCASE("with both integer and parent")
        {
            QueryFilter filter{
                world,
                {QueryTerm::makeWithComponent(integerComponent, 0), QueryTerm::makeWithComponent(parentComponent, 0)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                REQUIRE(view.begin() == view.end());
            }

            SUBCASE("pinned on e1")
            {
                auto view = filter.view().pin(0, e1);
                REQUIRE(view.begin() == view.end());
            }
        }
    }
}
