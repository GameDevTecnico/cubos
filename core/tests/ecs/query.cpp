#include <doctest/doctest.h>

#include <cubos/core/ecs/system/arguments/query.hpp>

#include "utils.hpp"

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

template <typename... Ts>
static std::size_t queryCount(World& world)
{
    QueryData<Ts...> queryD{world, {}};
    Query<Ts...> query{queryD.view()};
    return query.count();
}

template <typename... Ts>
static Opt<std::tuple<Ts...>> queryOne(World& world, Entity entity)
{
    QueryData<Ts...> query{world, {}};
    return query.at(entity);
}

TEST_CASE("ecs::Query")
{
    World world{};
    setupWorld(world);

    // Create a few entities.
    auto empty = world.create();
    auto int0 = world.create();
    auto int1 = world.create();
    auto int2 = world.create();
    auto int3 = world.create();
    auto foo = world.create();

    world.components(int0).add(IntegerComponent{0});
    world.components(int1).add(IntegerComponent{1}).add(ParentComponent{});
    world.components(int2).add(IntegerComponent{2});
    world.components(int3).add(IntegerComponent{3}).add(ParentComponent{});
    world.components(foo).add(ParentComponent{});

    // Check if direct access to entities works.
    CHECK(queryOne<>(world, empty).contains());
    CHECK_FALSE(queryOne<const IntegerComponent&>(world, empty).contains());
    CHECK_FALSE(queryOne<const IntegerComponent&, ParentComponent&>(world, empty).contains());
    CHECK_FALSE(queryOne<const IntegerComponent&, ParentComponent&>(world, int0).contains());
    CHECK(std::get<0>(*queryOne<const IntegerComponent&>(world, int0)).value == 0);
    CHECK(std::get<0>(*queryOne<const IntegerComponent&>(world, int1)).value == 1);

    // Check if direct access with optional components works.
    CHECK(std::get<0>(*queryOne<IntegerComponent&, Opt<ParentComponent&>>(world, int0)).value == 0);
    CHECK_FALSE(std::get<1>(*queryOne<IntegerComponent&, Opt<ParentComponent&>>(world, int0)).contains());
    CHECK(std::get<1>(*queryOne<IntegerComponent&, Opt<ParentComponent&>>(world, int1)).contains());

    // Check if the queries count the correct number of entities.
    CHECK(queryCount<>(world) == 6);
    CHECK(queryCount<const IntegerComponent&>(world) == 4);
    CHECK(queryCount<ParentComponent&>(world) == 3);
    CHECK(queryCount<IntegerComponent&, const ParentComponent&>(world) == 2);

    // Check if optional components are handled correctly.
    CHECK(queryCount<Opt<const IntegerComponent&>, Opt<ParentComponent&>>(world) == 6);
    CHECK(queryCount<Opt<IntegerComponent&>, const ParentComponent&>(world) == 3);
    CHECK(queryCount<IntegerComponent&, Opt<const ParentComponent&>>(world) == 4);

}
