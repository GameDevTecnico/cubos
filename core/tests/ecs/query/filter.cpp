#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/query/filter.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;

using namespace cubos::core::ecs;

// NOLINTBEGIN(readability-function-size)
TEST_CASE("ecs::QueryFilter")
{
    World world{};
    setupWorld(world);
    auto integerComponent = world.types().id(reflect<IntegerComponent>());
    auto parentComponent = world.types().id(reflect<ParentComponent>());
    auto emptyRelation = world.types().id(reflect<EmptyRelation>());
    auto symmetricRelation = world.types().id(reflect<SymmetricRelation>());
    auto treeRelation = world.types().id(reflect<TreeRelation>());

    // Create some entities.
    auto e1 = world.create();
    auto e2I = world.create();
    world.components(e2I).add(IntegerComponent{2});
    auto e3I = world.create();
    world.components(e3I).add(IntegerComponent{3});
    auto e4P = world.create();
    world.components(e4P).add(ParentComponent{e1});

    // Create relations between them.
    world.relate(e1, e1, EmptyRelation{});
    world.relate(e1, e2I, EmptyRelation{});
    world.relate(e2I, e1, EmptyRelation{});
    world.relate(e4P, e3I, EmptyRelation{});

    world.relate(e1, e1, SymmetricRelation{});
    world.relate(e2I, e3I, SymmetricRelation{});
    world.relate(e4P, e2I, SymmetricRelation{});

    world.relate(e1, e2I, TreeRelation{});
    world.relate(e2I, e4P, TreeRelation{});
    world.relate(e3I, e4P, TreeRelation{});

    auto a = ArchetypeId::Empty;
    auto aI = world.archetypeGraph().with(a, ColumnId::make(integerComponent));
    auto aP = world.archetypeGraph().with(a, ColumnId::make(parentComponent));

    SUBCASE("with a single target")
    {
        std::vector<QueryTerm> terms{};
        bool noTerms = false;

        SUBCASE("no terms")
        {
            noTerms = true;
        }

        SUBCASE("optional integer")
        {
            terms = {QueryTerm::makeOptComponent(integerComponent, 0)};
        }

        if (noTerms || !terms.empty())
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

    SUBCASE("with two targets and a single normal relation")
    {
        SUBCASE("unfiltered relation")
        {
            QueryFilter filter{world, {QueryTerm::makeRelation(emptyRelation, 0, 1)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e4P);
                REQUIRE(it->entities[1] == e3I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(0, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(1, e3I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e1")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e1 and e2I")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e3I and e4P")
                {
                    auto view = filter.view().pin(0, e3I).pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("e4P and e3I")
                {
                    auto view = filter.view().pin(0, e4P).pin(1, e3I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("flipped unfiltered relation")
        {
            QueryFilter filter{world, {QueryTerm::makeRelation(emptyRelation, 1, 0)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it->entities[0] == e3I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(0, e3I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(1, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e1")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e2I and e1")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e4P and e3I")
                {
                    auto view = filter.view().pin(0, e4P).pin(1, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("e3I and e4P")
                {
                    auto view = filter.view().pin(0, e3I).pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("relation filtered by the first term")
        {
            QueryFilter filter{world,
                               {
                                   QueryTerm::makeWithComponent(integerComponent, 0),
                                   QueryTerm::makeRelation(emptyRelation, 0, 1),
                               }};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(0, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(1, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e2I")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("e2I and e1")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("relation filtered by the second term")
        {
            QueryFilter filter{world,
                               {
                                   QueryTerm::makeRelation(emptyRelation, 0, 1),
                                   QueryTerm::makeWithComponent(integerComponent, 1),
                               }};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it->entities[0] == e4P);
                REQUIRE(it->entities[1] == e3I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(0, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(1, e3I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e2I")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e2I and e1")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e1);
                    REQUIRE(view.begin() == view.end());
                }
            }
        }

        SUBCASE("relation with a negative filter")
        {
            QueryFilter filter{world,
                               {
                                   QueryTerm::makeWithComponent(integerComponent, 0),
                                   QueryTerm::makeRelation(emptyRelation, 0, 1),
                                   QueryTerm::makeWithoutComponent(integerComponent, 1),
                               }};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(0, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e3I")
                {
                    auto view = filter.view().pin(1, e3I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e2I")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("e2I and e1")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e2I and e2I")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }
            }
        }
    }

    SUBCASE("with two targets and a single symmetric relation")
    {
        SUBCASE("unfiltered relation")
        {
            QueryFilter filter{world, {QueryTerm::makeRelation(symmetricRelation, 0, 1)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e3I);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("both targets")
            {
                SUBCASE("on e1 and e1")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I and e4P")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P and e2I")
                {
                    auto view = filter.view().pin(0, e4P).pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I and e3I")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e3I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e3I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e3I and e2I")
                {
                    auto view = filter.view().pin(0, e3I).pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("relation without integers on the first term")
        {
            QueryFilter filter{world,
                               {
                                   QueryTerm::makeWithoutComponent(integerComponent, 0),
                                   QueryTerm::makeRelation(symmetricRelation, 0, 1),
                               }};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e4P);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e1")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e2I and e4P")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e4P);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("e4P and e2I")
                {
                    auto view = filter.view().pin(0, e4P).pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e4P);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("relation without integers on the second term")
        {
            QueryFilter filter{world,
                               {
                                   QueryTerm::makeRelation(symmetricRelation, 0, 1),
                                   QueryTerm::makeWithoutComponent(integerComponent, 1),
                               }};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e1);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }

            SUBCASE("both targets pinned")
            {
                SUBCASE("e1 and e1")
                {
                    auto view = filter.view().pin(0, e1).pin(1, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e1);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e2I and e4P")
                {
                    auto view = filter.view().pin(0, e2I).pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("e4P and e2I")
                {
                    auto view = filter.view().pin(0, e4P).pin(1, e2I);
                    REQUIRE(view.begin() == view.end());
                }
            }
        }
    }

    SUBCASE("with two targets and a single tree relation")
    {
        SUBCASE("bottom to top traversal")
        {
            QueryFilter filter{world, {QueryTerm::makeRelation(treeRelation, 0, 1, Traversal::Up)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it->entities[0] == e3I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }

        SUBCASE("top to bottom traversal")
        {
            QueryFilter filter{world, {QueryTerm::makeRelation(treeRelation, 0, 1, Traversal::Down)}};

            SUBCASE("no pins")
            {
                auto view = filter.view();
                auto it = view.begin();
                REQUIRE(it->entities[0] == e2I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it->entities[0] == e3I);
                REQUIRE(it->entities[1] == e4P);
                ++it;
                REQUIRE(it->entities[0] == e1);
                REQUIRE(it->entities[1] == e2I);
                ++it;
                REQUIRE(it == view.end());
            }

            SUBCASE("first target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(0, e1);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(0, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(0, e4P);
                    REQUIRE(view.begin() == view.end());
                }
            }

            SUBCASE("second target pinned")
            {
                SUBCASE("on e1")
                {
                    auto view = filter.view().pin(1, e1);
                    REQUIRE(view.begin() == view.end());
                }

                SUBCASE("on e2I")
                {
                    auto view = filter.view().pin(1, e2I);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e1);
                    REQUIRE(it->entities[1] == e2I);
                    ++it;
                    REQUIRE(it == view.end());
                }

                SUBCASE("on e4P")
                {
                    auto view = filter.view().pin(1, e4P);
                    auto it = view.begin();
                    REQUIRE(it->entities[0] == e2I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it->entities[0] == e3I);
                    REQUIRE(it->entities[1] == e4P);
                    ++it;
                    REQUIRE(it == view.end());
                }
            }
        }
    }
}
// NOLINTEND(readability-function-size)
