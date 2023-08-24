/// @file
/// @brief Component @ref cubos::engine::Position.
/// @ingroup transform-plugin

#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Component which assigns a position to an entity.
    /// @sa LocalToWorld Holds the resulting transform matrix.
    /// @ingroup transform-plugin
    struct [[cubos::component("cubos/position", VecStorage)]] Position
    {
        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Position of the entity.
    };
} // namespace cubos::engine
