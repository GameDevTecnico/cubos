/// @file
/// @brief Component @ref cubos::engine::Rotation.

#pragma once

#include <glm/gtx/quaternion.hpp>

namespace cubos::engine
{
    /// @brief Component which assigns a rotation to an entity.
    /// @sa LocalToWorld Holds the resulting transform matrix.
    /// @ingroup transform-plugin
    struct [[cubos::component("cubos/rotation", VecStorage)]] Rotation
    {
        glm::quat quat = glm::quat(1.0F, 0.0F, 0.0F, 0.0F); ///< Rotation of the entity.
    };
} // namespace cubos::engine
