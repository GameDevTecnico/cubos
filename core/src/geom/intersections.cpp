#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::core::geom;

CUBOS_REFLECT_IMPL(cubos::core::geom::CollisionInfo)
{
    return core::ecs::TypeBuilder<CollisionInfo>("cubos::engine::CollisionInfo")
        .withField("collided", &CollisionInfo::collided)
        .withField("penetration", &CollisionInfo::penetration)
        .withField("position", &CollisionInfo::position)
        .withField("normal", &CollisionInfo::normal)
        .build();
}

CollisionInfo cubos::core::geom::BoxBoxIntersection(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                                                    const glm::mat4& localToWorld2)
{
    CollisionInfo collision = CollisionInfo{
        .collided = false, .penetration = 0.0F, .position = {0.0F, 0.0F, 0.0F}, .normal = {0.0F, 0.0F, 0.0F}};

    std::vector<glm::vec3> axes1 = getAxes(box1, localToWorld1);
    std::vector<glm::vec3> axes2 = getAxes(box2, localToWorld2);

    for (glm::vec3 axis : axes1)
    {
        glm::vec2 p1 = project(box1, axis, localToWorld1);
        glm::vec2 p2 = project(box2, axis, localToWorld2);

        if (!overlap(p1, p2))
        {
            // then we can guarantee that the shapes do not overlap
            return collision;
        }
        else
        {
            // get the overlap
            float o = getOverlap(p1, p2);
            // check for minimum
            if (o < collision.penetration)
            {
                // then set this one as the smallest
                collision.penetration = o;
                collision.normal = axis;
            }
        }
    }

    for (glm::vec3 axis : axes2)
    {
        glm::vec2 p1 = project(box1, axis, localToWorld1);
        glm::vec2 p2 = project(box2, axis, localToWorld2);

        if (!overlap(p1, p2))
        {
            // then we can guarantee that the shapes do not overlap
            return collision;
        }
        else
        {
            // get the overlap
            float o = getOverlap(p1, p2);
            // check for minimum
            if (o < collision.penetration)
            {
                // then set this one as the smallest
                collision.penetration = o;
                collision.normal = -axis;
            }
        }
    }

    collision.collided = true;
    return collision;
}

std::vector<glm::vec3> cubos::core::geom::getAxes(const Box& box, const glm::mat4& localToWorld)
{
    glm::vec3 normals[6];
    box.normals(normals);

    std::vector<glm::vec3> axes = {};
    for (glm::vec3 normal : normals)
    {
        axes.push_back(glm::normalize(localToWorld * glm::vec4(normal, 0.0F)));
        // cubos::core::gl::Debug::drawLine(glm::vec3(localToWorld.mat[3]),
        //                                  glm::normalize(localToWorld.mat * glm::vec4(normal, 0.0F)), true);
    }
    return axes;
}

glm::vec2 cubos::core::geom::project(const Box& box, glm::vec3 axis, const glm::mat4& localToWorld)
{
    glm::vec3 corners[8];
    box.corners(corners);

    double min = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[0], 1.0F)));
    double max = min;
    for (int i = 1; i < 8; i++)
    {
        // NOTE: the axis must be normalized to get accurate projections
        double p = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[i], 1.0F)));
        // cubos::core::gl::Debug::drawLine(glm::vec3(localToWorld.mat[3]), localToWorld.mat *
        // glm::vec4(corners[i], 1.0F),
        //                                  false, glm::vec3(0.0F, 1.0F, 0.0F));
        if (p < min)
        {
            min = p;
        }
        else if (p > max)
        {
            max = p;
        }
    }
    // CUBOS_DEBUG("mix max {} {}", min, max);
    return glm::vec2{min, max};
}

bool cubos::core::geom::overlap(glm::vec2 p1, glm::vec2 p2)
{
    if (((p1.x < p2.x) && (p2.x < p1.y)) || ((p1.x < p2.y) && (p2.y < p1.y))) // missing case where p2 contains p1
    {
        return true;
    }
    else if (((p2.x < p1.x) && (p1.x < p2.y)) || ((p2.x < p1.y) && (p1.y < p2.y)))
    {
        return true;
    }
    return false;
}

float cubos::core::geom::getOverlap(glm::vec2 p1, glm::vec2 p2)
{
    // float overlap = 0.0F;
    if (p2.x < p1.x)
    {
        if (p2.y < p1.y)
        {
            return glm::abs(p2.y - p1.x);
        }
        else // (p1.y < p2.y)
        {
            return glm::abs(p1.y - p1.x);
        }
    }
    else if (p1.x < p2.x)
    {
        if (p1.y < p2.y)
        {
            return glm::abs(p1.y - p2.x);
        }
        else // (p2.y < p1.y)
        {
            return glm::abs(p2.y - p2.x);
        }
    }
    return 0.0F;
}
