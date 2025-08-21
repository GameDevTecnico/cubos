/// @file
/// @brief Component @ref cubos::engine::ColliderBundle.
/// @ingroup collisions-plugin

#pragma once

#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Bundle that packs the components for a Collider entity.
    ///
    /// Adds @ref Collider, @ref CollisionLayers and @ref CollisionMask.
    ///
    /// A CollisionShape must be added separately.
    ///
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API ColliderBundle
    {
        CUBOS_REFLECT;

        /// @brief Whether the @ref Collider is an Area.
        bool isArea = false;

        /// @brief Whether the @ref Collider is Static.
        bool isStatic = false;

        /// @brief Whether the collider is active.
        bool isActive = true;

        /// @brief Layers of the collider.
        uint32_t layers = 0x00000001;

        /// @brief Mask of layers which the collider can collide with.
        uint32_t mask = 0x00000001;
    };
} // namespace cubos::engine
