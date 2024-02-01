/// @file
/// @brief Class @ref cubos::core::geom::Intersections.
/// @ingroup core-geom

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Contains info regarding a collision.
    /// @ingroup core-geom
    struct CollisionInfo
    {
        CUBOS_REFLECT;

        bool collided;      ///< If there was a collision
        float penetration;  ///< Penetration depth of the collision.
        glm::vec3 position; ///< Position of contact on the surface of the entity.
        glm::vec3 normal;   ///< Normal of contact on the surface of the entity.
    };

    CollisionInfo BoxBoxIntersection(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                                     const glm::mat4& localToWorld2);

    std::vector<glm::vec3> getAxes(const Box& box, const glm::mat4& localToWorld);

    glm::vec2 project(const Box& box, glm::vec3 axis, const glm::mat4& localToWorld);

    bool overlap(glm::vec2 p1, glm::vec2 p2);

    float getOverlap(glm::vec2 p1, glm::vec2 p2);

} // namespace cubos::core::geom
