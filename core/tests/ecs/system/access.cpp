#include <doctest/doctest.h>

#include <cubos/core/ecs/system/access.hpp>

using namespace cubos::core::ecs;

TEST_CASE("ecs::SystemAccess")
{
    SystemAccess foo{};
    SystemAccess bar{};

    SUBCASE("non-intersecting")
    {
        SUBCASE("both empty")
        {
        }

        SUBCASE("foo accesses a type and bar is empty")
        {
            foo.dataTypes.insert(DataTypeId{.inner = 1});
        }

        SUBCASE("both access different data types")
        {
            foo.dataTypes.insert(DataTypeId{.inner = 1});
            bar.dataTypes.insert(DataTypeId{.inner = 2});
        }

        CHECK_FALSE(foo.intersects(bar));
        CHECK_FALSE(bar.intersects(foo));
    }

    SUBCASE("intersecting")
    {
        SUBCASE("one accesses the world and the other is empty")
        {
            foo.usesWorld = true;
        }

        SUBCASE("one accesses the world and the other accesses other data types")
        {
            foo.usesWorld = true;
            bar.dataTypes.insert(DataTypeId{.inner = 2});
        }

        SUBCASE("both access the world")
        {
            foo.usesWorld = true;
            bar.usesWorld = true;
        }

        SUBCASE("both access the same data types")
        {
            foo.dataTypes.insert(DataTypeId{.inner = 1});
            foo.dataTypes.insert(DataTypeId{.inner = 2});
            bar.dataTypes.insert(DataTypeId{.inner = 2});
            bar.dataTypes.insert(DataTypeId{.inner = 3});
        }

        CHECK(foo.intersects(bar));
        CHECK(bar.intersects(foo));
    }
}
