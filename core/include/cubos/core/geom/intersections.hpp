/// @file
/// @brief Class @ref cubos::core::geom::Intersections.
/// @ingroup core-geom

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/geom/box.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Contains info regarding an intersection between shapes.
    /// @ingroup core-geom
    struct Intersection
    {
        CUBOS_REFLECT;

        float penetration;  ///< Penetration depth of the collision.
        glm::vec3 position; ///< Position of contact on the surface of the entity.
        glm::vec3 normal;   ///< Normal of contact on the surface of the entity.
    };

    /// @brief Computes the intersection between two box shapes.
    /// @param box1 Box shape of the first entity.
    /// @param localToWorld1 Local to world matrix of the first entity.
    /// @param box2 Box shape of the second entity.
    /// @param localToWorld2 Local to world matrix of the second entity.
    bool intersects(const Box& box1, const glm::mat4& localToWorld1, const Box& box2, const glm::mat4& localToWorld2,
                    Intersection& intersect);

} // namespace cubos::core::geom
