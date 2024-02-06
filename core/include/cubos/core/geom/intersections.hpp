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
    struct Intersect
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal belongs
        float penetration;               ///< Penetration depth of the collision.
        glm::vec3 position;              ///< Position of contact on the surface of the entity.
        glm::vec3 normal;                ///< Normal of contact on the surface of the entity.
    };

    /// @brief Computes the intersection between two box shapes.
    /// @param box1 Box shape of the first entity.
    /// @param localToWorld1 Local to world matrix of the first entity.
    /// @param box2 Box shape of the second entity.
    /// @param localToWorld2 Local to world matrix of the second entity.
    bool intersects(const cubos::core::ecs::Entity& ent1, const Box& box1, const glm::mat4& localToWorld1,
                    const cubos::core::ecs::Entity& ent2, const Box& box2, const glm::mat4& localToWorld2,
                    Intersect& intersect);

} // namespace cubos::core::geom
