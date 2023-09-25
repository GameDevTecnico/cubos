/// @file
/// @brief Component @ref cubos::engine::Camera.
/// @ingroup renderer-plugin

#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a camera used to render the world.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/camera", VecStorage)]] Camera
    {
        float fovY;  ///< Vertical field of view in degrees.
        float zNear; ///< Near clipping plane.
        float zFar;  ///< Far clipping plane.
    };
} // namespace cubos::engine