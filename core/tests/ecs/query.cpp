#include <doctest/doctest.h>

#include <cubos/core/ecs/system/query.hpp>

#include "utils.hpp"

using cubos::core::ecs::Entity;
using cubos::core::ecs::OptRead;
using cubos::core::ecs::OptWrite;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;

/// Counts the number of entities which match the given query.
/// @tparam Args The component types to query for.
/// @param query The query to use.
template <typename... Args>
static std::size_t queryCount(World& world)
{
    auto query = Query<Args...>(world);
    std::size_t counter = 0;
    for (auto entity : query)
    {
        (void)entity;
        counter += 1;
    }
    return counter;
}

/// Returns the result of the given query for a single entity, assuming that the query matches the
/// given entity.
/// @tparam Args The component types to query for.
/// @param world The world to query in.
/// @param entity The entity to query for.
template <typename Access, typename... Args>
static Access queryOne(World& world, Entity entity)
{
    return std::get<Access>(*Query<Access, Args...>(world)[entity]);
}

TEST_CASE("ecs::Query")
{
    World world{};
    setupWorld(world);

    // Create a few entities.
    auto empty = world.create();
    auto int0 = world.create(IntegerComponent{0});
    auto int1 = world.create(IntegerComponent{1}, ParentComponent{});
    world.create(ParentComponent{});
    world.create(IntegerComponent{2});
    world.create(IntegerComponent{3}, ParentComponent{});

    // Check if direct access to entities works.
    CHECK(Query<>(world)[empty].has_value());
    CHECK_FALSE(Query<Read<IntegerComponent>>(world)[empty].has_value());
    CHECK_FALSE(Query<Read<IntegerComponent>, Write<ParentComponent>>(world)[int0].has_value());
    CHECK(queryOne<Read<IntegerComponent>>(world, int0)->value == 0);
    CHECK(queryOne<Read<IntegerComponent>>(world, int1)->value == 1);

    // Check if direct access with optional components works.
    CHECK(queryOne<Read<IntegerComponent>, OptRead<ParentComponent>>(world, int0)->value == 0);
    CHECK_FALSE(queryOne<OptRead<ParentComponent>>(world, int0));

    // Check if the queries count the correct number of entities.
    CHECK(queryCount<>(world) == 6);
    CHECK(queryCount<Read<IntegerComponent>>(world) == 4);
    CHECK(queryCount<Write<ParentComponent>>(world) == 3);
    CHECK(queryCount<Write<IntegerComponent>, Read<ParentComponent>>(world) == 2);

    // Check if optional components are handled correctly.
    CHECK(queryCount<OptRead<IntegerComponent>, OptWrite<ParentComponent>>(world) == 6);
    CHECK(queryCount<OptWrite<IntegerComponent>, Read<ParentComponent>>(world) == 3);
    CHECK(queryCount<Write<IntegerComponent>, OptRead<ParentComponent>>(world) == 4);

    // Check if QueryInfo objects correctly report the components being queried.
    auto info = Query<Write<IntegerComponent>, Read<ParentComponent>, OptWrite<DetectDestructorComponent>>::info();
    CHECK(info.read.size() == 1);
    CHECK(info.read.contains(typeid(ParentComponent)));
    CHECK(info.written.size() == 2);
    CHECK(info.written.contains(typeid(IntegerComponent)));
    CHECK(info.written.contains(typeid(DetectDestructorComponent)));

    // If no components are specified, the info should be empty.
    info = Query<>::info();
    CHECK(info.read.empty());
    CHECK(info.written.empty());
}
