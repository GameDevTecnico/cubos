#include <doctest/doctest.h>

#include <cubos/core/geom/box.hpp>

#include "utils.hpp"

using cubos::core::geom::Box;

TEST_CASE("geom::Box")
{
    auto x = 1.0f;
    auto y = 2.0f;
    auto z = 3.0f;
    Box box{{x, y, z}};

    SUBCASE("diagonal")
    {
        glm::vec3 corners[2];
        box.diag(corners);
        CHECK_VEC3_EQ(corners[0], glm::vec3{-x, -y, -z});
        CHECK_VEC3_EQ(corners[1], glm::vec3{x, y, z});
    }

    SUBCASE("four corners")
    {
        glm::vec3 corners[4];
        box.corners4(corners);
        CHECK_VEC3_EQ(corners[0], glm::vec3{x, -y, -z});
        CHECK_VEC3_EQ(corners[1], glm::vec3{-x, y, -z});
        CHECK_VEC3_EQ(corners[2], glm::vec3{-x, -y, z});
        CHECK_VEC3_EQ(corners[3], glm::vec3{x, y, z});
    }

    SUBCASE("eight corners")
    {
        glm::vec3 corners[8];
        box.corners(corners);
        CHECK_VEC3_EQ(corners[0], glm::vec3{-x, -y, -z});
        CHECK_VEC3_EQ(corners[1], glm::vec3{x, y, z});
        CHECK_VEC3_EQ(corners[2], glm::vec3{-x, -y, z});
        CHECK_VEC3_EQ(corners[3], glm::vec3{x, y, -z});
        CHECK_VEC3_EQ(corners[4], glm::vec3{-x, y, -z});
        CHECK_VEC3_EQ(corners[5], glm::vec3{x, -y, z});
        CHECK_VEC3_EQ(corners[6], glm::vec3{-x, y, z});
        CHECK_VEC3_EQ(corners[7], glm::vec3{x, -y, -z});
    }
}
