#include <limits>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

static const float THRESHOLD = static_cast<float>(1.0e-5);

using namespace cubos::core::geom;

CUBOS_REFLECT_IMPL(cubos::core::geom::Intersection)
{
    return core::ecs::TypeBuilder<Intersection>("cubos::engine::Intersection")
        .withField("penetration", &Intersection::penetration)
        .withField("position", &Intersection::position)
        .withField("normal", &Intersection::normal)
        .build();
}

static void getAxes(const glm::mat4& localToWorld, glm::vec3 axes[])
{
    glm::vec3 normals[3];
    Box::normals(normals);

    for (int i = 0; i < 3; i++)
    {
        axes[i] = glm::normalize(localToWorld * glm::vec4(normals[i], 0.0F));
    }
}

static glm::vec2 project(const Box& box, glm::vec3 axis, const glm::mat4& localToWorld)
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

static bool overlap(glm::vec2 p1, glm::vec2 p2)
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
    return (glm::abs(p2.x - p1.x) < THRESHOLD) && (glm::abs(p1.y - p2.y) < THRESHOLD); // add threshold
}

static float getOverlap(glm::vec2 p1, glm::vec2 p2)
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
    return std::numeric_limits<double>::infinity();
}

bool cubos::core::geom::intersects(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                                   const glm::mat4& localToWorld2, Intersection& intersect)
{
    static glm::vec3 axes1[3];
    static glm::vec3 axes2[3];
    getAxes(localToWorld1, axes1);
    getAxes(localToWorld2, axes2);

    intersect.penetration = std::numeric_limits<double>::infinity();

    // Vector from the first entity's center of mass to the second
    glm::vec3 direction = glm::normalize(glm::vec3(localToWorld2[3]) - glm::vec3(localToWorld1[3]));

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
        if (o < intersect.penetration)
        {
            // then set this one as the smallest
            intersect.penetration = o;
            intersect.normal = glm::dot(direction, axis) < 0 ? -axis : axis;
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
            intersect.normal = glm::dot(direction, axis) < 0 ? -axis : axis;
        }
    }

    if (intersect.penetration == std::numeric_limits<double>::infinity())
    {
        intersect.penetration = 0.0F;
    }

    return true;
}
