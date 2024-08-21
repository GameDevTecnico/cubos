/// @file
/// @brief Relation @ref cubos::engine::ContactManifold.
/// @ingroup collisions-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Contains info regarding a contact point of a @ContactManifold.
    struct ContactPointData
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to.
        glm::vec3 position1;             ///< Position on the entity of the contact in global coordinates.
        glm::vec3 position2;             ///< Position on the other entity of the contact in global coordinates.
        float penetration;               ///< Penetration of the contact point. Always positive.
        /// TODO: normal and tangent impulse?
        /// The contact feature ID on the first shape. This indicates the ID of
        /// the vertex, edge, or face of the contact, if one can be determined.
        int id; /// TODO: use this
    };

    /// @brief Represents a contact interface between two bodies.
    struct ContactManifold
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity;      ///< Entity to which the normal is relative to.
        glm::vec3 normal;                     ///< A contact normal shared by all contacts in this manifold,
                                              ///< expressed in the local space of the first entity.
        std::vector<ContactPointData> points; ///< Contact points of this manifold.
    };
} // namespace cubos::engine
