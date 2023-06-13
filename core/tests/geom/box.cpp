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

    SUBCASE("two corners")
    {
        glm::vec3 corners[2];
        box.corners2(corners);
        CHECK_GLM_EQ(corners[0], glm::vec3{-x, -y, -z});
        CHECK_GLM_EQ(corners[1], glm::vec3{x, y, z});
    }

    SUBCASE("six corners")
    {
        glm::vec3 corners[6];
        box.corners6(corners);
        CHECK_GLM_EQ(corners[0], glm::vec3{-x, -y, -z});
        CHECK_GLM_EQ(corners[1], glm::vec3{x, y, z});
        CHECK_GLM_EQ(corners[2], glm::vec3{-x, -y, z});
        CHECK_GLM_EQ(corners[3], glm::vec3{x, y, -z});
        CHECK_GLM_EQ(corners[4], glm::vec3{-x, y, -z});
        CHECK_GLM_EQ(corners[5], glm::vec3{x, -y, z});
    }

    SUBCASE("eight corners")
    {
        glm::vec3 corners[8];
        box.corners(corners);
        CHECK_GLM_EQ(corners[0], glm::vec3{-x, -y, -z});
        CHECK_GLM_EQ(corners[1], glm::vec3{x, y, z});
        CHECK_GLM_EQ(corners[2], glm::vec3{-x, -y, z});
        CHECK_GLM_EQ(corners[3], glm::vec3{x, y, -z});
        CHECK_GLM_EQ(corners[4], glm::vec3{-x, y, -z});
        CHECK_GLM_EQ(corners[5], glm::vec3{x, -y, z});
        CHECK_GLM_EQ(corners[6], glm::vec3{-x, y, z});
        CHECK_GLM_EQ(corners[7], glm::vec3{x, -y, -z});
    }
}
