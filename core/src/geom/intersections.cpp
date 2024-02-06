#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::core::geom;

CUBOS_REFLECT_IMPL(cubos::core::geom::Intersect)
{
    return core::ecs::TypeBuilder<Intersect>("cubos::engine::CollisionInfo")
        .withField("penetration", &Intersect::penetration)
        .withField("position", &Intersect::position)
        .withField("normal", &Intersect::normal)
        .build();
}

void getAxes(const glm::mat4& localToWorld, glm::vec3 axes[])
{
    glm::vec3 normals[3];
    cubos::core::geom::Box::normals(normals);

    for (int i = 0; i < 3; i++)
    {
        axes[i] = glm::normalize(localToWorld * glm::vec4(normals[i], 0.0F));
    }
}

glm::vec2 project(const Box& box, glm::vec3 axis, const glm::mat4& localToWorld)
{
    glm::vec3 corners[8];
    box.corners(corners);

    double min = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[0], 1.0F)));
    double max = min;
    for (int i = 1; i < 8; i++)
    {
        double p = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[i], 1.0F)));
        if (p < min)
        {
            min = p;
        }
        else if (p > max)
        {
            max = p;
        }
    }
    return glm::vec2{min, max};
}

bool overlap(glm::vec2 p1, glm::vec2 p2)
{
    if ((p1.x < p2.x) && (p2.x < p1.y) && (p1.y < p2.y))
    {
        return true;
    }
    if ((p2.x < p1.x) && (p1.x < p2.y) && (p2.y < p1.y))
    {
        return true;
    }
    if ((p1.x < p2.x) && (p2.y < p1.y))
    {
        return true;
    }
    if ((p2.x < p1.x) && (p1.y < p2.y))
    {
        return true;
    }
    return (p2.x == p1.x) && (p1.y == p2.y);
}

float getOverlap(glm::vec2 p1, glm::vec2 p2)
{
    if (p2.x < p1.x)
    {
        if (p2.y < p1.y)
        {
            return glm::abs(p2.y - p1.x);
        }
        return glm::abs(p1.y - p1.x);
    }
    if (p1.x < p2.x)
    {
        if (p1.y < p2.y)
        {
            return glm::abs(p1.y - p2.x);
        }
        return glm::abs(p2.y - p2.x);
    }
    // TODO: change this
    return 110.0F;
}

bool cubos::core::geom::intersects(const cubos::core::ecs::Entity& ent1, const Box& box1,
                                   const glm::mat4& localToWorld1, const cubos::core::ecs::Entity& ent2,
                                   const Box& box2, const glm::mat4& localToWorld2, Intersect& intersect)
{
    static glm::vec3 axes1[3];
    static glm::vec3 axes2[3];
    getAxes(localToWorld1, axes1);
    getAxes(localToWorld2, axes2);

    // TODO: change this
    intersect.penetration = 100.0F;

    for (glm::vec3 axis : axes1)
    {

        glm::vec2 p1 = project(box1, axis, localToWorld1);
        glm::vec2 p2 = project(box2, axis, localToWorld2);

        if (!overlap(p1, p2))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
        // get the overlap
        float o = getOverlap(p1, p2);
        // check for minimum
        if (o < intersect.penetration) // do this but with threshold otherwise this thing sucks
        {
            // then set this one as the smallest
            intersect.penetration = o;
            intersect.normal = axis;
            intersect.entity = ent1;
        }
    }

    for (glm::vec3 axis : axes2)
    {
        glm::vec2 p1 = project(box1, axis, localToWorld1);
        glm::vec2 p2 = project(box2, axis, localToWorld2);

        if (!overlap(p1, p2))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }

        // get the overlap
        float o = getOverlap(p1, p2);
        // check for minimum
        if (o < intersect.penetration)
        {
            // then set this one as the smallest
            intersect.penetration = o;
            intersect.normal = -axis;
            intersect.entity = ent2;
        }
    }

    return true;
}
