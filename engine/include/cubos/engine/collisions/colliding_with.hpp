/// @file
/// @brief Relation @ref cubos::engine::CollidingWith.
/// @ingroup collisions-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>

namespace cubos::engine
{
    /// @brief Relation which represents a collision.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API CollidingWith
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normals in the manifolds are relative to..

        std::vector<ContactManifold> manifolds; ///< All contact interfaces between the entities
    };
} // namespace cubos::engine
