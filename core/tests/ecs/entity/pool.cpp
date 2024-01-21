#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/pool.hpp>

using cubos::core::ecs::EntityPool;

TEST_CASE("ecs::EntityPool")
{
    EntityPool pool{};

    auto e1 = pool.create({.inner = 1});
    REQUIRE(e1.index == 0);
    REQUIRE(e1.generation == 0);
    REQUIRE(pool.contains(e1));
    REQUIRE(pool.archetype(e1.index).inner == 1);
    REQUIRE(pool.generation(e1.index) == e1.generation);

    auto e2 = pool.create({.inner = 2});
    REQUIRE(e2.index == 1);
    REQUIRE(e2.generation == 0);
    REQUIRE(pool.contains(e2));
    REQUIRE(pool.archetype(e2.index).inner == 2);
    REQUIRE(pool.generation(e2.index) == e2.generation);

    pool.destroy(e1.index);
    REQUIRE_FALSE(pool.contains(e1));
    REQUIRE(pool.contains(e2));
    auto e3 = pool.create({.inner = 3});
    REQUIRE(e3.index == 0);
    REQUIRE(e3.generation == 1);
    REQUIRE(pool.contains(e3));
    REQUIRE(pool.archetype(e3.index).inner == 3);
    REQUIRE(pool.generation(e3.index) == e3.generation);
    REQUIRE_FALSE(pool.contains(e1));

    pool.archetype(e3.index, {.inner = 1});
    REQUIRE(pool.archetype(e3.index).inner == 1);
}
