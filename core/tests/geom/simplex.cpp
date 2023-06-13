#include <doctest/doctest.h>

#include <cubos/core/geom/simplex.hpp>

using cubos::core::geom::Simplex;

TEST_CASE("geom::Simplex")
{
    auto p1 = glm::vec3{1.0f};
    auto p2 = glm::vec3{2.0f};
    auto p3 = glm::vec3{3.0f};
    auto p4 = glm::vec3{4.0f};

    SUBCASE("empty")
    {
        auto empty = Simplex::empty();
        CHECK(empty.points.empty());
    }

    SUBCASE("point")
    {
        auto point = Simplex::point(p1);
        CHECK_EQ(point.points.size(), 1);
        CHECK_EQ(point.points[0], p1);
    }

    SUBCASE("line")
    {
        auto line = Simplex::line(p1, p2);
        CHECK_EQ(line.points.size(), 2);
        CHECK_EQ(line.points[0], p1);
        CHECK_EQ(line.points[1], p2);
    }

    SUBCASE("triangle")
    {
        auto triangle = Simplex::triangle(p1, p2, p3);
        CHECK_EQ(triangle.points.size(), 3);
        CHECK_EQ(triangle.points[0], p1);
        CHECK_EQ(triangle.points[1], p2);
        CHECK_EQ(triangle.points[2], p3);
    }

    SUBCASE("tetrahedron")
    {
        auto tetrahedron = Simplex::tetrahedron(p1, p2, p3, p4);
        CHECK_EQ(tetrahedron.points.size(), 4);
        CHECK_EQ(tetrahedron.points[0], p1);
        CHECK_EQ(tetrahedron.points[1], p2);
        CHECK_EQ(tetrahedron.points[2], p3);
        CHECK_EQ(tetrahedron.points[3], p4);
    }
}