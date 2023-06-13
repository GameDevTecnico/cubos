#include <doctest/doctest.h>

#include <cubos/core/geom/capsule.hpp>

using cubos::core::geom::Capsule;

TEST_CASE("geom::Capsule")
{
    Capsule capsule{1.0f, 2.0f};

    SUBCASE("height")
    {
        CHECK_EQ(capsule.height(), 4.0f);
    }

    SUBCASE("sphere")
    {
        Capsule sphere{1.0f};

        CHECK_EQ(sphere.height(), 2.0f);
    }
}