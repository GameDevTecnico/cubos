#include <doctest/doctest.h>

#include <cubos/core/geom/capsule.hpp>

using cubos::core::geom::Capsule;

TEST_CASE("geom::Capsule")
{
    auto r = 1.0f;
    auto l = 2.0f;
    Capsule capsule{r, l};

    SUBCASE("height")
    {
        CHECK_EQ(capsule.height(), 2 * r + l);
    }

    SUBCASE("sphere")
    {
        auto sphere = Capsule::sphere(1.0f);
        CHECK_EQ(sphere.height(), 2 * r);
    }
}
