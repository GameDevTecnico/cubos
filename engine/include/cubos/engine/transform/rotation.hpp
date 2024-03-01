/// @file
/// @brief Component @ref cubos::engine::Rotation.
/// @ingroup transform-plugin

#pragma once

#include <glm/gtx/quaternion.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which assigns a rotation to an entity.
    /// @sa LocalToWorld Holds the resulting transform matrix.
    /// @ingroup transform-plugin
    struct CUBOS_ENGINE_API Rotation
    {
        CUBOS_REFLECT;

        glm::quat quat = glm::quat(1.0F, 0.0F, 0.0F, 0.0F); ///< Rotation of the entity.

        /// @brief Returns a rotation component looking at the given direction.
        /// @param direction Direction to look at.
        /// @param up Up vector.
        /// @return Rotation component.
        static Rotation lookingAt(glm::vec3 direction, glm::vec3 up = {0.0F, 1.0F, 0.0F});
    };
} // namespace cubos::engine
