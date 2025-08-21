/// @file
/// @brief Component @ref cubos::engine::CollisionMask.
/// @ingroup collisions-plugin

#pragma once

#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component that identifies an entity as the owner of a Collider. Works with a CollisionShape (added
    /// separately).
    ///
    /// Between intersecting colliders exist the relations:
    /// @ref IntersectionStart.
    /// @ref CollidingWith.
    /// @ref IntersectionEnd.
    ///
    /// @note Since these relations are created during the collision phase, any system that uses them must run
    ///     after the `collisionsTag`.
    ///
    /// Next is an example of how to use the relations associated with Colliders:
    ///
    /// @code{.cpp}
    ///     cubos.system("exited area")
    ///    .after(collisionsTag)
    ///    .call([](Query<Entity, Collider&, IntersectionEnd&, Entity, Collider&> query,
    ///             State& state) {
    ///        for (auto [ent1, collider1, intersectionEnd, ent2, collider2] : query)
    ///        {
    ///             if (collider1.isArea && collider2.isStatic) {
    ///                 CUBOS_INFO("An area just stopped hitting a static collider. Disabling Area.");
    ///                 collider1.isActive = false;
    ///             }
    ///        }
    ///    });
    /// @endcode
    ///
    /// @ingroup collisions-plugin
    struct Collider
    {
        CUBOS_REFLECT;

        /// @brief Whether the collider is an Area.
        ///
        /// An area collider detects collisions with other Colliders, however, collision manifolds are not generated.
        ///
        /// Cannot be changed after creation.
        const bool isArea = false;

        /// @brief Whether the Collider is Static.
        ///
        /// Used when it does not move, or its movement is limited. Static Colliders do not collide with each other.
        ///
        /// Cannot be changed after creation.
        const bool isStatic = false;

        /// @brief Identities if a collider is active. When a Collider is not active, no collisions will be detected
        /// with it.
        bool isActive = true;
    };
} // namespace cubos::engine
