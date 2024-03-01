/// @file
/// @brief Component @ref cubos::engine::PerspectiveCamera.
/// @ingroup render-camera-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a perspective camera used to render the world.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup render-camera-plugin
    struct CUBOS_ENGINE_API PerspectiveCamera
    {
        CUBOS_REFLECT;

        /// @brief Whether the camera is drawing to a target.
        bool active = true;

        /// @brief Vertical field of view in degrees.
        float fovY = 60.0F;

        /// @brief Near clipping plane.
        float zNear = 0.1F;

        /// @brief Far clipping plane.
        float zFar = 1000.0F;
    };
} // namespace cubos::engine
