/// @file
/// @brief Component @ref cubos::engine::Camera.
/// @ingroup renderer-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a camera used to render the world.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct CUBOS_ENGINE_API Camera
    {
        CUBOS_REFLECT;

        float fovY = 60.0F;   ///< Vertical field of view in degrees.
        float zNear = 0.1F;   ///< Near clipping plane.
        float zFar = 1000.0F; ///< Far clipping plane.
    };
} // namespace cubos::engine
